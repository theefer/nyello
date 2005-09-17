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


/**
 * Trigger the song change.
 */
void
Playback::tickle() {
  lastRes = xmmsc_playback_tickle(connection);
  xmmsc_result_wait(lastRes);
  xmmsc_result_unref(lastRes);
}

void
Playback::jumpAbsolute(int pos) {
  lastRes = xmmsc_playlist_set_next(connection, pos);
  xmmsc_result_wait(lastRes);

  if (xmmsc_result_iserror(lastRes)) {
    cerr << "Couldn't advance in playlist: "
         << xmmsc_result_get_error(lastRes) << endl;
    return;
  }
  xmmsc_result_unref(lastRes);

  tickle();
}

void
Playback::jumpRelative(int offset) {
  lastRes = xmmsc_playlist_set_next_rel(connection, offset);
  xmmsc_result_wait(lastRes);

  if (xmmsc_result_iserror(lastRes)) {
    cerr << "Couldn't advance in playlist: "
         << xmmsc_result_get_error(lastRes) << endl;
    return;
  }
  xmmsc_result_unref(lastRes);

  tickle();
}

void
Playback::seekAbsolute(int position) {
  lastRes = xmmsc_playback_seek_ms(connection, position);
  xmmsc_result_wait(lastRes);

  if (xmmsc_result_iserror(lastRes)) {
    cerr << "Couldn't seek in current song: "
         << xmmsc_result_get_error(lastRes) << endl;
    return;
  }
  xmmsc_result_unref(lastRes);
}

void
Playback::seekRelative(int offset) {
  int position;
  position = 0;
  // FIXME: Compute current-time + offset
  /*
  position = current() + offset;
  if(position < 0) {
    position = 0;
  }
  */
  
  seekAbsolute(position);
}

bool
Playback::isPlaying() {
  return (getStatus() == XMMS_PLAYBACK_STATUS_PLAY);
}

bool
Playback::isPaused() {
  return (getStatus() == XMMS_PLAYBACK_STATUS_PAUSE);
}

/**
 * Return the mlib id of the song currently playing, or -1 if there is
 * no valid entry.
 */
int
Playback::getCurrentId() {
  int id;
  lastRes = xmmsc_playback_current_id(connection);
  xmmsc_result_wait(lastRes);

  if (xmmsc_result_iserror(lastRes)) {
    id = -1;
  }
  else {
    unsigned int uid;
    xmmsc_result_get_uint(lastRes, &uid);
    id = uid;
  }
  xmmsc_result_unref(lastRes);

  return id;
}

/**
 * Return the current position in the playlist, or -1 if there is no
 * valid position.
 */
int
Playback::getCurrentPosition() {
  int pos;
  lastRes = xmmsc_playlist_current_pos(connection);
  xmmsc_result_wait(lastRes);

  if (xmmsc_result_iserror(lastRes)) {
    pos = -1;
  }
  else {
    unsigned int upos;
    xmmsc_result_get_uint(lastRes, &upos);
    pos = upos;
  }
  xmmsc_result_unref(lastRes);

  return pos;
}

unsigned int
Playback::getCurrentPlaytime() {
  unsigned int playtime;
  lastRes = xmmsc_playback_playtime(connection);
  xmmsc_result_wait(lastRes);
  if(xmmsc_result_iserror(lastRes)) {
    cerr << "Could not get playtime: "
         << xmmsc_result_get_error(lastRes) << endl;

    // FIXME: Err, not really
    playtime = 0;
  }
  else {
    xmmsc_result_get_uint(lastRes, &playtime);
  }
  xmmsc_result_unref(lastRes);

  return playtime;
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
