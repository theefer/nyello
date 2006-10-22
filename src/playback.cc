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


#include "playback.hh"


Playback::Playback(xmmsc_connection_t* _connection) {
  connection = _connection;
  lastRes = NULL;
}


Playback::~Playback() { }


Delayed<void>*
Playback::play() {
  lastRes = xmmsc_playback_start(connection);
  return new Delayed<void>(lastRes, "Could not start playback: ");
}

Delayed<void>*
Playback::pause() {
  lastRes = xmmsc_playback_pause(connection);
  return new Delayed<void>(lastRes, "Could not pause playback: ");
}

Delayed<void>*
Playback::stop() {
  lastRes = xmmsc_playback_stop(connection);
  return new Delayed<void>(lastRes, "Could not stop playback: ");
}


/**
 * Trigger the song change.
 */
Delayed<void>*
Playback::tickle() {
  lastRes = xmmsc_playback_tickle(connection);
  return new Delayed<void>(lastRes);
}

Delayed<void>*
Playback::jumpAbsolute(int pos) {
  lastRes = xmmsc_playlist_set_next(connection, pos);
  Delayed<void>* del = new Delayed<void>(lastRes, "Couldn't advance in playlist: ");
  del->addCallback<Playback>(this, &Playback::tickle);
  return del;
}

Delayed<void>*
Playback::jumpRelative(int offset) {
  lastRes = xmmsc_playlist_set_next_rel(connection, offset);
  Delayed<void>* del = new Delayed<void>(lastRes, "Couldn't advance in playlist: ");
  del->addCallback<Playback>(this, &Playback::tickle);
  return del;
}

Delayed<void>*
Playback::seekAbsolute(int pos) {
  lastRes = xmmsc_playback_seek_ms(connection, pos);
  return new Delayed<void>(lastRes, "Couldn't seek in current song: ");
}

Delayed<void>*
Playback::seekRelative(int offset) {
  lastRes = xmmsc_playback_seek_ms_rel(connection, offset);
  return new Delayed<void>(lastRes, "Couldn't seek in current song: ");
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
 * Return whether playback is currently stopped.
 */
Delayed<bool>*
Playback::isStopped() {
  lastRes = xmmsc_playback_status(connection);
  return new Delayed<bool>(lastRes,
                           new ComparatorProduct<unsigned int, &xmmsc_result_get_uint>(XMMS_PLAYBACK_STATUS_STOP),
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
