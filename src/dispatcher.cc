#include "dispatcher.hh"


Dispatcher::Dispatcher(xmmsc_connection_t* connection) {
  conn = connection;
  playback = new Playback(connection);
  medialib = new MediaLibrary(connection);
  pparser  = new PatternParser(playback, medialib);
  output   = new Output();
  async    = new Asynchronizer(connection);

  StaticAsynchronizer::setAsynchronizer(async);

  command   = NULL;
  arguments = new char*[MAX_ARGUMENTS];
  argNumber = 0;

  // Use an {alias=>command} hashlist to speed up command matching
  commands = Command::listAll();
  list<Command*>::iterator cmdIt;
  for(cmdIt = commands.begin(); cmdIt != commands.end(); ++cmdIt) {
    vector<char*> aliases = (*cmdIt)->getAliases();
    vector<char*>::iterator aliasIt;
    for(aliasIt = aliases.begin(); aliasIt != aliases.end(); ++aliasIt) {
      commandList[*aliasIt] = *cmdIt;
    }
  }
}


Dispatcher::~Dispatcher() {
  delete playback;
  delete medialib;
  delete output;
}


Dispatcher* korv;
void foo(char* input) {
  // End of stream, quit
  if(input == NULL) {
    cout << endl;
    return;
  }

  // Empty command, do nothing
  if(strlen(input) == 0) {
    return;
  }

  add_history(input);
  korv->parseInput(input);
  korv->dispatch();
}


/**
 * Main loop, reading commands from the user and running them.
 */
void
Dispatcher::loop() {
  char* prompt = new char[MAX_PROMPT_LENGTH + 1];
  char* input = new char[MAX_COMMAND_LENGTH];

  /*
  // Main loop
  do {
    // Build prompt and get input
    snprintf(prompt, MAX_COMMAND_LENGTH, PROMPT, medialib->getCurrentPlaylistName());
    input = readline(prompt);

    // End of stream, quit
    if(input == NULL) {
      cout << endl;
      break;
    }

    // Empty command, do nothing
    if(strlen(input) == 0) {
      continue;
    }

    add_history(input);
    parseInput(input);
    dispatch();

  } while(true);
  */


  korv = this; // FIXME: *UGLY UGLY UGLY HACK*
  snprintf(prompt, MAX_COMMAND_LENGTH, PROMPT, medialib->getCurrentPlaylistName());
  rl_callback_handler_install(NULL, &foo);
  cout << prompt;
  cout.flush();


  // Main event loop
  while(true) {
    async->waitForData();

    // FIXME: Show prompt when needed
    if(showprompt) {
      snprintf(prompt, MAX_COMMAND_LENGTH, PROMPT, medialib->getCurrentPlaylistName());
      cout << prompt;
      cout.flush();
      showprompt = false;
    }
    
  } /* End main event loop */

}


/**
 * Parse the input and execute the action.
 */
void
Dispatcher::execute(int num_args, char* args[]) {
  command = *args;
  argNumber = num_args - 1;
  arguments = args + 1;

  dispatch();
}

/**
 * Execute the current command.
 */
void
Dispatcher::dispatch() {
  DispFnPtr fn_ptr;
  if(commandList.find(command) != commandList.end()) {
    fn_ptr = commandList[command]->getAction();
    (this->*fn_ptr)();
  }
  else if(command != NULL) {
    cerr << "Unknown command '" << command << "'!" << endl;
  }

  showprompt = true;
}


int
Dispatcher::parseInteger(char* ptr) {
  // FIXME: Validate the number before, throw an exception if needed
  return (int)strtol(ptr, NULL, 10);
}


/**
 * Check whether there is a command. Fill command, arguments, argNumber.
 */
bool
Dispatcher::parseInput(char* input) {
  char* buffer;

  // Reset previous variables
  if(command != NULL)
    delete command;

  for(int i = 0; i < argNumber; ++i) {
    delete arguments[i];
  }
  argNumber = 0;

  // Get the command (error if none)
  command = parseToken(&input);
  if(command == NULL) return false;

  while((buffer = parseToken(&input)) != NULL) {
    arguments[argNumber] = buffer;
    ++argNumber;
  }  

  return true;
}


