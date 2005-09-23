#ifndef __DELAYED_HH__
#define __DELAYED_HH__


// Generic hack-function to use methods as callback functions
template <class T> class Delayed;
template <void (Delayed::*func) (xmmsc_result_t*)>
void runDelayedMethod(xmmsc_result_t *res, void *del_ptr) {
  Delayed* d = (Delayed*)del_ptr;
  (d->*func)(res);
}


template <class T>
class Delayed {

  typedef T (Handler::*HandleFnPtr)(xmmsc_result_t* res);
  typedef void (Receiver::*ReceiveFnPtr)(T);

public:
  Delayed(xmmsc_result_t* res);
  ~Delayed();

  void callback(xmmsc_result_t* res);
  void wait();

  inline void addHandler(HandleFnPtr fn)   { handlers.push_back(fn); }
  inline void addReceiver(ReceiveFnPtr fn) { receivers.push_back(fn); }

private:
  list<HandleFnPtr>  handlers;
  list<ReceiveFnPtr> receivers;

  void unblock();
};



/*  == And here comes the implementation, in the header file due to
       boring GCC limitations...  == */

template <class T>
Delayed<T>::Delayed(xmmsc_result_t* res) {
  xmmsc_result_notifier_set(res, runDelayedMethod<&Delayed::callback>, this);
  xmmsc_result_unref(res);
}


template <class T>
Delayed<T>::~Delayed() {
  // FIXME: unref?
}


template <class T>
void
Delayed<T>::callback(xmmsc_result_t* res) {
  // Check for error
  if(xmmsc_result_iserror(res)) {
    // Handle
  }
  else {
    list<ReceiveFnPtr>::iterator receive;
    list<HandleFnPtr>::iterator  handle;
    for(handle = handlers.begin(); handle != handlers.end(); ++handle) {
      T product = (*handle)(res);
      for(receive = receivers.begin(); receive != receivers.end(); ++ receivers) {
        (*receive)(product);
      }
    }
  }
  
  // Unblock waiting
}


template <class T>
void
Delayed<T>::wait() {
  // FIXME: How to *wait* ?
}


template <class T>
void
Delayed<T>::unblock() {
  // FIXME: How to *unblock* waiters?
}


#endif  // __DELAYED_HH__
