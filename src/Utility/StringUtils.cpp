/*
 *  StringUtils.cpp
 *  StringUtils
 *
 *  Created by GOT / Eldorado on 2013/03/08.
 *
 */
#include <vector>
#include "StringUtils.h"

namespace MOE {

	//==============================================================
	// CStringUtils class
	//==============================================================
	// 文字列をトークン単位で分けます
	void CStringUtils::Tokenize(string str, vector<string> &tokens)
	{
		stringstream ss(str);
		string buffer;
		while (ss >> buffer){
			tokens.push_back(buffer);
		}
	}

	// string内の指定文字を削除します
	void CStringUtils::RemoveChar(string &str, s8 remove)
	{
		string::iterator it;
		for (it = str.begin(); it < str.end(); ){
			if (*it == remove){
				it = str.erase(it);
			}else{
				it++;
			}
		}
	}

}	// namespace MOE