#ifndef __COLUMNISER_HH__
#define __COLUMNISER_HH__


#include <iostream>
#include <vector>

#include "column.hh"
#include "printable.hh"


using namespace std;


typedef vector<Column*> ColumnList;


class Columniser {
public:
  Columniser();
  ~Columniser();

  void reset();

  void printHeader();
  void printHeader(char* message);
  void printFooter();
  void printFooter(char* message);

  void printHeadings();
  void printRow(Printable* entry);

  inline void setFrameChar(char fchar) { frameChar = fchar; }
  inline void setSelectedPrefix(char* prefix) { selectedPrefix = prefix; }
  void setUnselectedPrefix(char* prefix);

  void addColumn(Column* col);


private:
  char  frameChar;
  char* selectedPrefix;
  char* unselectedPrefix;

  vector<Column*> columns;
  int width;

};

#endif  // __COLUMNISER_HH__
