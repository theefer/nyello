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

#ifndef CMD_PARSER_SIGNATURE_H
#define CMD_PARSER_SIGNATURE_H

#include <string>
#include <list>
#include <vector>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "typedefs.h"


namespace cmd_parser {

	class _argument;

	template< typename T >
	class argument;

	class kw_argument;
	typedef boost::shared_ptr< kw_argument > kw_argument_ptr;

/*
	template< typename R >
	class sig_arg0;

	template< typename R, typename S1 >
	class sig_arg1;

	template< typename R, typename S1, typename S2 >
	class sig_arg2;

	template< typename R, typename S1, typename S2, typename S3 >
	class sig_arg3;
*/

	class _signature
	{
		public:
			_signature( const std::string& description );
			virtual ~_signature();

			bool run( const tokeniter& start, const tokeniter& end ) const;

			virtual void execute( const std::vector< std::string >& arglist ) const = 0;

		protected:
			std::list< boost::shared_ptr< _argument > > arguments;

		private:
			std::string description;
	};

	template< typename R, typename A1, typename A2, typename A3 >
	class signature3 : public _signature
	{

		public:
			signature3( const std::string& description, boost::function3<R, A1, A2, A3> f );
			~signature3();

			signature3< R, A1, A2, A3 > operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			signature3< R, A1, A2, A3 > operator <<( const kw_argument_ptr& arg );

			// FIXME: *NOT* type safe at the moment
/*
			sig_arg2< R, A2, A3 > operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			sig_arg3< R, A1, A2, A3 > operator <<( const kw_argument_ptr& arg );
*/

			void execute( const std::vector< std::string >& arglist ) const;

		private:
			boost::function3<R, A1, A2, A3> func;

			boost::shared_ptr< argument< A1 > > arg1;
			boost::shared_ptr< argument< A2 > > arg2;
			boost::shared_ptr< argument< A3 > > arg3;

	};

	template< typename R, typename A1, typename A2 >
	class signature2 : public _signature
	{

		public:
			signature2( const std::string& description, boost::function2<R, A1, A2> f );
			~signature2();

			signature2< R, A1, A2 > operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			signature2< R, A1, A2 > operator <<( const kw_argument_ptr& arg );
/*
			sig_arg1< R, A2 > operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			sig_arg2< R, A1, A2 > operator <<( const kw_argument_ptr& arg );
*/

			void execute( const std::vector< std::string >& arglist ) const;

		private:
			boost::function2<R, A1, A2> func;

			boost::shared_ptr< argument< A1 > > arg1;
			boost::shared_ptr< argument< A2 > > arg2;

	};

	template< typename R, typename A1 >
	class signature1 : public _signature
	{

		public:
			signature1( const std::string& description, boost::function1<R, A1> f );
			~signature1();

			signature1< R, A1 > operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			signature1< R, A1 > operator <<( const kw_argument_ptr& arg );
/*
			sig_arg0< R > operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			sig_arg1< R, A1 > operator <<( const kw_argument_ptr& arg );
*/

			void execute( const std::vector< std::string >& arglist ) const;

			// FIXME: Hack
			int argnum;

		private:
			boost::function1<R, A1> func;

			boost::shared_ptr< argument< A1 > > arg1;

	};

	template< typename R >
	class signature0 : public _signature
	{

		public:
			signature0( const std::string& description, boost::function0<R> f );
			~signature0();

//			signature0< R > operator <<( const boost::shared_ptr< argument< A1 > >& arg );
//			signature0< R > operator <<( const kw_argument_ptr& arg );
/*
//			sig_arg< R > operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			sig_arg0< R > operator <<( const kw_argument_ptr& arg );
*/

			void execute( const std::vector< std::string >& arglist ) const;

		private:
			boost::function0<R> func;

	};

/*
	template< typename R, typename S1, typename S2, typename S3 >
	class sig_arg3
	{
		public:
			sig_arg3( signature3< R, S1, S2, S3 >& sig_ );
			~sig_arg3();

			sig_arg2< R, S2, S3 > operator <<( const boost::shared_ptr< argument< S1 > >& arg );
			sig_arg3< R, S1, S2, S3 > operator << ( const kw_argument_ptr& arg );

		private:
			signature3< R, S1, S2, S3 >& sig;
	};
*/


	template< typename R, typename A1, typename A2, typename A3 >
	signature3< R, A1, A2, A3 >::signature3( const std::string& desc,
	                                         boost::function3<R, A1, A2, A3> f )
		: _signature( desc ), func( f )
	{
	}

	template< typename R, typename A1, typename A2, typename A3 >
	signature3< R, A1, A2, A3 >::~signature3()
	{
	}

	// FIXME: we need to record val args in argN !

	template< typename R, typename A1, typename A2, typename A3 >
	signature3< R, A1, A2, A3 >
	signature3< R, A1, A2, A3 >::operator <<( const boost::shared_ptr< argument< A1 > >& arg )
	{
		arguments.push_back( arg );
		return *this;
	}

