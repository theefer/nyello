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

#ifndef CMD_PARSER_SIG_ARGS_H
#define CMD_PARSER_SIG_ARGS_H

#include <list>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>

#include "typedefs.h"
#include "exceptions.h"
#include "argument.h"

namespace cmd_parser {


	class kw_argument;

	template< typename > class argument;


	template< typename > class signature0;
	template< typename, typename > class signature1;
	template< typename, typename, typename > class signature2;
	template< typename, typename, typename, typename > class signature3;

	template< typename > class sig_args0;
	template< typename, typename > class sig_args1;
	template< typename, typename, typename > class sig_args2;
	template< typename, typename, typename, typename > class sig_args3;


	template< typename R >
	class sig_args0
	{
		public:
			// FIXME: Should be private ideally
			sig_args0();
			virtual ~sig_args0();

			sig_args0< R >& operator <<( const boost::shared_ptr< kw_argument >& arg );

			R apply( const boost::function0< R >& callback,
			         const tokeniter& start, const tokeniter& end ) const;

		private:
			friend class ::cmd_parser::signature0< R >;

			std::list< boost::shared_ptr< kw_argument > > keywords;

	};

	template< typename R, typename A1 >
	class sig_args1
	{
		public:
			// FIXME: Should be private ideally
			sig_args1();
			virtual ~sig_args1();

			sig_args0< R >& operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			sig_args1< R, A1 >& operator <<( const boost::shared_ptr< kw_argument >& arg );

			R apply( const boost::function1< R, A1 >& callback,
			         const tokeniter& start, const tokeniter& end ) const;

		private:
			friend class ::cmd_parser::signature0< R >;
			friend class ::cmd_parser::signature1< R, A1 >;

			std::list< boost::shared_ptr< kw_argument > > keywords;
			boost::shared_ptr< argument< A1 > > value_arg;
			sig_args0< R > next_args;

	};

	template< typename R, typename A1, typename A2 >
	class sig_args2
	{
		public:
			// FIXME: Should be private ideally
			sig_args2();
			virtual ~sig_args2();

			sig_args1< R, A2 >& operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			sig_args2< R, A1, A2 >& operator <<( const boost::shared_ptr< kw_argument >& arg );

			R apply( const boost::function2< R, A1, A2 >& callback,
			         const tokeniter& start, const tokeniter& end ) const;

		private:
			friend class ::cmd_parser::signature0< R >;
			friend class ::cmd_parser::signature1< R, A1 >;
			friend class ::cmd_parser::signature2< R, A1, A2 >;

			std::list< boost::shared_ptr< kw_argument > > keywords;
			boost::shared_ptr< argument< A1 > > value_arg;
			sig_args1< R, A2 > next_args;

	};

	template< typename R, typename A1, typename A2, typename A3 >
	class sig_args3
	{
		public:
			// FIXME: Should be private ideally
			sig_args3();
			virtual ~sig_args3();

			sig_args2< R, A2, A3 >& operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			sig_args3< R, A1, A2, A3 >& operator <<( const boost::shared_ptr< kw_argument >& arg );

			R apply( const boost::function3< R, A1, A2, A3 >& callback,
			         const tokeniter& start, const tokeniter& end ) const;

		private:
			friend class ::cmd_parser::signature0< R >;
			friend class ::cmd_parser::signature1< R, A1 >;
			friend class ::cmd_parser::signature2< R, A1, A2 >;
			friend class ::cmd_parser::signature3< R, A1, A2, A3 >;

			std::list< boost::shared_ptr< kw_argument > > keywords;
			boost::shared_ptr< argument< A1 > > value_arg;
			sig_args2< R, A2, A3 > next_args;

	};




	template< typename R >
	sig_args0< R >::sig_args0()
		: keywords()
	{
	}

	template< typename R >
	sig_args0< R >::~sig_args0()
	{
	}

	template< typename R >
	sig_args0< R >&
	sig_args0< R >::operator <<( const boost::shared_ptr< kw_argument >& arg )
	{
		keywords.push_back( arg );
		return *this;
	}

	template< typename R >
	R
	sig_args0< R >::apply( const boost::function0< R >& callback,
	                       const tokeniter& start, const tokeniter& end ) const
	{
		tokeniter pos( start );

		// Match all keywords
		std::list< boost::shared_ptr< kw_argument > >::const_iterator it;
		for( it = keywords.begin(); it != keywords.end(); ++it ) {
			if( !(*it)->match( pos, end ) ) {
				// Signature mismatch, failed to parse an argument
				// FIXME: including "too few argument values!"
				// FIXME: throw?
			}
		}

		// End of recursion, call the function
		return callback();
	}


	template< typename R, typename A1 >
	sig_args1< R, A1 >::sig_args1()
		: keywords(), value_arg(), next_args()
	{
	}

	template< typename R, typename A1 >
	sig_args1< R, A1 >::~sig_args1()
	{
	}

	template< typename R, typename A1 >
	sig_args0< R >&
	sig_args1< R, A1 >::operator <<( const boost::shared_ptr< argument< A1 > >& arg )
	{

		// No value arg yet, save this one
		if( !value_arg ) {
			value_arg = arg;
		}
		// FIXME: Else, we're adding too many arguments to the signature! or assert in sig_args0 ?

		return next_args;
	}

