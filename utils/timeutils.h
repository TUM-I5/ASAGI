/**
 * @file
 *  This file is part of ASAGI.
 *
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU General Public License, wie von der Free Software Foundation,
 *  Version 3 der Lizenz oder (nach Ihrer Option) jeder spaeteren
 *  veroeffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 *  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 * 
 * @copyright 2013 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef UTILS_TIMEUTILS_H_
#define UTILS_TIMEUTILS_H_

#include <ctime>
#include <string>

namespace utils
{

/**
 * A collection of usefull time functions
 */
class TimeUtils
{
public:
	/**
	 * Formats a string using strftime
	 *
	 * Taken from http://stackoverflow.com/questions/7935483/c-function-to-format-time-t-as-stdstring-buffer-length
	 *
	 * @return A copy of formatString, with all %k replaced with the time information
	 */
	static std::string timeAsString(const std::string& formatString, time_t time)
	{
	    const struct tm *timeinfo = localtime(&time);

	    std::string buffer;
	    buffer.resize(formatString.size()*2);
	    size_t len = strftime(&buffer[0], buffer.size(), formatString.c_str(), timeinfo);
	    while (len == 0) {
	        buffer.resize(buffer.size()*2);
	        len = strftime(&buffer[0], buffer.size(), formatString.c_str(), timeinfo);
	    }
	    buffer.resize(len);
	    return buffer;
	}
};

}

#endif /* UTILS_TIMEUTILS_H_ */
