#include "playback.hh"


Playback::Playback(xmmsc_connection_t* _connection) {
  connection = _connection;
  lastRes = NULL;
}


Playback::~Playback() { }


void
Playback::play() {
  lastRes = xmmsc_playback_start(connection);
  xmmsc_result_wait(lastRes);
  if(xmmsc_result_iserror(lastRes)) {
    cerr << "Could not start playback: "
         << xmmsc_result_get_error(lastRes) << endl;
  }
  xmmsc_result_unref(lastRes);
}

void
Playback::pause() {
  lastRes = xmmsc_playback_pause(connection);
  xmmsc_result_wait(lastRes);
  if(xmmsc_result_iserror(lastRes)) {
    cerr << "Could not pause playback: "
         << xmmsc_result_get_error(lastRes) << endl;
  }
  xmmsc_result_unref(lastRes);
}

void
Playback::stop() {
  lastRes = xmmsc_playback_stop(connection);
  xmmsc_result_wait(lastRes);
  if(xmmsc_result_iserror(lastRes)) {
    cerr << "Could not stop playback: "
         << xmmsc_result_get_error(lastRes) << endl;
  }
  xmmsc_result_unref(lastRes);
}

void
Playback::jump(int offset) {
  lastRes = xmmsc_playlist_set_next_rel(connection, offset);
  xmmsc_result_wait(lastRes);

  if (xmmsc_result_iserror(lastRes)) {
    cerr << "Couldn't advance in playlist: "
         << xmmsc_result_get_error(lastRes) << endl;
    return;
  }
  xmmsc_result_unref(lastRes);

  // Trigger the song change now
  lastRes = xmmsc_playback_tickle(connection);
  xmmsc_result_wait(lastRes);
  xmmsc_result_unref(lastRes);
}

bool
Playback::isPlaying() {
  return (getStatus() == XMMS_PLAYBACK_STATUS_PLAY);
}

bool
Playback::isPaused() {
  return (getStatus() == XMMS_PLAYBACK_STATUS_PAUSE);
}

unsigned int
Playback::getStatus() {
  unsigned int status;
  lastRes = xmmsc_playback_status(connection);
  xmmsc_result_wait(lastRes);
  if(xmmsc_result_iserror(lastRes)) {
    cerr << "Could not get playback status: "
         << xmmsc_result_get_error(lastRes) << endl;

    // FIXME: Err, not really
    status = XMMS_PLAYBACK_STATUS_STOP;
  }
  else {
    xmmsc_result_get_uint(lastRes, &status);
  }
  xmmsc_result_unref(lastRes);

  return status;
}
