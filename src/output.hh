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
#include <string>

using namespace std;



class Output {

public:

  Output();
  ~Output();

  void printSongs(Printable* songs);

  void printPlaylists(Printable* playlists);
  void printCollections(Collection* collections);
  void printStatus(Printable* song, unsigned int status,
                   unsigned int playtime, int pos, string plname);
  void printEmptyStatus(unsigned int status, string plname);
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
