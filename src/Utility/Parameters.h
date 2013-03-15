/*
 *  Parameters.h
 *  Param for Script.
 *
 *  Created by GOT / Eldorado on 2013/03/08.
 *
 */
#ifndef INCLUDE_PARAMETERS_H
#define INCLUDE_PARAMETERS_H

//==============================================================
// header
//==============================================================
#include <map>
#include "../Core/Type.h"
#include "../Core/Math.h"

using namespace std;
using namespace MOE;
using namespace Math;

namespace MOE {

	class CRange;
	class CBPM;
	class CTriggerSystem;
	class CEvent;

	//==============================================================
	// CParameterSpace class
	// @Note:
	//	ネームスペース切ってパラメータを設定できます。
	//==============================================================
	class CParameterSpace
	{
	private:
		//these maps contain the parameters, one map per type. 
		map<string, f32> mFloats;
		map<string, s32> mInts;
		map<string, vec3> mVectors;
		map<string, vec4> mColor;
		map<string, string> mStrings;
		map<string, b8> mBools;
		map<string, CRange> mRanges;

	public:
		CParameterSpace();
		~CParameterSpace();

		void Clear();

		//insertion methods
		void AddF32(string name, f32 value);
		void AddS32(string name, s32 value);
		void AddVector3(string name, vec3 &value);
		void AddColor(string name, vec4 &value);
		void AddString(string name, string &value);
		void AddB8(string name, b8 value);
		void AddRange(string name, CRange &range);
	
		//accesssors
		f32 GetF32(string name);
		s32 GetS32(string name);
		vec3 GetVector3(string name);
		vec4 GetColor(string name);
		string GetString(string name);
		b8 GetB8(string name);
		CRange GetRange(string name);
	};

	//==============================================================
	// CParameters class
	// @Note:
	//	CParameterSpaceの補助クラス。こちらをメインで使用します。
	//  bpm、トリガーシステム、イベントはネームスペースは
	//	それぞれ、bpm、trigger、event固定になります。
	//==============================================================
	class CParameters
	{
	private:
		string mCurrentName;
		map<string, CParameterSpace> mParams;
		map<string, CBPM*> mBeats;
		map<string, CTriggerSystem*> mTriggers;
		map<string, CEvent*> mEvents;

	public:
		CParameters();
		~CParameters();
	
		//sets the current namespace so you can reference params only by their name
		void UseNamespace(string space);
		//clear it all out
		void Clear();
		//write it into a file
		void Save(string filename);

		//insertion generic parameters
		void AddF32(string space, string name, f32 value);
		void AddS32(string space, string name, s32 value);
		void AddVector3(string space, string name, vec3 &value);
		void AddColor(string space, string name, vec4 &value);
		void AddString(string space, string name, string &value);
		void AddB8(string space, string name, b8 value);
		void AddRange(string space, string name, CRange &range);

		//insertion sync parameters
		void AddBeat(string& name, CBPM *bpm);
		void AddTriggers(string& name, CTriggerSystem *triggers);
		void AddEvent(string& name, CEvent *e);

		//generic accessors
		f32 GetF32(string space, string name);
		f32 GetF32(string name);
		s32 GetS32(string space, string name);
		s32 GetS32(string name);
		string GetString(string space, string name);
		string GetString(string name);
		vec3 GetVector3(string space, string name);
		vec3 GetVector3(string name);
		vec4 GetColor(string space, string name);
		vec4 GetColor(string name);
		b8 GetB8(string space, string name);
		b8 GetB8(string name);
		CRange GetRange(string space, string name);
		CRange GetRange(string name);

		//sync accessors
		CBPM& GetBeat(const string &name);
		CTriggerSystem& GetTriggers(const string &name);
		CEvent& GetEvent(const string &name);

		//update
		void Update(f32 iNowFrame);
	};

}	// namespace MOE

#endif
