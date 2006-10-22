/* nyello - an advanced command-line client for XMMS2
 * Copyright (C) 2006  Sébastien Cevey
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */


#ifndef __PATTERNPARSER_HH__
#define __PATTERNPARSER_HH__


#include "playback.hh"
#include "medialibrary.hh"
#include "patternnodes.hh"
#include "patternquery.hh"
#include "sequence.hh"

#include <deque>
#include <iostream>

using namespace std;



/**
 *
 */
class PatternParser {
public:
  static const char CHAR_PATTERNREF = '+';
  static const char CHAR_MATCHFLAGS = '-';
  static const char CHAR_CURRENTSEQ = '_';

  static const int MAX_HISTORY_LEN  = 10;


  PatternParser(Playback* playback, MediaLibrary* medialib);
  ~PatternParser();

  PatternQuery* registerNewPattern(char** arguments, int numArgs);

private:
  Playback*     playback;
  MediaLibrary* medialib;
  deque<PatternQuery*> history;

  int numArgs;
  char** arguments;

  int currIndex;
  char* currArg;

  PatternOrderBy* orderby;
  PatternOrderBy* defaultOrderBy;

  char* nextArgument();

  PatternOrderBy* getOrder();

  PatternNode* parse();
  PatternNode* parseGroup();
  PatternOperator* parseOperator();
  PatternNode* parseCondition();
  bool parseGroupStart();
  bool parseGroupEnd();

  PatternNode* parseShortMatchFlags();
  PatternNode* parseLongMatchFlags();

  PatternNode* parsePatternReference();

  PatternNode* parsePlaylistSequence();
  PatternNode* parseHistorySequence();
  PatternNode* parseMLibSequence();
  PatternNode* parseCurrentPosSequence();

  PatternOrderBy* parseOrderBy(char* orderstr);

  PatternCondition* buildMatchCondition(char  flag, char* value);
  PatternCondition* buildMatchCondition(char* flag, char* value);

  // Utility methods
  void padHistory();
  unsigned int charToId(char* number);
  char* makeCopy(char* orig);
  char* makeCopy(char* orig, unsigned int length);
};

#endif  // __PATTERNPARSER_HH__
