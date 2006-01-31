#include "dispatcher.hh"

// Declare singleton instance
Dispatcher* Dispatcher::instance = NULL;


Dispatcher::Dispatcher(xmmsc_connection_t* connection) {
  conn     = connection;
  async    = new Asynchronizer(connection);
  StaticAsynchronizer::setAsynchronizer(async);

  playback = new Playback(connection);
  medialib = new MediaLibrary(connection);
  pparser  = new PatternParser(playback, medialib);
  output   = new Output();

  command   = NULL;
  arguments = new char*[MAX_ARGUMENTS];
  argNumber = 0;
  finished  = false;

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


Dispatcher*
Dispatcher::getInstance(xmmsc_connection_t* connection) {
  if(instance != NULL) {
    delete instance;
  }

  instance = new Dispatcher(connection);
  return instance;
}


Dispatcher*
Dispatcher::getInstance() {
  return instance;
}


Dispatcher::~Dispatcher() {
  delete playback;
  delete medialib;
  delete pparser;
  delete output;
  // FIXME: Delete asynchronizer?
}


/**
 * Function called whenever a full line is read by readline.
 */
void readline_callback(char* input) {
  Dispatcher* disp = Dispatcher::getInstance();

  // End of stream, quit
  if(input == NULL) {
    disp->stop();
    return;
  }

  // Empty command, do nothing
  if(strlen(input) == 0) {
    return;
  }

  add_history(input);

  disp->parseInput(input);
  disp->dispatch();
  disp->refreshPrompt();
}


/**
 * Main loop, reading commands from the user and running them.
 */
void
Dispatcher::loop() {
  refreshPrompt();

  // Main event loop
  while(!finished) {
    async->waitForData();
  }

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


void
Dispatcher::refreshPrompt() {
  char* prompt = new char[MAX_PROMPT_LENGTH + 1];
  snprintf(prompt, MAX_COMMAND_LENGTH, PROMPT, medialib->getCurrentPlaylistName().c_str());
  rl_callback_handler_install(prompt, &readline_callback);
  delete prompt;
}


/**
 * Stop the mainloop and leave the dispatcher.
 */
void
Dispatcher::stop() {
  finished = true;
  rl_callback_handler_remove();
  cout << endl;
}


/**
 * Wait on the given Delayed pointer and destroy it.
 */
void
Dispatcher::waitAndFree(Delayed<void>* del) {
  if(del != NULL) {
    del->wait();
    delete del;
  }
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
    AbstractResult* song = NULL;
    int curr_id = Delayed<unsigned int>::readAndFree(playback->getCurrentId());
    if(curr_id > 0) {
      Delayed<RichResult*>* res = medialib->getSongById(curr_id);
      song = Delayed<RichResult*>::readAndFree(res);

      if(song != NULL) {
        ResultList* rlist = new ResultList(song);
        output->printStatus(rlist,
                            Delayed<unsigned int>::readAndFree(playback->getStatus()),
                            Delayed<unsigned int>::readAndFree(playback->getCurrentPlaytime()),
                            Delayed<unsigned int>::readAndFree(playback->getCurrentPosition()) + 1,
                            medialib->getCurrentPlaylistName());
        delete rlist;
      }
    }

    if(song == NULL) {
      output->printEmptyStatus(Delayed<unsigned int>::readAndFree(playback->getStatus()),
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
  Delayed<void>* res = playback->play();
  waitAndFree(res);
}

/**
 * Pause playback.
 */
void
Dispatcher::actionPause() {
  Delayed<void>* res = playback->pause();
  waitAndFree(res);
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
  Delayed<void>* res = playback->stop();
  waitAndFree(res);
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

  Delayed<void>* res = playback->jumpRelative(offset);
  waitAndFree(res);
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

  Delayed<void>* res = playback->jumpRelative(offset);
  waitAndFree(res);
}

/**
 * Jump in the playlist.
 */
void
Dispatcher::actionJump() {
  int offset;
  if(argNumber == 1) {
    Delayed<void>* res;
    if((*arguments[0] == '+') || (*arguments[0] == '-')) {
      res = playback->jumpRelative(parseInteger(arguments[0]));
    }
    else {
      res = playback->jumpAbsolute(parseInteger(arguments[0]) - 1);
    }

    waitAndFree(res);
  }
  else {
    cerr << "Error: jump requires one argument!" << endl;
    return;
  }
}


/**
 * Seek in the song currently playing.
 */
void
Dispatcher::actionSeek() {
  int offset;
  if(argNumber == 1) {
    Delayed<void>* res;
    if((*arguments[0] == '+') || (*arguments[0] == '-')) {
      res = playback->seekRelative(parseInteger(arguments[0]) * 1000);
    }
    else {
      res = playback->seekAbsolute(parseInteger(arguments[0]) * 1000);
    }

    waitAndFree(res);
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
  AbstractResult* songlist;

  songlist = findSongsFromArgs();
  if(songlist == NULL) {
    cerr << "Error: failed to query the medialibrary!" << endl;
  }
  else {
    ResultList* rlist = new ResultList(songlist);
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
  Delayed<SongResult*>* pldel = NULL;

  // No playlist name, list current
  if(argNumber == 0) {
    pldel = medialib->getCurrentPlaylist();
    if(pldel != NULL) {
      songList = new SelectionResultList(pldel->getProduct(),
                                         Delayed<unsigned int>::readAndFree(playback->getCurrentPosition()));
      delete pldel;
    }
    else {
      cerr << "Error: problem while listing the playlist!" << endl;
    }
  }
  // List playlist with the given name
  else if(argNumber == 1) {
    pldel = medialib->getPlaylist(arguments[0]);
    if(pldel != NULL) {
      songList = new ResultList(pldel->getProduct());
      delete pldel;
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
 * Import new files in the medialibrary.
 */
void
Dispatcher::actionImport() {
  if(argNumber == 0) {
    cerr << "Error: you must specify at least one file!" << endl;
  }
  else {
    for(int i = 0; i < argNumber; ++i) {
      Delayed<void>* res = medialib->import(arguments[i]);
      waitAndFree(res);
    }
  }
}


/**
 * Enqueue the songs matched by the given pattern at the end of the
 * current playlist.
 */
void
Dispatcher::actionEnqueue() {
  AbstractResult* songlist;

  songlist = findSongsFromArgs();
  if(songlist == NULL) {
    return;
  }

  medialib->enqueueSongs(songlist);
}


/**
 * Insert the songs matched by the given pattern in the current
 * playlist, right after the current song.
 */
void
Dispatcher::actionInsert() {
  AbstractResult* songlist;
  unsigned int position;
  int playlist_size;

  songlist = findSongsFromArgs();
  if(songlist == NULL) {
    return;
  }

  position = Delayed<unsigned int>::readAndFree(playback->getCurrentPosition()) + 1;
  playlist_size = Delayed<int>::readAndFree(medialib->getCurrentPlaylistSize());
  if(position < playlist_size) {
    medialib->insertSongs(songlist, position);
  }
  else {
    medialib->enqueueSongs(songlist);
  }
}


/**
 * Replace the current song by the songs matched by the given pattern.
 */
void
Dispatcher::actionReplace() {
  AbstractResult* songlist;
  unsigned int position;
  int playlist_size;
  bool invalid_pos;

  songlist = findSongsFromArgs();
  if(songlist == NULL) {
    return;
  }

  position = Delayed<unsigned int>::readAndFree(playback->getCurrentPosition()) + 1;
  playlist_size = Delayed<int>::readAndFree(medialib->getCurrentPlaylistSize());
  if(position < playlist_size) {
    medialib->insertSongs(songlist, position);
  }
  else {
    medialib->enqueueSongs(songlist);
  }

  if(position <= playlist_size) {
    waitAndFree(medialib->removeSongAt(position - 1));
    waitAndFree(playback->jumpRelative(1));
  }
}


/**
 * Search the medialibrary for songs corresponding to the pattern
 * read from the arguments.
 */
AbstractResult*
Dispatcher::findSongsFromArgs() {
  Delayed<SelectResult*>* res;
  PatternQuery* query;

  query = pparser->registerNewPattern(arguments, argNumber);
  if(query == NULL) {
    cerr << "Error: failed to parse the pattern!" << endl;
    return NULL;
  }

  res = medialib->searchSongs(query);
  return Delayed<SelectResult*>::readAndFree(res);
}


/**
 * Remove all songs with a position matched by the sequences given in
 * argument from the current playlist.
 */
void
Dispatcher::actionRemove() {
  // Get list of songs in current playlist
  AbstractResult* playlist;
  playlist = medialib->getCurrentPlaylist()->getProduct();
  if(playlist == NULL) {
    return;
  }

  // Get list of songs matching the pattern
  AbstractResult* songlist;
  songlist = findSongsFromArgs();
  if(songlist == NULL) {
    return;
  }

  // Remove intersection of both lists
  bool playing(Delayed<bool>::readAndFree(playback->isPlaying()));
  unsigned int currentpos(Delayed<unsigned int>::readAndFree(playback->getCurrentPosition()));
  unsigned int pos(0);
  bool found(false), tickle(false);
  playlist->rewind();
  while(playlist->isValid()) {

    songlist->rewind();
    while(!found && songlist->isValid()) {
      unsigned int id(songlist->getId());
      if(playlist->getId() == songlist->getId()) {
        waitAndFree(medialib->removeSongAt(pos));
        found = true;

        if(!tickle && playing && (pos == currentpos)) {
          tickle = true;
        }
      }

      songlist->next();
    }

    if(found) {
      --currentpos;
      found = false;
    }
    else {
      ++pos;
    }
    playlist->next();
  }

  delete playlist;
  delete songlist;

  // Handle the mess if we removed the playing song
  if(tickle) {
    int plsize(Delayed<int>::readAndFree(medialib->getCurrentPlaylistSize()));
    if(currentpos < plsize - 1) {
      waitAndFree(playback->jumpRelative(1));
    }
    else {
      waitAndFree(playback->stop());
    }
  }
}

/**
 * Clear the current playlist.
 * FIXME: Should allow to clear other playlists.
 */
void
Dispatcher::actionClear() {
  if(argNumber == 0) {
    Delayed<void>* del = medialib->clearCurrentPlaylist();
    waitAndFree(del);
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
    Delayed<void>* del = medialib->shuffleCurrentPlaylist();
    waitAndFree(del);
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

  Delayed<RichResult*>* res = medialib->getPlaylists();
  AbstractResult* playlists = Delayed<RichResult*>::readAndFree(res);
  if(playlists == NULL) {
    cerr << "Error: failed to get the list of playlists!" << endl;
  }
  else {
    string curr_playlist = medialib->getCurrentPlaylistName();
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
    Delayed<void>* del = medialib->saveCurrentPlaylistAs(arguments[0]);
    waitAndFree(del);
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
    waitAndFree(medialib->usePlaylist(arguments[0]));
    waitAndFree(playback->tickle());
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
    Delayed<void>* del = medialib->removePlaylist(arguments[0]);
    waitAndFree(del);
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

