#ifndef __PATTERNQUERY_HH__
#define __PATTERNQUERY_HH__

#include <vector>
#include <list>
#include <string>
using namespace std;

#include "patternnodes.hh"
#include "abstractresult.hh"
#include "sequence.hh"


/**
 *
 */
class PatternQuery {
public:

  PatternQuery(PatternNode* top, PatternOrderBy* order);
  ~PatternQuery();

  void saveResults(AbstractResult* songlist);

  char* getSql();
  IdSequence* getIdsFromSequence(IdSequence* seq);

  inline PatternNode* getTopNode() { return topNode; }

private:
  PatternNode* topNode;
  PatternOrderBy* orderNode;

  MedialibQuery* queryCache;
  vector<unsigned int> resultCache;
};

#endif  // __PATTERNQUERY_HH__
