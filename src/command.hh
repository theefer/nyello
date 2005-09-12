#ifndef __COMMAND_HH__
#define __COMMAND_HH__

#include <list>
#include <vector>

class Command;

#include "dispatcher.hh"


class Command {
public:

  Command(char* name, char* shortName, DispFnPtr action,
          char* usage, char* description, char* help);
  ~Command();

  inline void addAlias(char* alias) { aliases.push_back(alias); }

  inline char* getName()        { return aliases[0]; }
  inline char* getShortName()   { return aliases[1]; }
  inline DispFnPtr getAction()  { return action; }
  inline char* getUsage()       { return usage; }
  inline char* getDescription() { return description; }
  inline char* getHelp()        { return help; }
  inline vector<char*> getAliases() { return aliases; }

  static list<Command*> listAll();


private:
  vector<char*> aliases;
  DispFnPtr action;
  char* usage;
  char* description;
  char* help;
};

#endif  // __COMMAND_HH__
