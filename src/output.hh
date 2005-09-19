#ifndef __OUTPUT_HH__
#define __OUTPUT_HH__

#define MAX_COUNTER_DISPLAY_LEN 20

class Output;

#include "printable.hh"
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

  void printPlaylists(Printable* playlists);
  void printCollections(Collection* collections);
  void printStatus(Printable* song, unsigned int status,
                   unsigned int playtime, int pos, const char* plname);
  void printEmptyStatus(unsigned int status, const char* plname);
  void printFilter();

  void printCommandSummary(list<Command*> commands);
  void printCommandHelp(Command* cmd);

private:
  Columniser* songCol;
  Columniser* playlistCol;

  char* getStatusString(unsigned int status);
  char* getPlaytimeString(unsigned int playtime);
};


#endif  // __OUTPUT_HH__
