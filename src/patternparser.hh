#ifndef __PATTERNPARSER_HH__
#define __PATTERNPARSER_HH__


#include "playback.hh"
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


  PatternParser(Playback* playback);
  ~PatternParser();

  PatternQuery* registerNewPattern(char** arguments, int numArgs);

private:
  Playback* playback;
  deque<PatternQuery*> history;

  int numArgs;
  char** arguments;

  int currIndex;
  char* currArg;

  char* orderby;
  char* defaultOrderby;

  char* nextArgument();

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

  PatternCondition* buildMatchCondition(char  flag, char* value);
  PatternCondition* buildMatchCondition(char* flag, char* value);

  // Utility methods
  void padHistory();
  unsigned int charToId(char* number);
};

#endif  // __PATTERNPARSER_HH__
