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

	class val_argument;
	class kw_argument;
	typedef boost::shared_ptr< val_argument > val_argument_ptr;
	typedef boost::shared_ptr< kw_argument > kw_argument_ptr;


	class _signature
	{
		public:
			_signature( const std::string& description );
			virtual ~_signature();

			bool run( const tokeniter& start, const tokeniter& end ) const;

			virtual void execute( const std::vector< std::string >& arglist ) const = 0;

		protected:
			std::list< boost::shared_ptr< _argument > > arguments;

			// Count value arguments
			int argcount;

			// FIXME: Check if the signature contains all the required argument objects

		private:
			std::string description;
	};

	template< typename R, typename A1, typename A2, typename A3 >
	class signature3 : public _signature
	{

		public:
			signature3( const std::string& description, boost::function3<R, A1, A2, A3> f );
			~signature3();

			// FIXME: conflicting overloading -- temp hack using simple pointers, probably not safe
//			signature3< R, A1, A2, A3 >& operator <<( const val_argument_ptr& arg );
//			signature3< R, A1, A2, A3 >& operator <<( const kw_argument_ptr& arg );
			signature3< R, A1, A2, A3 >& operator <<( val_argument* arg );
			signature3< R, A1, A2, A3 >& operator <<( kw_argument* arg );

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

			signature2< R, A1, A2 >& operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			signature2< R, A1, A2 >& operator <<( const kw_argument_ptr& arg );

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

			signature1< R, A1 >& operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			signature1< R, A1 >& operator <<( const kw_argument_ptr& arg );

			void execute( const std::vector< std::string >& arglist ) const;

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

			signature0< R >& operator <<( const kw_argument_ptr& arg );

			void execute( const std::vector< std::string >& arglist ) const;

		private:
			boost::function0<R> func;

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
	signature3< R, A1, A2, A3 >&
	signature3< R, A1, A2, A3 >::operator <<( val_argument* arg )
	{
		// Check type by casting and storing it
		try {
			switch( argcount ) {
			case 0:
				arg1 = boost::shared_ptr< argument< A1 > >( dynamic_cast< argument< A1 >* >( arg ) );
				arguments.push_back( arg1 );
				break;
			case 1:
				arg2 = boost::shared_ptr< argument< A2 > >( dynamic_cast< argument< A2 >* >( arg ) );
				arguments.push_back( arg2 );
				break;
			case 2:
				arg3 = boost::shared_ptr< argument< A3 > >( dynamic_cast< argument< A3 >* >( arg ) );
				arguments.push_back( arg3 );
				break;
			default:
				throw too_many_arguments_error( "cannot add more arguments" );
				break;
			}
		}
		catch( std::bad_cast& e ) {
			throw incompatible_argument_error( "argument object incompatible with signature" );
		}
		// Forward too many arguments error
		catch( too_many_arguments_error& ) {
			throw;
		}

		++argcount;

		return *this;
	}
/*
	template< typename R, typename A1, typename A2, typename A3 >
	signature3< R, A1, A2, A3 >&
	signature3< R, A1, A2, A3 >::operator <<( const val_argument_ptr& arg )
	{
		// Check type by casting and storing it
		try {
			switch( argcount ) {
			case 0:
				arg1 = dynamic_cast< const boost::shared_ptr< argument< A1 > >& >( arg );
				break;
			case 1:
				arg2 = dynamic_cast< const boost::shared_ptr< argument< A2 > >& >( arg );
				break;
			case 2:
				arg3 = dynamic_cast< const boost::shared_ptr< argument< A3 > >& >( arg );
				break;
			default:
				throw too_many_arguments_error( "cannot add more arguments" );
				break;
			}
		}
		catch( std::bad_cast& e ) {
			throw incompatible_argument_error( "argument object incompatible with signature" );
		}
		// Forward too many arguments error
		catch( too_many_arguments_error& ) {
			throw;
		}

		++argcount;

		arguments.push_back( arg );
		return *this;
	}
*/
	template< typename R, typename A1, typename A2, typename A3 >
	signature3< R, A1, A2, A3 >&
	signature3< R, A1, A2, A3 >::operator <<( kw_argument* arg )
	{
		arguments.push_back( boost::shared_ptr< kw_argument >( arg ) );
		return *this;
	}

	template< typename R, typename A1, typename A2, typename A3 >
	void
	signature3< R, A1, A2, A3 >::execute( const std::vector< std::string >& arglist ) const
	{
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
	signature2< R, A1, A2 >&
	signature2< R, A1, A2 >::operator <<( const boost::shared_ptr< argument< A1 > >& arg )
	{
		// Check type by casting and storing it
		try {
			switch( argcount ) {
			case 0:
				arg1 = dynamic_cast< const boost::shared_ptr< argument< A1 > >& >( arg );
				break;
			case 1:
				arg2 = dynamic_cast< const boost::shared_ptr< argument< A2 > >& >( arg );
				break;
			default:
				throw too_many_arguments_error( "cannot add more arguments" );
				break;
			}
		}
		catch( std::bad_cast& e ) {
			throw incompatible_argument_error( "argument object incompatible with signature" );
		}
		// Forward too many arguments error
		catch( too_many_arguments_error& ) {
			throw;
		}

		++argcount;

		arguments.push_back( arg );
		return *this;
	}

	template< typename R, typename A1, typename A2 >
	signature2< R, A1, A2 >&
	signature2< R, A1, A2 >::operator <<( const kw_argument_ptr& arg )
	{
		arguments.push_back( arg );
		return *this;
	}

	template< typename R, typename A1, typename A2 >
	void
	signature2< R, A1, A2 >::execute( const std::vector< std::string >& arglist ) const
	{
		A1 v1( arg1->extract( arglist[0] ) );
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
	signature1< R, A1 >&
	signature1< R, A1 >::operator <<( const boost::shared_ptr< argument< A1 > >& arg )
	{
		// Check type by casting and storing it
		try {
			switch( argcount ) {
			case 0:
				arg1 = dynamic_cast< const boost::shared_ptr< argument< A1 > >& >( arg );
				break;
			default:
				throw too_many_arguments_error( "cannot add more arguments" );
				break;
			}
		}
		catch( std::bad_cast& e ) {
			throw incompatible_argument_error( "argument object incompatible with signature" );
		}
		// Forward too many arguments error
		catch( too_many_arguments_error& ) {
			throw;
		}

		++argcount;

		arguments.push_back( arg );
		return *this;
	}

	template< typename R, typename A1 >
	signature1< R, A1 >&
	signature1< R, A1 >::operator <<( const kw_argument_ptr& arg )
	{
		arguments.push_back( arg );
		return *this;
	}

	template< typename R, typename A1 >
	void
	signature1< R, A1 >::execute( const std::vector< std::string >& arglist ) const
	{
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
	signature0< R >&
	signature0< R >::operator <<( const kw_argument_ptr& arg )
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
