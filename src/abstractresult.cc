/* nyello - an advanced command-line client for XMMS2
 * Copyright (C) 2006  Sébastien Cevey
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */


#include "abstractresult.hh"


AbstractResult::AbstractResult(xmmsc_result_t* _res) : res(_res) {
  counter = 0;
}

AbstractResult::~AbstractResult() {
  xmmsc_result_unref(res);
}

void
AbstractResult::rewind() {
  xmmsc_result_list_first(res);
  counter = 0;
}

bool
AbstractResult::isValid() {
  return xmmsc_result_list_valid(res);
}

char*
AbstractResult::getDictEntryAsStr(xmmsc_result_t* result, char* key) {
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
    // FIXME: overflow?
    xmmsc_result_get_dict_entry_str(result, key, &bufferStr);
    break;

  case XMMS_OBJECT_CMD_ARG_NONE:
  default:
    *bufferStr = '\0';
    break;
  }

  return bufferStr;
}
