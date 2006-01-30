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
