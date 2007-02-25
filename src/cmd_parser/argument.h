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

#ifndef CMD_PARSER_ARGUMENT_H
#define CMD_PARSER_ARGUMENT_H

#include <string>
#include <list>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <boost/lexical_cast.hpp>

#include "visitor.h"
#include "typedefs.h"
#include "exceptions.h"

namespace cmd_parser {

	template< typename T >
	class argument;

	class kw_argument;
	typedef boost::shared_ptr< kw_argument > kw_argument_ptr;

	class _argument
	{
		public:
			virtual ~_argument();

			virtual void complete( std::list< std::string >& alternatives ) const = 0;

			virtual void accept( visitor& v ) const
			{
				v.visit( *this );
			}

			virtual void advance( tokeniter& start, const tokeniter& end ) const = 0;
			
		protected:
			_argument();

	};


	class kw_argument : public _argument
	{
		public:
			kw_argument( const std::string& kw );
			virtual ~kw_argument();

			static kw_argument_ptr make( const std::string& kw );

			virtual bool match( tokeniter& start, const tokeniter& end ) const;
			void complete( std::list< std::string >& alternatives ) const;

			void accept( visitor& v ) const
			{
				v.visit( *this );
			}

			void advance( tokeniter& start, const tokeniter& end ) const;

			const std::string& get_keyword() const
			{ return keyword; }

		private:
			std::string keyword;

	};


	template< typename T >
	class argument : public _argument
	{
		public:
			argument( const std::string& name );
			argument( const std::string& name, const T& def_val );
			virtual ~argument();

			static boost::shared_ptr< argument< T > > make( const std::string& name );
			static boost::shared_ptr< argument< T > > make( const std::string& name,
			                                                const T& def_val );

			virtual T extract( tokeniter& start, const tokeniter& end ) const;

			void advance( tokeniter& start, const tokeniter& end ) const;

			void complete( std::list< std::string >& alternatives ) const;

		protected:
			std::string name;
			bool optional;
			T default_value;

			// FIXME: list< T > possible_values();
	};

	/**
	 * Argument that eats everything until the end of the input.
	 */
	template< typename T >
	class tail_argument : public argument< T >
	{
		public:
			tail_argument( const std::string& name );
			tail_argument( const std::string& name, const T& def_val );
			virtual ~tail_argument();

			static boost::shared_ptr< argument< T > > make( const std::string& name );
			static boost::shared_ptr< argument< T > > make( const std::string& name,
			                                                const T& def_val );

			virtual T extract( tokeniter& start, const tokeniter& end ) const;

			void advance( tokeniter& start, const tokeniter& end ) const;

	};



	template< typename T >
	argument< T >::argument( const std::string& n )
		: _argument(), name( n ), optional( false )
	{
	}

	template< typename T >
	argument< T >::argument( const std::string& n, const T& def_val )
		: _argument(), name( n ), optional( true ), default_value( def_val )
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
		return boost::shared_ptr< argument< T > >( new argument< T >( name, def_val ) );
	}

	template< typename T >
	T
	argument< T >::extract( tokeniter& start, const tokeniter& end ) const
	{
		// FIXME: Too many copies!? - not initialised?
		T value;

		if( start != end ) {
			try {
				value = boost::lexical_cast< T >( *start );
				advance( start, end );
			}
			catch( boost::bad_lexical_cast& ) {
				// Invalid value, use default value is argument optional
				if( optional ) {
					value = default_value;
				}
				else {
					throw incompatible_argument_error( "value extraction failed" );
				}
			}
		}
		// Empty value, use the default value
		else {
			// FIXME: duplicated!
			// Invalid value, use default value is argument optional
			if( optional ) {
				value = default_value;
			}
		}

		return value;
	}


	template< typename T >
	void
	argument< T >::complete( std::list< std::string >& alternatives ) const
	{
		// Workaround to signal we're waiting for some data
		alternatives.push_back( "" );
	}

	template< typename T >
	void
	argument< T >::advance( tokeniter& start, const tokeniter& end ) const
	{
		++start;
	}


	template< typename T >
	tail_argument< T >::tail_argument( const std::string& n )
		: argument< T >( n )
	{
	}

	template< typename T >
	tail_argument< T >::tail_argument( const std::string& n, const T& def_val )
		: argument< T >( n, def_val )
	{
	}

	template< typename T >
	tail_argument< T >::~tail_argument()
	{
	}

	template< typename T >
	boost::shared_ptr< argument< T > >
	tail_argument< T >::make( const std::string& name )
	{
		return boost::shared_ptr< argument< T > >( new tail_argument< T >( name ) );
	}

	template< typename T >
	boost::shared_ptr< argument< T > >
	tail_argument< T >::make( const std::string& name, const T& def_val )
	{
		return boost::shared_ptr< argument< T > >( new tail_argument< T >( name, def_val ) );
	}

	template< typename T >
	T
	tail_argument< T >::extract( tokeniter& start, const tokeniter& end ) const
	{
		// FIXME: Too many copies!? - not initialised?
		T value;

		if( start != end ) {
			tokeniter pos( start );
			try {
				std::string fullvalue;
				while( pos != end ) {
					if( pos != start ) {
						// FIXME: append could be nicer
						fullvalue.append( " " );
					}
					fullvalue.append( *pos );
					++pos;
				}
				value = boost::lexical_cast< T >( fullvalue );
				advance( start, end );
			}
			catch( boost::bad_lexical_cast& ) {
				// Invalid value, use default value is argument optional
				if( this->optional ) {
					value = this->default_value;
				}
				else {
					throw incompatible_argument_error( "value extraction failed" );
				}
			}
		}
		// Empty value, use the default value
		else {
			// FIXME: duplicated!
			// Invalid value, use default value is argument optional
			if( this->optional ) {
				value = this->default_value;
			}
		}

		return value;
	}

	template< typename T >
	void
	tail_argument< T >::advance( tokeniter& start, const tokeniter& end ) const
	{
		start = end;
	}
}

#endif  // CMD_PARSER_ARGUMENT_H
