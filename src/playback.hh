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

  DelayedVoid* play();
  DelayedVoid* pause();
  DelayedVoid* stop();

  void tickle();
  DelayedVoid* jumpAbsolute(int pos);
  DelayedVoid* jumpRelative(int offset);
  DelayedVoid* seekAbsolute(int position);
  DelayedVoid* seekRelative(int offset);
  
  Delayed<bool>* isPlaying();
  Delayed<bool>* isPaused();

  Delayed<unsigned int>* getCurrentId();
  Delayed<unsigned int>* getCurrentPosition();
  Delayed<unsigned int>* getCurrentPlaytime();

  Delayed<unsigned int>* getStatus();

private:
  xmmsc_connection_t* connection;
  xmmsc_result_t*     lastRes;

};


#endif  // __PLAYBACK_HH__
