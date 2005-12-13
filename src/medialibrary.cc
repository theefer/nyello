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
  newPlaylistName = NULL;


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
                                  "GROUP BY playlist_id");

  return new Delayed<RichResult*>(lastRes,
                                  new ObjectProduct<RichResult>(),
                                  "Could not list playlists: ");
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
DelayedVoid*
MediaLibrary::saveCurrentPlaylistAs(char* name) {
  // Invalid playlist name
  if(!validPlaylistName(name)) {
    cerr << "Error: invalid playlist name!" << endl;
    return NULL;
  }

  lastRes = xmmsc_medialib_playlist_save_current(connection, name);
  return new DelayedVoid(lastRes,
                         "Error: failed to save the playlist, server said:");
}


DelayedVoid*
MediaLibrary::usePlaylist(char* name) {
  // Invalid playlist name
  if(!validPlaylistName(name)) {
    cerr << "Error: invalid playlist name!" << endl;
    return NULL;
  }

  // Check that we can load that playlist
  if(!hasPlaylist(name)->getProduct()) {
    cerr << "Error: the playlist '" << name << "' does not exist!" << endl;
    return NULL;
  }

  // Seems ok, let's clear and load the new playlist instead
  newPlaylistName = name;
  DelayedVoid* del = clearCurrentPlaylist();
  del->addCallback<MediaLibrary>(this, &MediaLibrary::loadNewPlaylist);
  return del;
}


DelayedVoid*
MediaLibrary::loadNewPlaylist() {
  return loadPlaylist(newPlaylistName);
}

DelayedVoid*
MediaLibrary::loadPlaylist(char* name) {
  lastRes = xmmsc_medialib_playlist_load(connection, name);
  DelayedVoid* del = new DelayedVoid(lastRes,
                                     "Error: failed while changing the playlist, server said: ");
  del->addCallback<MediaLibrary>(this, &MediaLibrary::updateCurrentPlaylistName);
  return del;
}

void
MediaLibrary::updateCurrentPlaylistName() {
  if(currentPlaylistName != NULL)
    delete currentPlaylistName;
  currentPlaylistName = new char[MAX_PLAYLIST_NAME_LEN + 1];
  strncpy(currentPlaylistName, newPlaylistName, MAX_PLAYLIST_NAME_LEN);
  newPlaylistName = NULL;
}


/**
 * Remove the given playlist from the medialib.
 */
DelayedVoid*
MediaLibrary::removePlaylist(char* name) {
  // Invalid playlist name
  if(!validPlaylistName(name)) {
    cerr << "Error: invalid playlist name!" << endl;
    return NULL;
  }

  // Forbid to remove current playlist!
  if(strcmp(name, currentPlaylistName) == 0) {
    cerr << "Error: you cannot remove the current playlist!" << endl;
    return NULL;
  }

  lastRes = xmmsc_medialib_playlist_remove(connection, name);
  return new DelayedVoid(lastRes,
                         "Error: failed to remove the playlist, server said:");
}


/**
 * Empty the current playlist.
 */
DelayedVoid*
MediaLibrary::clearCurrentPlaylist() {
  lastRes = xmmsc_playlist_clear(connection);
  return new DelayedVoid(lastRes,
                         "Error: failed to clear the playlist, server said:");
}

/**
 * Shuffle the current playlist.
 */
DelayedVoid*
MediaLibrary::shuffleCurrentPlaylist() {
  lastRes = xmmsc_playlist_shuffle(connection);
  return new DelayedVoid(lastRes,
                         "Error: failed to shuffle the playlist, server said:");
}


/**
 * Checks whether there is a playlist with the given name in the
 * medialib.
 */
Delayed<bool>*
MediaLibrary::hasPlaylist(char* name) {
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

// FIXME: Remove this when queries are stable
#ifdef MLIB_QUERY_DEBUG
  cout << "QUERY: " << query->getSql() << endl;
#endif

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
