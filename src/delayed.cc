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


#include "delayed.hh"


Delayed<void>::Delayed(xmmsc_result_t* res, string err)
  : AbstractDelayed<void>(res, err) {
  pmaker = new VoidProduct();
}

Delayed<void>::Delayed(xmmsc_result_t* res, const char* err)
  : AbstractDelayed<void>(res, err) {
  pmaker = new VoidProduct();
}

void
Delayed<void>::createProduct() {
  pmaker->create();
}

void
Delayed<void>::runCallbacks() {
  while(receivers.size() > 0) {
    receivers.front()->run();
    delete receivers.front();
    receivers.pop_front();
  }
}
