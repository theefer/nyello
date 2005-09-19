#ifndef __RESULTLIST_HH__
#define __RESULTLIST_HH__

#include <xmmsclient/xmmsclient.h>

#include "printable.hh"
#include "abstractresult.hh"


class ResultList : public Printable {
public:

  ResultList(AbstractResult* result);
  ~ResultList();

  virtual unsigned int getId()  { return result->getId(); }
  virtual char* get(char* key)  { return result->get(key); }
  virtual int getInt(char* key) { return result->getInt(key); }
  virtual bool  next()          { return result->next(); }
  virtual bool  isSelected()    { return false; }
  virtual void  rewind()        {        result->rewind(); }
  virtual bool  isValid()       { return result->isValid(); }

protected:
  AbstractResult* result;

};


#endif  // __RESULTLIST_HH__
