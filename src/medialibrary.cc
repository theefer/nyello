#include "medialibrary.hh"

// FIXME: Temp for debug only!
#include <iostream>
using namespace std;


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
  usePlaylist("autosaved");

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
 * FIXME: Hide playlists starting with a '_' prefix.
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
                                  "WHERE id=playlist_id "
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
MediaLibrary::saveCurrentPlaylistAs(const char* name) {
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
MediaLibrary::removePlaylist(const char* name) {
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


/**
 * Return the name of the active playlist on the server.
 */
const char*
MediaLibrary::getCurrentPlaylistName() {
  return currentPlaylistName;
}


/**
 * Builds a vector of medialib ids from a result set.
 */
vector<int>*
MediaLibrary::getEntryVector(xmmsc_result_t* res) {
  vector<int>* entries = new vector<int>();
  char* entry;

  while(xmmsc_result_list_valid(res)) {
    // Get the next playlist entry, error if NULL
    xmmsc_result_get_dict_entry_str(res, "entry", &entry);
    if(!entry) {
      return NULL;
    }

    // FIXME: Only keep medialib entries?
    if(strncasecmp(entry, "mlib://", 7) == 0) {
      entries->push_back(atoi(entry + 7));
    }

    xmmsc_result_list_next (res);
  }

  return entries;
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

