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


/*

  == Nyello, a command-line XMMS2 client ==


  * Program usage

    Usage: nyello [command]


  * Analogy

    Playlist   = host
    Collection = directory

    Thus the prompt:

      nyello:rocklist /Electro/AirLive $


  * Commands

  * H   HELP [command]   Get general help or help on a specific command.

  * T   STATUS           Status of the server.
  
  * P   PLAY             Start playback.
  * P   PAUSE            Pause playback.
  * S   STOP [num]       Stop playback (option: in num songs)
  * K   SEEK [pos|offset] Seek to absolute position or relative offset (if +/-), in seconds.
  
  * R   PREVIOUS [offset] Play previous song in the current playlist.
  * N   NEXT     [offset] Play next song in the current playlist.

  * J   JUMP <pattern>   Jump in the current playlist to the first song matching the
                         pattern after the current song.
  
  * I   INFO <pattern>   Display info on the songs matched by the pattern, or the currently
                         playing song if no pattern.

-   F   FILTER <pattern> Display the structure of the given pattern filter.

  * L   LIST [playlist]  Display the list of songs in the given playlist, or the currently
                         playing playlist if no playlist given.

  * +   IMPORT <path>*   Import new files in the media-library.
  
  * E   ENQUEUE <pattern> [IN <playlist>?]
  * E+  INSERT  <pattern>
  * E-  REPLACE <pattern>

+   M   MOVE <pos|offset> <pattern>  Move a group of song in the playlist to an absolute
                                     position or a relative offset.
+   M   MOVE <pattern> AFTER <pattern>  Move a group of song in the playlist after the
                                        last song matching the pattern.
+   M   MOVE <pattern> BEFORE <pattern> Move a group of song in the playlist before the
                                        last song matching the pattern.
+   M   MOVE <pattern> TO <pos|offset>  Move a group of song in the playlist to an absolute
                                        position or a relative offset.
  * O   REMOVE  <pattern>     Remove songs matching the pattern from the current playlist.
  * C   CLEAR   [playlist]    Clear the playlist (equivalent to REMOVE *)
  * U   SHUFFLE [playlist]    Shuffle the playlist.

  * PL  PLAYLIST-LIST            List the existing playlists.
  * PS  PLAYLIST-SAVE-AS <name>  Save the current playlist as a new playlist.
  * PU  PLAYLIST-USE     <name>  Use the given playlist as current playlist.
  * PR  PLAYLIST-REMOVE  <name>  Delete the given playlist.

    PF  PLAYLIST-FEED    <collection>  Make the current playlist a dynamic playlist of
                                       the given collection, i.e. insert random songs
                                       from the collection and toggle the "auto-insert"
                                       flag of the playlist.

    CL  COLLECTION-LIST               List the collection tree.
    CC  COLLECTION-CURRENT            Show current active collection.
    CA  COLLECTION-APPLY   <pattern>  Apply the pattern to the current collection.
    CS  COLLECTION-SAVE-AS <name> <pattern>  Save the given pattern as a new collection.
    CE  COLLECTION-ENTER   <name>     Enter the given collection ("All Media" if no argument).
    CR  COLLECTION-REMOVE  <name>     Delete the given collection.

  * Q   QUIT, EXIT       Terminates nyello.

-   A   ADMIN  <command>
               q  quit                      Terminate xmms2d server.
               s  stats                     Display stats (uptime, server version,
                                            mlib size, unresolved ratio)
               h  rehash [pattern]          Rehash given songs (or all mlib if no pattern).
               o  remove <pattern>          Remove given songs from mlib.
               p  plugins                   List all plugins loaded in the server.
               c  config [property [value]] Set the value of a property, or display the property
                                            (all if no arg).

 */

#include "config.h"

#include "nyello.hh"
#include "dispatcher.hh"

#include <string>
using std::string;


/* Initialize program constants */
const string Nyello::CLIENT_NAME    = PACKAGE_NAME;
const string Nyello::CLIENT_VERSION = PACKAGE_VERSION;


int main (int argc, char* argv[])
{
  try {
    Dispatcher* disp = Dispatcher::getInstance();
    if(argc > 1) {
      string input;
      for( int i = 1; i < argc; ++i ) {
        if( i > 1 ) input += " ";

        input += argv[i];
      }
      disp->execute( input );
    }
    else {
      disp->loop();
    }
  }
  catch (...) {
    return 1;
  }

  return 0;
}
