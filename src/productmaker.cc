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

