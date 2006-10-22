/* nyello - an advanced command-line client for XMMS2
 * Copyright (C) 2006  Sébastien Cevey
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */


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
