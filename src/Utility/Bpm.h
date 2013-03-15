/*
 *  Bpm.h
 *  Beat per sync
 *
 *  Created by GOT / Eldorado on 2013/03/08.
 *
 */
#ifndef INCLUDE_BPM_H
#define INCLUDE_BPM_H

//==============================================================
// header
//==============================================================
#include "../Core/Type.h"
#include <string>

using namespace std;

namespace MOE {

	//==============================================================
	// CBPM class
	//	A class for beatsyncing.
	//==============================================================
	class CBPM
	{
	private:
		f32 mValue;
		s32	mOffset;
		s32 mFramecount;
		s32 mPreviousbeatdist;
		b8 mChanged;
		f32 mBeat;
		string mName;

	public:
		CBPM();
		~CBPM();

		void Init( f32 value, s32 offset );

		string GetName()
		{
			return mName;
		}
		void setName(string name)
		{
			mName = name;
		}

		void SetBPM(f32 bpm)
		{
			mValue = bpm;
		};
		void SetOffset( s32 offset )
		{
			mOffset = offset;
		};

		// called once per frame, sets the internals    
		void Update( f32 iNowFrame );
		// returns the distance to the last beat, 1 = on the beat, 0 = the next beat
		f32 GetValue();
		// how many beats have we hit
		s32 GetBeatcount();
		// returns true if the beat happened on this frame (could be used for strobes or something)
		b8 GetChanged();
	};

}	// namespace MOE

#endif	// _C_BPM_H_GUARD_
