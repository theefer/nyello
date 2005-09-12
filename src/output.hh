#ifndef __OUTPUT_HH__
#define __OUTPUT_HH__

#define MAX_COUNTER_DISPLAY_LEN 20

class Output;

#include "printable.hh"
#include "playlistlist.hh"
#include "collection.hh"
#include "columniser.hh"
#include "countercolumn.hh"
#include "command.hh"

#include <iostream>
#include <vector>

using namespace std;



class Output {

public:

  Output();
  ~Output();

  void printSongs(Printable* songs);

  void printPlaylists(PlaylistList* playlists);
  void printCollections(Collection* collections);
  void printStatus();
  void printFilter();

  void printCommandSummary(list<Command*> commands);
  void printCommandHelp(Command* cmd);

private:
  Columniser* songCol;
  Columniser* playlistCol;
};


#endif  // __OUTPUT_HH__
