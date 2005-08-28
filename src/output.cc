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
Output::printHelp() {
  // TODO: Display help from command objects  
  cout << "Available commands:"                                                       << endl;
  cout << "   (p)  play               Start playback"                                 << endl;
  cout << "   (p)  pause              Pause playback"                                 << endl;
  cout << "   (s)  stop               Stop playback"                                  << endl;
  cout << "   (r)  previous           Jump to previous song"                          << endl;
  cout << "   (n)  next               Jump to next song"                              << endl;
  cout << "   (l)  list               List the songs from a playlist"                 << endl;
  cout << "   (pl) playlist-list      List all the existing playlists"                << endl;
  cout << "   (pu) playlist-use       Change the active playlist"                     << endl;
  cout << "   (ps) playlist-save-as   Save current playlist under a new name"         << endl;
  cout << "   (pr) playlist-remove    Remove a playlist"                              << endl;
  cout << "   (h)  help               Display the help for all or a specific command" << endl;
  cout << "   (q)  quit               Exit nyello"                                    << endl;
}
