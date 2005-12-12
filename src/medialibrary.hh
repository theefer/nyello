#ifndef __MEDIALIBRARY_HH__
#define __MEDIALIBRARY_HH__

#define MAX_PLAYLIST_NAME_LEN 64

// FIXME: enable to show generated SQL queries
//#define MLIB_QUERY_DEBUG

#include <xmmsclient/xmmsclient.h>

#include <list>
#include <vector>
#include <iostream>

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
  Delayed<SongResult*>* getPlaylist(char* name);
  Delayed<RichResult*>* getPlaylists();

  int getPlaylistSize(char* name);
  int getCurrentPlaylistSize();

  DelayedVoid* saveCurrentPlaylistAs(char* name);
  DelayedVoid* usePlaylist(char* name);
  DelayedVoid* removePlaylist(char* name);

  DelayedVoid* clearCurrentPlaylist();
  DelayedVoid* shuffleCurrentPlaylist();

  bool hasPlaylist(char* name);

  Delayed<RichResult*>* getSongById(unsigned int id);
  AbstractResult* searchSongs(PatternQuery* query);

  void enqueueSongs(PatternQuery* query);
  void insertSongs(PatternQuery* query, unsigned int position);

  void removeSongAt(unsigned int position);

  Collection* newCollection();
  Collection* getCollection(char* name);
  Collection* getCollections();

  const char* getCurrentPlaylistName();

  // FIXME: HACK: should be private
  void catchPlaylistChanged(xmmsc_result_t *res);
  void catchPlaylistLoaded(xmmsc_result_t *res);

private:
  xmmsc_connection_t* connection;
  xmmsc_result_t*     lastRes;

  char* currentPlaylistName;
  char* newPlaylistName;

  AbstractResult* performQuery(PatternQuery* query);

  bool validPlaylistName(char* name);

  DelayedVoid* loadNewPlaylist();
  DelayedVoid* loadPlaylist(char* name);
  void updateCurrentPlaylistName();

};


#endif  // __MEDIALIBRARY_HH__
