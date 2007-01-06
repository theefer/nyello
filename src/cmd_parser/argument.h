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

#ifndef __ARGUMENT_H__
#define __ARGUMENT_H__

#include <string>
#include <boost/shared_ptr.hpp>


namespace cmd_parser {

	template< typename T >
	class argument;

	class kw_argument;
	typedef boost::shared_ptr< kw_argument > kw_argument_ptr;

	class _argument
	{
		public:
			~_argument();
			
		protected:
			_argument();

			// FIXME: bool match( const string& tk );
	};


	template< typename T >
	class argument : public _argument
	{
		public:
			argument( const std::string& name );
			argument( const std::string& name, const T& def_val );
			~argument();

			static boost::shared_ptr< argument< T > > make( const std::string& name );
			static boost::shared_ptr< argument< T > > make( const std::string& name,
			                                                const T& def_val );

		private:
			std::string name;
			bool optional;
			T default_value;

			// FIXME: list< T > possible_values();
	};


	class kw_argument : public _argument
	{
		public:
			kw_argument( const std::string& kw );
			~kw_argument();

			static kw_argument_ptr make( const std::string& kw );

		private:
			std::string keyword;

	};





	template< typename T >
	argument< T >::argument( const std::string& n )
		: name( n ), optional( false )
	{
	}

	template< typename T >
	argument< T >::argument( const std::string& n, const T& def_val )
		: name ( n ), optional( true ), default_value( def_val )
	{
	}

	template< typename T >
	argument< T >::~argument()
	{
	}

	template< typename T >
	boost::shared_ptr< argument< T > >
	argument< T >::make( const std::string& name )
	{
		return boost::shared_ptr< argument< T > >( new argument< T >( name ) );
	}

	template< typename T >
	boost::shared_ptr< argument< T > >
	argument< T >::make( const std::string& name, const T& def_val )
	{
		return 	boost::shared_ptr< argument< T > >( new argument< T >( name, def_val ) );
	}

}

#endif  // __ARGUMENT_H__
