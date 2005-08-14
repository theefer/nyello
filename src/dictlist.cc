#include "dictlist.hh"


DictList::DictList(xmmsc_result_t* _res) : res(_res) {
}

DictList::~DictList() {
  xmmsc_result_unref(res);
}

char*
DictList::get(char* key) {
  return getDictEntryAsStr(res, key);
}

bool
DictList::next() {
  return xmmsc_result_list_next(res);
}

bool
DictList::isValid() {
  return xmmsc_result_list_valid(res);
}
