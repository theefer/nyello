#include "countercolumn.hh"

CounterColumn::CounterColumn(char* _heading, int _width)
  : Column(NULL, _heading, _width) { }

CounterColumn::CounterColumn(char* _heading, int _width, char* _prefix, char* _suffix)
  : Column(NULL, _heading, _width, _prefix, _suffix) { }

CounterColumn::CounterColumn(char* _heading, int _width, char* _prefix, char* _suffix, ios_base::fmtflags _alignment)
  : Column(NULL, _heading, _width, _prefix, _suffix, _alignment) { }

CounterColumn::~CounterColumn() { }

void
CounterColumn::reset() {
  counter = 0;
}

char*
CounterColumn::fetchValue(Printable* entry) {
  // FIXME: Leak? When is it freed?
  char* current = new char[width];
  snprintf(current, width, "%d", counter);
  ++counter;
  return current;
}
