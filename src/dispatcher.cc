#include "dispatcher.hh"


Dispatcher::Dispatcher(xmmsc_connection_t* connection) {
  playback = new Playback(connection);
  medialib = new MediaLibrary(connection);
  output   = new Output();


  // TODO: Cleaner way to define this?
  commandList["h"]    = &Dispatcher::actionHelp;
  commandList["?"]    = &Dispatcher::actionHelp;
  commandList["help"] = &Dispatcher::actionHelp;

  // FIXME: 'p' should play AND pause, as a toggle
  commandList["p"]     = &Dispatcher::actionTogglePlay;
  commandList["play"]  = &Dispatcher::actionPlay;
  commandList["pause"] = &Dispatcher::actionPause;

  commandList["s"]     = &Dispatcher::actionStop;
  commandList["stop"]  = &Dispatcher::actionStop;

  commandList["n"   ]     = &Dispatcher::actionNext;
  commandList["next"]     = &Dispatcher::actionNext;
  commandList["r"]        = &Dispatcher::actionPrevious;
  commandList["previous"] = &Dispatcher::actionPrevious;

  commandList["l"]     = &Dispatcher::actionList;
  commandList["list"]  = &Dispatcher::actionList;

  commandList["pl"]               = &Dispatcher::actionPlaylistList;
  commandList["playlist-list"]    = &Dispatcher::actionPlaylistList;
  commandList["pu"]               = &Dispatcher::actionPlaylistUse;
  commandList["playlist-use"]     = &Dispatcher::actionPlaylistUse;
  commandList["ps"]               = &Dispatcher::actionPlaylistSaveAs;
  commandList["playlist-save-as"] = &Dispatcher::actionPlaylistSaveAs;
  commandList["pr"]               = &Dispatcher::actionPlaylistRemove;
  commandList["playlist-remove"]  = &Dispatcher::actionPlaylistRemove;

  commandList["q"]     = &Dispatcher::actionExit;
  commandList["exit"]  = &Dispatcher::actionExit;
  commandList["quit"]  = &Dispatcher::actionExit;
}


Dispatcher::~Dispatcher() {
  delete playback;
  delete medialib;
  delete output;
}


void
Dispatcher::loop() {
  char* prompt;
  char* input = new char[MAX_COMMAND_LENGTH];
  DispFnPtr fn_ptr;

  // Main loop
  do {
    // Build prompt and get input
    prompt = new char[MAX_PROMPT_LENGTH + 1];
    snprintf(prompt, MAX_COMMAND_LENGTH, PROMPT, medialib->getCurrentPlaylistName());
    input = readline(prompt);
    delete prompt;

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

    // Parse user input and execute action
    parseInput(input);
    if(commandList.find(command) != commandList.end()) {
      fn_ptr = commandList[command];
      (this->*fn_ptr)();
    }
    else if(command != NULL) {
      // Error: unknown command
      cerr << "Unknown command '" << command << "'!" << endl;
    }

  } while(true);

}

int
Dispatcher::parseInteger(char* ptr) {
  return (int)strtol(ptr, NULL, 10);
}


/**
 * Check whether there is a command. Fill command, arguments, argNumber.
 */
bool
Dispatcher::parseInput(char* input) {
  // We will need some temp vars
  const char* delimiters  = " ";
  char* buffer;

  // Free old vars
  //  if(command != NULL)
  //    delete command;

  if(arguments != NULL)
    delete[] arguments;

  arguments = new char*[MAX_ARGUMENTS];
  argNumber = 0;

  // Skip heading spaces, read command
  while(*input == ' ') ++input;
  command = strtok(input, delimiters);  // FIXME: Do not use strtok!

  // Woops, we didn't get any command
  if(command == NULL) return false;

  // Now save the arguments
  while(true) {
    buffer = strtok(NULL, delimiters);
    if(buffer == NULL) {
      break;
    }
    arguments[argNumber] = buffer;
    ++argNumber;
  }  

  return true;
}



void
Dispatcher::actionExit() {
  cout << "Exiting nyello..." << endl;
  exit(0);
}

void
Dispatcher::actionHelp() {
  // FIXME: Pass command objects containing help
  output->printHelp();
}

void
Dispatcher::actionStatus() {
  // TODO: Display status
}


void
Dispatcher::actionPlay() {
  playback->play();
}

void
Dispatcher::actionPause() {
  playback->pause();
}

void
Dispatcher::actionTogglePlay() {
  if(playback->isPlaying())
    actionPause();
  else
    actionPlay();
}

void
Dispatcher::actionStop() {
  playback->stop();
}

  
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

  playback->jump(offset);
}

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

  playback->jump(offset);
}


void
Dispatcher::actionInfo() {

}


void
Dispatcher::actionList() {
  Printable* songList;

  // No playlist name, list current
  if(argNumber == 0) {
    songList = medialib->getCurrentPlaylist();
  }
  // List playlist with the given name
  else if(argNumber == 1) {
    songList = medialib->getPlaylist(arguments[0]);
  }
  // Error, too many arguments
  else {
    cerr << "Error: too many arguments!" << endl;
    return;
  }

  // Error, no such playlist
  if(songList == NULL) {
    cerr << "Error: no such playlist!" << endl;
    return;
  }
  else {
    output->printSongs(songList);
  }
}


void
Dispatcher::actionEnqueue() {

}


void
Dispatcher::actionRemove() {

}


void
Dispatcher::actionPlaylistList() {
  // We don't want any argument
  if(argNumber != 0) {
    cerr << "Error: this command doesn't take any argument!" << endl;
    return;
  }

  PlaylistList* playlists = medialib->getPlaylists();
  if(playlists == NULL) {
    // FIXME: handle error
    cerr << "actionPlaylistList: NULL" << endl;
  }
  else {
    output->printPlaylists(playlists);
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
    medialib->usePlaylist(arguments[0]);
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

