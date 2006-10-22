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


#ifndef __RICHRESULT_HH__
#define __RICHRESULT_HH__

#include <xmmsclient/xmmsclient.h>

#include "abstractresult.hh"


class RichResult : public AbstractResult {
public:
  RichResult(xmmsc_result_t* res);
  ~RichResult();

  virtual unsigned int getId();

  virtual char* get(char* key);
  virtual int getInt(char* key);

  virtual bool  next();

};


#endif  // __RICHRESULT_HH__
