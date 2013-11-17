//
//  Thread.h
//

#ifndef Include_MOEThread_h
#define Include_MOEThread_h

#include "Type.h"
#include "Log.h"

#include <stdio.h>

#if MOE_PLATFORM_WINDOWS
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif

namespace MOE
{
	class Thread
	{
	public:
    	Thread() {
       		m_handle = 0;
    	}
    	virtual ~Thread() {
        	terminateThread();
    	}
    
    	// if return true, thread will end.
    	//virtual b8 operator()(void) = 0;
   		virtual b8 process(void) = 0;

	protected:
    	void go() {
        	m_handle = createThread();
    	}
    
	private:
	#if MOE_PLATFORM_WINDOWS
    	typedef HANDLE		ThreadHandle;
	#else
    	typedef pthread_t	ThreadHandle;
	#endif
    
    	ThreadHandle m_handle;
    
	#if MOE_PLATFORM_WINDOWS
    	static DWORD WINAPI threadFunc(LPVOID thisptr) {
	#else
    	static void* threadFunc(void* thisptr) {
	#endif
        	Thread* p = reinterpret_cast<Thread*>(thisptr);
        	//while (!(*p)());
        	while (!p->process());        
        	return 0;
    	}
    
    	ThreadHandle createThread() {
        	ThreadHandle hThread;
    #if MOE_PLATFORM_WINDOWS
        	unsigned long dwThreadId;
        	hThread = CreateThread(NULL, 0, &threadFunc, this, 0, &dwThreadId);
    #else
        	pthread_create(&hThread, NULL, &threadFunc, this);
    #endif
        	return hThread;
    	}
    
    	void terminateThread() {
        	if (!m_handle)
            	return;
    #if MOE_PLATFORM_WINDOWS
        	CloseHandle(m_handle);
    #else
        	pthread_cancel(m_handle);
    #endif
        	MOELogD("term Thread\n");
		}
	};


	class Sleep {
	public:
		Sleep(s32 t) {
    #if MOE_PLATFORM_WINDOWS
			::Sleep(t);
    #else
			usleep(t*1000);
			//pthread_yield_np();
    #endif
		}
	};


	class CriticalSection
	{
		public:
			CriticalSection(void) {
    #if MOE_PLATFORM_WINDOWS
				if (InitializeCriticalSectionAndSpinCount(&this->m_cSection, 0) == 0)
					throw("Could not create a CriticalSection");
    #else
				if (pthread_mutex_init(&this->m_cSection, NULL) != 0)
					throw("Could not create a CriticalSection");
    #endif
			}

    		~CriticalSection(void) {
				this->WaitForFinish(); // acquire ownership
    #if MOE_PLATFORM_WINDOWS
				DeleteCriticalSection(&this->m_cSection);
    #else
				pthread_mutex_destroy(&this->m_cSection);
    #endif
			}
    
			void WaitForFinish(void) {
				while (!this->TryEnter()) {
            		Sleep(1);// wait 1ms
				}
			}
    
			void Enter(void) {
				this->WaitForFinish(); // acquire ownership

        		/// Alternative codes
        		//#if MOE_PLATFORM_WINDOWS
        		//		EnterCriticalSection(&this->m_cSection);
        		//#else
        		//		pthread_mutex_lock(&this->m_cSection);
        		//#endif
			}
	
			void Leave(void) {
    #if MOE_PLATFORM_WINDOWS
				LeaveCriticalSection(&this->m_cSection);
    #else
				pthread_mutex_unlock(&this->m_cSection);
    #endif
			}
    
			b8 TryEnter(void) {
    #if MOE_PLATFORM_WINDOWS
				return (TryEnterCriticalSection(&this->m_cSection) == TRUE);
    #else
				return (pthread_mutex_trylock(&this->m_cSection) == 0);
    #endif
			}
    
		private:
    
    #if MOE_PLATFORM_WINDOWS
			CRITICAL_SECTION m_cSection; //!< internal system critical section object (windows)
    #else
			pthread_mutex_t m_cSection; //!< internal system critical section object (*nix)
    #endif
	}; 


	class Lock {
		public:
    		Lock()  { }
    		~Lock() { }
	    	void Enter() { m_cs.Enter();	}
    		void Leave() { m_cs.Leave();	}
		private:
    		CriticalSection m_cs;
	};
}
#endif
