#ifndef __PLAYBACK_HH__
#define __PLAYBACK_HH__


#include <iostream>

#include <xmmsclient/xmmsclient.h>


using namespace std;


/**
 * Contains methods to interact with the server for all the actions
 * that concern the playback (anything related with the playing flow).
 */
class Playback {
public:
  Playback(xmmsc_connection_t* connection);
  ~Playback();

  void play();
  void pause();
  void stop();

  void tickle();
  void jumpAbsolute(int pos);
  void jumpRelative(int offset);
  void seekAbsolute(int position);
  void seekRelative(int offset);
  
  bool isPlaying();
  bool isPaused();

  int getCurrentId();
  int getCurrentPosition();
  unsigned int getCurrentPlaytime();

  unsigned int getStatus();

private:
  xmmsc_connection_t* connection;
  xmmsc_result_t*     lastRes;

};


#endif  // __PLAYBACK_HH__
