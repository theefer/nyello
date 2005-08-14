#include "column.hh"


Column::Column(char* _key, char* _heading, int _width)
  : key(_key), heading(_heading), width(_width), prefix(""), suffix(""), alignment(ios_base::left), filler(' ') { }

Column::Column(char* _key, char* _heading, int _width, char* _prefix, char* _suffix)
  : key(_key), heading(_heading), width(_width), prefix(_prefix), suffix(_suffix), alignment(ios_base::left), filler(' ') { }

Column::Column(char* _key, char* _heading, int _width, char* _prefix, char* _suffix, ios_base::fmtflags _alignment)
  : key(_key), heading(_heading), width(_width), prefix(_prefix), suffix(_suffix), alignment(_alignment), filler(' ') { }

Column::~Column() { }

void
Column::reset() { }

char*
Column::fetchValue(Printable* entry) {
  return entry->get(key);
}
