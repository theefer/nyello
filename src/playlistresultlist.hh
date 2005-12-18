#ifndef __PLAYLISTRESULTLIST_HH__
#define __PLAYLISTRESULTLIST_HH__

#include <string>

#include <xmmsclient/xmmsclient.h>

#include "resultlist.hh"

using namespace std;


class PlaylistResultList : public ResultList {
public:

  PlaylistResultList(AbstractResult* result, string playingPlaylist);
  ~PlaylistResultList();

  virtual bool isSelected();

private:
  string playingPlaylist;

};


#endif  // __PLAYLISTRESULTLIST_HH__
