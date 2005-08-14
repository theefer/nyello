#ifndef __COUNTERCOLUMN_HH__
#define __COUNTERCOLUMN_HH__

#include "column.hh"


class CounterColumn : public Column {
public:
  CounterColumn(char* heading, int width);
  CounterColumn(char* heading, int width, char* prefix, char* suffix);
  CounterColumn(char* heading, int width, char* prefix, char* suffix, ios_base::fmtflags alignment);
  ~CounterColumn();

  void reset();

  char* fetchValue(Printable* entry);

private:
  int counter;

};


#endif  // __COUNTERCOLUMN_HH__
