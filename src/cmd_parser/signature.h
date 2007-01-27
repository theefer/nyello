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
#include <typeinfo>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "typedefs.h"
#include "exceptions.h"


namespace cmd_parser {

	class _argument;

	template< typename T >
	class argument;

	class kw_argument;


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
			signature3( const std::string& description, boost::function3< R, A1, A2, A3 > f );
			~signature3();

			// FIXME: Before run, check if the signature contains all the required argument objects
			signature2< R, A2, A3 >& operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			signature3< R, A1, A2, A3 >& operator <<( const boost::shared_ptr< kw_argument >& arg );

			void execute( const std::vector< std::string >& arglist ) const;

		private:
			boost::function3< R, A1, A2, A3 > func;

			boost::shared_ptr< argument< A1 > > currarg;
			boost::shared_ptr< signature2< R, A2, A3 > > nextargs;

	};

	template< typename R, typename A1, typename A2 >
	class signature2 : public _signature
	{

		public:
			signature2( const std::string& description, boost::function2< R, A1, A2 > f );
			~signature2();

			signature1< R, A1 >& operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			signature2< R, A1, A2 >& operator <<( const boost::shared_ptr< kw_argument >& arg );

			void execute( const std::vector< std::string >& arglist ) const;

		private:
			boost::function2< R, A1, A2 > func;

			boost::shared_ptr< argument< A1 > > currarg;
			boost::shared_ptr< signature1< R, A2 > > nextargs;

	};

	template< typename R, typename A1 >
	class signature1 : public _signature
	{

		public:
			signature1( const std::string& description, boost::function1< R, A1 > f );
			~signature1();

			signature0< R >& operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			signature1< R, A1 >& operator <<( const boost::shared_ptr< kw_argument >& arg );

			void execute( const std::vector< std::string >& arglist ) const;

		private:
			boost::function1< R, A1 > func;

			boost::shared_ptr< argument< A1 > > currarg;
			boost::shared_ptr< signature0< R > > nextargs;

	};

	template< typename R >
	class signature0 : public _signature
	{

		public:
			signature0( const std::string& description, boost::function0< R > f );
			~signature0();

			signature0< R >& operator <<( const boost::shared_ptr< kw_argument >& arg );

			void execute( const std::vector< std::string >& arglist ) const;

		private:
			boost::function0< R > func;

	};



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


	template< typename R, typename A1, typename A2, typename A3 >
	signature2< R, A2, A3 >&
	signature3< R, A1, A2, A3 >::operator <<( const boost::shared_ptr< argument< A1 > >& arg )
	{
		// FIXME: why a pointer?
		if( !nextargs ) {
			arguments.push_back( arg );
			currarg.reset( arg );
			nextargs.reset( new signature2< R, A2, A3 >() );
		}
		else {
			*nextargs << arg;
		}

		return *nextargs; 
	}

	template< typename R, typename A1, typename A2, typename A3 >
	signature3< R, A1, A2, A3 >&
	signature3< R, A1, A2, A3 >::operator <<( const boost::shared_ptr< kw_argument >& arg )
	{
		if( !nextargs ) {
			arguments.push_back( arg );
		}
		else {
			*nextargs << arg;
		}

		return *this;
	}

	template< typename R, typename A1, typename A2, typename A3 >
	void
	signature3< R, A1, A2, A3 >::execute( const std::vector< std::string >& arglist ) const
	{
		A1 v1( currarg->extract( arglist[0] ) );
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
	signature1< R, A1 >&
	signature2< R, A1, A2 >::operator <<( const boost::shared_ptr< argument< A1 > >& arg )
	{
		if( !nextargs ) {
			arguments.push_back( arg );
			currarg.reset( arg );
			nextargs.reset( new signature1< R, A2 >() );
		}
		else {
			*nextargs << arg;
		}

		return *nextargs; 
	}

	template< typename R, typename A1, typename A2 >
	signature2< R, A1, A2 >&
	signature2< R, A1, A2 >::operator <<( const boost::shared_ptr< kw_argument >& arg )
	{
		if( !nextargs ) {
			arguments.push_back( arg );
		}
		else {
			*nextargs << arg;
		}

		return *this;
	}

	template< typename R, typename A1, typename A2 >
	void
	signature2< R, A1, A2 >::execute( const std::vector< std::string >& arglist ) const
	{
		A1 v1( currarg->extract( arglist[0] ) );
		A2 v2( arg2->extract( arglist[1] ) );

		func( v1, v2 );
	}


	template< typename R, typename A1 >
	signature1< R, A1 >::signature1( const std::string& desc,
	                                 boost::function1<R, A1> f )
		: _signature( desc ), func( f )
	{
	}

	template< typename R, typename A1 >
	signature1< R, A1 >::~signature1()
	{
	}


	template< typename R, typename A1 >
	signature0< R >&
	signature1< R, A1 >::operator <<( const boost::shared_ptr< argument< A1 > >& arg )
	{
		if( !nextargs ) {
			arguments.push_back( arg );
			currarg.reset( arg );
			nextargs.reset( new signature0< R >() );
		}
		else {
			*nextargs << arg;
		}

		return *nextargs; 
	}

	template< typename R, typename A1 >
	signature1< R, A1 >&
	signature1< R, A1 >::operator <<( const boost::shared_ptr< kw_argument >& arg )
	{
		if( !nextargs ) {
			arguments.push_back( arg );
		}
		else {
			*nextargs << arg;
		}

		return *this;
	}

	template< typename R, typename A1 >
	void
	signature1< R, A1 >::execute( const std::vector< std::string >& arglist ) const
	{
		A1 v1( currarg->extract( arglist[0] ) );

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
	signature0< R >&
	signature0< R >::operator <<( const boost::shared_ptr< kw_argument >& arg )
	{
		arguments.push_back( arg );
		return *this;
	}

	template< typename R >
	void
	signature0< R >::execute( const std::vector< std::string >& arglist ) const
	{
		func();
	}

}

#endif  // CMD_PARSER_SIGNATURE_H
