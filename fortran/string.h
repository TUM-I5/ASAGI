#ifndef FORTRANSTRING_H
#define FORTRANSTRING_H

namespace fortran
{
	class String
	{
	public:
		static bool f2c(char* fstr, int len, char** cstr);
		static void c2f(char* cstr, char* fstr, int len);
	};
};

#endif