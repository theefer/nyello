#include "playback.hh"


Playback::Playback(xmmsc_connection_t* _connection) {
  connection = _connection;
  lastRes = NULL;
}


Playback::~Playback() { }


Delayed<int>*
Playback::play() {
  lastRes = xmmsc_playback_start(connection);
  return new Delayed<int>(lastRes, "Could not start playback: ");
}

Delayed<int>*
Playback::pause() {
  lastRes = xmmsc_playback_pause(connection);
  return new Delayed<int>(lastRes, "Could not pause playback: ");
}

Delayed<int>*
Playback::stop() {
  lastRes = xmmsc_playback_stop(connection);
  return new Delayed<int>(lastRes, "Could not stop playback: ");
}


/**
 * Trigger the song change.
 */
Delayed<int>*
Playback::tickle() {
  lastRes = xmmsc_playback_tickle(connection);
  return new Delayed<int>(lastRes);
}

Delayed<int>*
Playback::jumpAbsolute(int pos) {
  lastRes = xmmsc_playlist_set_next(connection, pos);
  Delayed<int>* del = new Delayed<int>(lastRes, "Couldn't advance in playlist: ");
  // FIXME:  del->add???(Playback::tickle, this);
  return del;
}

Delayed<int>*
Playback::jumpRelative(int offset) {
  lastRes = xmmsc_playlist_set_next_rel(connection, offset);
  Delayed<int>* del = new Delayed<int>(lastRes, "Couldn't advance in playlist: ");
  // FIXME:  del->add???(Playback::tickle, this);
  return del;
}

Delayed<int>*
Playback::seekAbsolute(int position) {
  lastRes = xmmsc_playback_seek_ms(connection, position);
  return new Delayed<int>(lastRes, "Couldn't seek in current song: ");
}

Delayed<int>*
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
  
  return seekAbsolute(position);
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


/**
 * Return the current playtime of the playing song, or 0 if there is
 * no playing song.
 */
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

/**
 * Return the current status of the player (playing, paused, stopped,
 * etc), or STOPPED if an error occured.
 */
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
