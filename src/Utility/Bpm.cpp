/*
 *  Bpm.h
 *  Beat per sync
 *
 *  Created by GOT / Eldorado on 2013/03/08.
 *
 */
//==============================================================
//  Headers
//==============================================================
#include "Bpm.h"

namespace MOE {

	//==============================================================
	//  BPM class
	//==============================================================
	CBPM::CBPM()
	{
	}

	CBPM::~CBPM()
	{
		mFramecount = 0;
		mValue = 0;
		mOffset = 0;
		mPreviousbeatdist = 0;
		mBeat = 0.0f;
	}

	void CBPM::Init( f32 value, s32 offset )
	{
		// value = beats per min | offset - time between ticks
		mFramecount = 0;
		mValue = value;
		mOffset = offset;
		mPreviousbeatdist = 0;
		mBeat = 0.0f;

		mChanged = false;
	}

	void CBPM::Update( f32 iNowFrame )
	{
		const s32 bpmmicroseconds = (s32)(60000000/mValue);
		s32 song_pos = (s32)iNowFrame;
		song_pos *= 1000;

		mChanged = false;

		int beatdist = (song_pos + mOffset) % bpmmicroseconds;

		if (beatdist < mPreviousbeatdist){
			mChanged = true;
			mFramecount++;
		}

		mBeat = (bpmmicroseconds-beatdist)/(f32)bpmmicroseconds;
		mPreviousbeatdist = beatdist;
	}

	f32 CBPM::GetValue()
	{
		return mBeat;
	}

	s32 CBPM::GetBeatcount()
	{
		return mFramecount;
	}

	b8 CBPM::GetChanged()
	{
		return mChanged;
	}

}	// namespace MOE