#include "printable.hh"


char*
Printable::getDictEntryAsStr(xmmsc_result_t* result, char* key) {
  char* bufferStr = new char[MAX_DICT_STRING_LEN+1];
  int32_t bufferInt;
  uint32_t bufferUint;

  xmmsc_result_value_type_t type;
  type = xmmsc_result_get_dict_entry_type(result, key);
  switch(type) {
  case XMMS_OBJECT_CMD_ARG_UINT32:
    xmmsc_result_get_dict_entry_uint32(result, key, &bufferUint);
    sprintf(bufferStr, "%u", bufferUint);
    break;

  case XMMS_OBJECT_CMD_ARG_INT32:
    xmmsc_result_get_dict_entry_int32(result, key, &bufferInt);
    sprintf(bufferStr, "%d", bufferInt);
    break;

  case XMMS_OBJECT_CMD_ARG_STRING:
    // FIXME: leak? overflow?
    xmmsc_result_get_dict_entry_str(result, key, &bufferStr);
    break;

  case XMMS_OBJECT_CMD_ARG_NONE:
  default:
    *bufferStr = '\0';
    break;
  }

  return bufferStr;
}
