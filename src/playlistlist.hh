#ifndef __PLAYLISTLIST_HH__
#define __PLAYLISTLIST_HH__

// We need strcmp
#include <iostream>

#include "dictlist.hh"


class PlaylistList : public DictList {
public:
  PlaylistList(xmmsc_result_t* res, char* playingPlaylist);

  bool isSelected();


private:
  /**
   * Name of the playlist currently being played.
   */
  char* playingPlaylist;

};


#endif  // __PLAYLISTLIST_HH__
