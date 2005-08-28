#ifndef __PATTERNQUERY_HH__
#define __PATTERNQUERY_HH__

#include <vector>
using namespace std;

#include "patternnodes.hh"
#include "playlistsonglist.hh"
#include "sequence.hh"


/**
 *
 */
class PatternQuery {
public:

  PatternQuery(PatternNode* top, char* order);
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
