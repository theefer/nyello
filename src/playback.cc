#include "playback.hh"


Playback::Playback(xmmsc_connection_t* _connection) {
  connection = _connection;
  lastRes = NULL;
}


Playback::~Playback() { }


DelayedVoid*
Playback::play() {
  lastRes = xmmsc_playback_start(connection);
  return new DelayedVoid(lastRes, "Could not start playback: ");
}

DelayedVoid*
Playback::pause() {
  lastRes = xmmsc_playback_pause(connection);
  return new DelayedVoid(lastRes, "Could not pause playback: ");
}

DelayedVoid*
Playback::stop() {
  lastRes = xmmsc_playback_stop(connection);
  return new DelayedVoid(lastRes, "Could not stop playback: ");
}


/**
 * Trigger the song change.
 */
void
Playback::tickle() {
  // FIXME: *wrong* ! We should return the DelayedVoid, but not doable now
  lastRes = xmmsc_playback_tickle(connection);
  DelayedVoid(lastRes).wait();
}

DelayedVoid*
Playback::jumpAbsolute(int pos) {
  lastRes = xmmsc_playlist_set_next(connection, pos);
  DelayedVoid* del = new DelayedVoid(lastRes, "Couldn't advance in playlist: ");
  del->addCallback<Playback>(this, &Playback::tickle);
  return del;
}

DelayedVoid*
Playback::jumpRelative(int offset) {
  lastRes = xmmsc_playlist_set_next_rel(connection, offset);
  DelayedVoid* del = new DelayedVoid(lastRes, "Couldn't advance in playlist: ");
  del->addCallback<Playback>(this, &Playback::tickle);
  return del;
}

DelayedVoid*
Playback::seekAbsolute(int position) {
  lastRes = xmmsc_playback_seek_ms(connection, position);
  return new DelayedVoid(lastRes, "Couldn't seek in current song: ");
}

DelayedVoid*
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

/**
 * Return whether a song is currently being played.
 */
Delayed<bool>*
Playback::isPlaying() {
  lastRes = xmmsc_playback_status(connection);
  return new Delayed<bool>(lastRes,
                           new ComparatorProduct<unsigned int, &xmmsc_result_get_uint>(XMMS_PLAYBACK_STATUS_PLAY),
                           "Could not get playback status: ");
}

/**
 * Return whether playback is currently paused.
 */
Delayed<bool>*
Playback::isPaused() {
  lastRes = xmmsc_playback_status(connection);
  return new Delayed<bool>(lastRes,
                           new ComparatorProduct<unsigned int, &xmmsc_result_get_uint>(XMMS_PLAYBACK_STATUS_PAUSE),
                           "Could not get playback status: ");
}

/**
 * Return the mlib id of the song currently playing, or -1 if there is
 * no valid entry.
 */
Delayed<unsigned int>*
Playback::getCurrentId() {
  lastRes = xmmsc_playback_current_id(connection);
  return new Delayed<unsigned int>(lastRes,
                                   new PrimitiveProduct<unsigned int, &xmmsc_result_get_uint>());
}

/**
 * Return the current position in the playlist, or -1 if there is no
 * valid position.
 */
Delayed<unsigned int>*
Playback::getCurrentPosition() {
  lastRes = xmmsc_playlist_current_pos(connection);
  return new Delayed<unsigned int>(lastRes,
                                   new PrimitiveProduct<unsigned int, &xmmsc_result_get_uint>());
}


/**
 * Return the current playtime of the playing song, or 0 if there is
 * no playing song.
 */
Delayed<unsigned int>*
Playback::getCurrentPlaytime() {
  lastRes = xmmsc_playback_playtime(connection);
  return new Delayed<unsigned int>(lastRes,
                                   new PrimitiveProduct<unsigned int, &xmmsc_result_get_uint>(),
                                   "Could not get playtime: ");
}

/**
 * Return the current status of the player (playing, paused, stopped,
 * etc), or STOPPED if an error occured.
 */
Delayed<unsigned int>*
Playback::getStatus() {
  lastRes = xmmsc_playback_status(connection);
  return new Delayed<unsigned int>(lastRes,
                                   new PrimitiveProduct<unsigned int, &xmmsc_result_get_uint>(),
                                   "Could not get playback status: ");
}
