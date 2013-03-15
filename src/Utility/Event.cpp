/*
 *  Event.cpp
 *  Event
 *
 *  Created by GOT / Eldorado on 2013/03/08.
 *
 */
//==============================================================
// header
//==============================================================
#include "Event.h"

//==============================================================
// Event class
//==============================================================
namespace MOE {

	f32 CEvent::GetValue( f32 iNowFrame )
	{
		s32 time = (s32)iNowFrame;
		f32 value = 0.0f;
		if (time >= mTime && time < mTime + mLength){
			value = (time - mTime) / (f32)mLength;
		}else if (time >= mTime + mLength){
			value = 1.0f;
		}else{
			value = 0.0f;
		}
		return value;
	}

	b8 CEvent::HasPassed( f32 iNowFrame )
	{
		s32 time = (s32)iNowFrame;
	
		return time >= mTime;
	}

}	// namespace MOE