/**
 * Parse and return the first token in the given string.  Memory is
 * allocated for the returned token and the passed reference is
 * updated to point after the parsed token.
 */
char*
Dispatcher::parseToken(char** strref) {
  int i = 0, len = 0;
  char* str = *strref;
  char* token = NULL;
  char escape;
  bool escaping = false;

  // Skip heading spaces
  while(*str == ' ') ++str;

  // Determine string escape character
  switch(*str) {
  case '\0': return NULL; break;

  case '"':  escape = '"';  ++str; break;
  case '\'': escape = '\''; ++str; break;
  default:   escape = ' ';         break;
  }

  // Save the token
  token = new char[MAX_TOKEN_SIZE + 1];
  while(*str != '\0' && (escaping || *str != escape) && i < MAX_TOKEN_SIZE) {
    if(!escaping && *str == '\\') {
      escaping = true;
    }
    else {
      escaping = false;
      token[i++] = *str;
    }
    ++str;
  }
  token[i] = '\0';

  if(*str == escape) ++str;
  *strref = str;

  return token;
}


/**
 * Terminate the execution.
 */
void
Dispatcher::actionExit() {
  cout << "Exiting nyello..." << endl;
  exit(0);
}

/**
 * Display the list of commands (if no argument) or the help of a
 * given command.
 */
void
Dispatcher::actionHelp() {
  if(argNumber == 0) {
    output->printCommandSummary(commands);
  }
  else if(argNumber == 1) {
    if(commandList.find(arguments[0]) != commandList.end()) {
      output->printCommandHelp(commandList[arguments[0]]);
    }
    else {
      cerr << "Error: cannot display help for unknown command '"
           << arguments[0] << "'!" << endl;
    }
  }
  else {
    cerr << "Error: too many arguments!" << endl;
  }
}

/**
 * Display informations about the song currently being played, or
 * general informations if no song is selected.
 */
void
Dispatcher::actionStatus() {
  if(argNumber == 0) {
    AbstractResult* song;
    int curr_id = playback->getCurrentId()->getProduct();
    if(curr_id > 0 && (song = medialib->getSongById(curr_id)) != NULL) {
      ResultList* rlist = new ResultList(song);
      output->printStatus(rlist,
                          playback->getStatus()->getProduct(),
                          playback->getCurrentPlaytime()->getProduct(),
                          playback->getCurrentPosition()->getProduct() + 1,
                          medialib->getCurrentPlaylistName());
      delete rlist;
    }
    else {
      output->printEmptyStatus(playback->getStatus()->getProduct(),
                               medialib->getCurrentPlaylistName());
    }
  }
  else {
    cerr << "Error: this command doesn't take any argument!" << endl;
  }
}


/**
 * Start playback.
 */
void
Dispatcher::actionPlay() {
  DelayedVoid* res = playback->play();
  res->wait();
  delete res;
}

/**
 * Pause playback.
 */
void
Dispatcher::actionPause() {
  DelayedVoid* res = playback->pause();
  res->wait();
  delete res;
}

/**
 * Toggle playback between play/pause.
 */
void
Dispatcher::actionTogglePlay() {
  Delayed<bool>* res = playback->isPlaying();
  if(res->getProduct())
    actionPause();
  else
    actionPlay();
  delete res;
}

/**
 * Stop playback.
 */
void
Dispatcher::actionStop() {
  DelayedVoid* res = playback->stop();
  res->wait();
  delete res;
}

  
/**
 * Jump backward in the playlist.
 */
void
Dispatcher::actionPrevious() {
  int offset;
  if(argNumber == 0) {
    offset = -1;
  }
  else if(argNumber == 1) {
    offset = -parseInteger(arguments[0]);
  }
  else {
    cerr << "Error: too many arguments!" << endl;
    return;
  }

  DelayedVoid* res = playback->jumpRelative(offset);
  res->wait();
  delete res;
}