	template< typename R, typename A1 >
	sig_args1< R, A1 >&
	sig_args1< R, A1 >::operator <<( const boost::shared_ptr< kw_argument >& arg )
	{
		keywords.push_back( arg );
		return *this;
	}

	template< typename R, typename A1 >
	R
	sig_args1< R, A1 >::apply( const boost::function1< R, A1 >& callback,
	                           const tokeniter& start, const tokeniter& end ) const
	{
		tokeniter pos( start );

		// Missing the argument object!
		if( !value_arg ) {
			throw incomplete_signature_error("incomplete signature, missing argument definition!");
		}

		// Match all keywords
		std::list< boost::shared_ptr< kw_argument > >::const_iterator it;
		for( it = keywords.begin(); it != keywords.end(); ++it ) {
			if( !(*it)->match( pos, end ) ) {
				// Signature mismatch, failed to parse an argument
				// FIXME: including "too few argument values!"
				// FIXME: throw?
			}
		}

		// Extract value
		A1 value( value_arg->extract( pos, end ) );

		// Recurse in next_args
		return next_args.apply( boost::bind( callback, value ), pos, end );
	}


	template< typename R, typename A1, typename A2 >
	sig_args2< R, A1, A2 >::sig_args2()
		: keywords(), value_arg(), next_args()
	{
	}

	template< typename R, typename A1, typename A2 >
	sig_args2< R, A1, A2 >::~sig_args2()
	{
	}

	template< typename R, typename A1, typename A2 >
	sig_args1< R, A2 >&
	sig_args2< R, A1, A2 >::operator <<( const boost::shared_ptr< argument< A1 > >& arg )
	{
		// No value arg yet, save this one
		if( !value_arg ) {
			value_arg = arg;
		}
		else {
			next_args << arg;
		}

		return next_args;
	}

	template< typename R, typename A1, typename A2 >
	sig_args2< R, A1, A2 >&
	sig_args2< R, A1, A2 >::operator <<( const boost::shared_ptr< kw_argument >& arg )
	{
		keywords.push_back( arg );
		return *this;
	}

	template< typename R, typename A1, typename A2 >
	R
	sig_args2< R, A1, A2 >::apply( const boost::function2< R, A1, A2 >& callback,
	                               const tokeniter& start, const tokeniter& end ) const
	{
		tokeniter pos( start );

		// Missing the argument object!
		if( !value_arg ) {
			throw incomplete_signature_error("incomplete signature, missing argument definition!");
		}

		// Match all keywords
		std::list< boost::shared_ptr< kw_argument > >::const_iterator it;
		for( it = keywords.begin(); it != keywords.end(); ++it ) {
			if( !(*it)->match( pos, end ) ) {
				// Signature mismatch, failed to parse an argument
				// FIXME: including "too few argument values!"
				// FIXME: throw?
			}
		}

		// Extract value
		A1 value( value_arg->extract( pos, end ) );

		// Recurse in next_args
		return next_args.apply( boost::bind( callback, value, _1 ), pos, end );
	}



	template< typename R, typename A1, typename A2, typename A3 >
	sig_args3< R, A1, A2, A3 >::sig_args3()
		: keywords(), value_arg(), next_args()
	{
	}

	template< typename R, typename A1, typename A2, typename A3 >
	sig_args3< R, A1, A2, A3 >::~sig_args3()
	{
	}

	template< typename R, typename A1, typename A2, typename A3 >
	sig_args2< R, A2, A3 >&
	sig_args3< R, A1, A2, A3 >::operator <<( const boost::shared_ptr< argument< A1 > >& arg )
	{
		// No value arg yet, save this one
		if( !value_arg ) {
			value_arg = arg;
		}
		else {
			next_args << arg;
		}

		return next_args;
	}

	template< typename R, typename A1, typename A2, typename A3 >
	sig_args3< R, A1, A2, A3 >&
	sig_args3< R, A1, A2, A3 >::operator <<( const boost::shared_ptr< kw_argument >& arg )
	{
		keywords.push_back( arg );
		return *this;
	}

	template< typename R, typename A1, typename A2, typename A3 >
	R
	sig_args3< R, A1, A2, A3 >::apply( const boost::function3< R, A1, A2, A3 >& callback,
	                                   const tokeniter& start, const tokeniter& end ) const
	{
		tokeniter pos( start );

		// Missing the argument object!
		if( !value_arg ) {
			throw incomplete_signature_error("incomplete signature, missing argument definition!");
		}

		// Match all keywords
		std::list< boost::shared_ptr< kw_argument > >::const_iterator it;
		for( it = keywords.begin(); it != keywords.end(); ++it ) {
			if( !(*it)->match( pos, end ) ) {
				// Signature mismatch, failed to parse an argument
				// FIXME: including "too few argument values!"
				// FIXME: throw?
			}
		}

		// Extract value
		A1 value( value_arg->extract( pos, end ) );

		// Recurse in next_args
		return next_args.apply( boost::bind( callback, value, _1, _2 ), pos, end );
	}


	// FIXME: in sig_args0, match keywords and then run func!
}

#endif  // CMD_PARSER_SIG_ARGS_H
