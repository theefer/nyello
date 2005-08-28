#ifndef __DICTLIST_HH__
#define __DICTLIST_HH__

#include <xmmsclient/xmmsclient.h>

#include "printable.hh"


// FIXME: I'd rather not...
#define MAX_DICT_STRING_LEN 255


class DictList : public Printable {
public:
  DictList(xmmsc_result_t* res);
  ~DictList();

  virtual unsigned int getId();
  virtual char* get(char* key);
  virtual bool  next();
  virtual bool isSelected() = 0;
  virtual void  rewind();
  virtual bool  isValid();

protected:
  /**
   * Result referencing the list of ids of songs in the playlist.
   */
  xmmsc_result_t* res;

  /**
   * Id of the current song.
   */
  unsigned int currentId;

};


#endif  // __DICTLIST_HH__
