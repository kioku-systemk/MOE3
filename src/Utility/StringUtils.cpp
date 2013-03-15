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
	// ��������g�[�N���P�ʂŕ����܂�
	void CStringUtils::Tokenize(string str, vector<string> &tokens)
	{
		stringstream ss(str);
		string buffer;
		while (ss >> buffer){
			tokens.push_back(buffer);
		}
	}

	// string���̎w�蕶�����폜���܂�
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