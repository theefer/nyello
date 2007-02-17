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

#ifndef CMD_PARSER_ARGUMENT_COMPLETION_VISITOR_H
#define CMD_PARSER_ARGUMENT_COMPLETION_VISITOR_H

#include <list>
#include <string>

#include "visitor.h"
#include "typedefs.h"

namespace cmd_parser {

	class argument_completion_visitor : public visitor
	{
		public:
			argument_completion_visitor( const std::string& input_ );
			~argument_completion_visitor();

			const std::list< std::string >& get_completions() const
			{ return alternatives; }

			void visit( const interpreter& obj );
			void visit( const command& obj );
			void visit( const _signature& obj );
			void visit( const sig_args& obj );
			void visit( const sig_args_val& obj );
			void visit( const kw_argument& obj );

			void visit( const _argument& obj );

		protected:
			const std::string input;
			tokeniter start, pos, end;
			std::list< std::string > alternatives;
	};

}

#endif  // CMD_PARSER_ARGUMENT_COMPLETION_VISITOR_H
