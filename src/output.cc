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
Output::printPlaylists(PlaylistList* playlists) {
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
