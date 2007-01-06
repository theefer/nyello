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

#include "cmd_parser.h"

#include <string>
using std::string;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;


void
foo( string s, double a, double b )
{
	std::cout << "foo called with s=" << s << std::endl;
}

void
bar( int i, string s, string t )
{
	std::cout << "bar called with i=" << i << " s=" << s << " t=" << t << std::endl;
}

void
abs_seek( int i )
{
	std::cout << "abs_seek called with i=" << i << std::endl;
}

void
rel_seek( int i )
{
	std::cout << "rel_seek called with i=" << i << std::endl;
}

class pattern_argument : public cmd_parser::argument< string >
{
public:
	pattern_argument( const string& s )
		: cmd_parser::argument< string >( s )
		{}
};


int main()
{
	using namespace cmd_parser;

	// todo: add alias, set help

	interpreter inter;
	inter.add_command("enqueue", "enqueue songs in a playlist")
		.add_alias("en")
		.add_signature<void, string, double, double>( "in the current playlist", boost::function3<void, string, double, double>( &foo ) )
			<< (shared_ptr<argument<string> >(new pattern_argument( "pattern" )))
		;
/*
		.add_signature<int, string, string>( "in a given playlist", &bar )
			<< kw_argument::make( "in" ) << argument<string>::make( "playlist", "Default" ) << pattern_argument( "pattern" );
*/
	inter.add_command("seek", "Seek in seconds in the current song.")
		.add_signature<void, int>( "Seek to an absolute position.", &abs_seek )
			<< argument<int>::make( "pos" )
		;
/*
		.add_signature<void, int>( "Seek to a relative position.", &rel_seek )
			<< offset_argument( "offset" );
*/
	// or should << accept argument* or even shared_ptr, i.e. mallocated argument.
	// Easy for hand-made, offer a transparent static function for standard kw or type args,
	// e.g. argument< T >::make( foo )

	inter.gen_help(std::cout);
	inter.gen_help("seek", std::cout);

	return 0;
}

