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

#include "visitor.h"
#include "typedefs.h"
#include "exceptions.h"
#include "sig_args.h"


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

			virtual bool run( const tokeniter& start, const tokeniter& end ) const = 0;
			virtual void complete( const tokeniter& start, const tokeniter& end,
			                       std::list< std::string >& alternatives ) const = 0;

			void accept( visitor& v ) const
			{
				v.visit( *this );
			}

			virtual const sig_args& get_arguments() const = 0;

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
			sig_args2< R, A2, A3 >& operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			sig_args3< R, A1, A2, A3 >& operator <<( const boost::shared_ptr< kw_argument >& arg );

			bool run( const tokeniter& start, const tokeniter& end ) const;
			void complete( const tokeniter& start, const tokeniter& end,
			               std::list< std::string >& alternatives ) const;

			const sig_args& get_arguments() const
			{ return arguments; }

		private:
			boost::function3< R, A1, A2, A3 > func;

			sig_args3< R, A1, A2, A3 > arguments;

	};

	template< typename R, typename A1, typename A2 >
	class signature2 : public _signature
	{

		public:
			signature2( const std::string& description, boost::function2< R, A1, A2 > f );
			~signature2();

			sig_args1< R, A1 >& operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			sig_args2< R, A1, A2 >& operator <<( const boost::shared_ptr< kw_argument >& arg );

			bool run( const tokeniter& start, const tokeniter& end ) const;
			void complete( const tokeniter& start, const tokeniter& end,
			               std::list< std::string >& alternatives ) const;

			const sig_args& get_arguments() const
			{ return arguments; }

		private:
			boost::function2< R, A1, A2 > func;

			sig_args2< R, A1, A2 > arguments;

	};

	template< typename R, typename A1 >
	class signature1 : public _signature
	{

		public:
			signature1( const std::string& description, boost::function1< R, A1 > f );
			~signature1();

			sig_args0< R >& operator <<( const boost::shared_ptr< argument< A1 > >& arg );
			sig_args1< R, A1 >& operator <<( const boost::shared_ptr< kw_argument >& arg );

			bool run( const tokeniter& start, const tokeniter& end ) const;
			void complete( const tokeniter& start, const tokeniter& end,
			               std::list< std::string >& alternatives ) const;

			const sig_args& get_arguments() const
			{ return arguments; }

		private:
			boost::function1< R, A1 > func;

			sig_args1< R, A1 > arguments;

	};

	template< typename R >
	class signature0 : public _signature
	{

		public:
			signature0( const std::string& description, boost::function0< R > f );
			~signature0();

			sig_args0< R >& operator <<( const boost::shared_ptr< kw_argument >& arg );

			bool run( const tokeniter& start, const tokeniter& end ) const;
			void complete( const tokeniter& start, const tokeniter& end,
			               std::list< std::string >& alternatives ) const;

			const sig_args& get_arguments() const
			{ return arguments; }

		private:
			boost::function0< R > func;

			sig_args0< R > arguments;

	};



	template< typename R, typename A1, typename A2, typename A3 >
	signature3< R, A1, A2, A3 >::signature3( const std::string& desc,
	                                         boost::function3<R, A1, A2, A3> f )
		: _signature( desc ), func( f ), arguments()
	{
	}

	template< typename R, typename A1, typename A2, typename A3 >
	signature3< R, A1, A2, A3 >::~signature3()
	{
	}


	template< typename R, typename A1, typename A2, typename A3 >
	sig_args2< R, A2, A3 >&
	signature3< R, A1, A2, A3 >::operator <<( const boost::shared_ptr< argument< A1 > >& arg )
	{
		return arguments << arg;
	}

	template< typename R, typename A1, typename A2, typename A3 >
	sig_args3< R, A1, A2, A3 >&
	signature3< R, A1, A2, A3 >::operator <<( const boost::shared_ptr< kw_argument >& arg )
	{
		return arguments << arg;
	}

	template< typename R, typename A1, typename A2, typename A3 >
	bool
	signature3< R, A1, A2, A3 >::run( const tokeniter& start, const tokeniter& end ) const
	{
		try {
			// FIXME: don't drop the return value!
			arguments.apply( func, start, end );
			return true;
		}
		// FIXME: only catch "mismatching sig" exception!
		catch(...) {
			return false;
		}
	}

	template< typename R, typename A1, typename A2, typename A3 >
	void
	signature3< R, A1, A2, A3 >::complete( const tokeniter& start, const tokeniter& end,
	                                       std::list< std::string >& alternatives ) const
	{
		arguments.complete( start, end, alternatives );
	}


	template< typename R, typename A1, typename A2 >
	signature2< R, A1, A2 >::signature2( const std::string& desc,
	                                     boost::function2<R, A1, A2> f )
		: _signature( desc ), func( f ), arguments()
	{
	}

	template< typename R, typename A1, typename A2 >
	signature2< R, A1, A2 >::~signature2()
	{
	}


	template< typename R, typename A1, typename A2 >
	sig_args1< R, A1 >&
	signature2< R, A1, A2 >::operator <<( const boost::shared_ptr< argument< A1 > >& arg )
	{
		return arguments << arg;
	}

	template< typename R, typename A1, typename A2 >
	sig_args2< R, A1, A2 >&
	signature2< R, A1, A2 >::operator <<( const boost::shared_ptr< kw_argument >& arg )
	{
		return arguments << arg;
	}

	template< typename R, typename A1, typename A2 >
	bool
	signature2< R, A1, A2 >::run( const tokeniter& start, const tokeniter& end ) const
	{
		try {
			// FIXME: don't drop the return value!
			arguments.apply( func, start, end );
			return true;
		}
		// FIXME: only catch "mismatching sig" exception!
		catch(...) {
			return false;
		}
	}

	template< typename R, typename A1, typename A2 >
	void
	signature2< R, A1, A2 >::complete( const tokeniter& start, const tokeniter& end,
	                                   std::list< std::string >& alternatives ) const
	{
		arguments.complete( start, end, alternatives );
	}


	template< typename R, typename A1 >
	signature1< R, A1 >::signature1( const std::string& desc,
	                                 boost::function1<R, A1> f )
		: _signature( desc ), func( f ), arguments()
	{
	}

	template< typename R, typename A1 >
	signature1< R, A1 >::~signature1()
	{
	}

	template< typename R, typename A1 >
	sig_args0< R >&
	signature1< R, A1 >::operator <<( const boost::shared_ptr< argument< A1 > >& arg )
	{
		return arguments << arg;
	}

	template< typename R, typename A1 >
	sig_args1< R, A1 >&
	signature1< R, A1 >::operator <<( const boost::shared_ptr< kw_argument >& arg )
	{
		return arguments << arg;
	}

	template< typename R, typename A1 >
	bool
	signature1< R, A1 >::run( const tokeniter& start, const tokeniter& end ) const
	{
		try {
			// FIXME: don't drop the return value!
			arguments.apply( func, start, end );
			return true;
		}
		// FIXME: only catch "mismatching sig" exception!
		catch(...) {
			return false;
		}
	}

	template< typename R, typename A1 >
	void
	signature1< R, A1 >::complete( const tokeniter& start, const tokeniter& end,
	                               std::list< std::string >& alternatives ) const
	{
		arguments.complete( start, end, alternatives );
	}


	template< typename R >
	signature0< R >::signature0( const std::string& desc,
	                             boost::function0<R> f )
		: _signature( desc ), func( f ), arguments()
	{
	}

	template< typename R >
	signature0< R >::~signature0()
	{
	}

	template< typename R >
	sig_args0< R >&
	signature0< R >::operator <<( const boost::shared_ptr< kw_argument >& arg )
	{
		return arguments << arg;
	}

	template< typename R >
	bool
	signature0< R >::run( const tokeniter& start, const tokeniter& end ) const
	{
		try {
			// FIXME: don't drop the return value!
			arguments.apply( func, start, end );
			return true;
		}
		// FIXME: only catch "mismatching sig" exception!
		catch(...) {
			return false;
		}
	}

	template< typename R >
	void
	signature0< R >::complete( const tokeniter& start, const tokeniter& end,
	                           std::list< std::string >& alternatives ) const
	{
		arguments.complete( start, end, alternatives );
	}

}

#endif  // CMD_PARSER_SIGNATURE_H
