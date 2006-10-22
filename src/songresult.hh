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


#ifndef __SONGRESULT_HH__
#define __SONGRESULT_HH__

#include <xmmsclient/xmmsclient.h>

#include "abstractresult.hh"
#include "delayed.hh"


class SongResult : public AbstractResult {
public:
  SongResult(xmmsc_result_t* res, xmmsc_connection_t* conn);
  ~SongResult();

  virtual unsigned int getId();

  virtual char* get(char* key);
  virtual int getInt(char* key);

  virtual bool  next();


private:
  /**
   * The connection to the server.
   */
  xmmsc_connection_t* conn;

  /**
   * Result referencing the dict storing the metadata of the current
   * song.
   */
  xmmsc_result_t* cache;
  Delayed<void>* delay;

  void fetchCache();
  void flushCache();

};


#endif  // __SONGRESULT_HH__
