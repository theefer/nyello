#include "asynchronizer.hh"

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

