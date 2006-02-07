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
  currentPlaylistName = "autosaved";
  newPlaylistName = "";

  saveCurrentPlaylistAs("autosaved");
  usePlaylist("autosaved");

  // FIXME: Find current playlist name or start with the autosaved playlist
  //        If no matching playlist, create a new one ("current-$N") and use it?


  // Setup signal hooks
  lastRes = xmmsc_broadcast_playlist_changed(connection);
  xmmsc_result_notifier_set(lastRes, runMediaLibraryMethod<&MediaLibrary::catchPlaylistChanged>, this);
  xmmsc_result_unref(lastRes);

  lastRes = xmmsc_broadcast_playback_current_id(connection);
  xmmsc_result_notifier_set(lastRes, runMediaLibraryMethod<&MediaLibrary::catchPlaylistPositionChanged>, this);
  xmmsc_result_unref(lastRes);

  /* FIXME: Causes problems because of the way other clients load playlists
  lastRes = xmmsc_broadcast_medialib_playlist_loaded(connection);
  xmmsc_result_notifier_set(lastRes, runMediaLibraryMethod<&MediaLibrary::catchPlaylistLoaded>, this);
  xmmsc_result_unref(lastRes);
  */
}


Delayed<SongResult*>*
MediaLibrary::getCurrentPlaylist() {
  lastRes = xmmsc_playlist_list(connection);
  return new Delayed<SongResult*>(lastRes,
                                  new ComplexObjectProduct<SongResult, xmmsc_connection_t*>(connection));
}


/**
 * Get the Playlist object of the playlist with the given name in
 * the medialib.
 */
Delayed<SongResult*>*
MediaLibrary::getPlaylist(const char* name) {
  // Invalid playlist name
  if(!validPlaylistName(name)) {
    cerr << "Error: invalid playlist name!" << endl;
    return NULL;
  }

  // Use the other method for current playlist (shows position)
  if(currentPlaylistName == name) {
    return getCurrentPlaylist();
  }

  // Get list of songs of the given playlist
  lastRes = xmmsc_medialib_playlist_list(connection, name);
  return new Delayed<SongResult*>(lastRes,
                                  new ComplexObjectProduct<SongResult, xmmsc_connection_t*>(connection));
}


/**
 * Get the list of all the Playlist objects in the medialib.
 * Playlists starting with a '_' prefix are hidden.
 */
Delayed<RichResult*>*
MediaLibrary::getPlaylists() {
  // Retrieve all the playlists
  lastRes = xmmsc_medialib_select(connection, 
                                  "SELECT name, COUNT(entry) AS size "
                                  "FROM Playlist "
                                  "LEFT JOIN PlaylistEntries ON id=playlist_id "
                                  "WHERE substr(name, 0, 1) <> \"_\" "
                                  "GROUP BY id "
                                  "ORDER BY name");

  return new Delayed<RichResult*>(lastRes,
                                  new ObjectProduct<RichResult>(),
                                  "Could not list playlists: ");
}


/**
 * Get the size of the given playlist.
 * If an error occured (e.g. invalid playlist name), return -1.
 */
Delayed<int>*
MediaLibrary::getPlaylistSize(const char* name) {
  int size = -1;
  char* curr_name = xmmsc_sqlite_prepare_string(name);
  string query = "SELECT count(entry) AS size "
                 "FROM Playlist, PlaylistEntries "
                 "WHERE id=playlist_id AND "
                 "      name=";
  query += curr_name;
  delete curr_name;

  string errmsg = "Error: failed to get size of playlist '";
  errmsg += name;
  errmsg += "', server said: ";

  lastRes = xmmsc_medialib_select(connection, query.c_str());
  return new Delayed<int>(lastRes,
                          new PrimitiveDictProduct<int, &xmmsc_result_get_dict_entry_int32>("size"),
                          errmsg);
}


/**
 * Get the size of the current playlist.
 */
Delayed<int>*
MediaLibrary::getCurrentPlaylistSize() {
  return getPlaylistSize(currentPlaylistName.c_str());
}


/**
 * Save the current server playlist in the medialib under the given
 * name (unless we are loading a playlist).
 */
Delayed<void>*
MediaLibrary::saveCurrentPlaylistAs(const char* name) {
  // Invalid playlist name
  if(!validPlaylistName(name)) {
    cerr << "Error: invalid playlist name!" << endl;
    return NULL;
  }

  if(newPlaylistName.size() > 0) {
    return NULL;
  }

  lastRes = xmmsc_medialib_playlist_save_current(connection, name);
  return new Delayed<void>(lastRes,
                         "Error: failed to save the playlist, server said: ");
}


