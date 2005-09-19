#include "selectresult.hh"


SelectResult::SelectResult(xmmsc_result_t* _res, xmmsc_connection_t* _conn)
  : SongResult(_res, _conn) {
}

SelectResult::~SelectResult() {
}

unsigned int
SelectResult::getId() {
  int currentId;
  xmmsc_result_get_dict_entry_int32(res, "id", &currentId);
  return currentId;
}
