#include "productmaker.hh"

StringMatcherProduct::StringMatcherProduct(string _str) : str(_str) {
}

bool
StringMatcherProduct::create() {
  char* entry_name;
  bool found = false;

  while(xmmsc_result_list_valid(res) && !found) {
    xmmsc_result_get_string(res, &entry_name);
    if(str == entry_name) {
      found = true;
    }
    xmmsc_result_list_next(res);
  }

  return found;
}

