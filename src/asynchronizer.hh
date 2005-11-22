#ifndef __ASYNCHRONIZER_HH__
#define __ASYNCHRONIZER_HH__

#include <xmmsclient/xmmsclient.h>
#include <readline/readline.h>


class Asynchronizer {
public:
  static const int WAIT_XMMSIPC  = 1;
  static const int WAIT_READLINE = 2;

  Asynchronizer(xmmsc_connection_t* connection);
  ~Asynchronizer();

  void waitForData();
  void waitForData(int waitflags);

private:
  xmmsc_connection_t* conn;
  int xmmsIpc;
  int stdinput;

};


#endif  // __ASYNCHRONIZER_HH__
