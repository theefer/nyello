#include "uri.hh"

Uri::Uri(char* path) {
  // FIXME: Glob it
  // FIXME: handle HTTP URIs too

  if(realpath(path, uri) != NULL) {
    valid = (lstat(path, &fstats) == 0);
  }
  else {
    valid = false;
  }
}

Uri::~Uri() {
}

bool
Uri::isDirectory() {
  return S_ISDIR(fstats.st_mode);
}

bool
Uri::exists() {
  return valid;
}
