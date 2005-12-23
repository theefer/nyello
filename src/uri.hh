#ifndef __URI_HH__
#define __URI_HH__

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>


class Uri {
public:
  Uri(char* path);
  ~Uri();

  bool isDirectory();
  bool exists();

  inline char* getPath() { return uri; }

private:
  bool valid;
  char uri[PATH_MAX];
  struct stat fstats;
};


#endif  // __URI_HH__
