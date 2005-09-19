#ifndef __SONGLIST_HH__
#define __SONGLIST_HH__

#include <xmmsclient/xmmsclient.h>

#include "printable.hh"


class SongList : public Printable {
public:
  PlaylistSongList(xmmsc_result_t* res, xmmsc_connection_t* conn);
  ~PlaylistSongList();

  unsigned int getId();
  char* get(char* key);
  bool  next();
  bool  isSelected();
  void  rewind();
  bool  isValid();


private:
  /**
   * Pointer to the data result object.
   */
  SongResult* songres;

};


#endif  // __SONGLIST_HH__
