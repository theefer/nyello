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


#include "column.hh"


Column::Column(char* _key, char* _heading, int _width)
  : key(_key), heading(_heading), width(_width), prefix(""), suffix(""), alignment(ios_base::left), filler(' ') { }

Column::Column(char* _key, char* _heading, int _width, char* _prefix, char* _suffix)
  : key(_key), heading(_heading), width(_width), prefix(_prefix), suffix(_suffix), alignment(ios_base::left), filler(' ') { }

Column::Column(char* _key, char* _heading, int _width, char* _prefix, char* _suffix, ios_base::fmtflags _alignment)
  : key(_key), heading(_heading), width(_width), prefix(_prefix), suffix(_suffix), alignment(_alignment), filler(' ') { }

Column::~Column() { }

void
Column::reset() { }

char*
Column::fetchValue(Printable* entry) {
  return entry->get(key);
}
