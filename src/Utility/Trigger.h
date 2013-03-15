/*
 *  Trigger.h
 *  Trigger
 *
 *  Created by GOT / Eldorado on 2013/03/07.
 *
 */
#ifndef INCLUDE_TRIGGER_H
#define INCLUDE_TRIGGER_H

//==============================================================
// header
//==============================================================
#include <string>
#include <vector>
#include <map>

#include "../Core/Type.h"

using namespace std;

namespace MOE {

	//==============================================================
	// CTrigger class
	//==============================================================
	class CTrigger
	{
	public:
		CTrigger() {};
		~CTrigger() {};

		s32 mTime;
		s32 mLength;
	};

	//==============================================================
	// CTriggerSystem class
	//==============================================================
	class CTriggerSystem
	{
	private:
		vector<CTrigger> mTriggers;

	public:
		CTriggerSystem();
		~CTriggerSystem();

		void AddTrigger(CTrigger &trigger);
		void GetTrigger(s32 index, s32 &time, s32 &length);
		f32 GetValue(f32 iNowFrame);
		s32 GetPosition(f32 iNowFrame);
		s32 GetCount();
	};

}	// namespace MOE

#endif	// _C_TRIGGER_H_GUARD_
