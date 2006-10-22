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


#ifndef __ABSTRACTRESULT_HH__
#define __ABSTRACTRESULT_HH__

#include <xmmsclient/xmmsclient.h>


class AbstractResult {
public:
  AbstractResult(xmmsc_result_t* res);
  ~AbstractResult();

  virtual unsigned int getId() = 0;
  virtual unsigned int getCounter() { return counter; }

  virtual char* get(char* key) = 0;
  virtual int getInt(char* key) = 0;

  virtual bool  next() = 0;
  virtual void  rewind();
  virtual bool  isValid();


protected:
  static const int MAX_DICT_STRING_LEN = 255;

  /**
   * Result referencing the data.
   */
  xmmsc_result_t* res;

  /**
   * Counter saving the index of the current entry (starting at 0).
   */
  unsigned int counter;


  char* getDictEntryAsStr(xmmsc_result_t* result, char* key);

};


#endif  // __ABSTRACTRESULT_HH__
