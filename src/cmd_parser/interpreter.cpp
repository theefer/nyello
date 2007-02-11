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

#include "interpreter.h"
#include "command.h"
#include "exceptions.h"

#include <iostream>
using namespace std;


namespace cmd_parser {

	interpreter::interpreter()
		: commands()
	{
	}

	interpreter::~interpreter()
	{
		// Delete commands
		std::list< command* >::iterator it;
		for( it = commands.begin(); it != commands.end(); ++it ) {
			delete *it;
		}
	}

	void
	interpreter::run( const std::string& input ) const
	{
		command* cmd;

		boost::char_separator<char> sep( " " );
		tokenizer tok( input, sep );
		cmd = find_command( tok );

		if( cmd != NULL ) {
			cmd->run( tok.begin(), tok.end() );
		}
		else {
			throw command_not_found_error("no command matches input: "
		                                  + input);
		}
	}

	void
	interpreter::help( std::ostream& os ) const
	{
		os << "Available commands:" << std::endl;

		std::list< command* >::const_iterator it;
		for( it = commands.begin(); it != commands.end(); ++it ) {
			os << "   ";
			os.width( (*it)->get_max_name_length() );
			os << (*it)->get_name() << "   "
			   << (*it)->get_description() << std::endl;
		}
	}

	void
	interpreter::help( const std::string& cmd_name, std::ostream& os ) const
	{
		command* cmd;

		boost::char_separator<char> sep( " " );
		tokenizer tok( cmd_name, sep );
		cmd = find_command( tok );

		if( cmd != NULL ) {
			cmd->help( os );
		}
		else {
			// FIXME: error?
			os << "no command named: " << cmd_name << std::endl;
		}

	}

	command&
	interpreter::add_command( const std::string& cmd_name, const std::string& cmd_desc )
	{
		command* cmd( new command( cmd_name, cmd_desc ) );

		commands.push_back( cmd );

		return *cmd;
	}

	command*
	interpreter::find_command( const tokenizer& tokens ) const
	{

		std::list< command* >::const_iterator it;
		for( it = commands.begin(); it != commands.end(); ++it ) {
			if( (*it)->match( tokens.begin(), tokens.end() ) ) {
				return *it;
			}
		}

		return NULL;
	}
}
