/*
 *  Range.h
 *  Range
 *
 *  Created by GOT / Eldorado on 2013/03/08.
 *
 */
#ifndef INCLUDE_RANGE_H
#define INCLUDE_RANGE_H

//==============================================================
// header
//==============================================================
#include "../Core/Type.h"

namespace MOE {

	//==============================================================
	// CRange class
	//	@Note:
	//	範囲系パラメータを制御します。CParameters.hにあるクラスと連動して使用します。
	//==============================================================
	class CRange
	{
	private:
		f32 mMinimum;
		f32 mMaximum;

	public:
		CRange();
		CRange(f32 minimum, f32 maximum);
		CRange(const class CRange &r)
		{
			mMinimum = r.mMinimum;
			mMaximum = r.mMaximum;
		}
		~CRange();

		void Set(f32 minimum, f32 maximum);
		f32 GetValue(f32 t);
		f32 Lerp(f32 t);
		f32 GetMinimum()
		{
			return mMinimum;
		}
		f32 GetMaximum()
		{
			return mMaximum;
		}
		f32 GetRandValue();
	};

}	// namespace MOE

#endif
