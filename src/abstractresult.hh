#ifndef __ABSTRACTRESULT_HH__
#define __ABSTRACTRESULT_HH__

#include <xmmsclient/xmmsclient.h>


class AbstractResult {
public:
  AbstractResult(xmmsc_result_t* res);
  ~AbstractResult();

  virtual unsigned int getId() = 0;
  virtual unsigned int getCounter() { return counter; }

  virtual char* get(char* key) = 0;
  virtual bool  next() = 0;
  virtual void  rewind();
  virtual bool  isValid();


protected:
  static const int MAX_DICT_STRING_LEN = 255;

  /**
   * Result referencing the data.
   */
  xmmsc_result_t* res;

  /**
   * Counter saving the index of the current entry (starting at 0).
   */
  unsigned int counter;


  char* getDictEntryAsStr(xmmsc_result_t* result, char* key);

};


#endif  // __ABSTRACTRESULT_HH__
