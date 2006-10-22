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


#include "output.hh"

Output::Output() {
  // Special column for mlib ids
  Column* cid = new Column("id", "Id", 5, "", "", ios_base::right);
  cid->setFiller('0');

  songCol = new Columniser();
  songCol->addColumn(new CounterColumn("Pos", 4, "", ". ", ios_base::right));
  songCol->addColumn(cid);
  songCol->addColumn(new Column("artist", "Artist", 20, " | ", ""));
  songCol->addColumn(new Column("album", "Album", 25, " | ", ""));
  songCol->addColumn(new Column("title", "Title", 32, " | ", ""));

  playlistCol = new Columniser();
  playlistCol->addColumn(new Column("name", "Playlist name", 40));
  playlistCol->addColumn(new Column("size", "Song count",    8,  "(", " songs)"));
}

Output::~Output() {
  delete songCol;
  delete playlistCol;
}


void
Output::printPlaylists(Printable* playlists) {
  playlistCol->reset();
  while(playlists->isValid()) {
    playlistCol->printRow(playlists);
    playlists->next();
  }
}

void
Output::printSongs(Printable* songs) {
  int counter = 0;
  char counterDisplay[MAX_COUNTER_DISPLAY_LEN];

  // Display result header
  songCol->reset();
  songCol->printHeader("Results");
  songCol->printHeadings();

  // Display all the songs
  songs->rewind();
  while(songs->isValid()) {
    songCol->printRow(songs);
    songs->next();
    ++counter;
  }

  // Display counter
  snprintf(counterDisplay, MAX_COUNTER_DISPLAY_LEN, "Count: %d", counter);
  songCol->printFooter(counterDisplay);
}


/**
 * Display the current status of playback.
 */
void
Output::printStatus(Printable* song, unsigned int status,
                    unsigned int playtime, int pos, string plname) {
  // FIXME: All this output should be customizable!

  // FIXME: Why can't we rewind here?
  // song->rewind();
  char* playtime_str = getPlaytimeString(playtime);
  int portion = (playtime * 100) / song->getInt("duration");

  cout << song->get("artist") << " - " << song->get("album");

  if(strlen(song->get("year")) > 0)
    cout << " (" << song->get("year") << ")";

  cout << " - " << song->get("title") << endl
       << "[" << getStatusString(status) << "] "
       << pos << "/" << song->get("id") << "  "
       << playtime_str << " (" << portion << "%)" << endl
       << "playlist: " << plname << endl;

  delete playtime_str;
}

/**
 * Short version of status, only displaying the playback status and
 * informations not needing song or position informations.
 */
void
Output::printEmptyStatus(unsigned int status, string plname) {
  cout << "[" << getStatusString(status)
       << "] no entry currently selected" << endl
       << "playlist: " << plname << endl;
}

void
Output::printCommandSummary(list<Command*> commands) {
  list<Command*>::iterator it;
  cout.flags(ios_base::left);
  cout << "Available commands:" << endl;
  for(it = commands.begin(); it != commands.end(); ++it) {
    cout << "  ";
    cout.width(2);
    cout << (*it)->getShortName() << "   ";
    cout.width(19);
    cout << (*it)->getName();
    cout << (*it)->getDescription()
         << endl;
  }
}

void
Output::printCommandHelp(Command* cmd) {
  cout << "Usage: " << cmd->getUsage() << endl << endl;
  cout << cmd->getHelp() << endl;
}

/**
 * Return a textual version of a status id.
 */
char*
Output::getStatusString(unsigned int status) {
  char* status_str;
  switch(status) {
  case XMMS_PLAYBACK_STATUS_PLAY:   status_str = "playing";  break;
  case XMMS_PLAYBACK_STATUS_PAUSE:  status_str = "paused";   break;
  case XMMS_PLAYBACK_STATUS_STOP:   status_str = "stopped";  break;
  default:                          status_str = "unknown";  break;
  }
  return status_str;
}

/**
 * Formats the millisecond playtime into a "MM:SS" string.
 * The returned string must be manually freed after usage!
 */
char*
Output::getPlaytimeString(unsigned int playtime) {
  char* buffer = new char[10];
  playtime /= 1000;
  snprintf(buffer, 9, "%02d:%02d", 
           playtime / 60, playtime % 60);
  return buffer;
}
