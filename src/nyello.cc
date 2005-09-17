/*

  == Nyello, a command-line XMMS2 client ==


  * Program usage

    Usage: nyello [options] [<host> [<port>]]

      -h, --help     display this help and exit
      -v, --version  output version information and exit


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
  * S   STOP [option]    Stop playback (option: now, graceful, end)
    K   SEEK [pos|offset] Seek to absolute position or relative offset (if +/-), in seconds.
  
  * R   PREVIOUS [offset] Play previous song in the current playlist.
  * N   NEXT     [offset] Play next song in the current playlist.
  * J   JUMP     [pos|offset] Jump to given position in the current playlist
                              or to a given offset (if starts by +/-)
  
  * I   INFO <pattern>   Display info on the songs matched by the pattern, or the currently
                         playing song if no pattern.

-   F   FILTER <pattern> Display the current (i.e. latest) pattern filter used, and set 
                         it to the given pattern.

  * L   LIST [playlist]  Display the list of songs in the given playlist, or the currently
                         playing playlist if no playlist given.
  
  * E   ENQUEUE <pattern> [IN <playlist>?]
  * E+  INSERT  <pattern>
  * E-  REPLACE <pattern>
    X   EXCLUDE <pattern>     Remove songs matching the pattern from the current playlist.
??? ?   REMOVE  <id-sequence> Remove songs of given position from the current playlist.
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
    CA  COLLECTION-APPLY   <pattern>  Apply the pattern to the current collection.
    CS  COLLECTION-SAVE-AS <path> <pattern>  Save the given pattern as a new collection.
    CE  COLLECTION-ENTER   <path>     Enter the given collection.
    CR  COLLECTION-REMOVE  <path>     Delete the given collection.

  * Q   QUIT, EXIT       Terminates nyello.


  * Patterns

    Flags:
      -a, --artist <artist>
      -l, --album  <album>
      -t, --title  <title>
      -y, --year   <year>
      -g, --genre  <genre>
      -n, --track  <track>
      -z, --any    <any>    (default if no flag given)

      · with uppercase equivalent: exact match (case, string)
      · supports comparators (>, >=, etc) for year, track
      · use parentheses to group conditions
      · no parsing is done on the match values (replaces escaping):
          -z AND -z - -a &
      · can have several fields per flag:
          -at pink AND -lG Soundtrack <=> (-a pink OR -t pink) AND ...
      · whitespace separated tokens, thus:
          -a Air Femme argent <=> -a Air AND -z Femme AND -z argent

      -o, --orderby <fieldlist>

      · fieldlist a comma-separated list of fields to order by, either
        in short or long form
      · reverse ordering is done by using uppercase:
          -o a,-year,L,track

    Combinators:

      Short or long versions (case-sensitive!) can be used:
        &, AND (default if omitted)
        |, OR
        %, XOR
        ^, NOT

    Wildcards:

      +   previous pattern  (equivalent to +1)
      +N  N-th previous pattern, with N >= 1 and N <= arbitrary limit

    Identifiers:

      rocklist/7 7-th song of the "rocklist" playlist
      /7         7-th song of the current playlist (equivalent to currentpl/7)
      N#7        7-th song matched by the N-th previous pattern
      7          song with id 7 in the medialib

      · ranges and lists can be provided, too:
        #7-13    7-th to 13-th songs matched by the last pattern
        /7,13    7-th and 13-th song of the current playlist
        #-13     First to 13-th song matched by the last pattern
        rock/7-  All songs from the 7th of the rock playlist
        2#       All songs matched by the pattern before the last

        (note: unbounded ranges are not possible with medialib ids)

     Default rules:

       · If no pattern is given at all, the result of the pattern is
         the current song
 */

#include <iostream>


#include <xmmsclient/xmmsclient.h>


#include "dispatcher.hh"


using namespace std;



int main(int argc, char* argv[]) {
  // Main variables
  int retval = 0;
  Dispatcher* disp;

  // TODO: Init with CL options and config

  char* path = getenv("XMMS_PATH");

  // Init xmms2 connection
  xmmsc_connection_t* connection;
  connection = xmmsc_init("nyello");
  if(!connection) {
    cerr << "Could not init xmmsc_connection!" << endl;
    retval = 1;
  }

  else if(!xmmsc_connect(connection, path)) {
    cerr << "Could not connect to server: "
         << xmmsc_get_last_error(connection) << endl;
    retval = 1;
  }

  // Everything is fine, we can run the dispatcher
  else {
    disp = new Dispatcher(connection);
    if(argc > 1)
      disp->execute(argc - 1, argv + 1);
    else
      disp->loop();
  }


  xmmsc_unref(connection);

  return retval;
}
