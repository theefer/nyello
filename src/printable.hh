#ifndef __PRINTABLE_HH__
#define __PRINTABLE_HH__

#include <xmmsclient/xmmsclient.h>


class Printable {
public:

  virtual unsigned int getId() = 0;
  virtual char* get(char* key) = 0;
  virtual int getInt(char* key) = 0;
  virtual bool  next() = 0;
  virtual bool  isSelected() = 0;
  virtual void  rewind() = 0;
  virtual bool  isValid() = 0;

};


#endif  // __PRINTABLE_HH__
