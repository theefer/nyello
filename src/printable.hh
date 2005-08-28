#ifndef __PRINTABLE_HH__
#define __PRINTABLE_HH__

#include <xmmsclient/xmmsclient.h>

// FIXME: I'd rather not...
#define MAX_DICT_STRING_LEN 255


class Printable {
public:

  virtual unsigned int getId() = 0;
  virtual char* get(char* key) = 0;
  virtual bool  next() = 0;
  virtual bool  isSelected() = 0;
  virtual void  rewind() = 0;
  virtual bool  isValid() = 0;

  char* getDictEntryAsStr(xmmsc_result_t* result, char* key);

};


#endif  // __PRINTABLE_HH__
