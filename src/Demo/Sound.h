#ifndef INCLUDE_SOUND_H
#define INCLUDE_SOUND_H


#include <bass.h>

class Sound
{
public:
	Sound()
	{
		m_inited = false;
		m_chan = 0;
        m_isplaying = false;
		if (!(m_inited = init()))
			printf("faild play music");
	}

	~Sound()
	{
		term();
	}

	bool Load(const char* soundfile)
	{
		if (!m_inited)
			return false;
        m_soundfile = std::string(soundfile);
        
		BASS_MusicFree(m_chan);
		BASS_StreamFree(m_chan);
		if (!(m_chan = BASS_StreamCreateFile(FALSE, soundfile,0,0,0))
		&&  !(m_chan  = BASS_MusicLoad(FALSE,soundfile,0,0, BASS_MUSIC_RAMP,1))) 
		{
			return false;
		}	
		return true;
	}

	void Play()
	{
		if (!m_inited)
			return;
        
        m_isplaying = true;
		BASS_ChannelPlay(m_chan,FALSE);
	}
    bool IsPlaying() const
    {
        return m_isplaying;
    }
	void Stop()
	{
		if (!m_inited)
			return;

        m_isplaying = false;
		BASS_ChannelStop(m_chan);
	}
	void Pause()
	{
		if (!m_inited)
			return;
        
        m_isplaying = false;
		BASS_ChannelPause(m_chan);
	}

	double GetPosTime() const
	{
		if(!m_inited)
			return 0.0;
		const QWORD pos = BASS_ChannelGetPosition(m_chan, BASS_POS_BYTE);
		const double pos_time = BASS_ChannelBytes2Seconds(m_chan, pos);
		return pos_time;
	}
    bool SetPosTime(double tm)
    {
        if (!m_inited)
            return false;
        QWORD bytetime = BASS_ChannelSeconds2Bytes(m_chan, tm);
        BASS_ChannelSetPosition(m_chan, bytetime, BASS_POS_BYTE);
        return true;
    }
	float* GetFFTData()
	{
		static float fft[256]={};
		BASS_ChannelGetData(m_chan,fft,BASS_DATA_FFT512); // get the FFT data 512samples->256floats
		return fft;
	}
    unsigned int GetFFTNum()
    {
        return 256;
    }
    void SetVolume(float vol)
    {
        BASS_SetVolume(vol);
    }
    
    float GetVolume()
    {
        return BASS_GetVolume();
    }
    
    const char* GetFileName() const
    {
        return m_soundfile.c_str();
    }
private:
	DWORD m_chan;
	bool m_inited;
    bool m_isplaying;
    std::string m_soundfile;

	bool init()
	{
		// check the correct BASS was loaded
		if (HIWORD(BASS_GetVersion())!=BASSVERSION) {
			return false;
		}
		if (!BASS_Init(-1,44100,0,0,NULL)) 
			return false;
	
		return true;
	}
	void term()
	{
		BASS_Free();
	}
};

#endif // INCLUDE_SOUND_H
