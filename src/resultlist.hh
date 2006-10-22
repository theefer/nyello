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


#ifndef __RESULTLIST_HH__
#define __RESULTLIST_HH__

#include <xmmsclient/xmmsclient.h>

#include "printable.hh"
#include "abstractresult.hh"


class ResultList : public Printable {
public:

  ResultList(AbstractResult* result);
  ~ResultList();

  virtual unsigned int getId()  { return result->getId(); }
  virtual char* get(char* key)  { return result->get(key); }
  virtual int getInt(char* key) { return result->getInt(key); }
  virtual bool  next()          { return result->next(); }
  virtual bool  isSelected()    { return false; }
  virtual void  rewind()        {        result->rewind(); }
  virtual bool  isValid()       { return result->isValid(); }

protected:
  AbstractResult* result;

};


#endif  // __RESULTLIST_HH__
