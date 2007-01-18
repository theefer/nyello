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

#include "signature.h"
#include "argument.h"

namespace cmd_parser {

	_signature::_signature( const std::string& desc )
		: arguments(), description( desc )
	{
	}

	_signature::~_signature()
	{
	}

	bool
	_signature::run( const tokeniter& start, const tokeniter& end ) const
	{
		// First try to parse the arguments ; if works, execute() it
		tokeniter pos( start );
		std::vector< std::string > strarglist;
		std::list< boost::shared_ptr< _argument > >::const_iterator it;
		for( it = arguments.begin(); it != arguments.end(); ++it ) {

			// End of input and we still have arguments to parse!
			if( pos == end ) {
				// FIXME: Error => too few argument values!
				return false;
			}

			if( !(*it)->parse( pos, end, strarglist ) ) {
				// Signature mismatch, failed to parse an argument
				return false;
			}

		}

		// Argument values still remain
		if( pos != end ) {
			// FIXME: error => too many argument values!
			return false;
		}

		execute( strarglist );

		return true;
	}

}
