#ifndef DEBUG_DBG_H
#define DEBUG_DBG_H

#include <algorithm>
#include <iostream>
#include <sstream>

/**
 * Most of the code is taken form QDebug form the Qt Framework
 */
namespace debug
{
	/**
	 * This class debugging output
	 */
	class Dbg
	{
	public:
		enum DebugType { DEBUG, FATAL };
	private:
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
			
			Stream(DebugType t, int r)
				: type(t), rank(r), ref(1),
				buffer(std::stringstream::out),
				space(true) { }
		} *stream;
	public:
		Dbg(DebugType t, int rank) : stream(new Stream(t, rank))
		{
			stream->buffer << "ASAGI: ";
		}
		Dbg(const Dbg& o) : stream(o.stream) { stream->ref++; };
		~Dbg()
		{
			if (!--stream->ref) {
				if (stream->rank == 0) {
					stream->buffer << std::endl;
					std::cerr << stream->buffer.str();
				}
				delete stream;
			}
		}
		
		Dbg &operator=(const Dbg& other)
		{
			if (this != &other) {
				Dbg copy(other);
				std::swap(stream, copy.stream);
			}
			return *this;
		}
		
		
		// Space handling
		Dbg &space()
		{
			stream->space = true;
			stream->buffer << ' ';
			return *this;
		}
		Dbg &nospace()
		{
			stream->space = false;
			return *this;
		}
		Dbg &maybeSpace()
		{
			if (stream->space)
				stream->buffer << ' ';
			return *this;
		}
		
		// Const Char
		Dbg &operator<<(const char* t)
		{
			stream->buffer << t;
			return maybeSpace();
		}
		
		// Strings
		Dbg &operator<<(const std::string& t)
		{
			stream->buffer << '"' << t << '"';
			return maybeSpace();
		}
		
		// Numbers
		Dbg &operator<<(int t)
		{
			stream->buffer << t;
			return  maybeSpace();
		}
		
		Dbg &operator<<(unsigned int t)
		{
			stream->buffer << t;
			return  maybeSpace();
		}
		
		Dbg &operator<<(long t)
		{
			stream->buffer << t;
			return maybeSpace();
		}
		
		Dbg &operator<<(unsigned long t)
		{
			stream->buffer << t;
			return maybeSpace();
		}
		
		Dbg &operator<<(double t)
		{
			stream->buffer << t;
			return maybeSpace();
		}
		
		// Pointers
		Dbg &operator<<(const void* t) {
			stream->buffer << t;
			return maybeSpace();
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
		
		template<typename T> NoDbg &operator<<(const T&)
		{
			return *this;
		}
	};
}

#ifdef NDEBUG
inline debug::NoDbg dbgDebug( int = 0 ) { return debug::NoDbg(); }
#else // NDEBUG
inline debug::Dbg dbgDebug( int rank = 0 )
{
	return debug::Dbg(debug::Dbg::DEBUG, rank);
}
#endif // NDEBUG

// Use for variables unused when compilin with NDEBUG
#ifdef NDEBUG
#define NDBG_UNUSED(x) ((void) x)
#else // NDEBUG
#define NDBG_UNUSED(x)
#endif // NDEBUG

#endif // DEBUG_DBG_H
