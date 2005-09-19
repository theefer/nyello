#include "richresult.hh"


RichResult::RichResult(xmmsc_result_t* _res) : AbstractResult(_res) {
}

RichResult::~RichResult() {
}

unsigned int
RichResult::getId() {
  unsigned int currentId;
  xmmsc_result_get_dict_entry_uint32(res, "id", &currentId);
  return currentId;
}

char*
RichResult::get(char* key) {
  return getDictEntryAsStr(res, key);
}

bool
RichResult::next() {
  ++counter;
  return xmmsc_result_list_next(res);
}
