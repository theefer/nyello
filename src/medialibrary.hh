#ifndef __MEDIALIBRARY_HH__
#define __MEDIALIBRARY_HH__

#define MAX_PLAYLIST_NAME_LEN 64

// FIXME: enable to show generated SQL queries
//#define MLIB_QUERY_DEBUG

#include <xmmsclient/xmmsclient.h>

#include <list>
#include <vector>
#include <iostream>
#include <string>

#include "abstractresult.hh"
#include "richresult.hh"
#include "songresult.hh"
#include "selectresult.hh"
#include "patternquery.hh"
#include "collection.hh"
#include "delayed.hh"



using namespace std;


class MediaLibrary {

public:

  MediaLibrary(xmmsc_connection_t* connection);

  Delayed<SongResult*>* getCurrentPlaylist();
  Delayed<SongResult*>* getPlaylist(const char* name);
  Delayed<RichResult*>* getPlaylists();

  Delayed<int>* getPlaylistSize(const char* name);
  Delayed<int>* getCurrentPlaylistSize();

  DelayedVoid* saveCurrentPlaylistAs(const char* name);
  DelayedVoid* usePlaylist(const char* name);
  DelayedVoid* removePlaylist(const char* name);

  DelayedVoid* clearCurrentPlaylist();
  DelayedVoid* shuffleCurrentPlaylist();

  Delayed<bool>* hasPlaylist(const char* name);

  Delayed<RichResult*>* getSongById(unsigned int id);
  AbstractResult* searchSongs(PatternQuery* query);

  void enqueueSongs(PatternQuery* query);
  void insertSongs(PatternQuery* query, unsigned int position);

  DelayedVoid* removeSongAt(unsigned int position);

  DelayedVoid* import(char* uri);

  Collection* newCollection();
  Collection* getCollection(char* name);
  Collection* getCollections();

  string getCurrentPlaylistName();

  // FIXME: HACK: should be private
  void catchPlaylistChanged(xmmsc_result_t *res);
  void catchPlaylistLoaded(xmmsc_result_t *res);

private:
  xmmsc_connection_t* connection;
  xmmsc_result_t*     lastRes;

  string currentPlaylistName;
  string newPlaylistName;

  AbstractResult* performQuery(PatternQuery* query);

  bool validPlaylistName(const char* name);

  DelayedVoid* loadNewPlaylist();
  DelayedVoid* loadPlaylist(const char* name);
  void updateCurrentPlaylistName();

};


#endif  // __MEDIALIBRARY_HH__
