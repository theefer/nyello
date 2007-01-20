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
#include "exceptions.h"

#include <boost/algorithm/string.hpp>
namespace ba = boost::algorithm;

namespace cmd_parser {

	std::string::size_type command::max_name_length = 0;

	command::command( const std::string& n, const std::string& desc )
		: name( n ), aliases(), description( desc ), helptext(), signatures()
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
		helptext = h;

		return *this;
	}

	void
	command::help( std::ostream& os ) const
	{
		os << name;

		if( aliases.size() > 0 ) {
			os << " (";
			std::list< std::string >::const_iterator it;
			for( it = aliases.begin(); it != aliases.end(); ++it ) {
				os << *it;
				if( it != aliases.begin() ) {
					os << ", ";
				}
			}
			os << ")";
		}

		os << ": " << description << std::endl;
		os << "usage: " << name;

		// FIXME: arguments, possibly several signatures

		os << std::endl;

		// FIXME: format?
		os << helptext;

		os << std::endl;

		return;
	}

	bool
	command::match( const tokeniter& start, const tokeniter& end ) const
	{
		if( match_string( name, *start ) ) {
			return true;
		}
		else {
			std::list< std::string >::const_iterator it;
			for( it = aliases.begin(); it != aliases.end(); ++it ) {
				if( match_string( *it, *start ) ) {
					return true;
				}
			}
		}

		return false;
	}

	void
	command::run( const tokeniter& start, const tokeniter& end ) const
	{
		// If failed match, throw, else great!
		tokeniter args( start );
		++args;

		std::list< _signature* >::const_iterator it;
		for( it = signatures.begin(); it != signatures.end(); ++it ) {
			if( (*it)->run( args, end ) ) {
				return;
			}
		}

		throw wrong_signature_error("no matching signature for command '"
		                            + name + "'");
	}

	bool
	command::match_string( const std::string& name, const std::string& cmd ) const
	{
		// FIXME: custom case-sensitivity!
		return name == cmd;
	}
}
