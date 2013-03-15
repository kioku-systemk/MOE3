//
// CParameters.cpp
//	@Author: Got / Eldorado
//
//	@Note:
//	スクリプト系パラメータ。
//
//===================================================================================
// define
//===================================================================================
#pragma warning ( disable : 4786)

//===================================================================================
// header
//===================================================================================
#include "../Core/Log.h"
#include "Range.h"
#include "BPM.h"
#include "Trigger.h"
#include "Event.h"
#include "Parameters.h"

namespace MOE {

	//===================================================================================
	// CParameterSpace class
	//===================================================================================
	CParameterSpace::CParameterSpace()
	{
	}

	CParameterSpace::~CParameterSpace()
	{
		Clear();
	}

	void CParameterSpace::Clear()
	{
		mFloats.clear();
		mInts.clear();
		mVectors.clear();
		mColor.clear();
		mStrings.clear();
		mBools.clear();
		mRanges.clear();
	}

	vec3 CParameterSpace::GetVector3(string name)
	{
		if(mVectors.find(name) == mVectors.end()){
			MOELogW( "not found vector3 %s\n", name.c_str() );
			return vec3(0, 0, 0);
		}else{
			return mVectors[name];
		}
	}

	vec4 CParameterSpace::GetColor(string name)
	{
		if(mColor.find(name) == mColor.end()){
			MOELogW( "not found color %s\n", name.c_str() );
			return vec4(0, 0, 0, 1);
		}else{
			return mColor[name];
		}
	}

	f32 CParameterSpace::GetF32(string name)
	{
		if(mFloats.find(name) == mFloats.end()){
			MOELogW( "not found float %s\n", name.c_str() );
			return 0.0f;
		}else{
			return mFloats[name];
		}
	}

	s32 CParameterSpace::GetS32(string name)
	{
		if(mInts.find(name) == mInts.end()){
			MOELogW( "not found int %s\n", name.c_str() );
			return 0;
		}else{
			return mInts[name];
		}
	}

	string CParameterSpace::GetString(string name)
	{
		if(mStrings.find(name) == mStrings.end()){
			MOELogW( "not found string %s\n", name.c_str() );
			return "";
		}else{
			return mStrings[name];
		}
	}
	b8 CParameterSpace::GetB8(string name)
	{
		if(mBools.find(name) == mBools.end()){
			MOELogW( "not found b8 %s\n", name.c_str() );
			return "";
		}else{
			return mBools[name];
		}
	}

	CRange CParameterSpace::GetRange(string name)
	{
		if (mRanges.find(name) == mRanges.end()){
			MOELogW( "not found range %s\n", name.c_str() );
			return CRange(0, 0);
		}else{
			return mRanges[name];
		}
	}

	void CParameterSpace::AddF32(string name, f32 value)
	{
		if (mFloats.find(name) != mFloats.end()){
			MOELogW( "already exist float %s\n", name.c_str() );
		}else{
			mFloats[name] = value;
		}
	}

	void CParameterSpace::AddS32(string name, s32 value)
	{
		if (mInts.find(name) != mInts.end()){
			MOELogW( "already exist int %s\n", name.c_str() );
		}else{
			mInts[name] = value;
		}
	}
	void CParameterSpace::AddVector3(string name, vec3 &value)
	{
		if (mVectors.find(name) != mVectors.end()){
			MOELogW( "already exist vector3 %s\n", name.c_str() );
		}else{
			mVectors[name] = value;
		}
	}
	void CParameterSpace::AddColor(string name, vec4 &value)
	{
		if (mColor.find(name) != mColor.end()){
			MOELogW( "already exist color %s\n", name.c_str() );
		}else{
			mColor[name] = value;
		}
	}
	void CParameterSpace::AddString(string name, string &value)
	{
		if (mStrings.find(name) != mStrings.end()){
			MOELogW( "already exist string %s\n", name.c_str() );
		}else{
			mStrings[name] = value;
		}
	}
	void CParameterSpace::AddB8(string name, b8 value)
	{
		if (mBools.find(name) != mBools.end()){
			MOELogW( "already exist bool %s\n", name.c_str() );
		}else{
			mBools[name] = value;
		}
	}

	void CParameterSpace::AddRange(string name, CRange &value)
	{
		if (mRanges.find(name) != mRanges.end()){
			MOELogW( "already exist range %s\n", name.c_str() );
		}else{
			mRanges[name] = value;
		}
	}

	//===================================================================================
	// CParameters class
	//===================================================================================
	CParameters::CParameters()
	{
		mCurrentName = "";
	}

