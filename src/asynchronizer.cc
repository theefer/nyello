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


#include "asynchronizer.hh"


Asynchronizer* StaticAsynchronizer::async = NULL;


Asynchronizer::Asynchronizer(xmmsc_connection_t* connection) : conn(connection) {
  xmmsIpc = xmmsc_io_fd_get(conn);
  stdinput = 0;
}

Asynchronizer::~Asynchronizer() {
}

void
Asynchronizer::waitForData() {
  waitForData(WAIT_XMMSIPC | WAIT_READLINE);
}

void
Asynchronizer::waitForData(int waitflags) {
  fd_set rfds, wfds;
  int modfds;

  // Setup fds
  FD_ZERO(&rfds);
  FD_ZERO(&wfds);

  if(waitflags & WAIT_XMMSIPC) {
    FD_SET(xmmsIpc, &rfds);
  }

  if(waitflags & WAIT_READLINE) {
    FD_SET(stdinput, &rfds);
  }

  if((waitflags & WAIT_XMMSIPC) && xmmsc_io_want_out(conn)) {
    FD_SET(xmmsIpc, &wfds);
  }

  // Select on the fds
  modfds = select(xmmsIpc + 1, &rfds, &wfds, NULL, NULL);

  if(modfds < 0) {
    // FIXME: Error
  }
  else if(modfds == 0) {
    // FIXME: Nothing happened
  }
  // Handle the data
  else {
    // We can read or write ipc data
    if(waitflags & WAIT_XMMSIPC) {
      if(FD_ISSET(xmmsIpc, &rfds)) {
        xmmsc_io_in_handle(conn);
      }
      if(FD_ISSET(xmmsIpc, &wfds)) {
        xmmsc_io_out_handle(conn);
      }
    }

    // We have user input
    if((waitflags & WAIT_READLINE) && FD_ISSET(stdinput, &rfds)) {
      rl_callback_read_char();
    }
  }

}

