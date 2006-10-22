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


#ifndef __COLUMNISER_HH__
#define __COLUMNISER_HH__


#include <iostream>
#include <vector>

#include "column.hh"
#include "printable.hh"


using namespace std;


typedef vector<Column*> ColumnList;


class Columniser {
public:
  Columniser();
  ~Columniser();

  void reset();

  void printHeader();
  void printHeader(char* message);
  void printFooter();
  void printFooter(char* message);

  void printHeadings();
  void printRow(Printable* entry);

  inline void setFrameChar(char fchar) { frameChar = fchar; }
  inline void setSelectedPrefix(char* prefix) { selectedPrefix = prefix; }
  void setUnselectedPrefix(char* prefix);

  void addColumn(Column* col);


private:
  char  frameChar;
  char* selectedPrefix;
  char* unselectedPrefix;

  vector<Column*> columns;
  int width;

};

#endif  // __COLUMNISER_HH__