	CParameters::~CParameters()
	{
	}

	//utils
	void CParameters::Clear()
	{
		map<string, CParameterSpace>::iterator it;
		for (it = mParams.begin(); it != mParams.end(); it++){
			CParameterSpace &s = (*it).second;
			s.Clear();
		}

		mBeats.clear();
		mTriggers.clear();
		mEvents.clear();
	}

	void CParameters::Save(string filename)
	{
	}

	void CParameters::UseNamespace(string space)
	{
		mCurrentName = space;
	}

	//insertion
	void CParameters::AddF32(string space, string name, f32 value)
	{
		CParameterSpace &s = mParams[space];
		s.AddF32(name, value);
	}

	void CParameters::AddB8(string space, string name, b8 value)
	{
		CParameterSpace &s = mParams[space];
		s.AddB8(name, value);
	}

	void CParameters::AddS32(string space, string name, s32 value)
	{
		CParameterSpace &s = mParams[space];
		s.AddS32(name, value);
	}

	void CParameters::AddVector3(string space, string name, vec3 &value)
	{
		CParameterSpace &s = mParams[space];
		s.AddVector3(name, value);
	}

	void CParameters::AddColor(string space, string name, vec4 &value)
	{
		CParameterSpace &s = mParams[space];
		s.AddColor(name, value);
	}

	void CParameters::AddString(string space, string name, string &value)
	{
		CParameterSpace &s = mParams[space];
		s.AddString(name, value);
	}

	void CParameters::AddRange(string space, string name, CRange &value)
	{
		CParameterSpace &s = mParams[space];
		s.AddRange(name, value);
	}

	void CParameters::AddBeat(string& name, CBPM *bpm)
	{
		mBeats[name] = bpm;
	}

	void CParameters::AddTriggers(string& name, CTriggerSystem *triggers)
	{
		mTriggers[name] = triggers;
	}

	void CParameters::AddEvent(string& name, CEvent *e)
	{
		mEvents[name] = e;
	}

	f32 CParameters::GetF32(string space, string name)
	{
		CParameterSpace &s = mParams[space];
		return s.GetF32(name);
	}

	f32 CParameters::GetF32(string name)
	{
		return GetF32(mCurrentName, name);
	}

	s32 CParameters::GetS32(string space, string name)
	{
		CParameterSpace &s = mParams[space];
		return s.GetS32(name);
	}

	s32 CParameters::GetS32(string name)
	{
		return GetS32(mCurrentName, name);
	}

	b8 CParameters::GetB8(string space, string name)
	{
		CParameterSpace &s = mParams[space];
		return s.GetB8(name);
	}

	b8 CParameters::GetB8(string name)
	{
		return GetB8(mCurrentName, name);
	}

	vec3 CParameters::GetVector3(string space, string name)
	{
		CParameterSpace &s = mParams[space];
		return s.GetVector3(name);
	}

	vec3 CParameters::GetVector3(string name)
	{
		return GetVector3(mCurrentName, name);
	}

	vec4 CParameters::GetColor(string space, string name)
	{
		CParameterSpace &s = mParams[space];
		return s.GetColor(name);
	}

	vec4 CParameters::GetColor(string name)
	{
		return GetColor(mCurrentName, name);
	}

	string CParameters::GetString(string space, string name)
	{
		CParameterSpace &s = mParams[space];
		return s.GetString(name);
	}

	string CParameters::GetString(string name)
	{
		return GetString(mCurrentName, name);
	}

	CRange CParameters::GetRange(string space, string name)
	{
		CParameterSpace &s = mParams[space];
		return s.GetRange(name);
	}

	CRange CParameters::GetRange(string name)
	{
		return GetRange(mCurrentName, name);
	}

	CBPM& CParameters::GetBeat(const string &name)
	{
		return *mBeats[name];
	}

	CTriggerSystem& CParameters::GetTriggers(const string &name)
	{
		if(mTriggers.find(name) == mTriggers.end()){
			MOELogW( "Cannot find triggers %s!\n", name );
		}
		return *mTriggers[name];
	}

	CEvent& CParameters::GetEvent(const string &name)
	{
		if(mEvents.find(name) == mEvents.end()){
			MOELogW( "Cannot find event %s!\n", name );
		}
		return *mEvents[name];
	}

	void CParameters::Update(f32 iNowFrame)
	{
		// update BPM counters
		map<std::string, CBPM*>::iterator it;
		for (it = mBeats.begin(); it != mBeats.end(); it++){
			CBPM *beat = (*it).second;
			beat->Update(iNowFrame);
		}
	}

}	// namespace MOE