/**
 * Jump forward in the playlist.
 */
void
Dispatcher::actionNext() {
  int offset;
  if(argNumber == 0) {
    offset = 1;
  }
  else if(argNumber == 1) {
    offset = parseInteger(arguments[0]);
  }
  else {
    cerr << "Error: too many arguments!" << endl;
    return;
  }

  DelayedVoid* res = playback->jumpRelative(offset);
  res->wait();
  delete res;
}

/**
 * Jump in the playlist.
 */
void
Dispatcher::actionJump() {
  int offset;
  if(argNumber == 1) {
    DelayedVoid* res;
    if((*arguments[0] == '+') || (*arguments[0] == '-')) {
      res = playback->jumpRelative(parseInteger(arguments[0]));
    }
    else {
      res = playback->jumpAbsolute(parseInteger(arguments[0]) - 1);
    }

    res->wait();
    delete res;
  }
  else {
    cerr << "Error: jump requires one argument!" << endl;
    return;
  }
}


/**
 * Display the songs matching the given pattern.
 */
void
Dispatcher::actionInfo() {
  AbstractResult* songList;
  PatternQuery* query;

  query = pparser->registerNewPattern(arguments, argNumber);
  if(query == NULL) {
    cerr << "Error: failed to parse the pattern!" << endl;
    return;
  }

  songList = medialib->searchSongs(query);
  if(songList == NULL) {
    cerr << "Error: failed to query the medialibrary!" << endl;
  }
  else {
    ResultList* rlist = new ResultList(songList);
    output->printSongs(rlist);
    delete rlist;
  }  
}


/**
 * Display the content of the current playlist, or the playlist given
 * in argument if any.
 */
void
Dispatcher::actionList() {
  Printable* songList = NULL;
  AbstractResult* playlist = NULL;

  // No playlist name, list current
  if(argNumber == 0) {
    playlist = medialib->getCurrentPlaylist();
    if(playlist != NULL) {
      songList = new SelectionResultList(playlist,
                                         playback->getCurrentPosition()->getProduct());
    }
    else {
      cerr << "Error: problem while listing the playlist!" << endl;
    }
  }
  // List playlist with the given name
  else if(argNumber == 1) {
    playlist = medialib->getPlaylist(arguments[0]);
    if(playlist != NULL) {
      songList = new ResultList(playlist);
    }
    else {
      cerr << "Error: problem while listing the playlist!" << endl;
    }
  }
  // Error, too many arguments
  else {
    cerr << "Error: too many arguments!" << endl;
    return;
  }

  // Error, no such playlist
  if(songList == NULL) {
    cerr << "Error: no such playlist!" << endl;
  }
  else {
    output->printSongs(songList);
    delete songList;
  }
}


/**
 * Enqueue the songs matched by the given pattern at the end of the
 * current playlist.
 */
void
Dispatcher::actionEnqueue() {
  PatternQuery* query;
  query = pparser->registerNewPattern(arguments, argNumber);
  if(query == NULL) {
    cerr << "Error: failed to parse the pattern!" << endl;
    return;
  }
  medialib->enqueueSongs(query);
}


/**
 * Insert the songs matched by the given pattern in the current
 * playlist, right after the current song.
 */
void
Dispatcher::actionInsert() {
  PatternQuery* query;
  unsigned int position;
  query = pparser->registerNewPattern(arguments, argNumber);
  if(query == NULL) {
    cerr << "Error: failed to parse the pattern!" << endl;
    return;
  }
  position = playback->getCurrentPosition()->getProduct() + 1;
  if(position > 0) {
    medialib->insertSongs(query, position);
  }
  else {
    medialib->enqueueSongs(query);
  }
}


/**
 * Replace the current song by the songs matched by the given pattern.
 */
