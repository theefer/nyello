#ifndef __COLUMN_HH__
#define __COLUMN_HH__

#include "printable.hh"
#include <iostream>

using namespace std;


class Column {
public:
  Column(char* key, char* heading, int width);
  Column(char* key, char* heading, int width, char* prefix, char* suffix);
  Column(char* key, char* heading, int width, char* prefix, char* suffix, ios_base::fmtflags alignment);
  ~Column();

  virtual void reset();
  virtual char* fetchValue(Printable* entry);

  inline char* getKey()     { return key; }
  inline char* getHeading() { return heading; }
  inline int   getWidth()   { return width; }
  inline char* getPrefix()  { return prefix; }
  inline char* getSuffix()  { return suffix; }
  inline ios_base::fmtflags getAlignment() { return alignment; }

  inline char  getFiller()  { return filler; }
  inline void  setFiller(char fill) { filler = fill; }

protected:
  char* key;
  char* heading;

  int width;

  char* prefix;
  char* suffix;

  ios_base::fmtflags alignment;
  char filler;

};


#endif  // __COLUMN_HH__
