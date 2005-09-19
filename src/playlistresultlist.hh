#ifndef __PLAYLISTRESULTLIST_HH__
#define __PLAYLISTRESULTLIST_HH__

#include <cstring>

#include <xmmsclient/xmmsclient.h>

#include "resultlist.hh"


class PlaylistResultList : public ResultList {
public:

  PlaylistResultList(AbstractResult* result, const char* playingPlaylist);
  ~PlaylistResultList();

  virtual bool isSelected();

private:
  const char* playingPlaylist;

};


#endif  // __PLAYLISTRESULTLIST_HH__
