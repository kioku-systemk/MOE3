/*
 *  Trigger.h
 *  Trigger
 *
 *  Created by GOT / Eldorado on 2013/03/07.
 *
 */

//==============================================================
// header
//==============================================================
#include "../Core/Log.h"
#include "Trigger.h"
#include <algorithm>

namespace {

	//==============================================================
	// sort function
	//==============================================================
	bool triggerSortCmpFunc(const MOE::CTrigger &a, const MOE::CTrigger &b)
	{
		return a.mTime < b.mTime;
	}

}

namespace MOE {

	//==============================================================
	// CTriggerSystem class
	//==============================================================
	CTriggerSystem::CTriggerSystem()
	{
		mTriggers.clear();
	}

	CTriggerSystem::~CTriggerSystem()
	{
	}

	void CTriggerSystem::AddTrigger(CTrigger &trigger)
	{
		mTriggers.push_back(trigger);
		sort(mTriggers.begin(), mTriggers.end(), triggerSortCmpFunc);
	}

	f32 CTriggerSystem::GetValue( f32 iNowFrame )
	{
		f32 value = 0.0;
		s32 time = (s32)iNowFrame;
		vector<CTrigger>::iterator it;

		for (it = mTriggers.begin(); it < mTriggers.end(); it++){
			CTrigger &t = *it;
			if (time >= t.mTime && time < t.mTime + t.mLength){
				//1 time == t.time
				//0 time == t.time + length
				float pos = 1.0f - (time - t.mTime) / (float)t.mLength;
				value += pos;
			}
		}
		//clamp
		value = min(value, 1);
		return value;
	}

	void CTriggerSystem::GetTrigger(s32 index, s32 &time, s32 &length)
	{
		s32 size = GetCount();
		if (index < 0){
			MOELogW( "trying to get trigger that has the index (%d)", index );
			return;
		}
		if (index >= size){
			MOELogW( "trying to get trigger %d from the system that has only size(%d)", index, size );
			return;
		}
		time = mTriggers[index].mTime;
		length = mTriggers[index].mLength;
	}

	s32 CTriggerSystem::GetPosition(f32 iNowFrame)
	{
		s32 time = (s32)iNowFrame;
		vector<CTrigger>::iterator it;

		s32 count = 0;
		s32 size = (s32)mTriggers.size();

		for (it = mTriggers.begin(); it < mTriggers.end(); it++){
			CTrigger &t = *it;
			if (time < t.mTime){
				break;
			}
			count++;
		}

		return count;
	}

	s32 CTriggerSystem::GetCount()
	{
		return (s32)mTriggers.size();
	}

}	// namespace MOE