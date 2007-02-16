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

#ifndef CMD_PARSER_COMMAND_H
#define CMD_PARSER_COMMAND_H

#include <string>
#include <list>
#include <iostream>
#include <boost/function.hpp>

#include "typedefs.h"


namespace cmd_parser {

	class _signature;

	template< typename R >
	class signature0;

	template< typename R, typename A1 >
	class signature1;

	template< typename R, typename A1, typename A2 >
	class signature2;

	template< typename R, typename A1, typename A2, typename A3 >
	class signature3;

	class command
	{

		public:
			command( const std::string& name, const std::string& desc );
			~command();

			command& add_alias( const std::string& alias );
			command& set_help( const std::string& help );

			template< typename R >
			signature0< R >& add_signature( const std::string& description,
			                                boost::function0<R> f );

			template< typename R, typename A1 >
			signature1< R, A1 >& add_signature( const std::string& description,
			                                    boost::function1<R, A1> f );

			template< typename R, typename A1, typename A2 >
			signature2< R, A1, A2 >& add_signature( const std::string& description,
			                                        boost::function2<R, A1, A2> f );

			template< typename R, typename A1, typename A2, typename A3 >
			signature3< R, A1, A2, A3 >& add_signature( const std::string& description,
			                                            boost::function3<R, A1, A2, A3> f );

			bool match( const tokeniter& start, const tokeniter& end ) const;
			void run( const tokeniter& start, const tokeniter& end ) const;
			void complete( const tokeniter& start, const tokeniter& end,
			               std::list< std::string >& alternatives ) const;

			inline const std::string& get_name() const;
			inline const std::string& get_description() const;
			inline std::string::size_type get_max_name_length() const;

			void help( std::ostream& os ) const;

		private:
			static std::string::size_type max_name_length;

			std::string name;
			std::list< std::string > aliases;

			std::string description;
			std::string helptext;

			std::list< _signature* > signatures;

			bool match_command( const std::string& input ) const;
			bool match_string( const std::string& cmd, const std::string& input ) const;

	};


	inline const std::string&
	command::get_name() const
	{
		return name;
	}

	inline const std::string&
	command::get_description() const
	{
		return description;
	}

	inline std::string::size_type
	command::get_max_name_length() const
	{
		return max_name_length;
	}


	template< typename R >
	signature0< R >&
	command::add_signature( const std::string& description, boost::function0<R> f )
	{
		signature0< R >* sig( new signature0< R >( description, f ) );

		signatures.push_back( sig );

		return *sig;
	}

	template< typename R, typename A1 >
	signature1< R, A1 >&
	command::add_signature( const std::string& description, boost::function1<R, A1> f )
	{
		signature1< R, A1 >* sig( new signature1< R, A1 >( description, f ) );

		signatures.push_back( sig );

		return *sig;
	}

	template< typename R, typename A1, typename A2 >
	signature2< R, A1, A2 >&
	command::add_signature( const std::string& description, boost::function2<R, A1, A2> f )
	{
		signature2< R, A1, A2 >* sig( new signature2< R, A1, A2 >( description, f ) );

		signatures.push_back( sig );

		return *sig;
	}

	template< typename R, typename A1, typename A2, typename A3 >
	signature3< R, A1, A2, A3 >&
	command::add_signature( const std::string& description, boost::function3<R, A1, A2, A3> f )
	{
		signature3< R, A1, A2, A3 >* sig( new signature3< R, A1, A2, A3 >( description, f ) );

		signatures.push_back( sig );

		return *sig;
	}

}

#endif  // CMD_PARSER_COMMAND_H
