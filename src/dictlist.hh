#ifndef __DICTLIST_HH__
#define __DICTLIST_HH__

#include <xmmsclient/xmmsclient.h>

#include <string.h>

#include "printable.hh"


// FIXME: I'd rather not...
#define MAX_DICT_STRING_LEN 255


class DictList : public Printable {
public:
  DictList(xmmsc_result_t* res);
  ~DictList();

  char* get(char* key);
  bool  next();
  virtual bool  isSelected() = 0;
  bool  isValid();


private:
  /**
   * Result referencing the list of ids of songs in the playlist.
   */
  xmmsc_result_t* res;

};


#endif  // __DICTLIST_HH__
