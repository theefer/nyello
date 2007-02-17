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

#include "argument_completion_visitor.h"

#include "interpreter.h"
#include "command.h"
#include "signature.h"
#include "sig_args.h"
#include "argument.h"

namespace cmd_parser {

	argument_completion_visitor::argument_completion_visitor( const std::string& input_ )
		: input( input_ )
	{
	}

	argument_completion_visitor::~argument_completion_visitor()
	{
	}

	void
	argument_completion_visitor::visit( const interpreter& obj )
	{
		boost::char_separator<char> sep( " " );
		tokenizer tok( input, sep );
		const command& cmd( obj.find_command( tok ) );

		start = tok.begin();
		end = tok.end();

		cmd.accept( *this );
	}

	void
	argument_completion_visitor::visit( const command& obj )
	{
		tokeniter args( start );

		++args;

		const std::list< _signature* >& signatures( obj.get_signatures() );
		std::list< _signature* >::const_iterator it;
		for( it = signatures.begin(); it != signatures.end(); ++it ) {
			// Reset the pos token iterator for each signature
			pos = args;
			(*it)->accept( *this );
		}
	}

	void
	argument_completion_visitor::visit( const _signature& obj )
	{
		obj.get_arguments().accept( *this );
	}


	void
	argument_completion_visitor::visit( const sig_args& obj )
	{
		// Match all keywords
		const std::list< boost::shared_ptr< kw_argument > >& keywords( obj.get_keywords() );
		std::list< boost::shared_ptr< kw_argument > >::const_iterator it;
		for( it = keywords.begin(); it != keywords.end(); ++it ) {
			// End of arguments, completion on current keyword argument
			if( pos == end ) {
				(*it)->accept( *this );
				return;
			}
			// Signature mismatch, give up for this signature
			// FIXME: propose as alternative if incomplete match?
			else if( !(*it)->match( pos, end ) ) {
				return;
			}
		}
	}

	void
	argument_completion_visitor::visit( const sig_args_val& obj )
	{
		// Missing the argument object!
		if( !obj.get_value_arg() ) {
			throw incomplete_signature_error("incomplete signature, missing argument definition!");
		}

		// Match all keywords
		const std::list< boost::shared_ptr< kw_argument > >& keywords( obj.get_keywords() );
		std::list< boost::shared_ptr< kw_argument > >::const_iterator it;
		for( it = keywords.begin(); it != keywords.end(); ++it ) {
			// End of arguments, completion on current keyword argument
			if( pos == end ) {
				(*it)->accept( *this );
				return;
			}
			// Signature mismatch, give up for this signature
			// FIXME: propose as alternative if incomplete match?
			else if( !(*it)->match( pos, end ) ) {
				return;
			}
		}

		// End of argument list, completion on value argument
		if( pos == end ) {
			obj.get_value_arg()->accept( *this );
			return;
		}

		// Extract value to skip the tokens and continue
		obj.get_value_arg()->advance( pos, end );

		// Recurse in next_args
		obj.get_next_args().accept( *this );
	}


	void
	argument_completion_visitor::visit( const kw_argument& obj )
	{
		alternatives.push_back( obj.get_keyword() );
	}

	void
	argument_completion_visitor::visit( const _argument& obj )
	{
		// No idea what the value could be, don't guess
		// FIXME: commented workaround: alternatives.push_back( "" );
		obj.complete( alternatives );
	}


}
