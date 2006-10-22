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


#ifndef __COLUMN_HH__
#define __COLUMN_HH__

#include "printable.hh"
#include <iostream>

using namespace std;


class Column {
public:
  Column(char* key, char* heading, int width);
  Column(char* key, char* heading, int width, char* prefix, char* suffix);
  Column(char* key, char* heading, int width, char* prefix, char* suffix, ios_base::fmtflags alignment);
  ~Column();

  virtual void reset();
  virtual char* fetchValue(Printable* entry);

  inline char* getKey()     { return key; }
  inline char* getHeading() { return heading; }
  inline int   getWidth()   { return width; }
  inline char* getPrefix()  { return prefix; }
  inline char* getSuffix()  { return suffix; }
  inline ios_base::fmtflags getAlignment() { return alignment; }

  inline char  getFiller()  { return filler; }
  inline void  setFiller(char fill) { filler = fill; }

protected:
  char* key;
  char* heading;

  int width;

  char* prefix;
  char* suffix;

  ios_base::fmtflags alignment;
  char filler;

};


#endif  // __COLUMN_HH__
