/*
 *  StringUtils.h
 *  StringUtils
 *
 *  Created by GOT / Eldorado on 2013/03/08.
 *
 */
#ifndef INCLUDE_STRINGUTILS_H
#define INCLUDE_STRINGUTILS_H

//==============================================================
// header
//==============================================================
#include <sstream>
#include "../Core/Type.h"

using namespace std;

namespace MOE {

	//==============================================================
	// CStringUtils class
	//==============================================================
	class CStringUtils
	{
	public:
		template <class T> static b8 ConvertStringToNumber(T &t, const string &s, ios_base &(*f)(ios_base&))
		{
			std::istringstream iss(s);
			return !(iss >> f >> t).fail();
		};

		static void Tokenize(string str, vector<string> &tokens);
		static void RemoveChar(string &str, s8 remove);
	};

}	// namespace MOE

#endif
