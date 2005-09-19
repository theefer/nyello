#ifndef __SELECTRESULT_HH__
#define __SELECTRESULT_HH__

#include <xmmsclient/xmmsclient.h>

#include "songresult.hh"


/**
 * Special case of the SongResult, when the id must be fetched as the
 * "id" key of the result dict.
 */
class SelectResult : public SongResult {
public:
  SelectResult(xmmsc_result_t* res, xmmsc_connection_t* conn);
  ~SelectResult();

  virtual unsigned int getId();

};


#endif  // __SELECTRESULT_HH__
