#include "richresult.hh"


RichResult::RichResult(xmmsc_result_t* _res) : AbstractResult(_res) {
}

RichResult::~RichResult() {
}

unsigned int
RichResult::getId() {
  return getInt("id");
}

char*
RichResult::get(char* key) {
  return getDictEntryAsStr(res, key);
}

int
RichResult::getInt(char* key) {
  int32_t bufferInt;
  xmmsc_result_get_dict_entry_int32(res, key, &bufferInt);
  return bufferInt;
}

bool
RichResult::next() {
  ++counter;
  return xmmsc_result_list_next(res);
}
