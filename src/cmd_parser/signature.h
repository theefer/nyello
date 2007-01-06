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
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>


namespace cmd_parser {

	class _argument;

	template< typename T >
	class argument;

	class kw_argument;
	typedef boost::shared_ptr< kw_argument > kw_argument_ptr;
/*
	template< typename S1, typename S2, typename S3 >
	class sig_arg;
*/

	class _signature
	{
		public:
			_signature( const std::string& description );
			virtual ~_signature();

			bool run( const std::string& input ) const;

		protected:
			virtual void execute() const = 0;

			// FIXME: Or static arguments, 'cause we need to call func later and ignore kw!
			std::list< boost::shared_ptr< _argument > > arguments;

		private:
			std::string description;
	};

	template< typename R, typename A1, typename A2, typename A3 >
	class signature3 : public _signature
	{
		struct parameters
		{
			A1 p1;
			A2 p2;
			A3 p3;
		};

		public:
			signature3( const std::string& description, boost::function3<R, A1, A2, A3> f );
			~signature3();

			signature3< R, A1, A2, A3 > operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			signature3< R, A1, A2, A3 > operator <<( const kw_argument_ptr& arg );
/*
			sig_arg< A2, A3 > operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			sig_arg< A1, A2, A3 > operator <<( const kw_argument_ptr& arg );
*/

		protected:
			void execute() const;

		private:
			boost::function3<R, A1, A2, A3> func;

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
			sig_arg< A2, A3 > operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			sig_arg< A1, A2, A3 > operator <<( const kw_argument_ptr& arg );
*/

		protected:
			void execute() const;

		private:
			boost::function2<R, A1, A2> func;

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
			sig_arg< A2, A3 > operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			sig_arg< A1, A2, A3 > operator <<( const kw_argument_ptr& arg );
*/

		protected:
			void execute() const;

		private:
			boost::function1<R, A1> func;

	};

/*
	template< typename S1, typename S2, typename S3 >
	class sig_arg
	{
		public:
			sig_arg( signature< S1, S2, S3 >& sig );
			~sig_arg();

			sig_arg< S2, S3 > operator <<( const boost::shared_ptr< argument< S1 > >& arg );
			sig_arg< S1, S2, S3 > operator << ( const kw_argument_ptr& arg );

		private:
			signature< S1, S2, S3 >& sig;
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
	signature3< R, A1, A2, A3 >::execute() const// parameters params )
	{
		//func( params.p1, params.p2, params.p3 );
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
	signature2< R, A1, A2 >::execute() const
	{
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
	signature1< R, A1 >
	signature1< R, A1 >::operator <<( const boost::shared_ptr< argument< A1 > >& arg )
	{
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
	signature1< R, A1 >::execute() const
	{
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
