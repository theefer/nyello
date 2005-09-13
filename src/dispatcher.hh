#ifndef __DISPATCHER_HH__
#define __DISPATCHER_HH__


#define MAX_PROMPT_LENGTH   24
#define MAX_COMMAND_LENGTH 256

#define PROMPT "nyello:%s $ "

class Dispatcher;
typedef void (Dispatcher::*DispFnPtr)();


#include <iostream>
#include <map>

#include <xmmsclient/xmmsclient.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "playback.hh"
#include "medialibrary.hh"
#include "patternparser.hh"
#include "command.hh"
#include "output.hh"

#include "patternquery.hh"


using namespace std;


/**
 * Wrapper for the comparison function between two char*, needed for
 * the member map.
 */
struct charCmp {
  bool operator() (const char* s1, const char* s2) const {
    return strcmp(s1, s2) < 0;
  }
};


class Dispatcher {
public:
  Dispatcher(xmmsc_connection_t* connection);
  ~Dispatcher();

  void loop();
  void execute(int num_args, char* args[]);
  void dispatch();


  void actionExit();

  void actionHelp();
  void actionStatus();

  void actionPlay();
  void actionPause();
  void actionTogglePlay();
  void actionStop();
  
  void actionPrevious();
  void actionNext();

  void actionInfo();
  void actionList();

  void actionEnqueue();
  void actionInsert();
  void actionReplace();
  void actionRemove();

  void actionPlaylistList();
  void actionPlaylistSaveAs();
  void actionPlaylistUse();
  void actionPlaylistRemove();

  void actionPlaylistRandom();

  void actionCollectionApply();
  void actionCollectionSaveAs();
  void actionCollectionEnter();
  void actionCollectionRemove();


private:
  static const int MAX_ARGUMENTS  = 64;
  static const int MAX_TOKEN_SIZE = 64;

  Playback*      playback;
  MediaLibrary*  medialib;
  PatternParser* pparser;
  Output*        output;

  char* command;
  char** arguments;
  int   argNumber;

  list<Command*> commands;
  map<const char*, Command*, charCmp> commandList;


  void parsePattern(char* ptr, int num);
  void parsePatternFromArgs();

  int parseInteger(char* ptr);

  bool parseInput(char* input);
  char* parseToken(char** str);
};


#endif  // __DISPATCHER_HH__
