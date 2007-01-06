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

#ifndef CMD_PARSER_INTERPRETER_H
#define CMD_PARSER_INTERPRETER_H

#include <string>
#include <list>
#include <iostream>


namespace cmd_parser {

	class command;

	class interpreter
	{

		public:
			interpreter();
			~interpreter();

			void run( const std::string& input ) const;

			void gen_help( std::ostream& os ) const;
			void gen_help( const std::string& cmd, std::ostream& os ) const;

			command& add_command( const std::string& cmd_name, const std::string& cmd_desc );

		private:
			std::list< command* > commands;

	};

}

#endif  // CMD_PARSER_INTERPRETER_H
