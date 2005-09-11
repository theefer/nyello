#ifndef __PATTERNQUERY_HH__
#define __PATTERNQUERY_HH__

#include <vector>
#include <list>
#include <utility>
#include <string>
using namespace std;

#include "patternnodes.hh"
#include "playlistsonglist.hh"
#include "sequence.hh"

/**
 * First element: name of the field to order by (artist, album, etc)
 * Second element: whether we order ascendingly
 */
typedef list<pair<char*,bool> > OrderByList;


/**
 *
 */
class PatternQuery {
public:

  PatternQuery(PatternNode* top, OrderByList* order);
  ~PatternQuery();

  void saveResults(Printable* songlist);

  char* getSql();
  IdSequence* getIdsFromSequence(IdSequence* seq);

  inline PatternNode* getTopNode() { return topNode; }

private:
  PatternNode* topNode;

  MedialibQuery* queryCache;
  vector<unsigned int> resultCache;
};

#endif  // __PATTERNQUERY_HH__
