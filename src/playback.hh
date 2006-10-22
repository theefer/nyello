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


#ifndef __PLAYBACK_HH__
#define __PLAYBACK_HH__


#include <iostream>

using namespace std;

#include <xmmsclient/xmmsclient.h>

#include "delayed.hh"


/**
 * Contains methods to interact with the server for all the actions
 * that concern the playback (anything related with the playing flow).
 */
class Playback {
public:
  Playback(xmmsc_connection_t* connection);
  ~Playback();

  Delayed<void>* play();
  Delayed<void>* pause();
  Delayed<void>* stop();

  Delayed<void>* tickle();
  Delayed<void>* jumpAbsolute(int pos);
  Delayed<void>* jumpRelative(int offset);
  Delayed<void>* seekAbsolute(int pos);
  Delayed<void>* seekRelative(int offset);
  
  Delayed<bool>* isPlaying();
  Delayed<bool>* isPaused();
  Delayed<bool>* isStopped();

  Delayed<unsigned int>* getCurrentId();
  Delayed<unsigned int>* getCurrentPosition();
  Delayed<unsigned int>* getCurrentPlaytime();

  Delayed<unsigned int>* getStatus();

private:
  xmmsc_connection_t* connection;
  xmmsc_result_t*     lastRes;

};


#endif  // __PLAYBACK_HH__
