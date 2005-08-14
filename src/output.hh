#ifndef __OUTPUT_HH__
#define __OUTPUT_HH__


#include "printable.hh"
#include "playlistlist.hh"
#include "collection.hh"
#include "columniser.hh"
#include "countercolumn.hh"

#include <iostream>
#include <vector>

#define MAX_COUNTER_DISPLAY_LEN 20

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
  void printHelp();

private:
  Columniser* songCol;
  Columniser* playlistCol;
};


#endif  // __OUTPUT_HH__
