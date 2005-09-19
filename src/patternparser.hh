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

  PatternOrderBy* parseOrderBy(char* orderstr);

  PatternCondition* buildMatchCondition(char  flag, char* value);
  PatternCondition* buildMatchCondition(char* flag, char* value);

  // Utility methods
  void padHistory();
  unsigned int charToId(char* number);
  char* makeCopy(char* orig);
};

#endif  // __PATTERNPARSER_HH__
