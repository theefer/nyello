#include "dictlist.hh"


DictList::DictList(xmmsc_result_t* _res) : res(_res) {
}

DictList::~DictList() {
  xmmsc_result_unref(res);
}

unsigned int
DictList::getId() {
  int32_t buffer;
  xmmsc_result_get_dict_entry_int32(res, "id", &buffer);
  currentId = buffer;
  return currentId;
}

char*
DictList::get(char* key) {
  return getDictEntryAsStr(res, key);
}

bool
DictList::next() {
  return xmmsc_result_list_next(res);
}

void
DictList::rewind() {
  xmmsc_result_list_first(res);
}

bool
DictList::isValid() {
  return xmmsc_result_list_valid(res);
}
