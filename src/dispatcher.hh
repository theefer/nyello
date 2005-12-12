#ifndef __DISPATCHER_HH__
#define __DISPATCHER_HH__

// FIXME: Should be dynamic and customizable
#define PROMPT "nyello:%s $ "

class Dispatcher;
typedef void (Dispatcher::*DispFnPtr)();


#include <iostream>
#include <map>

#include <xmmsclient/xmmsclient.h>

#include <readline/readline.h>
#include <readline/history.h>

//#include "delayed.hh"
#include "playback.hh"
#include "medialibrary.hh"
#include "patternparser.hh"
#include "command.hh"
#include "output.hh"
#include "sequence.hh"

#include "patternquery.hh"
#include "abstractresult.hh"
#include "resultlist.hh"
#include "selectionresultlist.hh"
#include "playlistresultlist.hh"


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
  void actionJump();

  void actionInfo();
  void actionList();

  void actionEnqueue();
  void actionInsert();
  void actionReplace();
  void actionRemove();
  void actionClear();
  void actionShuffle();

  void actionPlaylistList();
  void actionPlaylistSaveAs();
  void actionPlaylistUse();
  void actionPlaylistRemove();

  void actionPlaylistRandom();

  void actionCollectionApply();
  void actionCollectionSaveAs();
  void actionCollectionEnter();
  void actionCollectionRemove();

  // FIXME: Should remain private?
  bool parseInput(char* input);

private:
  static const int MAX_ARGUMENTS  = 64;
  static const int MAX_TOKEN_SIZE = 64;
  static const int MAX_PROMPT_LENGTH = 24;
  static const int MAX_COMMAND_LENGTH = 256;

  xmmsc_connection_t* conn;

  Playback*      playback;
  MediaLibrary*  medialib;
  PatternParser* pparser;
  Output*        output;
  Asynchronizer* async;

  char* command;
  char** arguments;
  int   argNumber;

  list<Command*> commands;
  map<const char*, Command*, charCmp> commandList;

  bool showprompt;


  void parsePattern(char* ptr, int num);
  void parsePatternFromArgs();

  int parseInteger(char* ptr);

  char* parseToken(char** str);

  void waitAndFree(DelayedVoid* del);
};


#endif  // __DISPATCHER_HH__
