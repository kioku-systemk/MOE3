/*
 *  Parser.h
 *  Parser for Script.
 *
 *  Created by GOT / Eldorado on 2013/03/08.
 *
 */
#ifndef _C_PARSER_H_GUARD_
#define _C_PARSER_H_GUARD_

//==============================================================
// header
//==============================================================
#include <string>
#include <vector>
#include "../Core/Type.h"
#include "../Core/Math.h"

using namespace std;

namespace MOE {

	class CParameters;
	class CBPM;
	class CTriggerSystem;
	class CEvent;

	//==============================================================
	// CParser class
	//==============================================================
	class CParser
	{
	private:
		s32 mLine;
		string mCurrentNamespace;
		vector<pair<s32, string>> mFile;
		s32 mCurrentLineIndex;
		s32 mTotalLineCount;

	private:
		// File Ctrl
		b8 _ReadFile(string filename);
		b8 _GetNextLine(vector<string> &tokens);
		void _Error(const char *msg);

		// Parser
		b8 _ParseNamespace(CParameters &params);
		b8 _ParseBPM(CBPM &beat);
		b8 _ParseTrigger(CTriggerSystem &triggers);
		b8 _ParseEvent(CEvent &e);

		//converts
		vec3 _ConvertToVector3(string token1, string token2, string token3);

	public:
		CParser();
		~CParser();

		// パーサー全体
		b8 Parse(string filename, CParameters &params);
	};

}	// namespace MOE

#endif