Delayed<void>*
MediaLibrary::usePlaylist(const char* name) {
  // Invalid playlist name
  if(!validPlaylistName(name)) {
    cerr << "Error: invalid playlist name!" << endl;
    return NULL;
  }

  // Check that we can load that playlist
  if(!Delayed<bool>::readAndFree(hasPlaylist(name))) {
    cerr << "Error: the playlist '" << name << "' does not exist!" << endl;
    return NULL;
  }

  // Seems ok, let's clear and load the new playlist instead
  newPlaylistName = name;
  Delayed<void>* del = clearCurrentPlaylist();
  del->addCallback<MediaLibrary>(this, &MediaLibrary::loadNewPlaylist);
  return del;
}


Delayed<void>*
MediaLibrary::loadNewPlaylist() {
  return loadPlaylist(newPlaylistName.c_str());
}

Delayed<void>*
MediaLibrary::loadPlaylist(const char* name) {
  lastRes = xmmsc_medialib_playlist_load(connection, name);
  Delayed<void>* del = new Delayed<void>(lastRes,
                                         "Error: failed while changing the playlist, server said: ");
  del->addCallback<MediaLibrary>(this, &MediaLibrary::updateCurrentPlaylistName);
  return del;
}

void
MediaLibrary::updateCurrentPlaylistName() {
  currentPlaylistName = newPlaylistName;
  newPlaylistName = "";
}


/**
 * Remove the given playlist from the medialib.
 */
Delayed<void>*
MediaLibrary::removePlaylist(const char* name) {
  // Invalid playlist name
  if(!validPlaylistName(name)) {
    cerr << "Error: invalid playlist name!" << endl;
    return NULL;
  }

  // Forbid to remove current playlist!
  if(currentPlaylistName == name) {
    cerr << "Error: you cannot remove the current playlist!" << endl;
    return NULL;
  }

  lastRes = xmmsc_medialib_playlist_remove(connection, name);
  return new Delayed<void>(lastRes,
                         "Error: failed to remove the playlist, server said: ");
}


/**
 * Empty the current playlist.
 */
Delayed<void>*
MediaLibrary::clearCurrentPlaylist() {
  lastRes = xmmsc_playlist_clear(connection);
  return new Delayed<void>(lastRes,
                         "Error: failed to clear the playlist, server said: ");
}

/**
 * Shuffle the current playlist.
 */
Delayed<void>*
MediaLibrary::shuffleCurrentPlaylist() {
  lastRes = xmmsc_playlist_shuffle(connection);
  return new Delayed<void>(lastRes,
                         "Error: failed to shuffle the playlist, server said: ");
}


/**
 * Checks whether there is a playlist with the given name in the
 * medialib.
 */
Delayed<bool>*
MediaLibrary::hasPlaylist(const char* name) {
  // Get list of playlists and compare names
  lastRes = xmmsc_medialib_playlists_list(connection);
  return new Delayed<bool>(lastRes,
                           new StringMatcherProduct(name));
}


/**
 * Return an object containing the informations about the song with
 * the given id.
 */
Delayed<RichResult*>*
MediaLibrary::getSongById(unsigned int id) {
  lastRes = xmmsc_medialib_get_info(connection, id);
  return new Delayed<RichResult*>(lastRes,
                                  new ObjectProduct<RichResult>());
}


/**
 * Enqueue all songs in the list to the current playlist.
 */
void
MediaLibrary::enqueueSongs(AbstractResult* songlist) {
  Delayed<void>* res;

  songlist->rewind();
  while(songlist->isValid()) {
    stringstream errmsg;
    errmsg << "Error: couldn't add song #" << songlist->getId()
           << ", server said: ";

    // FIXME: We should not wait in MediaLibrary!
    lastRes = xmmsc_playlist_add_id(connection, songlist->getId());
    res = new Delayed<void>(lastRes, errmsg.str());
    res->wait();
    delete res;

    songlist->next();
  }
}

/**
 * Insert all songs in the list in the current playlist at the given
 * position.
 */
void
MediaLibrary::insertSongs(AbstractResult* songlist, unsigned int position) {
  Delayed<void>* res;

  songlist->rewind();
  while(songlist->isValid()) {
    stringstream errmsg;
    errmsg << "Error: couldn't add song #" << songlist->getId()
           << " at position " << position << ", server said: ";

    // FIXME: We should not wait in MediaLibrary!
    lastRes = xmmsc_playlist_insert_id(connection, position, songlist->getId());
    res = new Delayed<void>(lastRes, errmsg.str());
    res->wait();
    delete res;


    songlist->next();
    ++position;
  }
}


