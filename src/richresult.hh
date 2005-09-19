#ifndef __RICHRESULT_HH__
#define __RICHRESULT_HH__

#include <xmmsclient/xmmsclient.h>

#include "abstractresult.hh"


class RichResult : public AbstractResult {
public:
  RichResult(xmmsc_result_t* res);
  ~RichResult();

  virtual unsigned int getId();

  virtual char* get(char* key);
  virtual bool  next();

};


#endif  // __RICHRESULT_HH__
