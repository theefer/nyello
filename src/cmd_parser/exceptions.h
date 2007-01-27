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

#ifndef CMD_PARSER_EXCEPTIONS_H
#define CMD_PARSER_EXCEPTIONS_H

#include <stdexcept>
#include <string>


namespace cmd_parser {

	/** @class wrong_signature_error
	 *  @brief Thrown if no signature associated to the command matches.
	 */
	class wrong_signature_error : public std::runtime_error
	{
		public:
			explicit wrong_signature_error( const std::string& what_arg );

	};

	/** @class incomplete_signature_error
	 *  @brief Thrown if the signature is not fully defined,
	 *          i.e. it does not contain all the advertised argument
	 *          objects.
	 */
	class incomplete_signature_error : public std::runtime_error
	{
		public:
			explicit incomplete_signature_error( const std::string& what_arg );

	};

	/** @class command_not_found_error
	 *  @brief Thrown if the input does not match any command.
	 */
	class command_not_found_error : public std::runtime_error
	{
		public:
			explicit command_not_found_error( const std::string& what_arg );

	};

	/** @class too_many_arguments_error
	 *  @brief Thrown if too many argument objects are added to a signature.
	 */
	class too_many_arguments_error : public std::runtime_error
	{
		public:
			explicit too_many_arguments_error( const std::string& what_arg );

	};

	/** @class incompatible_argument_error
	 *  @brief Thrown if the argument object added to a signature does
	 *         not match the expected type.
	 */
	class incompatible_argument_error : public std::runtime_error
	{
		public:
			explicit incompatible_argument_error( const std::string& what_arg );

	};

}

#endif  // CMD_PARSER_EXCEPTIONS_H
