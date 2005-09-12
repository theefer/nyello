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

  // FIXME: Find current playlist name or start with the autosaved playlist
  //        If no matching playlist, create a new one ("current-$N") and use it?

  // FIXME: Let's not mess with the PL on startup for now
  //  usePlaylist("autosaved");

  currentPlaylistName = "autosaved";

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


PlaylistSongList*
MediaLibrary::getCurrentPlaylist() {
  unsigned int curPos;

  // Retrieve position of current playing song
  lastRes = xmmsc_playlist_current_pos(connection);
  xmmsc_result_wait(lastRes);
  xmmsc_result_get_uint(lastRes, &curPos);
  xmmsc_result_unref(lastRes);

  // Get list of songs in the current playlist
  lastRes = xmmsc_playlist_list(connection);
  xmmsc_result_wait(lastRes);

  if(xmmsc_result_iserror(lastRes)) {
    return NULL;
  }

  return new PlaylistSongList(lastRes, connection, curPos);
}


/**
 * Get the Playlist object of the playlist with the given name in
 * the medialib.
 */
PlaylistSongList*
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

  return new PlaylistSongList(lastRes, connection);

  // OLD: "SELECT songs.*, substr(playlistentries.entry,8,10) AS id FROM playlistentries, playlist LEFT JOIN songs ON songs.id=substr(playlistentries.entry,8,10) WHERE playlist_id=playlist.id AND playlist.name=\"%s\" ORDER BY pos",
}


/**
 * Get the list of all the Playlist objects in the medialib.
 * Playlists starting with a '_' prefix are hidden.
 */
PlaylistList*
MediaLibrary::getPlaylists() {
  PlaylistList* plist;

  // Retrieve all the playlists
  lastRes = xmmsc_medialib_select(connection, 
                                  "SELECT name, MAX(cnt) AS size "
                                  "FROM Playlist, "
                                  "     (  SELECT id, 0 "
                                  "        FROM playlist "
                                  "      UNION "
                                  "        SELECT playlist_id, "
                                  "               count(entry) AS cnt "
                                  "        FROM playlistentries) "
                                  "WHERE id=playlist_id AND"
                                  "      substr(name, 0, 1) <> \"_\""
                                  "GROUP BY playlist_id");
  xmmsc_result_wait(lastRes);

  // Oops, no playlist, return empty list!
  if(!xmmsc_result_list_valid(lastRes)) {
    return NULL;
  }

  return new PlaylistList(lastRes, currentPlaylistName);
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
      currentPlaylistName = name;
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

  // FIXME: Forbid to remove current playlist!

  lastRes = xmmsc_medialib_playlist_remove(connection, name);
  xmmsc_result_wait(lastRes);
  if (xmmsc_result_iserror(lastRes)) {
    cerr << "Error: failed to remove the playlist, server said:" << endl
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
 * Queries the medialib and return the resulting list of songs.
 */
QuerySongList*
MediaLibrary::searchSongs(PatternQuery* query) {
  return performQuery(query);  
}

/**
 * Enqueue all songs matching the given query to the current playlist.
 */
void
MediaLibrary::enqueueSongs(PatternQuery* query) {
  QuerySongList* songlist = performQuery(query);
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
  QuerySongList* songlist = performQuery(query);
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
  xmmsc_playlist_remove(connection, position);
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


QuerySongList*
MediaLibrary::performQuery(PatternQuery* query) {
  QuerySongList* songlist;
  char* sql = query->getSql();
  cout << "QUERY: " << query->getSql() << endl;
  lastRes = xmmsc_medialib_select(connection, sql);
  xmmsc_result_wait(lastRes);

  if(xmmsc_result_iserror(lastRes)) {
    return NULL;
  }

  songlist = new QuerySongList(lastRes, connection);
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