void
Dispatcher::actionReplace() {
  PatternQuery* query;
  unsigned int position;
  query = pparser->registerNewPattern(arguments, argNumber);
  if(query == NULL) {
    cerr << "Error: failed to parse the pattern!" << endl;
    return;
  }
  position = playback->getCurrentPosition()->getProduct() + 1;
  if(position > 0) {
    medialib->insertSongs(query, position);
    medialib->removeSongAt(position - 1);
    DelayedVoid* res = playback->jumpRelative(1);
    res->wait();
    delete res;
  }
  else {
    medialib->enqueueSongs(query);
  }
}

/**
 * Remove all songs with a position matched by the sequences given in
 * argument from the current playlist.
 */
void
Dispatcher::actionRemove() {
  if(argNumber > 0) {
    int playlist_len = medialib->getCurrentPlaylistSize();
    PositionSequence* positions = new PositionSequence();
    for(int i = 0; i < argNumber; ++i) {
      positions->parseAdd(arguments[i]);
    }
    for(int pos = playlist_len; pos > 0; --pos) {
      if(positions->contains(pos)) {
        medialib->removeSongAt(pos - 1);
      }
    }
  }
  else {
    cerr << "Error: this command requires arguments!" << endl;    
  }
}

/**
 * Clear the current playlist.
 * FIXME: Should allow to clear other playlists.
 */
void
Dispatcher::actionClear() {
  if(argNumber == 0) {
    medialib->clearCurrentPlaylist();
  }
  else {
    cerr << "Error: this command doesn't take any argument!" << endl;
  }
}

/**
 * Shuffle the current playlist.
 * FIXME: Should allow to shuffle other playlists.
 */
void
Dispatcher::actionShuffle() {
  if(argNumber == 0) {
    medialib->shuffleCurrentPlaylist();
  }
  else {
    cerr << "Error: this command doesn't take any argument!" << endl;
  }
}


/**
 * Display a list of all the playlists present in the medialib.
 */
void
Dispatcher::actionPlaylistList() {
  // We don't want any argument
  if(argNumber != 0) {
    cerr << "Error: this command doesn't take any argument!" << endl;
    return;
  }

  AbstractResult* playlists = medialib->getPlaylists()->getProduct();
  if(playlists == NULL) {
    cerr << "Error: failed to get the list of playlists!" << endl;
  }
  else {
    const char* curr_playlist = medialib->getCurrentPlaylistName();
    PlaylistResultList* plist = new PlaylistResultList(playlists, curr_playlist);
    output->printPlaylists(plist);
    delete plist;
  }
}


/**
 * Save the current server playlist under the given name.
 */
void
Dispatcher::actionPlaylistSaveAs() {
  // Save the current playlist under the new name
  if(argNumber == 1) {
    medialib->saveCurrentPlaylistAs(arguments[0]);
  }
  // Missing the playlist name
  else if(argNumber == 0) {
    cerr << "Error: you must specify the name of the playlist!" << endl;
  }
  else {
    cerr << "Error: too many arguments!" << endl;
  }
}


/**
 * Change the current playlist.
 */
void
Dispatcher::actionPlaylistUse() {
  // Use the given playlist
  if(argNumber == 1) {
    DelayedVoid* del = medialib->usePlaylist(arguments[0]);
    if(del != NULL) {
      del->wait();
      delete del;
    }
  }
  // Missing the playlist name
  else if(argNumber == 0) {
    cerr << "Error: you must specify the name of the playlist!" << endl;
  }
  else {
    cerr << "Error: too many arguments!" << endl;
  }
}


/**
 * Remove the given playlist from the medialibrary.
 */
void
Dispatcher::actionPlaylistRemove() {
  // Remove the given playlist
  if(argNumber == 1) {
    medialib->removePlaylist(arguments[0]);
  }
  // Missing the playlist name
  else if(argNumber == 0) {
    cerr << "Error: you must specify the name of the playlist!" << endl;
  }
  else {
    cerr << "Error: too many arguments!" << endl;
  }
}

void
Dispatcher::actionPlaylistRandom() {

}

void
Dispatcher::actionCollectionApply() {

}

void
Dispatcher::actionCollectionSaveAs() {

}

void
Dispatcher::actionCollectionEnter() {

}

void
Dispatcher::actionCollectionRemove() {

}

