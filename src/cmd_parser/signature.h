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

#ifndef __SIGNATURE_H__
#define __SIGNATURE_H__

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

	template< typename S1, typename S2, typename S3 >
	class sig_arg;


	class _signature
	{
		public:
			_signature( const std::string& description );
			~_signature();

		private:
			std::string description;
	};

	template< typename R, typename A1, typename A2, typename A3 >
	class signature : public _signature
	{

		public:
			signature( const std::string& description, boost::function3<R, A1, A2, A3> f );
			~signature();

			sig_arg< A2, A3 > operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			sig_arg< A1, A2, A3 > operator <<( const kw_argument_ptr& arg );

		private:
			boost::function3<R, A1, A2, A3> func;

			// FIXME: Or 3 static arguments, 'cause we need to call func later and ignore kw!
			std::list< _argument* > arguments;

	};


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



	template< typename R, typename A1, typename A2, typename A3 >
	signature< R, A1, A2, A3 >::signature( const std::string& desc,
	                                       boost::function3<R, A1, A2, A3> f )
		: _signature( desc ), func( f ), arguments()
	{
	}

	template< typename R, typename A1, typename A2, typename A3 >
	signature< R, A1, A2, A3 >::~signature()
	{
	}


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

}

#endif  // __SIGNATURE_H__
