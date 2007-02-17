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

#include "command_completion_visitor.h"

#include "interpreter.h"
#include "command.h"

namespace cmd_parser {

	command_completion_visitor::command_completion_visitor()
	{
	}

	command_completion_visitor::~command_completion_visitor()
	{
	}

	void
	command_completion_visitor::visit( const interpreter& obj )
	{
		const std::list< command* >& cmds( obj.get_commands() );
		std::list< command* >::const_iterator it;
		for( it = cmds.begin(); it != cmds.end(); ++it ) {
			(*it)->accept( *this );
		}
	}

	void
	command_completion_visitor::visit( const command& obj )
	{
		// FIXME: push aliases too?
		alternatives.push_back( obj.get_name() );
	}

}
