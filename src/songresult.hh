#ifndef __SONGRESULT_HH__
#define __SONGRESULT_HH__

#include <xmmsclient/xmmsclient.h>

#include "abstractresult.hh"


class SongResult : public AbstractResult {
public:
  SongResult(xmmsc_result_t* res, xmmsc_connection_t* conn);
  ~SongResult();

  virtual unsigned int getId();

  virtual char* get(char* key);
  virtual bool  next();


private:
  /**
   * The connection to the server.
   */
  xmmsc_connection_t* conn;

  /**
   * Result referencing the dict storing the metadata of the current
   * song.
   */
  xmmsc_result_t* cache;

};


#endif  // __SONGRESULT_HH__
