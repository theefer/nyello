/* cmd_parser - a typed command parser
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

#include "exceptions.h"

#include <stdexcept>
#include <string>
using std::string;


namespace cmd_parser {

	wrong_signature_error::wrong_signature_error( const string& what_arg ) :
		runtime_error( what_arg )
	{
	}

	command_not_found_error::command_not_found_error( const string& what_arg ) :
		runtime_error( what_arg )
	{
	}

}
