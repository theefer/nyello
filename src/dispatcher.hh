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

#ifndef __DISPATCHER_HH__
#define __DISPATCHER_HH__

#include <xmmsclient/xmmsclient++.h>
using namespace Xmms;

#include <string>
using std::string;

#include "readlinelistener.hh"


class Dispatcher
{
public:
  static Dispatcher* getInstance();

  ~Dispatcher();

  void execute( const string& input );
  void loop();

  void stop();
  const string getPrompt() const;

private:
  static Dispatcher* instance;

  Client client;

  ReadlineListener* readlineListener;

  Dispatcher();
  void initConnection();
};

/*
// FIXME: Should be dynamic and customizable
#define PROMPT "nyello:%s $ "

class Dispatcher;
typedef void (Dispatcher::*DispFnPtr)();


#include <iostream>
#include <map>


#include <readline/readline.h>
#include <readline/history.h>

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
 *
struct charCmp {
  bool operator() (const char* s1, const char* s2) const {
    return strcmp(s1, s2) < 0;
  }
};


class Dispatcher {
public:
  static Dispatcher* getInstance();

  ~Dispatcher();

  void loop();
  void execute(int num_args, char* args[]);
  void dispatch();
  void refreshPrompt();
  void stop();


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
  void actionSeek();

  void actionInfo();
  void actionList();

  void actionImport();

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

  bool parseInput(char* input);

private:
  Client& client;

  static const int MAX_ARGUMENTS  = 64;
  static const int MAX_TOKEN_SIZE = 128;
  static const int MAX_PROMPT_LENGTH = 24;
  static const int MAX_COMMAND_LENGTH = 256;

  static Dispatcher* instance;

  Playback*      playback;
  MediaLibrary*  medialib;
  PatternParser* pparser;
  Output*        output;
  Asynchronizer* async;

  bool finished;

  char* command;
  char** arguments;
  int   argNumber;

  list<Command*> commands;
  map<const char*, Command*, charCmp> commandList;


  Dispatcher();

  void parsePattern(char* ptr, int num);
  void parsePatternFromArgs();

  AbstractResult* findSongsFromArgs();

  int parseInteger(char* ptr);
  char* parseToken(char** str);

  void waitAndFree(Delayed<void>* del);
};
*/

#endif  // __DISPATCHER_HH__