	template< typename R, typename A1, typename A2, typename A3 >
	signature3< R, A1, A2, A3 >
	signature3< R, A1, A2, A3 >::operator <<( const kw_argument_ptr& arg )
	{
		arguments.push_back( arg );
		return *this;
	}

	template< typename R, typename A1, typename A2, typename A3 >
	void
	signature3< R, A1, A2, A3 >::execute( const std::vector< std::string >& arglist ) const
	{
		// FIXME: check if arglist.size() == 3 ?

		A1 v1( arg1->extract( arglist[0] ) );
		A2 v2( arg2->extract( arglist[1] ) );
		A3 v3( arg3->extract( arglist[2] ) );

		func( v1, v2, v3 );
	}



	template< typename R, typename A1, typename A2 >
	signature2< R, A1, A2 >::signature2( const std::string& desc,
	                                     boost::function2<R, A1, A2> f )
		: _signature( desc ), func( f )
	{
	}

	template< typename R, typename A1, typename A2 >
	signature2< R, A1, A2 >::~signature2()
	{
	}


	template< typename R, typename A1, typename A2 >
	signature2< R, A1, A2 >
	signature2< R, A1, A2 >::operator <<( const boost::shared_ptr< argument< A1 > >& arg )
	{
		arguments.push_back( arg );
		return *this;
	}

	template< typename R, typename A1, typename A2 >
	signature2< R, A1, A2 >
	signature2< R, A1, A2 >::operator <<( const kw_argument_ptr& arg )
	{
		arguments.push_back( arg );
		return *this;
	}

	template< typename R, typename A1, typename A2 >
	void
	signature2< R, A1, A2 >::execute( const std::vector< std::string >& arglist ) const
	{
		// FIXME: check if arglist.size() == 2 ?

		A1 v1( arg1->extract( arglist[0] ) );
		A2 v2( arg2->extract( arglist[1] ) );

		func( v1, v2 );
	}


	template< typename R, typename A1 >
	signature1< R, A1 >::signature1( const std::string& desc,
	                                 boost::function1<R, A1> f )
		: _signature( desc ), func( f )
	{
		// FIXME: hack!
		argnum = 0;
	}

	template< typename R, typename A1 >
	signature1< R, A1 >::~signature1()
	{
	}


	template< typename R, typename A1 >
	signature1< R, A1 >
	signature1< R, A1 >::operator <<( const boost::shared_ptr< argument< A1 > >& arg )
	{
		// FIXME: Horrible hack ; we need a type-safe way to do that!
		switch( argnum ) {
		case 0:  arg1 = arg; ++argnum;                 break;
		default: throw "woops, too many arguments";    break;
		}

		arguments.push_back( arg );
		return *this;
	}

	template< typename R, typename A1 >
	signature1< R, A1 >
	signature1< R, A1 >::operator <<( const kw_argument_ptr& arg )
	{
		arguments.push_back( arg );
		return *this;
	}

	template< typename R, typename A1 >
	void
	signature1< R, A1 >::execute( const std::vector< std::string >& arglist ) const
	{
		// FIXME: check if arglist.size() == 1 ?

		A1 v1( arg1->extract( arglist[0] ) );

		func( v1 );
	}


	template< typename R >
	signature0< R >::signature0( const std::string& desc,
	                             boost::function0<R> f )
		: _signature( desc ), func( f )
	{
	}

	template< typename R >
	signature0< R >::~signature0()
	{
	}

	template< typename R >
	void
	signature0< R >::execute( const std::vector< std::string >& arglist ) const
	{
		// FIXME: check if arglist.size() == 0 ?

		func();
	}

/*

	template< typename R, typename A1, typename A2, typename A3 >
	sig_arg< A2, A3 >
	signature< R, A1, A2, A3 >::operator <<( const boost::shared_ptr< argument< A1 > >& arg )
	{
		arguments.push_back( arg );
		return sig_arg< A2, A3 >( *this );
	}

	template< typename R, typename A1, typename A2, typename A3 >
	sig_arg< A1, A2, A3 >
	signature< R, A1, A2, A3 >::operator <<( const kw_argument_ptr& arg )
	{
		arguments.push_back( arg );
		return sig_arg< A1, A2, A3 >( *this );
	}

	template< typename S1, typename S2, typename S3 >
	sig_arg< S1, S2, S3 >::sig_arg( signature< S1, S2, S3 >& s )
		: sig( s )
	{
	}

	template< typename S1, typename S2, typename S3 >
	sig_arg< S1, S2, S3 >::~sig_arg()
	{
	}

	template< typename S1, typename S2, typename S3 >
	sig_arg< S2, S3 >
	sig_arg< S1, S2, S3 >::operator <<( const boost::shared_ptr< argument< S1 > >& arg )
	{
		return (sig << arg );
	}

	template< typename S1, typename S2, typename S3 >
	sig_arg< S1, S2, S3 >
	sig_arg< S1, S2, S3 >::operator << ( const kw_argument_ptr& arg )
	{
		return (sig << arg);
	}
*/
}

#endif  // CMD_PARSER_SIGNATURE_H
