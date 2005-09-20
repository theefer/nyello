#include "medialibrary.hh"


// Generic hack-function to use methods as callback functions
template <void (MediaLibrary::*func) (xmmsc_result_t*)>
void runMediaLibraryMethod(xmmsc_result_t *res, void *mlib_ptr) {
  MediaLibrary* ml = (MediaLibrary*)mlib_ptr;
  (ml->*func)(res);
}


MediaLibrary::MediaLibrary(xmmsc_connection_t* _connection) {
  connection = _connection;
  lastRes    = NULL;
  currentPlaylistName = NULL;


  // FIXME: Find current playlist name or start with the autosaved playlist
  //        If no matching playlist, create a new one ("current-$N") and use it?

  // FIXME: Let's not mess with the PL on startup for now
  //usePlaylist("autosaved");

  currentPlaylistName = new char[MAX_PLAYLIST_NAME_LEN + 1];
  strncpy(currentPlaylistName, "autosaved", MAX_PLAYLIST_NAME_LEN);

  // Setup signal hooks
  /* FIXME: segfault?
  lastRes = xmmsc_broadcast_playlist_changed(connection);
  xmmsc_result_notifier_set(lastRes, runMediaLibraryMethod<&MediaLibrary::catchPlaylistChanged>, this);
  xmmsc_result_unref(lastRes);
  */

  /* FIXME: segfault?
  lastRes = xmmsc_broadcast_medialib_playlist_loaded(connection);
  xmmsc_result_notifier_set(lastRes, runMediaLibraryMethod<&MediaLibrary::catchPlaylistLoaded>, this);
  xmmsc_result_unref(lastRes);
  */
}


AbstractResult*
MediaLibrary::getCurrentPlaylist() {
  lastRes = xmmsc_playlist_list(connection);
  xmmsc_result_wait(lastRes);

  if(xmmsc_result_iserror(lastRes)) {
    return NULL;
  }

  return new SongResult(lastRes, connection);
}


/**
 * Get the Playlist object of the playlist with the given name in
 * the medialib.
 */
AbstractResult*
MediaLibrary::getPlaylist(char* name) {
  // Invalid playlist name
  if(!validPlaylistName(name)) {
    cerr << "Error: invalid playlist name!" << endl;
    return NULL;
  }

  // Use the other method for current playlist (shows position)
  if(strcmp(name, currentPlaylistName) == 0) {
    return getCurrentPlaylist();
  }

  // Get list of songs of the given playlist
  lastRes = xmmsc_medialib_playlist_list(connection, name);
  xmmsc_result_wait(lastRes);

  if(xmmsc_result_iserror(lastRes)) {
    return NULL;
  }

  return new SongResult(lastRes, connection);

  // OLD: "SELECT songs.*, substr(playlistentries.entry,8,10) AS id FROM playlistentries, playlist LEFT JOIN songs ON songs.id=substr(playlistentries.entry,8,10) WHERE playlist_id=playlist.id AND playlist.name=\"%s\" ORDER BY pos",
}


/**
 * Get the list of all the Playlist objects in the medialib.
 * Playlists starting with a '_' prefix are hidden.
 */
AbstractResult*
MediaLibrary::getPlaylists() {
  // Retrieve all the playlists
  lastRes = xmmsc_medialib_select(connection, 
                                  "SELECT name, COUNT(entry) AS size "
                                  "FROM Playlist "
                                  "LEFT JOIN PlaylistEntries ON id=playlist_id "
                                  "WHERE substr(name, 0, 1) <> \"_\" "
                                  "GROUP BY playlist_id");
  xmmsc_result_wait(lastRes);

  // Oops, no playlist, return empty list!
  if(!xmmsc_result_list_valid(lastRes)) {
    return NULL;
  }

  return new RichResult(lastRes);
}


/**
 * Get the size of the given playlist.
 * If an error occured (e.g. invalid playlist name), return -1.
 */
int
MediaLibrary::getPlaylistSize(char* name) {
  int size = -1;
  char* curr_name = xmmsc_sqlite_prepare_string(name);
  string query = "SELECT count(entry) AS size "
                 "FROM Playlist, PlaylistEntries "
                 "WHERE id=playlist_id AND "
                 "      name=";
  query += curr_name;
  delete curr_name;

  lastRes = xmmsc_medialib_select(connection, query.c_str());
  xmmsc_result_wait(lastRes);

  if (xmmsc_result_iserror(lastRes)) {
    cerr << "Error: failed to get size of playlist '" << name 
         << "', server said:" << endl
         << xmmsc_result_get_error(lastRes) << endl;
  }
  else {
    xmmsc_result_get_dict_entry_int32(lastRes, "size", &size);
  }

  xmmsc_result_unref(lastRes);

  return size;
}


/**
 * Get the size of the current playlist.
 */
int
MediaLibrary::getCurrentPlaylistSize() {
  return getPlaylistSize(currentPlaylistName);
}


