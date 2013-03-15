/*
 *  Event.h
 *  Event
 *
 *  Created by GOT / Eldorado on 2013/03/08.
 *
 */
#ifndef INCLUDE_EVENT_H
#define INCLUDE_EVENT_H

//==============================================================
// header
//==============================================================
#include <string>
#include <vector>
#include <map>
#include "../Core/Type.h"

//==============================================================
// Event class
//==============================================================
namespace MOE {

	class CEvent
	{
	public:
		s32 mTime;
		s32 mLength;

	public:
		CEvent() {};
		~CEvent() {};

		f32 GetValue(f32 iNowFrame);
		b8 HasPassed(f32 iNowFrame);
	};

}	// namespace MOE

#endif	// _C_EVENT_H_GUARD_
