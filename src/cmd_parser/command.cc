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

#include "command.h"
#include "signature.h"

#include <boost/algorithm/string.hpp>
namespace ba = boost::algorithm;

namespace cmd_parser {

	std::string::size_type command::max_name_length = 0;

	command::command( const std::string& n, const std::string& desc )
		: name( n ), aliases(), description( desc ), help(), signatures()
	{
		if( name.size() > max_name_length ) {
			max_name_length = name.size();
		}
	}

	command::~command()
	{
		// Delete signatures
		std::list< _signature* >::iterator it;
		for( it = signatures.begin(); it != signatures.end(); ++it ) {
			delete *it;
		}
	}

	command&
	command::add_alias( const std::string& alias )
	{
		aliases.push_back( alias );

		return *this;
	}

	command&
	command::set_help( const std::string& h )
	{
		help = h;

		return *this;
	}

	bool
	command::match( const std::string& input ) const
	{
		// FIXME: Try to match command name or aliases
		// If no match, abort, if failed match, throw, else great!
		if( match_command( input ) ) {
			std::list< _signature* >::const_iterator it;
			for( it = signatures.begin(); it != signatures.end(); ++it ) {
				if( (*it)->run( input ) ) {
					return true;
				}
			}

			throw wrong_signature_error("no matching signature for command '"
			                            + name + "'");
		}

		return false;
	}

	bool
	command::match_command( const std::string& input ) const
	{
		if( match_string( name, input ) ) {
			return true;
		}
		else {
			std::list< std::string >::const_iterator it;
			for( it = aliases.begin(); it != aliases.end(); ++it ) {
				if( match_string( *it, input ) ) {
					return true;
				}
			}
		}

		return false;
	}

	bool
	command::match_string( const std::string& cmd, const std::string& input ) const
	{
		// FIXME: better, faster start detection!
		return ( ba::equals( cmd, input ) || ba::istarts_with( cmd + " ", input ) );
	}
}