/**
 * Save the current server playlist in the medialib under the given
 * name.
 */
void
MediaLibrary::saveCurrentPlaylistAs(char* name) {
  // Invalid playlist name
  if(!validPlaylistName(name)) {
    cerr << "Error: invalid playlist name!" << endl;
    return;
  }

  lastRes = xmmsc_medialib_playlist_save_current(connection, name);
  xmmsc_result_wait(lastRes);
  if (xmmsc_result_iserror(lastRes)) {
    cerr << "Error: failed to save the playlist, server said:" << endl
         << xmmsc_result_get_error(lastRes) << endl;
  }

  xmmsc_result_unref(lastRes);
}


void
MediaLibrary::usePlaylist(char* name) {
  // Invalid playlist name
  if(!validPlaylistName(name)) {
    cerr << "Error: invalid playlist name!" << endl;
    return;
  }

  // Check that we can load that playlist
  if(!hasPlaylist(name)) {
    cerr << "Error: the playlist '" << name << "' does not exist!" << endl;
    return;
  }

  // Seems ok, let's clear and load the new playlist instead
  lastRes = xmmsc_playlist_clear(connection);
  xmmsc_result_wait(lastRes);
  if(xmmsc_result_iserror(lastRes)) {
    cerr << "Error: failed while clearing the playlist, server said:" << endl
         << xmmsc_result_get_error(lastRes) << endl;
  }
  else {
    lastRes = xmmsc_medialib_playlist_load(connection, name);
    xmmsc_result_wait(lastRes);
    if(xmmsc_result_iserror(lastRes)) {
      cerr << "Error: failed while changing the playlist, server said:" << endl
           << xmmsc_result_get_error(lastRes) << endl;
    }
    else {
      if(currentPlaylistName != NULL)
        delete currentPlaylistName;
      currentPlaylistName = new char[MAX_PLAYLIST_NAME_LEN + 1];
      strncpy(currentPlaylistName, name, MAX_PLAYLIST_NAME_LEN);
    }
  }

  xmmsc_result_unref(lastRes);
}


/**
 * Remove the given playlist from the medialib.
 */
void
MediaLibrary::removePlaylist(char* name) {
  // Invalid playlist name
  if(!validPlaylistName(name)) {
    cerr << "Error: invalid playlist name!" << endl;
    return;
  }

  // Forbid to remove current playlist!
  if(strcmp(name, currentPlaylistName) == 0) {
    cerr << "Error: you cannot remove the current playlist!" << endl;
    return;
  }

  lastRes = xmmsc_medialib_playlist_remove(connection, name);
  xmmsc_result_wait(lastRes);
  if (xmmsc_result_iserror(lastRes)) {
    cerr << "Error: failed to remove the playlist, server said:" << endl
         << xmmsc_result_get_error(lastRes) << endl;
  }

  xmmsc_result_unref(lastRes);
}


/**
 * Empty the current playlist.
 */
void
MediaLibrary::clearCurrentPlaylist() {
  lastRes = xmmsc_playlist_clear(connection);
  xmmsc_result_wait(lastRes);
  if (xmmsc_result_iserror(lastRes)) {
    cerr << "Error: failed to clear the playlist, server said:" << endl
         << xmmsc_result_get_error(lastRes) << endl;
  }

  xmmsc_result_unref(lastRes);
}

/**
 * Shuffle the current playlist.
 */
void
MediaLibrary::shuffleCurrentPlaylist() {
  lastRes = xmmsc_playlist_shuffle(connection);
  xmmsc_result_wait(lastRes);
  if (xmmsc_result_iserror(lastRes)) {
    cerr << "Error: failed to shuffle the playlist, server said:" << endl
         << xmmsc_result_get_error(lastRes) << endl;
  }

  xmmsc_result_unref(lastRes);
}


/**
 * Checks whether there is a playlist with the given name in the
 * medialib.
 */
bool
MediaLibrary::hasPlaylist(char* name) {
  char* entry_name;
  bool found = false;

  // FIXME: Or SELECT ... WHERE name=$name ?

  lastRes = xmmsc_medialib_select(connection,
                                  "SELECT name FROM Playlist");
  xmmsc_result_wait(lastRes);

  while(xmmsc_result_list_valid(lastRes)) {
    xmmsc_result_get_dict_entry_str(lastRes, "name", &entry_name);
    if(entry_name != NULL && strcmp(name, entry_name) == 0) {
      found = true;
      break;
    }

    xmmsc_result_list_next(lastRes);
  }

  /* FIXME: Whoops wait until #295 gets committed :-(
  lastRes = xmmsc_medialib_playlists_list(connection);
  xmmsc_result_wait(lastRes);

  while(xmmsc_result_list_valid(lastRes)) {
    char* entry_name;
    xmmsc_result_get_string(lastRes, &entry_name);
    cout << name << " vs " << entry_name << endl;
    if(strcmp(name, entry_name) == 0) {
      found = true;
      break;
    }
    xmmsc_result_list_next(lastRes);
  }

  */

  xmmsc_result_unref(lastRes);
  return found;
}


