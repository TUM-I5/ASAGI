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
 * @copyright 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef DEBUG_DBG_H
#define DEBUG_DBG_H

#include "utils/timeutils.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>

#ifndef DEBUG_LEVEL
#ifdef NDEBUG
#define DEBUG_LEVEL 2
#else // NDEBUG
#define DEBUG_LEVEL 3
#endif // NDEBUG
#endif // DEBUG_LEVEL

#ifndef DEBUG_PREFIX
#define DEBUG_PREFIX "%a %b %d %X"
#endif // DEBUG_PRFIX

#ifndef DEBUG_ABORT
#define DEBUG_ABORT abort()
#endif // DEBUG_ABORT

/**
 * @brief Responsible for debugging and info messages
 */
namespace utils
{

/**
 * Handles debugging output
 * 
 * Most of the code is taken form QDebug form the Qt Framework
 */
class Dbg
{
public:
	/** Message type */
	enum DebugType {
		/** A debug messages */
		DEBUG,
		/** A info message (printed to stdout) */
		INFO,
		/** A warning message */
		WARNING,
		/** A fatal error */
		ERROR
	};
private:
	/** Contains all information for a debug message */
	struct Stream {
		/** The debug type */
		DebugType type;
		/** MPI Rank, set to 0 to print message */
		int rank;
		/** References */
		int ref;
		/** Buffer for the output */
		std::stringstream buffer;
		/** Print additional space */
		bool space;
		
		/**
		 * Set defaults for a debug message
		 */
		Stream(DebugType t, int r)
			: type(t), rank(r), ref(1),
			buffer(std::stringstream::out),
			space(true) { }
	} *stream;
	/**
	 * Pointer to all information about the message
	 */
public:
	/**
	 * Start a new Debug message
	 * 
	 * @param t Type of the message
	 * @param rank Rank of the current process, only messages form rank
	 *  0 will be printed
	 */
	Dbg(DebugType t, int rank)
		: stream(new Stream(t, rank))
	{
		stream->buffer << utils::TimeUtils::timeAsString(DEBUG_PREFIX, time(0L));

		switch (t) {
		case DEBUG:
			stream->buffer << ", Debug: ";
			break;
		case INFO:
			stream->buffer << ", Info:  ";
			break;
		case WARNING:
			stream->buffer << ", Warn:  ";
			break;
		case ERROR:
			stream->buffer << ", Error: ";
			break;
		}
	}
	/**
	 * Copy constructor
	 */
	Dbg(const Dbg& o) : stream(o.stream) { stream->ref++; };
	~Dbg()
	{
		if (!--stream->ref) {
			if (stream->rank == 0) {
				if (stream->type == INFO)
					std::cout << stream->buffer.str() << std::endl;
				else
					std::cerr << stream->buffer.str() << std::endl;
			}

			if (stream->type == ERROR) {
				delete stream;
				DEBUG_ABORT;
			}

			delete stream;
		}
	}
	
	/**
	 * Copy operator
	 */
	Dbg &operator=(const Dbg& other)
	{
		if (this != &other) {
			Dbg copy(other);
			std::swap(stream, copy.stream);
		}
		return *this;
	}
	
	
	/********* Space handling *********/
	
	/**
	 * Add a space to output message and activate spaces
	 */
	Dbg &space()
	{
		stream->space = true;
		stream->buffer << ' ';
		return *this;
	}
	/**
	 * Deactivate spaces
	 */
	Dbg &nospace()
	{
		stream->space = false;
		return *this;
	}
	/**
	 * Add space of activated
	 */
	Dbg &maybeSpace()
	{
		if (stream->space)
			stream->buffer << ' ';
		return *this;
	}
	
	/**
	 * Default function to add messages
	 */
	template<typename T>
	Dbg &operator<<(T t)
	{
		stream->buffer << t;
		return maybeSpace();
	}

	/**
	 * Operator to add functions like std::endl
	 */
	Dbg &operator<<(std::ostream& (*func)(std::ostream&))
	{
		stream->buffer << func;
		return *this; // No space in this case
	}
};

/**
 * Dummy debug class, does nothing
 */
class NoDbg
{
public:
	NoDbg() {};
	~NoDbg() {};
	
	/**
	 * Do nothing with the message
	 */
	template<typename T>
	NoDbg &operator<<(const T&)
	{
		return *this;
	}

	/**
	 * Operator to add functions like std::endl
	 */
	NoDbg &operator<<(std::ostream& (*func)(std::ostream&))
	{
		return *this;
	}
};

/**
 * Add a string variable to the message
 */
template<>
inline Dbg& Dbg::operator<<(const std::string& t)
{
	stream->buffer << '"' << t << '"';
	return maybeSpace();
}

/**
 * Add a std::vector<T> to the message
 */
template <typename T>
inline Dbg &operator<<(Dbg debug, const std::vector<T> &list)
{
	debug.nospace() << '(';
	for (size_t i = 0; i < list.size(); i++) {
		if (i)
			debug << ", ";
		debug << list[i];
	}
	debug << ')';
	
	return debug.space();
}

}

// Define global functions

/**
 * Create error message and exit
 */
inline
utils::Dbg dbgError()
{
	return utils::Dbg(utils::Dbg::ERROR, 0);
}

#if DEBUG_LEVEL >= 1
/**
 * Create a warning message if enabled
 *
 * @relates utils::Dbg
 */
inline
utils::Dbg dbgWarning( int rank = 0 )
{
	return utils::Dbg(utils::Dbg::WARNING, rank);
}
#else // DEBUG_LEVEL >= 1
/**
 * Create a dummy warning message if disabled
 *
 * @relates utils::NoDbg
 */
inline
utils::NoDbg dbgWarning( int = 0 ) { return utils::NoDbg(); }
#endif // DEBUG_LEVEL >= 1

#if DEBUG_LEVEL >= 2
/**
 * Create a info message if enabled
 *
 * @relates utils::Dbg
 */
inline
utils::Dbg dbgInfo( int rank = 0 )
{
	return utils::Dbg(utils::Dbg::INFO, rank);
}
#else // DEBUG_LEVEL >= 2
/**
 * Create a dummy info message if disabled
 *
 * @relates utils::NoDbg
 */
inline
utils::NoDbg dbgInfo( int = 0 ) { return utils::NoDbg(); }
#endif // DEBUG_LEVEL >= 2

#if DEBUG_LEVEL >= 3
/**
 * Create a debug message if enabled
 *
 * @relates utils::Dbg
 */
inline
utils::Dbg dbgDebug( int rank = 0 )
{
	return utils::Dbg(utils::Dbg::DEBUG, rank);
}
#else // DEBUG_LEVEL >= 3
/**
 * Create a dummy debug message if disabled
 *
 * @relates utils::NoDbg
 */
inline
utils::NoDbg dbgDebug( int = 0 ) { return utils::NoDbg(); }
#endif // DEBUG_LEVEL >= 3


// Use for variables unused when compiling with NDEBUG
#ifdef NDEBUG
#define NDBG_UNUSED(x) ((void) x)
#else // NDEBUG
#define NDBG_UNUSED(x)
#endif // NDEBUG

#endif // DEBUG_DBG_H
