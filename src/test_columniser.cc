#include "columniser.hh"

int
main() {
  Columniser* ciser = new Columniser();
  ciser->addColumn(new Column("id", "Id", 10, "", ""));
  ciser->addColumn(new Column("artist", "Artist", 20, " | ", ""));
  ciser->addColumn(new Column("album", "Album", 25, " | ", ""));
  ciser->addColumn(new Column("title", "Title", 28, " | ", " |"));
  ciser->printHeader("Result of ...");
  ciser->printHeadings();
  ciser->printFooter("Count: 7");
  return 0;
}
