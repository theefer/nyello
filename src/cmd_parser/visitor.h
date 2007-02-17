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

#ifndef CMD_PARSER_VISITOR_H
#define CMD_PARSER_VISITOR_H

namespace cmd_parser {

	class interpreter;
	class command;
	class _signature;
	class sig_args;
	class sig_args_val;
	class kw_argument;

//	template< typename > class argument;
	class _argument;


	class visitor
	{
		public:
			virtual ~visitor() {}

			virtual void visit( const interpreter& obj ) = 0;
			virtual void visit( const command& obj ) = 0;
			virtual void visit( const _signature& obj ) = 0;
			virtual void visit( const sig_args& obj ) = 0;
			virtual void visit( const sig_args_val& obj ) = 0;

			virtual void visit( const kw_argument& obj ) = 0;

//			template< typename T >
//			virtual void visit( const argument< T >& obj ) = 0;
			virtual void visit( const _argument& obj ) = 0;

	};

}

#endif  // CMD_PARSER_VISITOR_H
