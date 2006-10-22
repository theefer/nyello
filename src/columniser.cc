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


#include "columniser.hh"


Columniser::Columniser()
  : frameChar('-'), selectedPrefix("* "), unselectedPrefix("  ") {
  width = strlen(unselectedPrefix);
}

Columniser::~Columniser() {
  // Free the column objects
  ColumnList::iterator it;
  for(it = columns.begin(); it != columns.end(); ++it) {
    delete (*it);
  }
}

/**
 * Reset all columns.  To be used before each new sequence of outputs.
 */
void
Columniser::reset() {
  ColumnList::iterator it;
  for(it = columns.begin(); it != columns.end(); ++it) {
    (*it)->reset();
  }
}

void
Columniser::printHeader() {
  cout.fill(frameChar);
  cout.width(width);
  cout << frameChar << endl;
  cout.fill(' ');
}

void
Columniser::printHeader(char* message) {
  // FIXME: Horrible hack!!
  char* buffer = new char[strlen(message) + 5];
  strcpy(buffer, "[ ");
  strcpy(buffer+2, message);
  strcpy(buffer+2+strlen(message), " ]");

  cout.flags(ios_base::left);
  cout << frameChar << frameChar;
  cout.width(width - 2);
  cout.fill(frameChar);
  cout << buffer << endl;
  cout.fill(' ');

  // FIXME: Horrible hack!!
  delete buffer;
}


void
Columniser::printFooter() {
  cout.fill(frameChar);
  cout.width(width);
  cout << frameChar << endl;
  cout.fill(' ');
}

void
Columniser::printFooter(char* message) {
  // FIXME: Horrible hack!!
  char* buffer = new char[strlen(message) + 5];
  strcpy(buffer, "[ ");
  strcpy(buffer+2, message);
  strcpy(buffer+2+strlen(message), " ]");

  cout.flags(ios_base::right);
  cout.fill(frameChar);
  cout.width(width - 2);
  cout << buffer << frameChar << frameChar << endl;
  cout.fill(' ');

  delete buffer;
}


void
Columniser::printHeadings() {
  cout.flags(ios_base::left);
  cout << unselectedPrefix;

  ColumnList::iterator it;
  for(it = columns.begin(); it != columns.end(); ++it) {
    cout.flags((*it)->getAlignment());
    cout << (*it)->getPrefix();
    cout.width((*it)->getWidth());
    cout << (*it)->getHeading();
    cout << (*it)->getSuffix();
  }

  cout << endl;
}


void
Columniser::printRow(Printable* entry) {
  // Print un/selected prefix
  cout.flags(ios_base::left);
  cout << (entry->isSelected() ? selectedPrefix : unselectedPrefix);

  char* tmp;
  ColumnList::iterator it;
  for(it = columns.begin(); it != columns.end(); ++it) {
    cout.flags((*it)->getAlignment());
    cout.fill((*it)->getFiller());
    cout << (*it)->getPrefix();

    // FIXME: We want padding AND length trimming! -- but CLEAN!
    tmp = (*it)->fetchValue(entry);
    if(tmp != NULL) {
      if(strlen(tmp) > (*it)->getWidth()) {
        cout.write(tmp, (*it)->getWidth() - 2);
        cout << "..";
      }
      else {
        cout.width((*it)->getWidth());
        cout << tmp;
      }
    }
    else {
      cout.width((*it)->getWidth());
      cout << " ";
    }

    cout << (*it)->getSuffix();
  }

  cout << endl;
}


void
Columniser::setUnselectedPrefix(char* prefix) {
  width -= strlen(unselectedPrefix);
  unselectedPrefix = prefix;
  width += strlen(unselectedPrefix);
}


void
Columniser::addColumn(Column* col) {
  columns.push_back(col);
  width += strlen(col->getPrefix()) + strlen(col->getSuffix());
  width += col->getWidth();
}
