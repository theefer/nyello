#ifndef __MEDIALIBRARY_HH__
#define __MEDIALIBRARY_HH__

#define MAX_PLAYLIST_NAME_LEN 64

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



using namespace std;


class MediaLibrary {

public:

  MediaLibrary(xmmsc_connection_t* connection);

  AbstractResult* getCurrentPlaylist();
  AbstractResult* getPlaylist(char* name);
  AbstractResult* getPlaylists();

  int getPlaylistSize(char* name);
  int getCurrentPlaylistSize();

  void saveCurrentPlaylistAs(char* name);
  void usePlaylist(char* name);
  void removePlaylist(char* name);

  void clearCurrentPlaylist();
  void shuffleCurrentPlaylist();

  bool hasPlaylist(char* name);

  AbstractResult* getSongById(unsigned int id);
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

  AbstractResult* performQuery(PatternQuery* query);

  bool validPlaylistName(char* name);

};


#endif  // __MEDIALIBRARY_HH__