/**
 * Remove the song at the given position from the playlist.
 */
Delayed<void>*
MediaLibrary::removeSongAt(unsigned int position) {
  stringstream errmsg;
  errmsg << "Error: failed to remove song at position " << position
         << ", server said: ";

  lastRes = xmmsc_playlist_remove(connection, position);
  return new Delayed<void>(lastRes, errmsg.str());
}


/**
 * Import files into the medialib.  The argument can be an HTTP URL, a
 * file pattern (parsed by glob) or a directory, in which case it it
 * recursively imported.
 */
Delayed<void>*
MediaLibrary::import(char* uri) {
  Delayed<void>* res = NULL;
  string errmsg;
  Uri location(uri);

  // FIXME: Do we want a more verbose output?

  // Skip invalid location
  if(!location.exists()) {
    cout << "Error: location '" << uri << "' does not exist!" << endl;    
  }

  // Import directory
  else if(location.isDirectory()) {
    /*
    cout << "Importing directory '" << location.getPath() << "'" << endl;
    errmsg = "Error: cannot import directory '";
    errmsg += uri;
    errmsg += "' into the medialib, server said: ";

    res = new Delayed<void>(lastRes, errmsg);
    */

    lastRes = xmmsc_medialib_path_import(connection, location.getPath());
    res = new Delayed<void>(lastRes);
  }

  // Add file entry
  else {
    /*
    cout << "Importing file '" << location.getPath() << "'" << endl;
    errmsg = "Error: cannot import file '";
    errmsg += uri;
    errmsg += "' into the medialib, server said: ";

    res = new Delayed<void>(lastRes, errmsg);
    */

    lastRes = xmmsc_medialib_add_entry(connection, location.getPath());
    res = new Delayed<void>(lastRes);
  }

  return res;
}


/**
 * Return the name of the active playlist on the server.
 */
string
MediaLibrary::getCurrentPlaylistName() {
  return currentPlaylistName;
}


/**
 * Queries the medialib and return the resulting list of songs.
 */
Delayed<SelectResult*>*
MediaLibrary::searchSongs(PatternQuery* query) {
  Delayed<SelectResult*>* del;
  string sql(query->getSql());

// FIXME: Remove this when queries are stable
#ifdef MLIB_QUERY_DEBUG
  cout << "QUERY: " << query->getSql() << endl;
#endif

  lastRes = xmmsc_medialib_select(connection, sql.c_str());
  del = new Delayed<SelectResult*>(lastRes,
                                   new ComplexObjectProduct<SelectResult, xmmsc_connection_t*>(connection));
  del->addCallback<PatternQuery>(query, &PatternQuery::saveResults);
  return del;
}


bool
MediaLibrary::validPlaylistName(const char* name) {
  // Playlists starting with '_' are hidden
  if(*name == '_')
    return false;

  return true;
}


/**
 * Save the active playlist to the medialibrary when it changes.
 */
void
MediaLibrary::catchPlaylistChanged(xmmsc_result_t *res) {
  if (xmmsc_result_iserror(res)) {
    cerr << "Error on playlist change: " << xmmsc_result_get_error(res) << endl;
  }

  saveCurrentPlaylistAs(currentPlaylistName.c_str());
}

/**
 * Save the active playlist to the medialibrary when we change songs,
 * so the pointer in the Playlist table is up-to-date.
 */
void
MediaLibrary::catchPlaylistPositionChanged(xmmsc_result_t *res) {
  if (xmmsc_result_iserror(res)) {
    cerr << "Error on playlist position change: " << xmmsc_result_get_error(res) << endl;
  }

  saveCurrentPlaylistAs(currentPlaylistName.c_str());
}

void
MediaLibrary::catchPlaylistLoaded(xmmsc_result_t *res) {
  char* loadedName;
  if (xmmsc_result_iserror(res)) {
    cerr << "error: " << xmmsc_result_get_error (res) << endl;
  }

  // Copy name of the loaded playlist as the current playlist
  xmmsc_result_get_string(res, &loadedName);
  currentPlaylistName = loadedName;

  // DEBUG:
  cout << "DEBUG: Playlist loaded, new current playlist: "
       << currentPlaylistName << endl;
}
