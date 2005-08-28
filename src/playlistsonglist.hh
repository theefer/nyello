#ifndef __PLAYLISTSONGLIST_HH__
#define __PLAYLISTSONGLIST_HH__

#include <xmmsclient/xmmsclient.h>

#include "printable.hh"


class PlaylistSongList : public Printable {
public:
  PlaylistSongList(xmmsc_result_t* res, xmmsc_connection_t* conn, int playingPos = -1);
  ~PlaylistSongList();

  unsigned int getId();
  char* get(char* key);
  bool  next();
  inline bool  isSelected() { return (currentPos == playingPos); }
  void  rewind();
  bool  isValid();


private:
  /**
   * Result referencing the list of ids of songs in the playlist.
   */
  xmmsc_result_t* res;

  /**
   * The connection to the server.
   */
  xmmsc_connection_t* conn;

  /**
   * Result referencing the dict storing the metadata of the current
   * song.
   */
  xmmsc_result_t* cache;

  /**
   * Id of the current song.
   */
  unsigned int currentId;

  /**
   * Position of the entry currently referenced by the object.
   */
  unsigned int currentPos;

  /**
   * Position of the song currently being played.
   * Set to -1 to disable the selection.
   */
  int playingPos;

};


#endif  // __PLAYLISTSONGLIST_HH__