/**
 * Return an object containing the informations about the song with
 * the given id.  NULL is returned if an error occurs.
 */
AbstractResult*
MediaLibrary::getSongById(unsigned int id) {
  lastRes = xmmsc_medialib_get_info(connection, id);
  xmmsc_result_wait(lastRes);

  if(xmmsc_result_iserror(lastRes)) {
    return NULL;
  }

  return new RichResult(lastRes);
}


/**
 * Queries the medialib and return the resulting list of songs.
 */
AbstractResult*
MediaLibrary::searchSongs(PatternQuery* query) {
  return performQuery(query);  
}

/**
 * Enqueue all songs matching the given query to the current playlist.
 */
void
MediaLibrary::enqueueSongs(PatternQuery* query) {
  AbstractResult* songlist = performQuery(query);
  songlist->rewind();
  while(songlist->isValid()) {
    lastRes = xmmsc_playlist_add_id(connection, songlist->getId());
    xmmsc_result_wait(lastRes);
    if(xmmsc_result_iserror(lastRes)) {
      cerr << "Error: couldn't add song #" << songlist->getId()
           << ", server said: "
           << xmmsc_result_get_error(lastRes) << endl;
    }
    xmmsc_result_unref(lastRes);

    songlist->next();
  }
}

/**
 * Insert all songs matching the given query in the current playlist
 * at the given position.
 */
void
MediaLibrary::insertSongs(PatternQuery* query, unsigned int position) {
  AbstractResult* songlist = performQuery(query);
  songlist->rewind();
  while(songlist->isValid()) {
    lastRes = xmmsc_playlist_insert_id(connection, position, songlist->getId());
    xmmsc_result_wait(lastRes);
    if(xmmsc_result_iserror(lastRes)) {
      cerr << "Error: couldn't add song #" << songlist->getId()
           << " at position " << position << ", server said: "
           << xmmsc_result_get_error(lastRes) << endl;
    }
    xmmsc_result_unref(lastRes);

    songlist->next();
    ++position;
  }
}


/**
 * Remove the song at the given position from the playlist.
 */
void
MediaLibrary::removeSongAt(unsigned int position) {
  lastRes = xmmsc_playlist_remove(connection, position);
  xmmsc_result_wait(lastRes);
  if(xmmsc_result_iserror(lastRes)) {
    cerr << "Error: failed to remove song at position "
         << position << ", server said: "
         << xmmsc_result_get_error(lastRes) << endl;
  }
  xmmsc_result_unref(lastRes);
}


/**
 * Return the name of the active playlist on the server.
 */
const char*
MediaLibrary::getCurrentPlaylistName() {
  return currentPlaylistName;
}


AbstractResult*
MediaLibrary::performQuery(PatternQuery* query) {
  AbstractResult* songlist;
  char* sql = query->getSql();
  cout << "QUERY: " << query->getSql() << endl;
  lastRes = xmmsc_medialib_select(connection, sql);
  xmmsc_result_wait(lastRes);

  if(xmmsc_result_iserror(lastRes)) {
    return NULL;
  }

  songlist = new SelectResult(lastRes, connection);
  query->saveResults(songlist);
  return songlist;
}


bool
MediaLibrary::validPlaylistName(char* name) {
  // Playlists starting with '_' are hidden
  if(*name == '_')
    return false;

  return true;
}


void
MediaLibrary::catchPlaylistChanged(xmmsc_result_t *res) {
  if (xmmsc_result_iserror(res)) {
    cerr << "error: " << xmmsc_result_get_error (res) << endl;
  }

  // FIXME: Ignore if playlist loaded too ?
  // FIXME: not ALL clients should do it!
  if(currentPlaylistName != NULL) {
    saveCurrentPlaylistAs(currentPlaylistName);
  }

  cout << "PL changed, command type: " << endl;
  //  xmmsc_result_dict_foreach (res, dump_dict, NULL);
  xmmsc_result_unref(res);
}


void
MediaLibrary::catchPlaylistLoaded(xmmsc_result_t *res) {
  char* loadedName;
  if (xmmsc_result_iserror(res)) {
    cerr << "error: " << xmmsc_result_get_error (res) << endl;
  }

  // Copy name of the loaded playlist as the current playlist
  xmmsc_result_get_string(res, &loadedName);
  currentPlaylistName = new char[ strlen(loadedName) + 1 ];
  strcpy(currentPlaylistName, loadedName);

  // FIXME: Leak! delete previous name memory!

  xmmsc_result_unref(res);

  // DEBUG:
  cout << "Playlist loaded, new current playlist: "
       << currentPlaylistName << endl;
}
