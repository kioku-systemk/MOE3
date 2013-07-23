/*
 *  Range.h
 *  Range
 *
 *  Created by GOT / Eldorado on 2013/03/08.
 *
 */
//==============================================================
// header
//==============================================================
#include "Range.h"

namespace MOE {

	//==============================================================
	// CCRange class
	//==============================================================
	CRange::CRange()
	{
		mMinimum = 0;
		mMaximum = 0;
	}

	CRange::CRange(f32 minimum, f32 maximum)
	{
		Set(minimum, maximum);
	}

	CRange::~CRange()
	{
	}

	void CRange::Set(f32 minimum, f32 maximum)
	{
		mMinimum = minimum;
		mMaximum = maximum;
	}

	f32 CRange::GetValue(f32 t)
	{
		return mMinimum + (mMaximum - mMinimum) * t;
	}

	f32 CRange::Lerp(f32 t)
	{
		if (t < 0.0f){
			return mMinimum;
		}
		if (t > 1.0f){
			return mMaximum;
		}
		return mMinimum + (mMaximum - mMinimum) * t;
	}

	f32 CRange::GetRandValue()
	{
		return mMinimum + (mMaximum - mMinimum) * (float)((rand()%RAND_MAX)/(float)RAND_MAX);
	}

}	// namespace MOE