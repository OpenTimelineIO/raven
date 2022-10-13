/*
SoLoud audio engine
Copyright (c) 2013-2015 Jari Komppa

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/
/*
License of the speech synth that can be enabled with YES_IMGUISOLOUD_SPEECH

The speech synth is based on rsynth by the late 
Nick Ing-Simmons (et al).

He described the legal status as:

    This is a text to speech system produced by 
    integrating various pieces of code and tables 
    of data, which are all (I believe) in the 
    public domain.
    
Since then, the rsynth source code has passed legal
checks by several open source organizations, so it
"should" be pretty safe.

The primary copyright claims seem to have to do
with text-to-speech dictionary use, which I've
removed completely.

I've done some serious refactoring, clean-up and 
feature removal on the source, as all I need is 
"a" free, simple speech synth, not a "good" 
speech synth. Since I've removed a bunch of stuff,
this is probably safer public domain release
than the original.

(I'm rather surprised there's no good public domain
speech synths out there; after all, it's 2013..)

I'm placing my changes in public domain as well,
or if that's not acceptable for you, then CC0:
http://creativecommons.org/publicdomain/zero/1.0/

The SoLoud interface files (soloud_speech.*) are
under ZLib/LibPNG license.

-- Jari Komppa
   2013
*/
/*
License of the TED / SID support that can be enabled with YES_IMGUISOLOUD_TEDSID

The TED / SID support is based on tedplay (c) 2012 Attila Grosz, used under Unlicense http://unlicense.org/
*/

#ifndef NO_IMGUISOLOUD_METHODS
#include <imgui.h>
#undef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#endif //NO_IMGUISOLOUD_METHODS

#include "imguisoloud.h"

// suppress some warning-----------------------------------------------------------------------
#ifdef _MSC_VER
#   pragma warning(disable:4100)
//#   pragma warning(disable: 4146) // unary minus operator applied to unsigned type, result still unsigned  // used by (-lsb) construct to turn 0 into 0 and 1 into 0xFFFF
#else //_MSC_VER
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-parameter"
//#   pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#   pragma GCC diagnostic ignored "-Wunused-variable"
#endif //_MSC_VER

//extern FILE* ImFileOpen(const char* filename, const char* mode);

//----soloud_internal.h------------------------------------------------------------------------
namespace SoLoud
{
	// SDL back-end initialization call
	result sdl_init(SoLoud::Soloud *aSoloud, unsigned int aFlags = Soloud::CLIP_ROUNDOFF, unsigned int aSamplerate = 44100, unsigned int aBuffer = 2048, unsigned int aChannels = 2);

	// SDL "non-dynamic" back-end initialization call
	result sdlstatic_init(SoLoud::Soloud *aSoloud, unsigned int aFlags = Soloud::CLIP_ROUNDOFF, unsigned int aSamplerate = 44100, unsigned int aBuffer = 2048, unsigned int aChannels = 2);

	// SDL2 "non-dynamic" back-end initialization call
	result sdl2static_init(SoLoud::Soloud *aSoloud, unsigned int aFlags = Soloud::CLIP_ROUNDOFF, unsigned int aSamplerate = 44100, unsigned int aBuffer = 2048, unsigned int aChannels = 2);

	// OpenAL back-end initialization call
	result openal_init(SoLoud::Soloud *aSoloud, unsigned int aFlags = Soloud::CLIP_ROUNDOFF, unsigned int aSamplerate = 44100, unsigned int aBuffer = 2048, unsigned int aChannels = 2);

	// Core Audio driver back-end initialization call
	result coreaudio_init(SoLoud::Soloud *aSoloud, unsigned int aFlags = Soloud::CLIP_ROUNDOFF, unsigned int aSamplerate = 44100, unsigned int aBuffer = 2048, unsigned int aChannels = 2);

	// OpenSL ES back-end initialization call
	result opensles_init(SoLoud::Soloud *aSoloud, unsigned int aFlags = Soloud::CLIP_ROUNDOFF, unsigned int aSamplerate = 44100, unsigned int aBuffer = 2048, unsigned int aChannels = 2);

	// PortAudio back-end initialization call
	result portaudio_init(SoLoud::Soloud *aSoloud, unsigned int aFlags = Soloud::CLIP_ROUNDOFF, unsigned int aSamplerate = 44100, unsigned int aBuffer = 2048, unsigned int aChannels = 2);

	// WinMM back-end initialization call
	result winmm_init(SoLoud::Soloud *aSoloud, unsigned int aFlags = Soloud::CLIP_ROUNDOFF, unsigned int aSamplerate = 44100, unsigned int aBuffer = 4096, unsigned int aChannels = 2);

	// XAudio2 back-end initialization call
	result xaudio2_init(SoLoud::Soloud *aSoloud, unsigned int aFlags = Soloud::CLIP_ROUNDOFF, unsigned int aSamplerate = 44100, unsigned int aBuffer = 2048, unsigned int aChannels = 2);

	// WASAPI back-end initialization call
	result wasapi_init(SoLoud::Soloud *aSoloud, unsigned int aFlags = Soloud::CLIP_ROUNDOFF, unsigned int aSamplerate = 44100, unsigned int aBuffer = 4096, unsigned int aChannels = 2);

	// OSS back-end initialization call
	result oss_init(SoLoud::Soloud *aSoloud, unsigned int aFlags = Soloud::CLIP_ROUNDOFF, unsigned int aSamplerate = 44100, unsigned int aBuffer = 2048, unsigned int aChannels = 2);

	// ALSA back-end initialization call
	result alsa_init(SoLoud::Soloud *aSoloud, unsigned int aFlags = Soloud::CLIP_ROUNDOFF, unsigned int aSamplerate = 44100, unsigned int aBuffer = 2048, unsigned int aChannels = 2);

	// null driver back-end initialization call
	result null_init(SoLoud::Soloud *aSoloud, unsigned int aFlags = Soloud::CLIP_ROUNDOFF, unsigned int aSamplerate = 44100, unsigned int aBuffer = 2048, unsigned int aChannels = 2);

	// Deinterlace samples in a buffer. From 12121212 to 11112222
	void deinterlace_samples_float(const float *aSourceBuffer, float *aDestBuffer, unsigned int aSamples, unsigned int aChannels);

	// Interlace samples in a buffer. From 11112222 to 12121212
	void interlace_samples_float(const float *aSourceBuffer, float *aDestBuffer, unsigned int aSamples, unsigned int aChannels);

	// Convert to 16-bit and interlace samples in a buffer. From 11112222 to 12121212
	void interlace_samples_s16(const float *aSourceBuffer, short *aDestBuffer, unsigned int aSamples, unsigned int aChannels);
};

#define FOR_ALL_VOICES_PRE \
		handle *h_ = NULL; \
		handle th_[2] = { aVoiceHandle, 0 }; \
		lockAudioMutex(); \
		h_ = voiceGroupHandleToArray(aVoiceHandle); \
		if (h_ == NULL) h_ = th_; \
		while (*h_) \
		{ \
			int ch = getVoiceFromHandle(*h_); \
			if (ch != -1)  \
			{

#define FOR_ALL_VOICES_POST \
			} \
			h_++; \
		} \
		unlockAudioMutex();

#define FOR_ALL_VOICES_PRE_3D \
		handle *h_ = NULL; \
		handle th_[2] = { aVoiceHandle, 0 }; \
		h_ = voiceGroupHandleToArray(aVoiceHandle); \
		if (h_ == NULL) h_ = th_; \
				while (*h_) \
						{ \
			int ch = (*h_ & 0xfff) - 1; \
			if (ch != -1 && m3dData[ch].mHandle == *h_)  \
						{

#define FOR_ALL_VOICES_POST_3D \
						} \
			h_++; \
						} 
//----soloud_thread.h---------------------------------------------------------------------------
namespace SoLoud
{
	namespace Thread
	{
		typedef void (*threadFunction)(void *aParam);

        struct ThreadHandleData;
        typedef ThreadHandleData* ThreadHandle;

		void * createMutex();
		void destroyMutex(void *aHandle);
		void lockMutex(void *aHandle);
		void unlockMutex(void *aHandle);

		ThreadHandle createThread(threadFunction aThreadFunction, void *aParameter);

		void sleep(int aMSec);
        void wait(ThreadHandle aThreadHandle);
        void release(ThreadHandle aThreadHandle);

#define MAX_THREADPOOL_TASKS 1024

		class PoolTask
		{
		public:
			virtual void work() = 0;
		};

		class Pool
		{
		public:
			// Initialize and run thread pool. For thread count 0, work is done at addWork call.
			void init(int aThreadCount);
			// Ctor, sets known state
			Pool();
			// Dtor. Waits for the threads to finish. Work may be unfinished.
			~Pool();
			// Add work to work list. Object is not automatically deleted when work is done.
			void addWork(PoolTask *aTask);
			// Called from worker thread to get a new task. Returns null if no work available.
			PoolTask *getWork();
		public:
			int mThreadCount; // number of threads
			ThreadHandle *mThread; // array of thread handles
			void *mWorkMutex; // mutex to protect task array/maxtask
			PoolTask *mTaskArray[MAX_THREADPOOL_TASKS]; // pointers to tasks
			int mMaxTask; // how many tasks are pending
			int mRobin; // cyclic counter, used to pick jobs for threads
			volatile int mRunning; // running flag, used to flag threads to stop
		};
	}
}
//----soloud_thread.cpp---------------------------------------------------------------------------
#ifdef WINDOWS_VERSION
#include <Windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

namespace SoLoud
{
	namespace Thread
	{
#ifdef WINDOWS_VERSION
        struct ThreadHandleData
        {
            HANDLE thread;
        };

		void * createMutex()
		{
			CRITICAL_SECTION * cs = new CRITICAL_SECTION;
			InitializeCriticalSectionAndSpinCount(cs, 100);
			return (void*)cs;
		}

		void destroyMutex(void *aHandle)
		{
			CRITICAL_SECTION *cs = (CRITICAL_SECTION*)aHandle;
			DeleteCriticalSection(cs);
			delete cs;
		}

		void lockMutex(void *aHandle)
		{
			CRITICAL_SECTION *cs = (CRITICAL_SECTION*)aHandle;
			if (cs)
			{
				EnterCriticalSection(cs);
			}
		}

		void unlockMutex(void *aHandle)
		{
			CRITICAL_SECTION *cs = (CRITICAL_SECTION*)aHandle;
			if (cs)
			{
				LeaveCriticalSection(cs);
			}
		}

		struct soloud_thread_data
		{
			threadFunction mFunc;
			void *mParam;
		};

		static DWORD WINAPI threadfunc(LPVOID d)
		{
			soloud_thread_data *p = (soloud_thread_data *)d;
			p->mFunc(p->mParam);
			delete p;
			return 0;
		}

        ThreadHandle createThread(threadFunction aThreadFunction, void *aParameter)
		{
			soloud_thread_data *d = new soloud_thread_data;
			d->mFunc = aThreadFunction;
			d->mParam = aParameter;
			HANDLE h = CreateThread(NULL,0,threadfunc,d,0,NULL);
            if (0 == h)
            {
                return 0;
            }
            ThreadHandleData *threadHandle = new ThreadHandleData;
            threadHandle->thread = h;
            return threadHandle;
		}

		void sleep(int aMSec)
		{
			Sleep(aMSec);
		}

        void wait(ThreadHandle aThreadHandle)
        {
            WaitForSingleObject(aThreadHandle->thread, INFINITE);
        }

        void release(ThreadHandle aThreadHandle)
        {
            CloseHandle(aThreadHandle->thread);
            delete aThreadHandle;
        }

#else // pthreads
        struct ThreadHandleData
        {
            pthread_t thread;
        };

		void * createMutex()
		{
			pthread_mutex_t *mutex;
			mutex = new pthread_mutex_t;
		
			pthread_mutexattr_t attr;
			pthread_mutexattr_init(&attr);

			pthread_mutex_init(mutex, &attr);
		
			return (void*)mutex;
		}

		void destroyMutex(void *aHandle)
		{
			pthread_mutex_t *mutex = (pthread_mutex_t*)aHandle;

			if (mutex)
			{
				pthread_mutex_destroy(mutex);
				delete mutex;
			}
		}

		void lockMutex(void *aHandle)
		{
			pthread_mutex_t *mutex = (pthread_mutex_t*)aHandle;
			if (mutex)
			{
				pthread_mutex_lock(mutex);
			}
		}

		void unlockMutex(void *aHandle)
		{
			pthread_mutex_t *mutex = (pthread_mutex_t*)aHandle;
			if (mutex)
			{
				pthread_mutex_unlock(mutex);
			}
		}

		struct soloud_thread_data
		{
			threadFunction mFunc;
			void *mParam;
		};

		static void * threadfunc(void * d)
		{
			soloud_thread_data *p = (soloud_thread_data *)d;
			p->mFunc(p->mParam);
			delete p;
			return 0;
		}

		ThreadHandle createThread(threadFunction aThreadFunction, void *aParameter)
		{
			soloud_thread_data *d = new soloud_thread_data;
			d->mFunc = aThreadFunction;
			d->mParam = aParameter;

			ThreadHandleData *threadHandle = new ThreadHandleData;
			pthread_create(&threadHandle->thread, NULL, threadfunc, (void*)d);
            return threadHandle;
		}

		void sleep(int aMSec)
		{
			usleep(aMSec * 1000);
		}

        void wait(ThreadHandle aThreadHandle)
        {
            pthread_join(aThreadHandle->thread, 0);
        }

        void release(ThreadHandle aThreadHandle)
        {
            delete aThreadHandle;
        }
#endif

		static void poolWorker(void *aParam)
		{
			Pool *myPool = (Pool*)aParam;
			while (myPool->mRunning)
			{
				PoolTask *t = myPool->getWork();
				if (!t)
				{
					sleep(1);
				}
				else
				{
					t->work();
				}
			}
		}

		Pool::Pool()
		{
			mRunning = 0;
			mThreadCount = 0;
			mThread = 0;
			mWorkMutex = 0;
			mRobin = 0;
			mMaxTask = 0;
		}

		Pool::~Pool()
		{
			mRunning = 0;
			int i;
			for (i = 0; i < mThreadCount; i++)
			{
				wait(mThread[i]);
				release(mThread[i]);
			}
			delete[] mThread;
			if (mWorkMutex)
				destroyMutex(mWorkMutex);
		}

		void Pool::init(int aThreadCount)
		{
			if (aThreadCount > 0)
			{
				mMaxTask = 0;
				mWorkMutex = createMutex();
				mRunning = 1;
				mThreadCount = aThreadCount;
				mThread = new ThreadHandle[aThreadCount];
				int i;
				for (i = 0; i < mThreadCount; i++)
				{
					mThread[i] = createThread(poolWorker, this);
				}
			}
		}

		void Pool::addWork(PoolTask *aTask)
		{
			if (mThreadCount == 0)
			{
				aTask->work();
			}
			else
			{
				if (mWorkMutex) lockMutex(mWorkMutex);
				if (mMaxTask == MAX_THREADPOOL_TASKS)
				{
					// If we're at max tasks, do the task on calling thread 
					// (we're in trouble anyway, might as well slow down adding more work)
					if (mWorkMutex) unlockMutex(mWorkMutex);
					aTask->work();
				}
				else
				{
					mTaskArray[mMaxTask] = aTask;
					mMaxTask++;
					if (mWorkMutex) unlockMutex(mWorkMutex);
				}
			}
		}

		PoolTask * Pool::getWork()
		{
			PoolTask *t = 0;
			if (mWorkMutex) lockMutex(mWorkMutex);
			if (mMaxTask > 0)
			{
				int r = mRobin % mMaxTask;
				mRobin++;
				t = mTaskArray[r];
				mTaskArray[r] = mTaskArray[mMaxTask - 1];
				mMaxTask--;
			}
			if (mWorkMutex) unlockMutex(mWorkMutex);
			return t;
		}
	}
}
//----soloud_fft.h------------------------------------------------------------------------------
namespace SoLoud
{
	namespace FFT
	{
		// Perform 1024 unit FFT. Buffer must have 1024 floats, and will be overwritten
		void fft1024(float *aBuffer);

		// Perform 256 unit FFT. Buffer must have 256 floats, and will be overwritten
		void fft256(float *aBuffer);
		
		// Perform 256 unit IFFT. Buffer must have 256 floats, and will be overwritten
		void ifft256(float *aBuffer);
	};
};
//----soloud_fft.cpp------------------------------------------------------------------------------
#include <string.h>

extern int Soloud_fft_bitrev_10[1024];
extern int Soloud_fft_bitrev_8[256];
extern float Soloud_fft_trig_10[508];
extern float Soloud_fft_trig_8[124];

static void do_fft_8(float *f, const float *x)
{
    float tempbuf[256];
    float *sf = tempbuf;
    float *df = f;

    /* Do the transformation in several pass */
    {
        int pass;
        int nbr_coef;
        int h_nbr_coef;
        int d_nbr_coef;
        int coef_index;

        /* First and second pass at once */
        {
            int *bit_rev_lut_ptr = Soloud_fft_bitrev_8;
            coef_index = 0;
            do
            {
                int rev_index_0 = bit_rev_lut_ptr[coef_index];
                int rev_index_1 = bit_rev_lut_ptr[coef_index + 1];
                int rev_index_2 = bit_rev_lut_ptr[coef_index + 2];
                int rev_index_3 = bit_rev_lut_ptr[coef_index + 3];

                float *df2 = df + coef_index;
                df2[1] = x[rev_index_0] - x[rev_index_1];
                df2[3] = x[rev_index_2] - x[rev_index_3];

                float sf_0 = x[rev_index_0] + x[rev_index_1];
                float sf_2 = x[rev_index_2] + x[rev_index_3];

                df2[0] = sf_0 + sf_2;
                df2[2] = sf_0 - sf_2;

                coef_index += 4;
            } 
            while (coef_index < 256);
        }

        /* Third pass */
        {
            coef_index = 0;
            float sqrt2_2 = (float)sqrt(2.0f) * 0.5f;
            do
            {
                float v;

                sf[coef_index] = df[coef_index] + df[coef_index + 4];
                sf[coef_index + 4] = df[coef_index] - df[coef_index + 4];
                sf[coef_index + 2] = df[coef_index + 2];
                sf[coef_index + 6] = df[coef_index + 6];

                v = (df[coef_index + 5] - df[coef_index + 7]) * sqrt2_2;
                sf[coef_index + 1] = df[coef_index + 1] + v;
                sf[coef_index + 3] = df[coef_index + 1] - v;

                v = (df[coef_index + 5] + df[coef_index + 7]) * sqrt2_2;
                sf[coef_index + 5] = v + df[coef_index + 3];
                sf[coef_index + 7] = v - df[coef_index + 3];

                coef_index += 8;
            } 
            while (coef_index < 256);
        }

        /* Next pass */
        for (pass = 3; pass < 8; ++pass)
        {
            coef_index = 0;
            nbr_coef = 1 << pass;
            h_nbr_coef = nbr_coef >> 1;
            d_nbr_coef = nbr_coef << 1;
			float *cos_ptr = Soloud_fft_trig_8 + (int)(1 << (pass - 1)) - 4;

            do
            {
                int i;
                float *sf1r = sf + coef_index;
                float *sf2r = sf1r + nbr_coef;
                float *dfr = df + coef_index;
                float *dfi = dfr + nbr_coef;

                /* Extreme coefficients are always real */
                dfr[0] = sf1r[0] + sf2r[0];
                dfi[0] = sf1r[0] - sf2r[0];    // dfr [nbr_coef] =
                dfr[h_nbr_coef] = sf1r[h_nbr_coef];
                dfi[h_nbr_coef] = sf2r[h_nbr_coef];

                /* Others are conjugate complex numbers */
               float *sf1i = sf1r + h_nbr_coef;
               float *sf2i = sf1i + nbr_coef;
                for (i = 1; i < h_nbr_coef; ++i)
                {
                    float c = cos_ptr[i];                    // cos (i*PI/nbr_coef);
                    float s = cos_ptr[h_nbr_coef - i];    // sin (i*PI/nbr_coef);
                    float v;

                    v = sf2r[i] * c - sf2i[i] * s;
                    dfr[i] = sf1r[i] + v;
                    dfi[-i] = sf1r[i] - v;    // dfr [nbr_coef - i] =

                    v = sf2r[i] * s + sf2i[i] * c;
                    dfi[i] = v + sf1i[i];
                    dfi[nbr_coef - i] = v - sf1i[i];
                }

                coef_index += d_nbr_coef;
            } 
            while (coef_index < 256);

            /* Prepare to the next pass */
            {
                float *temp_ptr = df;
                df = sf;
                sf = temp_ptr;
            }
        }
    }
}


static void do_fft_10(float *f, const float *x)
{
    float tempbuf[1024];
    float *sf = tempbuf;
    float *df = f;

    /* Do the transformation in several pass */
    {
        int pass;
        int nbr_coef;
        int h_nbr_coef;
        int d_nbr_coef;
        int coef_index;

        /* First and second pass at once */
        {
            int *bit_rev_lut_ptr = Soloud_fft_bitrev_10;
            coef_index = 0;

            do
            {
                int rev_index_0 = bit_rev_lut_ptr[coef_index];
                int rev_index_1 = bit_rev_lut_ptr[coef_index + 1];
                int rev_index_2 = bit_rev_lut_ptr[coef_index + 2];
                int rev_index_3 = bit_rev_lut_ptr[coef_index + 3];

                float *df2 = df + coef_index;
                df2[1] = x[rev_index_0] - x[rev_index_1];
                df2[3] = x[rev_index_2] - x[rev_index_3];

                float sf_0 = x[rev_index_0] + x[rev_index_1];
                float sf_2 = x[rev_index_2] + x[rev_index_3];

                df2[0] = sf_0 + sf_2;
                df2[2] = sf_0 - sf_2;

                coef_index += 4;
            } 
			while (coef_index < 1024);
        }

        /* Third pass */
        {
            coef_index = 0;
            float sqrt2_2 = (float)sqrt(2.0f) * 0.5f;
            do
            {
                float v;

                sf[coef_index] = df[coef_index] + df[coef_index + 4];
                sf[coef_index + 4] = df[coef_index] - df[coef_index + 4];
                sf[coef_index + 2] = df[coef_index + 2];
                sf[coef_index + 6] = df[coef_index + 6];

                v = (df[coef_index + 5] - df[coef_index + 7]) * sqrt2_2;
                sf[coef_index + 1] = df[coef_index + 1] + v;
                sf[coef_index + 3] = df[coef_index + 1] - v;

                v = (df[coef_index + 5] + df[coef_index + 7]) * sqrt2_2;
                sf[coef_index + 5] = v + df[coef_index + 3];
                sf[coef_index + 7] = v - df[coef_index + 3];

                coef_index += 8;
            } 
			while (coef_index < 1024);
        }

        /* Next pass */
        for (pass = 3; pass < 10; ++pass)
        {
            coef_index = 0;
            nbr_coef = 1 << pass;
            h_nbr_coef = nbr_coef >> 1;
            d_nbr_coef = nbr_coef << 1;
			float *cos_ptr = Soloud_fft_trig_10 + (int)(1 << (pass - 1)) - 4;
            do
            {
                int i;
                float *sf1r = sf + coef_index;
                float *sf2r = sf1r + nbr_coef;
                float *dfr = df + coef_index;
                float *dfi = dfr + nbr_coef;

                /* Extreme coefficients are always real */
                dfr[0] = sf1r[0] + sf2r[0];
                dfi[0] = sf1r[0] - sf2r[0];    // dfr [nbr_coef] =
                dfr[h_nbr_coef] = sf1r[h_nbr_coef];
                dfi[h_nbr_coef] = sf2r[h_nbr_coef];

                /* Others are conjugate complex numbers */
                float *sf1i = sf1r + h_nbr_coef;
                float *sf2i = sf1i + nbr_coef;
                for (i = 1; i < h_nbr_coef; ++i)
                {
                    float c = cos_ptr[i];                    // cos (i*PI/nbr_coef);
                    float s = cos_ptr[h_nbr_coef - i];    // sin (i*PI/nbr_coef);
                    float v;

                    v = sf2r[i] * c - sf2i[i] * s;
                    dfr[i] = sf1r[i] + v;
                    dfi[-i] = sf1r[i] - v;    // dfr [nbr_coef - i] =

                    v = sf2r[i] * s + sf2i[i] * c;
                    dfi[i] = v + sf1i[i];
                    dfi[nbr_coef - i] = v - sf1i[i];
                }

                coef_index += d_nbr_coef;
            } 
            while (coef_index < 1024);

            /* Prepare to the next pass */
            {
                float *temp_ptr = df;
                df = sf;
                sf = temp_ptr;
            }
        }
    }
}

static void do_ifft_8(float *f, float *x)
{
    float tempbuf[256];
    float *sf = f;
    float *df;
    float *df_temp;

    df = x;
    df_temp = tempbuf;

    /* Do the transformation in several pass */
    {
        int pass;
        int nbr_coef;
        int h_nbr_coef;
        int d_nbr_coef;
        int coef_index;

        /* First pass */
        for (pass = 8 - 1; pass >= 3; --pass)
        {
            coef_index = 0;
            nbr_coef = 1 << pass;
            h_nbr_coef = nbr_coef >> 1;
            d_nbr_coef = nbr_coef << 1;
            float *cos_ptr = Soloud_fft_trig_8 + (int)(1 << (pass - 1)) - 4;
            do
            {
                int i;
                float *sfr = sf + coef_index;
                float *sfi = sfr + nbr_coef;
                float *df1r = df + coef_index;
                float *df2r = df1r + nbr_coef;

                /* Extreme coefficients are always real */
                df1r[0] = sfr[0] + sfi[0];        // + sfr [nbr_coef]
                df2r[0] = sfr[0] - sfi[0];        // - sfr [nbr_coef]
                df1r[h_nbr_coef] = sfr[h_nbr_coef] * 2;
                df2r[h_nbr_coef] = sfi[h_nbr_coef] * 2;

                /* Others are conjugate complex numbers */
                float *df1i = df1r + h_nbr_coef;
                float *df2i = df1i + nbr_coef;
                for (i = 1; i < h_nbr_coef; ++i)
                {
                    df1r[i] = sfr[i] + sfi[-i];        // + sfr [nbr_coef - i]
                    df1i[i] = sfi[i] - sfi[nbr_coef - i];

                    float c = cos_ptr[i];                    // cos (i*PI/nbr_coef);
                    float s = cos_ptr[h_nbr_coef - i];    // sin (i*PI/nbr_coef);
                    float vr = sfr[i] - sfi[-i];        // - sfr [nbr_coef - i]
                    float vi = sfi[i] + sfi[nbr_coef - i];

                    df2r[i] = vr * c + vi * s;
                    df2i[i] = vi * c - vr * s;
                }

                coef_index += d_nbr_coef;
            } 
			while (coef_index < 256);

            /* Prepare to the next pass */
            if (pass < 8 - 1)
            {
                float *temp_ptr = df;
                df = sf;
                sf = temp_ptr;
            }
            else
            {
                sf = df;
                df = df_temp;
            }
        }

        /* Antepenultimate pass */
        {
            float sqrt2_2 = (float)sqrt(2.0f) * 0.5f;
            coef_index = 0;
            do
            {
                df[coef_index] = sf[coef_index] + sf[coef_index + 4];
                df[coef_index + 4] = sf[coef_index] - sf[coef_index + 4];
                df[coef_index + 2] = sf[coef_index + 2] * 2;
                df[coef_index + 6] = sf[coef_index + 6] * 2;

                df[coef_index + 1] = sf[coef_index + 1] + sf[coef_index + 3];
                df[coef_index + 3] = sf[coef_index + 5] - sf[coef_index + 7];

                float vr = sf[coef_index + 1] - sf[coef_index + 3];
                float vi = sf[coef_index + 5] + sf[coef_index + 7];

                df[coef_index + 5] = (vr + vi) * sqrt2_2;
                df[coef_index + 7] = (vi - vr) * sqrt2_2;

                coef_index += 8;
            } 
			while (coef_index < 256);
        }

        /* Penultimate and last pass at once */
        {
            coef_index = 0;
            int *bit_rev_lut_ptr = Soloud_fft_bitrev_8;
            float *sf2 = df;
            do
            {
                {
                    float b_0 = sf2[0] + sf2[2];
                    float b_2 = sf2[0] - sf2[2];
                    float b_1 = sf2[1] * 2;
                    float b_3 = sf2[3] * 2;

                    x[bit_rev_lut_ptr[0]] = b_0 + b_1;
                    x[bit_rev_lut_ptr[1]] = b_0 - b_1;
                    x[bit_rev_lut_ptr[2]] = b_2 + b_3;
                    x[bit_rev_lut_ptr[3]] = b_2 - b_3;
                }
                {
                    float b_0 = sf2[4] + sf2[6];
                    float b_2 = sf2[4] - sf2[6];
                    float b_1 = sf2[5] * 2;
                    float b_3 = sf2[7] * 2;

                    x[bit_rev_lut_ptr[4]] = b_0 + b_1;
                    x[bit_rev_lut_ptr[5]] = b_0 - b_1;
                    x[bit_rev_lut_ptr[6]] = b_2 + b_3;
                    x[bit_rev_lut_ptr[7]] = b_2 - b_3;
                }

                sf2 += 8;
                coef_index += 8;
                bit_rev_lut_ptr += 8;
            } 
			while (coef_index < 256);
        }
    }

    // rescale

    float mul = (1.0f / 256);
    int i = 256 - 1;

    do
    {
        x[i] *= mul;
        --i;
    } 
    while (i >= 0);
}


namespace SoLoud
{
    namespace FFT
    {
        void fft1024(float *aBuffer)
        {
            float temp[1024];
            memcpy(temp, aBuffer, sizeof(float) * 1024);
            do_fft_10(aBuffer, temp);
        }    

        void fft256(float *aBuffer)
        {
            float temp[256];
            memcpy(temp, aBuffer, sizeof(float) * 256);
            do_fft_8(aBuffer, temp);
        }
        
        void ifft256(float *aBuffer)
        {
            float temp[256];
            memcpy(temp, aBuffer, sizeof(float) * 256);
			do_ifft_8(temp, aBuffer);
        }
    };
};
//----soloud_fft_lut.cpp------------------------------------------------------------------------
/* SoLoud Lookup Table Generator (c)2015 Jari Komppa http://iki.fi/sol/ */

int Soloud_fft_bitrev_10[1024] = {
0, 512, 256, 768, 128, 640, 384, 896, 64, 576, 320, 832, 192, 704, 448, 
960, 32, 544, 288, 800, 160, 672, 416, 928, 96, 608, 352, 864, 224, 736, 
480, 992, 16, 528, 272, 784, 144, 656, 400, 912, 80, 592, 336, 848, 208, 
720, 464, 976, 48, 560, 304, 816, 176, 688, 432, 944, 112, 624, 368, 880, 
240, 752, 496, 1008, 8, 520, 264, 776, 136, 648, 392, 904, 72, 584, 328, 
840, 200, 712, 456, 968, 40, 552, 296, 808, 168, 680, 424, 936, 104, 616, 
360, 872, 232, 744, 488, 1000, 24, 536, 280, 792, 152, 664, 408, 920, 88, 
600, 344, 856, 216, 728, 472, 984, 56, 568, 312, 824, 184, 696, 440, 952, 
120, 632, 376, 888, 248, 760, 504, 1016, 4, 516, 260, 772, 132, 644, 388, 
900, 68, 580, 324, 836, 196, 708, 452, 964, 36, 548, 292, 804, 164, 676, 
420, 932, 100, 612, 356, 868, 228, 740, 484, 996, 20, 532, 276, 788, 148, 
660, 404, 916, 84, 596, 340, 852, 212, 724, 468, 980, 52, 564, 308, 820, 
180, 692, 436, 948, 116, 628, 372, 884, 244, 756, 500, 1012, 12, 524, 268, 
780, 140, 652, 396, 908, 76, 588, 332, 844, 204, 716, 460, 972, 44, 556, 
300, 812, 172, 684, 428, 940, 108, 620, 364, 876, 236, 748, 492, 1004, 
28, 540, 284, 796, 156, 668, 412, 924, 92, 604, 348, 860, 220, 732, 476, 
988, 60, 572, 316, 828, 188, 700, 444, 956, 124, 636, 380, 892, 252, 764, 
508, 1020, 2, 514, 258, 770, 130, 642, 386, 898, 66, 578, 322, 834, 194, 
706, 450, 962, 34, 546, 290, 802, 162, 674, 418, 930, 98, 610, 354, 866, 
226, 738, 482, 994, 18, 530, 274, 786, 146, 658, 402, 914, 82, 594, 338, 
850, 210, 722, 466, 978, 50, 562, 306, 818, 178, 690, 434, 946, 114, 626, 
370, 882, 242, 754, 498, 1010, 10, 522, 266, 778, 138, 650, 394, 906, 74, 
586, 330, 842, 202, 714, 458, 970, 42, 554, 298, 810, 170, 682, 426, 938, 
106, 618, 362, 874, 234, 746, 490, 1002, 26, 538, 282, 794, 154, 666, 410, 
922, 90, 602, 346, 858, 218, 730, 474, 986, 58, 570, 314, 826, 186, 698, 
442, 954, 122, 634, 378, 890, 250, 762, 506, 1018, 6, 518, 262, 774, 134, 
646, 390, 902, 70, 582, 326, 838, 198, 710, 454, 966, 38, 550, 294, 806, 
166, 678, 422, 934, 102, 614, 358, 870, 230, 742, 486, 998, 22, 534, 278, 
790, 150, 662, 406, 918, 86, 598, 342, 854, 214, 726, 470, 982, 54, 566, 
310, 822, 182, 694, 438, 950, 118, 630, 374, 886, 246, 758, 502, 1014, 
14, 526, 270, 782, 142, 654, 398, 910, 78, 590, 334, 846, 206, 718, 462, 
974, 46, 558, 302, 814, 174, 686, 430, 942, 110, 622, 366, 878, 238, 750, 
494, 1006, 30, 542, 286, 798, 158, 670, 414, 926, 94, 606, 350, 862, 222, 
734, 478, 990, 62, 574, 318, 830, 190, 702, 446, 958, 126, 638, 382, 894, 
254, 766, 510, 1022, 1, 513, 257, 769, 129, 641, 385, 897, 65, 577, 321, 
833, 193, 705, 449, 961, 33, 545, 289, 801, 161, 673, 417, 929, 97, 609, 
353, 865, 225, 737, 481, 993, 17, 529, 273, 785, 145, 657, 401, 913, 81, 
593, 337, 849, 209, 721, 465, 977, 49, 561, 305, 817, 177, 689, 433, 945, 
113, 625, 369, 881, 241, 753, 497, 1009, 9, 521, 265, 777, 137, 649, 393, 
905, 73, 585, 329, 841, 201, 713, 457, 969, 41, 553, 297, 809, 169, 681, 
425, 937, 105, 617, 361, 873, 233, 745, 489, 1001, 25, 537, 281, 793, 153, 
665, 409, 921, 89, 601, 345, 857, 217, 729, 473, 985, 57, 569, 313, 825, 
185, 697, 441, 953, 121, 633, 377, 889, 249, 761, 505, 1017, 5, 517, 261, 
773, 133, 645, 389, 901, 69, 581, 325, 837, 197, 709, 453, 965, 37, 549, 
293, 805, 165, 677, 421, 933, 101, 613, 357, 869, 229, 741, 485, 997, 21, 
533, 277, 789, 149, 661, 405, 917, 85, 597, 341, 853, 213, 725, 469, 981, 
53, 565, 309, 821, 181, 693, 437, 949, 117, 629, 373, 885, 245, 757, 501, 
1013, 13, 525, 269, 781, 141, 653, 397, 909, 77, 589, 333, 845, 205, 717, 
461, 973, 45, 557, 301, 813, 173, 685, 429, 941, 109, 621, 365, 877, 237, 
749, 493, 1005, 29, 541, 285, 797, 157, 669, 413, 925, 93, 605, 349, 861, 
221, 733, 477, 989, 61, 573, 317, 829, 189, 701, 445, 957, 125, 637, 381, 
893, 253, 765, 509, 1021, 3, 515, 259, 771, 131, 643, 387, 899, 67, 579, 
323, 835, 195, 707, 451, 963, 35, 547, 291, 803, 163, 675, 419, 931, 99, 
611, 355, 867, 227, 739, 483, 995, 19, 531, 275, 787, 147, 659, 403, 915, 
83, 595, 339, 851, 211, 723, 467, 979, 51, 563, 307, 819, 179, 691, 435, 
947, 115, 627, 371, 883, 243, 755, 499, 1011, 11, 523, 267, 779, 139, 651, 
395, 907, 75, 587, 331, 843, 203, 715, 459, 971, 43, 555, 299, 811, 171, 
683, 427, 939, 107, 619, 363, 875, 235, 747, 491, 1003, 27, 539, 283, 795, 
155, 667, 411, 923, 91, 603, 347, 859, 219, 731, 475, 987, 59, 571, 315, 
827, 187, 699, 443, 955, 123, 635, 379, 891, 251, 763, 507, 1019, 7, 519, 
263, 775, 135, 647, 391, 903, 71, 583, 327, 839, 199, 711, 455, 967, 39, 
551, 295, 807, 167, 679, 423, 935, 103, 615, 359, 871, 231, 743, 487, 999, 
23, 535, 279, 791, 151, 663, 407, 919, 87, 599, 343, 855, 215, 727, 471, 
983, 55, 567, 311, 823, 183, 695, 439, 951, 119, 631, 375, 887, 247, 759, 
503, 1015, 15, 527, 271, 783, 143, 655, 399, 911, 79, 591, 335, 847, 207, 
719, 463, 975, 47, 559, 303, 815, 175, 687, 431, 943, 111, 623, 367, 879, 
239, 751, 495, 1007, 31, 543, 287, 799, 159, 671, 415, 927, 95, 607, 351, 
863, 223, 735, 479, 991, 63, 575, 319, 831, 191, 703, 447, 959, 127, 639, 
383, 895, 255, 767, 511, 1023};

int Soloud_fft_bitrev_8[256] = {
0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 112, 240, 
8, 136, 72, 200, 40, 168, 104, 232, 24, 152, 88, 216, 56, 184, 120, 248, 
4, 132, 68, 196, 36, 164, 100, 228, 20, 148, 84, 212, 52, 180, 116, 244, 
12, 140, 76, 204, 44, 172, 108, 236, 28, 156, 92, 220, 60, 188, 124, 252, 
2, 130, 66, 194, 34, 162, 98, 226, 18, 146, 82, 210, 50, 178, 114, 242, 
10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 58, 186, 122, 250, 
6, 134, 70, 198, 38, 166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246, 
14, 142, 78, 206, 46, 174, 110, 238, 30, 158, 94, 222, 62, 190, 126, 254, 
1, 129, 65, 193, 33, 161, 97, 225, 17, 145, 81, 209, 49, 177, 113, 241, 
9, 137, 73, 201, 41, 169, 105, 233, 25, 153, 89, 217, 57, 185, 121, 249, 
5, 133, 69, 197, 37, 165, 101, 229, 21, 149, 85, 213, 53, 181, 117, 245, 
13, 141, 77, 205, 45, 173, 109, 237, 29, 157, 93, 221, 61, 189, 125, 253, 
3, 131, 67, 195, 35, 163, 99, 227, 19, 147, 83, 211, 51, 179, 115, 243, 
11, 139, 75, 203, 43, 171, 107, 235, 27, 155, 91, 219, 59, 187, 123, 251, 
7, 135, 71, 199, 39, 167, 103, 231, 23, 151, 87, 215, 55, 183, 119, 247, 
15, 143, 79, 207, 47, 175, 111, 239, 31, 159, 95, 223, 63, 191, 127, 255
};

float Soloud_fft_trig_10[508] = {
1.000000000000000000f, 0.923879504203796390f, 0.707106769084930420f, 
0.382683396339416500f, 1.000000000000000000f, 0.980785250663757320f, 
0.923879504203796390f, 0.831469595432281490f, 0.707106769084930420f, 
0.555570185184478760f, 0.382683396339416500f, 0.195090278983116150f, 
1.000000000000000000f, 0.995184719562530520f, 0.980785250663757320f, 
0.956940352916717530f, 0.923879504203796390f, 0.881921231746673580f, 
0.831469595432281490f, 0.773010432720184330f, 0.707106769084930420f, 
0.634393274784088130f, 0.555570185184478760f, 0.471396714448928830f, 
0.382683396339416500f, 0.290284633636474610f, 0.195090278983116150f, 
0.098017096519470215f, 1.000000000000000000f, 0.998795449733734130f, 
0.995184719562530520f, 0.989176511764526370f, 0.980785250663757320f, 
0.970031261444091800f, 0.956940352916717530f, 0.941544055938720700f, 
0.923879504203796390f, 0.903989315032958980f, 0.881921231746673580f, 
0.857728600502014160f, 0.831469595432281490f, 0.803207516670227050f, 
0.773010432720184330f, 0.740951120853424070f, 0.707106769084930420f, 
0.671558916568756100f, 0.634393274784088130f, 0.595699310302734380f, 
0.555570185184478760f, 0.514102697372436520f, 0.471396714448928830f, 
0.427555054426193240f, 0.382683396339416500f, 0.336889833211898800f, 
0.290284633636474610f, 0.242980137467384340f, 0.195090278983116150f, 
0.146730437874794010f, 0.098017096519470215f, 0.049067631363868713f, 
1.000000000000000000f, 0.999698817729949950f, 0.998795449733734130f, 
0.997290432453155520f, 0.995184719562530520f, 0.992479562759399410f, 
0.989176511764526370f, 0.985277652740478520f, 0.980785250663757320f, 
0.975702106952667240f, 0.970031261444091800f, 0.963776051998138430f, 
0.956940352916717530f, 0.949528157711029050f, 0.941544055938720700f, 
0.932992815971374510f, 0.923879504203796390f, 0.914209723472595210f, 
0.903989315032958980f, 0.893224298954010010f, 0.881921231746673580f, 
0.870086967945098880f, 0.857728600502014160f, 0.844853579998016360f, 
0.831469595432281490f, 0.817584812641143800f, 0.803207516670227050f, 
0.788346409797668460f, 0.773010432720184330f, 0.757208824157714840f, 
0.740951120853424070f, 0.724247097969055180f, 0.707106769084930420f, 
0.689540505409240720f, 0.671558916568756100f, 0.653172850608825680f, 
0.634393274784088130f, 0.615231573581695560f, 0.595699310302734380f, 
0.575808167457580570f, 0.555570185184478760f, 0.534997582435607910f, 
0.514102697372436520f, 0.492898166179656980f, 0.471396714448928830f, 
0.449611306190490720f, 0.427555054426193240f, 0.405241280794143680f, 
0.382683396339416500f, 0.359894990921020510f, 0.336889833211898800f, 
0.313681721687316890f, 0.290284633636474610f, 0.266712725162506100f, 
0.242980137467384340f, 0.219101205468177800f, 0.195090278983116150f, 
0.170961856842041020f, 0.146730437874794010f, 0.122410632669925690f, 
0.098017096519470215f, 0.073564521968364716f, 0.049067631363868713f, 
0.024541186168789864f, 1.000000000000000000f, 0.999924719333648680f, 
0.999698817729949950f, 0.999322354793548580f, 0.998795449733734130f, 
0.998118102550506590f, 0.997290432453155520f, 0.996312618255615230f, 
0.995184719562530520f, 0.993906974792480470f, 0.992479562759399410f, 
0.990902662277221680f, 0.989176511764526370f, 0.987301409244537350f, 
0.985277652740478520f, 0.983105480670928960f, 0.980785250663757320f, 
0.978317379951477050f, 0.975702106952667240f, 0.972939968109130860f, 
0.970031261444091800f, 0.966976463794708250f, 0.963776051998138430f, 
0.960430502891540530f, 0.956940352916717530f, 0.953306019306182860f, 
0.949528157711029050f, 0.945607304573059080f, 0.941544055938720700f, 
0.937339007854461670f, 0.932992815971374510f, 0.928506076335906980f, 
0.923879504203796390f, 0.919113874435424800f, 0.914209723472595210f, 
0.909168004989624020f, 0.903989315032958980f, 0.898674488067626950f, 
0.893224298954010010f, 0.887639641761779790f, 0.881921231746673580f, 
0.876070082187652590f, 0.870086967945098880f, 0.863972842693328860f, 
0.857728600502014160f, 0.851355195045471190f, 0.844853579998016360f, 
0.838224709033966060f, 0.831469595432281490f, 0.824589312076568600f, 
0.817584812641143800f, 0.810457170009613040f, 0.803207516670227050f, 
0.795836865901947020f, 0.788346409797668460f, 0.780737221240997310f, 
0.773010432720184330f, 0.765167236328125000f, 0.757208824157714840f, 
0.749136388301849370f, 0.740951120853424070f, 0.732654273509979250f, 
0.724247097969055180f, 0.715730786323547360f, 0.707106769084930420f, 
0.698376238346099850f, 0.689540505409240720f, 0.680601000785827640f, 
0.671558916568756100f, 0.662415742874145510f, 0.653172850608825680f, 
0.643831551074981690f, 0.634393274784088130f, 0.624859452247619630f, 
0.615231573581695560f, 0.605511009693145750f, 0.595699310302734380f, 
0.585797846317291260f, 0.575808167457580570f, 0.565731763839721680f, 
0.555570185184478760f, 0.545324981212615970f, 0.534997582435607910f, 
0.524589657783508300f, 0.514102697372436520f, 0.503538370132446290f, 
0.492898166179656980f, 0.482183754444122310f, 0.471396714448928830f, 
0.460538685321807860f, 0.449611306190490720f, 0.438616216182708740f, 
0.427555054426193240f, 0.416429519653320310f, 0.405241280794143680f, 
0.393992006778717040f, 0.382683396339416500f, 0.371317178010940550f, 
0.359894990921020510f, 0.348418653011322020f, 0.336889833211898800f, 
0.325310260057449340f, 0.313681721687316890f, 0.302005916833877560f, 
0.290284633636474610f, 0.278519660234451290f, 0.266712725162506100f, 
0.254865616559982300f, 0.242980137467384340f, 0.231058076024055480f, 
0.219101205468177800f, 0.207111343741416930f, 0.195090278983116150f, 
0.183039844036102290f, 0.170961856842041020f, 0.158858105540275570f, 
0.146730437874794010f, 0.134580671787261960f, 0.122410632669925690f, 
0.110222168266773220f, 0.098017096519470215f, 0.085797272622585297f, 
0.073564521968364716f, 0.061320696026086807f, 0.049067631363868713f, 
0.036807179450988770f, 0.024541186168789864f, 0.012271494604647160f, 
1.000000000000000000f, 0.999981164932250980f, 0.999924719333648680f, 
0.999830603599548340f, 0.999698817729949950f, 0.999529421329498290f, 
0.999322354793548580f, 0.999077737331390380f, 0.998795449733734130f, 
0.998475551605224610f, 0.998118102550506590f, 0.997723042964935300f, 
0.997290432453155520f, 0.996820271015167240f, 0.996312618255615230f, 
0.995767414569854740f, 0.995184719562530520f, 0.994564592838287350f, 
0.993906974792480470f, 0.993211925029754640f, 0.992479562759399410f, 
0.991709768772125240f, 0.990902662277221680f, 0.990058183670043950f, 
0.989176511764526370f, 0.988257586956024170f, 0.987301409244537350f, 
0.986308097839355470f, 0.985277652740478520f, 0.984210073947906490f, 
0.983105480670928960f, 0.981963872909545900f, 0.980785250663757320f, 
0.979569792747497560f, 0.978317379951477050f, 0.977028131484985350f, 
0.975702106952667240f, 0.974339365959167480f, 0.972939968109130860f, 
0.971503913402557370f, 0.970031261444091800f, 0.968522071838378910f, 
0.966976463794708250f, 0.965394437313079830f, 0.963776051998138430f, 
0.962121427059173580f, 0.960430502891540530f, 0.958703458309173580f, 
0.956940352916717530f, 0.955141186714172360f, 0.953306019306182860f, 
0.951435029506683350f, 0.949528157711029050f, 0.947585582733154300f, 
0.945607304573059080f, 0.943593442440032960f, 0.941544055938720700f, 
0.939459204673767090f, 0.937339007854461670f, 0.935183525085449220f, 
0.932992815971374510f, 0.930766940116882320f, 0.928506076335906980f, 
0.926210224628448490f, 0.923879504203796390f, 0.921514034271240230f, 
0.919113874435424800f, 0.916679084300994870f, 0.914209723472595210f, 
0.911706030368804930f, 0.909168004989624020f, 0.906595706939697270f, 
0.903989315032958980f, 0.901348829269409180f, 0.898674488067626950f, 
0.895966231822967530f, 0.893224298954010010f, 0.890448689460754390f, 
0.887639641761779790f, 0.884797096252441410f, 0.881921231746673580f, 
0.879012227058410640f, 0.876070082187652590f, 0.873094975948333740f, 
0.870086967945098880f, 0.867046236991882320f, 0.863972842693328860f, 
0.860866904258728030f, 0.857728600502014160f, 0.854557991027832030f, 
0.851355195045471190f, 0.848120331764221190f, 0.844853579998016360f, 
0.841554939746856690f, 0.838224709033966060f, 0.834862887859344480f, 
0.831469595432281490f, 0.828045010566711430f, 0.824589312076568600f, 
0.821102499961853030f, 0.817584812641143800f, 0.814036309719085690f, 
0.810457170009613040f, 0.806847572326660160f, 0.803207516670227050f, 
0.799537241458892820f, 0.795836865901947020f, 0.792106568813323970f, 
0.788346409797668460f, 0.784556567668914790f, 0.780737221240997310f, 
0.776888430118560790f, 0.773010432720184330f, 0.769103348255157470f, 
0.765167236328125000f, 0.761202394962310790f, 0.757208824157714840f, 
0.753186762332916260f, 0.749136388301849370f, 0.745057761669158940f, 
0.740951120853424070f, 0.736816525459289550f, 0.732654273509979250f, 
0.728464365005493160f, 0.724247097969055180f, 0.720002472400665280f, 
0.715730786323547360f, 0.711432158946990970f, 0.707106769084930420f, 
0.702754735946655270f, 0.698376238346099850f, 0.693971455097198490f, 
0.689540505409240720f, 0.685083627700805660f, 0.680601000785827640f, 
0.676092684268951420f, 0.671558916568756100f, 0.666999876499176030f, 
0.662415742874145510f, 0.657806694507598880f, 0.653172850608825680f, 
0.648514389991760250f, 0.643831551074981690f, 0.639124453067779540f, 
0.634393274784088130f, 0.629638195037841800f, 0.624859452247619630f, 
0.620057165622711180f, 0.615231573581695560f, 0.610382795333862300f, 
0.605511009693145750f, 0.600616455078125000f, 0.595699310302734380f, 
0.590759694576263430f, 0.585797846317291260f, 0.580813944339752200f, 
0.575808167457580570f, 0.570780694484710690f, 0.565731763839721680f, 
0.560661554336547850f, 0.555570185184478760f, 0.550457954406738280f, 
0.545324981212615970f, 0.540171444416046140f, 0.534997582435607910f, 
0.529803574085235600f, 0.524589657783508300f, 0.519355952739715580f, 
0.514102697372436520f, 0.508830130100250240f, 0.503538370132446290f, 
0.498227655887603760f, 0.492898166179656980f, 0.487550139427185060f, 
0.482183754444122310f, 0.476799190044403080f, 0.471396714448928830f, 
0.465976476669311520f, 0.460538685321807860f, 0.455083549022674560f, 
0.449611306190490720f, 0.444122105836868290f, 0.438616216182708740f, 
0.433093786239624020f, 0.427555054426193240f, 0.422000229358673100f, 
0.416429519653320310f, 0.410843133926391600f, 0.405241280794143680f, 
0.399624168872833250f, 0.393992006778717040f, 0.388345003128051760f, 
0.382683396339416500f, 0.377007365226745610f, 0.371317178010940550f, 
0.365612953901290890f, 0.359894990921020510f, 0.354163497686386110f, 
0.348418653011322020f, 0.342660695314407350f, 0.336889833211898800f, 
0.331106275320053100f, 0.325310260057449340f, 0.319501996040344240f, 
0.313681721687316890f, 0.307849615812301640f, 0.302005916833877560f, 
0.296150863170623780f, 0.290284633636474610f, 0.284407496452331540f, 
0.278519660234451290f, 0.272621333599090580f, 0.266712725162506100f, 
0.260794073343276980f, 0.254865616559982300f, 0.248927563428878780f, 
0.242980137467384340f, 0.237023577094078060f, 0.231058076024055480f, 
0.225083872675895690f, 0.219101205468177800f, 0.213110283017158510f, 
0.207111343741416930f, 0.201104596257209780f, 0.195090278983116150f, 
0.189068630337715150f, 0.183039844036102290f, 0.177004188299179080f, 
0.170961856842041020f, 0.164913088083267210f, 0.158858105540275570f, 
0.152797147631645200f, 0.146730437874794010f, 0.140658199787139890f, 
0.134580671787261960f, 0.128498077392578130f, 0.122410632669925690f, 
0.116318590939044950f, 0.110222168266773220f, 0.104121595621109010f, 
0.098017096519470215f, 0.091908916831016541f, 0.085797272622585297f, 
0.079682394862174988f, 0.073564521968364716f, 0.067443877458572388f, 
0.061320696026086807f, 0.055195201188325882f, 0.049067631363868713f, 
0.042938213795423508f, 0.036807179450988770f, 0.030674761161208153f, 
0.024541186168789864f, 0.018406687304377556f, 0.012271494604647160f, 
0.006135840900242329f
};

float Soloud_fft_trig_8[124] = {
1.000000000000000000f, 0.923879504203796390f, 0.707106769084930420f, 
0.382683396339416500f, 1.000000000000000000f, 0.980785250663757320f, 
0.923879504203796390f, 0.831469595432281490f, 0.707106769084930420f, 
0.555570185184478760f, 0.382683396339416500f, 0.195090278983116150f, 
1.000000000000000000f, 0.995184719562530520f, 0.980785250663757320f, 
0.956940352916717530f, 0.923879504203796390f, 0.881921231746673580f, 
0.831469595432281490f, 0.773010432720184330f, 0.707106769084930420f, 
0.634393274784088130f, 0.555570185184478760f, 0.471396714448928830f, 
0.382683396339416500f, 0.290284633636474610f, 0.195090278983116150f, 
0.098017096519470215f, 1.000000000000000000f, 0.998795449733734130f, 
0.995184719562530520f, 0.989176511764526370f, 0.980785250663757320f, 
0.970031261444091800f, 0.956940352916717530f, 0.941544055938720700f, 
0.923879504203796390f, 0.903989315032958980f, 0.881921231746673580f, 
0.857728600502014160f, 0.831469595432281490f, 0.803207516670227050f, 
0.773010432720184330f, 0.740951120853424070f, 0.707106769084930420f, 
0.671558916568756100f, 0.634393274784088130f, 0.595699310302734380f, 
0.555570185184478760f, 0.514102697372436520f, 0.471396714448928830f, 
0.427555054426193240f, 0.382683396339416500f, 0.336889833211898800f, 
0.290284633636474610f, 0.242980137467384340f, 0.195090278983116150f, 
0.146730437874794010f, 0.098017096519470215f, 0.049067631363868713f, 
1.000000000000000000f, 0.999698817729949950f, 0.998795449733734130f, 
0.997290432453155520f, 0.995184719562530520f, 0.992479562759399410f, 
0.989176511764526370f, 0.985277652740478520f, 0.980785250663757320f, 
0.975702106952667240f, 0.970031261444091800f, 0.963776051998138430f, 
0.956940352916717530f, 0.949528157711029050f, 0.941544055938720700f, 
0.932992815971374510f, 0.923879504203796390f, 0.914209723472595210f, 
0.903989315032958980f, 0.893224298954010010f, 0.881921231746673580f, 
0.870086967945098880f, 0.857728600502014160f, 0.844853579998016360f, 
0.831469595432281490f, 0.817584812641143800f, 0.803207516670227050f, 
0.788346409797668460f, 0.773010432720184330f, 0.757208824157714840f, 
0.740951120853424070f, 0.724247097969055180f, 0.707106769084930420f, 
0.689540505409240720f, 0.671558916568756100f, 0.653172850608825680f, 
0.634393274784088130f, 0.615231573581695560f, 0.595699310302734380f, 
0.575808167457580570f, 0.555570185184478760f, 0.534997582435607910f, 
0.514102697372436520f, 0.492898166179656980f, 0.471396714448928830f, 
0.449611306190490720f, 0.427555054426193240f, 0.405241280794143680f, 
0.382683396339416500f, 0.359894990921020510f, 0.336889833211898800f, 
0.313681721687316890f, 0.290284633636474610f, 0.266712725162506100f, 
0.242980137467384340f, 0.219101205468177800f, 0.195090278983116150f, 
0.170961856842041020f, 0.146730437874794010f, 0.122410632669925690f, 
0.098017096519470215f, 0.073564521968364716f, 0.049067631363868713f, 
0.024541186168789864f
};
//----soloud.cpp--------------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <math.h> // sin

#ifdef SOLOUD_SSE_INTRINSICS
#include <xmmintrin.h>
#endif

//#define FLOATING_POINT_DEBUG

#ifdef FLOATING_POINT_DEBUG
#include <float.h>
#endif

#if !defined(WITH_SDL2) && !defined(WITH_SDL) && !defined(WITH_PORTAUDIO) && \
   !defined(WITH_OPENAL) && !defined(WITH_XAUDIO2) && !defined(WITH_WINMM) && \
   !defined(WITH_WASAPI) && !defined(WITH_OSS) && !defined(WITH_SDL_STATIC) && \
   !defined(WITH_SDL2_STATIC) && !defined(WITH_ALSA) && !defined(WITH_OPENSLES) && \
   !defined(WITH_NULL) && !defined(WITH_COREAUDIO)
#error It appears you haven't enabled any of the back-ends. Please #define one or more of the WITH_ defines (or use premake) '
#endif


namespace SoLoud
{
	AlignedFloatBuffer::AlignedFloatBuffer()
	{
		mBasePtr = 0;
		mData = 0;
	}

	result AlignedFloatBuffer::init(unsigned int aFloats)
	{
		delete[] mBasePtr;
		mBasePtr = 0;
		mData = 0;
#ifdef DISABLE_SIMD
		mBasePtr = new unsigned char[aFloats * sizeof(float)];
		if (mBasePtr == NULL)
			return OUT_OF_MEMORY;
		mData = mBasePtr;
#else
		mBasePtr = new unsigned char[aFloats * sizeof(float) + 16];
		if (mBasePtr == NULL)
			return OUT_OF_MEMORY;
		mData = (float *)(((size_t)mBasePtr + 15)&~15);
#endif
		return SO_NO_ERROR;
	}

	AlignedFloatBuffer::~AlignedFloatBuffer()
	{
		delete[] mBasePtr;
	}


	Soloud::Soloud()
	{
#ifdef FLOATING_POINT_DEBUG
		unsigned int u;
		u = _controlfp(0, 0);
		u = u & ~(_EM_INVALID | /*_EM_DENORMAL |*/ _EM_ZERODIVIDE | _EM_OVERFLOW /*| _EM_UNDERFLOW  | _EM_INEXACT*/);
		_controlfp(u, _MCW_EM);
#endif
		
		mScratchSize = 0;
		mScratchNeeded = 0;
		mSamplerate = 0;
		mBufferSize = 0;
		mFlags = 0;
		mGlobalVolume = 0;
		mPlayIndex = 0;
		mBackendData = NULL;
		mAudioThreadMutex = NULL;
		mPostClipScaler = 0;
		mBackendCleanupFunc = NULL;
		mChannels = 2;		
		mStreamTime = 0;
		mLastClockedTime = 0;
		mAudioSourceID = 1;
		mBackendString = 0;
		mBackendID = 0;
		int i;
		for (i = 0; i < FILTERS_PER_STREAM; i++)
		{
			mFilter[i] = NULL;
			mFilterInstance[i] = NULL;
		}
		for (i = 0; i < 256; i++)
		{
			mFFTData[i] = 0;
			mVisualizationWaveData[i] = 0;
			mWaveData[i] = 0;
		}
		for (i = 0; i < VOICE_COUNT; i++)
		{
			mVoice[i] = 0;
		}
		mVoiceGroup = 0;
		mVoiceGroupCount = 0;

		m3dPosition[0] = 0;
		m3dPosition[1] = 0;
		m3dPosition[2] = 0;
		m3dAt[0] = 0;
		m3dAt[1] = 0;
		m3dAt[2] = -1;
		m3dUp[0] = 0;
		m3dUp[1] = 1;
		m3dUp[2] = 0;		
		m3dVelocity[0] = 0;
		m3dVelocity[1] = 0;
		m3dVelocity[2] = 0;		
		m3dSoundSpeed = 343.3f;
		mMaxActiveVoices = 16;
		mHighestVoice = 0;
		mActiveVoiceDirty = true;
	}

	Soloud::~Soloud()
	{
		// let's stop all sounds before deinit, so we don't mess up our mutexes
		stopAll();
		deinit();
		unsigned int i;
		for (i = 0; i < FILTERS_PER_STREAM; i++)
		{
			delete mFilterInstance[i];
		}
		for (i = 0; i < mVoiceGroupCount; i++)
			delete[] mVoiceGroup[i];
		delete[] mVoiceGroup;
	}

	void Soloud::deinit()
	{
		if (mBackendCleanupFunc)
			mBackendCleanupFunc(this);
		mBackendCleanupFunc = 0;
		if (mAudioThreadMutex)
			Thread::destroyMutex(mAudioThreadMutex);
		mAudioThreadMutex = NULL;
	}

	result Soloud::init(unsigned int aFlags, unsigned int aBackend, unsigned int aSamplerate, unsigned int aBufferSize, unsigned int aChannels)
	{		
		if (aBackend >= BACKEND_MAX || aChannels == 3 || aChannels == 5 || aChannels > 6)
			return INVALID_PARAMETER;

		deinit();

		mAudioThreadMutex = Thread::createMutex();

		mBackendID = 0;
		mBackendString = 0;

		int samplerate = 44100;
		int buffersize = 2048;
		int inited = 0;

		if (aSamplerate != Soloud::AUTO) samplerate = aSamplerate;
		if (aBufferSize != Soloud::AUTO) buffersize = aBufferSize;

#if defined(WITH_SDL_STATIC)
		if (aBackend == Soloud::SDL || 
			aBackend == Soloud::AUTO)
		{
			if (aBufferSize == Soloud::AUTO) buffersize = 2048;

			int ret = sdlstatic_init(this, aFlags, samplerate, buffersize, aChannels);
			if (ret == 0)
			{
				inited = 1;
				mBackendID = Soloud::SDL;
			}

			if (ret != 0 && aBackend != Soloud::AUTO)
				return ret;			
		}
#endif

#if defined(WITH_SDL2_STATIC)
		if (aBackend == Soloud::SDL2 ||
			aBackend == Soloud::AUTO)
		{
			if (aBufferSize == Soloud::AUTO) buffersize = 2048;

			int ret = sdl2static_init(this, aFlags, samplerate, buffersize, aChannels);
			if (ret == 0)
			{
				inited = 1;
				mBackendID = Soloud::SDL2;
			}

			if (ret != 0 && aBackend != Soloud::AUTO)
				return ret;
		}
#endif

#if defined(WITH_SDL) || defined(WITH_SDL2)
		if (aBackend == Soloud::SDL || 
			aBackend == Soloud::SDL2 ||
			aBackend == Soloud::AUTO)
		{
			if (aBufferSize == Soloud::AUTO) buffersize = 2048;

			int ret = sdl_init(this, aFlags, samplerate, buffersize, aChannels);
			if (ret == 0)
			{
				inited = 1;
				mBackendID = Soloud::SDL;
			}

			if (ret != 0 && aBackend != Soloud::AUTO)
				return ret;			
		}
#endif

#if defined(WITH_PORTAUDIO)
		if (!inited &&
			(aBackend == Soloud::PORTAUDIO ||
			aBackend == Soloud::AUTO))
		{
			if (aBufferSize == Soloud::AUTO) buffersize = 2048;

			int ret = portaudio_init(this, aFlags, samplerate, buffersize, aChannels);
			if (ret == 0)
			{
				inited = 1;
				mBackendID = Soloud::PORTAUDIO;
			}

			if (ret != 0 && aBackend != Soloud::AUTO)
				return ret;			
		}
#endif

#if defined(WITH_XAUDIO2)
		if (!inited &&
			(aBackend == Soloud::XAUDIO2 ||
			aBackend == Soloud::AUTO))
		{
			if (aBufferSize == Soloud::AUTO) buffersize = 4096;

			int ret = xaudio2_init(this, aFlags, samplerate, buffersize, aChannels);
			if (ret == 0)
			{
				inited = 1;
				mBackendID = Soloud::XAUDIO2;
			}

			if (ret != 0 && aBackend != Soloud::AUTO)
				return ret;			
		}
#endif

#if defined(WITH_WINMM)
		if (!inited &&
			(aBackend == Soloud::WINMM ||
			aBackend == Soloud::AUTO))
		{
			if (aBufferSize == Soloud::AUTO) buffersize = 4096;

			int ret = winmm_init(this, aFlags, samplerate, buffersize, aChannels);
			if (ret == 0)
			{
				inited = 1;
				mBackendID = Soloud::WINMM;
			}

			if (ret != 0 && aBackend != Soloud::AUTO)
				return ret;			
		}
#endif

#if defined(WITH_WASAPI)
		if (!inited &&
			(aBackend == Soloud::WASAPI ||
			aBackend == Soloud::AUTO))
		{
			if (aBufferSize == Soloud::AUTO) buffersize = 4096;

			int ret = wasapi_init(this, aFlags, samplerate, buffersize, aChannels);
			if (ret == 0)
			{
				inited = 1;
				mBackendID = Soloud::WASAPI;
			}

			if (ret != 0 && aBackend != Soloud::AUTO)
				return ret;			
		}
#endif

#if defined(WITH_ALSA)
		if (!inited &&
			(aBackend == Soloud::ALSA ||
			aBackend == Soloud::AUTO))
		{
			if (aBufferSize == Soloud::AUTO) buffersize = 2048;

			int ret = alsa_init(this, aFlags, samplerate, buffersize, aChannels);
			if (ret == 0)
			{
				inited = 1;
				mBackendID = Soloud::ALSA;
			}

			if (ret != 0 && aBackend != Soloud::AUTO)
				return ret;			
		}
#endif

#if defined(WITH_OSS)
		if (!inited &&
			(aBackend == Soloud::OSS ||
			aBackend == Soloud::AUTO))
		{
			if (aBufferSize == Soloud::AUTO) buffersize = 2048;

			int ret = oss_init(this, aFlags, samplerate, buffersize, aChannels);
			if (ret == 0)
			{
				inited = 1;
				mBackendID = Soloud::OSS;
			}

			if (ret != 0 && aBackend != Soloud::AUTO)
				return ret;			
		}
#endif

#if defined(WITH_OPENAL)
		if (!inited &&
			(aBackend == Soloud::OPENAL ||
			aBackend == Soloud::AUTO))
		{
			if (aBufferSize == Soloud::AUTO) buffersize = 4096;

			int ret = openal_init(this, aFlags, samplerate, buffersize, aChannels);
			if (ret == 0)
			{
				inited = 1;
				mBackendID = Soloud::OPENAL;
			}

			if (ret != 0 && aBackend != Soloud::AUTO)
				return ret;			
		}
#endif

#if defined(WITH_COREAUDIO)
		if (!inited &&
			(aBackend == Soloud::COREAUDIO ||
			aBackend == Soloud::AUTO))
		{
			if (aBufferSize == Soloud::AUTO) buffersize = 2048;

			int ret = coreaudio_init(this, aFlags, samplerate, buffersize, aChannels);
			if (ret == 0)
			{
				inited = 1;
				mBackendID = Soloud::COREAUDIO;
			}

			if (ret != 0 && aBackend != Soloud::AUTO)
				return ret;			
		}
#endif

#if defined(WITH_OPENSLES)
		if (!inited &&
			(aBackend == Soloud::OPENSLES ||
			aBackend == Soloud::AUTO))
		{
			if (aBufferSize == Soloud::AUTO) buffersize = 4096;

			int ret = opensles_init(this, aFlags, samplerate, buffersize, aChannels);
			if (ret == 0)
			{
				inited = 1;
				mBackendID = Soloud::OPENSLES;
			}

			if (ret != 0 && aBackend != Soloud::AUTO)
				return ret;			
		}
#endif

#if defined(WITH_NULL)
		if (!inited &&
			(aBackend == Soloud::NULLDRIVER))
		{
			if (aBufferSize == Soloud::AUTO) buffersize = 2048;

			int ret = null_init(this, aFlags, samplerate, buffersize, aChannels);
			if (ret == 0)
			{
				inited = 1;
				mBackendID = Soloud::NULLDRIVER;
			}

			if (ret != 0)
				return ret;			
		}
#endif

		if (!inited && aBackend != Soloud::AUTO)
			return NOT_IMPLEMENTED;
		if (!inited)
			return UNKNOWN_ERROR;
		return 0;
	}

	void Soloud::postinit(unsigned int aSamplerate, unsigned int aBufferSize, unsigned int aFlags, unsigned int aChannels)
	{		
		mGlobalVolume = 1;
		mChannels = aChannels;
		mSamplerate = aSamplerate;
		mBufferSize = aBufferSize;
		mScratchSize = aBufferSize;
		if (mScratchSize < SAMPLE_GRANULARITY * 2) mScratchSize = SAMPLE_GRANULARITY * 2;
		if (mScratchSize < 4096) mScratchSize = 4096;
		mScratchNeeded = mScratchSize;
		mScratch.init(mScratchSize * MAX_CHANNELS);
		mOutputScratch.init(mScratchSize * MAX_CHANNELS);
		mFlags = aFlags;
		mPostClipScaler = 0.95f;
		switch (mChannels)
		{
		case 1:
			m3dSpeakerPosition[0 * 3 + 0] = 0;
			m3dSpeakerPosition[0 * 3 + 1] = 0;
			m3dSpeakerPosition[0 * 3 + 2] = 1;
			break;
		case 2:
			m3dSpeakerPosition[0 * 3 + 0] = 2;
			m3dSpeakerPosition[0 * 3 + 1] = 0;
			m3dSpeakerPosition[0 * 3 + 2] = 1;
			m3dSpeakerPosition[1 * 3 + 0] = -2;
			m3dSpeakerPosition[1 * 3 + 1] = 0;
			m3dSpeakerPosition[1 * 3 + 2] = 1;
			break;
		case 4:
			m3dSpeakerPosition[0 * 3 + 0] = 2;
			m3dSpeakerPosition[0 * 3 + 1] = 0;
			m3dSpeakerPosition[0 * 3 + 2] = 1;
			m3dSpeakerPosition[1 * 3 + 0] = -2;
			m3dSpeakerPosition[1 * 3 + 1] = 0;
			m3dSpeakerPosition[1 * 3 + 2] = 1;
			// I suppose technically the second pair should be straight left & right,
			// but I prefer moving them a bit back to mirror the front speakers.
			m3dSpeakerPosition[2 * 3 + 0] = 2;
			m3dSpeakerPosition[2 * 3 + 1] = 0;
			m3dSpeakerPosition[2 * 3 + 2] = -1;
			m3dSpeakerPosition[3 * 3 + 0] = -2;
			m3dSpeakerPosition[3 * 3 + 1] = 0;
			m3dSpeakerPosition[3 * 3 + 2] = -1;
			break;
		case 6:
			m3dSpeakerPosition[0 * 3 + 0] = 2;
			m3dSpeakerPosition[0 * 3 + 1] = 0;
			m3dSpeakerPosition[0 * 3 + 2] = 1;
			m3dSpeakerPosition[1 * 3 + 0] = -2;
			m3dSpeakerPosition[1 * 3 + 1] = 0;
			m3dSpeakerPosition[1 * 3 + 2] = 1;

			// center and subwoofer. 
			m3dSpeakerPosition[2 * 3 + 0] = 0;
			m3dSpeakerPosition[2 * 3 + 1] = 0;
			m3dSpeakerPosition[2 * 3 + 2] = 1;
			// Sub should be "mix of everything". We'll handle it as a special case and make it a null vector.
			m3dSpeakerPosition[3 * 3 + 0] = 0;
			m3dSpeakerPosition[3 * 3 + 1] = 0;
			m3dSpeakerPosition[3 * 3 + 2] = 0;

			// I suppose technically the second pair should be straight left & right,
			// but I prefer moving them a bit back to mirror the front speakers.
			m3dSpeakerPosition[4 * 3 + 0] = 2;
			m3dSpeakerPosition[4 * 3 + 1] = 0;
			m3dSpeakerPosition[4 * 3 + 2] = -1;
			m3dSpeakerPosition[5 * 3 + 0] = -2;
			m3dSpeakerPosition[5 * 3 + 1] = 0;
			m3dSpeakerPosition[5 * 3 + 2] = -1;
			break;


		}
	}

	const char * Soloud::getErrorString(result aErrorCode) const
	{
		switch (aErrorCode)
		{
		case SO_NO_ERROR: return "No error";
		case INVALID_PARAMETER: return "Some parameter is invalid";
		case FILE_NOT_FOUND: return "File not found";
		case FILE_LOAD_FAILED: return "File found, but could not be loaded";
		case DLL_NOT_FOUND: return "DLL not found, or wrong DLL";
		case OUT_OF_MEMORY: return "Out of memory";
		case NOT_IMPLEMENTED: return "Feature not implemented";
		/*case UNKNOWN_ERROR: return "Other error";*/
		}
		return "Other error";
	}


	float * Soloud::getWave()
	{
		int i;
		lockAudioMutex();
		for (i = 0; i < 256; i++)
			mWaveData[i] = mVisualizationWaveData[i];
		unlockAudioMutex();
		return mWaveData;
	}


	float * Soloud::calcFFT()
	{
		lockAudioMutex();
		float temp[1024];
		int i;
		for (i = 0; i < 256; i++)
		{
			temp[i*2] = mVisualizationWaveData[i];
			temp[i*2+1] = 0;
			temp[i+512] = 0;
			temp[i+768] = 0;
		}
		unlockAudioMutex();

		SoLoud::FFT::fft1024(temp);

		for (i = 0; i < 256; i++)
		{
			float real = temp[i];
			float imag = temp[i+512];
			mFFTData[i] = (float)sqrt(real*real+imag*imag);
		}

		return mFFTData;
	}

#ifdef SOLOUD_SSE_INTRINSICS
	void Soloud::clip(AlignedFloatBuffer &aBuffer, AlignedFloatBuffer &aDestBuffer, unsigned int aSamples, float aVolume0, float aVolume1)
	{
		float vd = (aVolume1 - aVolume0) / aSamples;
		float v = aVolume0;
		unsigned int i, j, c, d;
		// Clip
		if (mFlags & CLIP_ROUNDOFF)
		{
			float nb = -1.65f;		__m128 negbound = _mm_load_ps1(&nb);
			float pb = 1.65f;		__m128 posbound = _mm_load_ps1(&pb);
			float ls = 0.87f;		__m128 linearscale = _mm_load_ps1(&ls);
			float cs = -0.1f;		__m128 cubicscale = _mm_load_ps1(&cs);
			float nw = -0.9862875f;	__m128 negwall = _mm_load_ps1(&nw);
			float pw = 0.9862875f;	__m128 poswall = _mm_load_ps1(&pw);
			__m128 postscale = _mm_load_ps1(&mPostClipScaler);
			AlignedFloatBuffer volumes;
			volumes.init(4);
			volumes.mData[0] = v;
			volumes.mData[1] = v + vd;
			volumes.mData[2] = v + vd + vd;
			volumes.mData[3] = v + vd + vd + vd;
			vd *= 4;
			__m128 vdelta = _mm_load_ps1(&vd);
			c = 0;
			d = 0;
			for (j = 0; j < mChannels; j++)
			{
				__m128 vol = _mm_load_ps(volumes.mData);

				for (i = 0; i < aSamples / 4; i++)
				{
					//float f1 = origdata[c] * v;	c++; v += vd;
					__m128 f = _mm_load_ps(&aBuffer.mData[c]);
					c += 4;
					f = _mm_mul_ps(f, vol);
					vol = _mm_add_ps(vol, vdelta);

					//float u1 = (f1 > -1.65f);
					__m128 u = _mm_cmpgt_ps(f, negbound);

					//float o1 = (f1 < 1.65f);
					__m128 o = _mm_cmplt_ps(f, posbound);

					//f1 = (0.87f * f1 - 0.1f * f1 * f1 * f1) * u1 * o1;
					__m128 lin = _mm_mul_ps(f, linearscale);
					__m128 cubic = _mm_mul_ps(f, f);
					cubic = _mm_mul_ps(cubic, f);
					cubic = _mm_mul_ps(cubic, cubicscale);
					f = _mm_add_ps(cubic, lin);

					//f1 = f1 * u1 + !u1 * -0.9862875f;
					__m128 lowmask = _mm_andnot_ps(u, negwall);
					__m128 ilowmask = _mm_and_ps(u, f);
					f = _mm_add_ps(lowmask, ilowmask);

					//f1 = f1 * o1 + !o1 * 0.9862875f;
					__m128 himask = _mm_andnot_ps(o, poswall);
					__m128 ihimask = _mm_and_ps(o, f);
					f = _mm_add_ps(himask, ihimask);

					// outdata[d] = f1 * postclip; d++;
					f = _mm_mul_ps(f, postscale);
					_mm_store_ps(&aDestBuffer.mData[d], f);
					d += 4;
				}
			}
		}
		else
		{
			float nb = -1.0f;	__m128 negbound = _mm_load_ps1(&nb);
			float pb = 1.0f;	__m128 posbound = _mm_load_ps1(&pb);
			__m128 postscale = _mm_load_ps1(&mPostClipScaler);
			AlignedFloatBuffer volumes;
			volumes.init(4);
			volumes.mData[0] = v;
			volumes.mData[1] = v + vd;
			volumes.mData[2] = v + vd + vd;
			volumes.mData[3] = v + vd + vd + vd;
			vd *= 4;
			__m128 vdelta = _mm_load_ps1(&vd);
			c = 0;
			d = 0;
			for (j = 0; j < mChannels; j++)
			{
				__m128 vol = _mm_load_ps(volumes.mData);
				for (i = 0; i < aSamples / 4; i++)
				{
					//float f1 = aBuffer.mData[c] * v; c++; v += vd;
					__m128 f = _mm_load_ps(&aBuffer.mData[c]);
					c += 4;
					f = _mm_mul_ps(f, vol);
					vol = _mm_add_ps(vol, vdelta);

					//f1 = (f1 <= -1) ? -1 : (f1 >= 1) ? 1 : f1;
					f = _mm_max_ps(f, negbound);
					f = _mm_min_ps(f, posbound);

					//aDestBuffer.mData[d] = f1 * mPostClipScaler; d++;
					f = _mm_mul_ps(f, postscale);
					_mm_store_ps(&aDestBuffer.mData[d], f);
					d += 4;
				}
			}
		}
	}
#else // fallback code
	void Soloud::clip(AlignedFloatBuffer &aBuffer, AlignedFloatBuffer &aDestBuffer, unsigned int aSamples, float aVolume0, float aVolume1)
	{
		float vd = (aVolume1 - aVolume0) / aSamples;
		float v = aVolume0;
		unsigned int i, j, c, d;
		// Clip
		if (mFlags & CLIP_ROUNDOFF)
		{
			c = 0;
			d = 0;
			for (j = 0; j < mChannels; j++)
			{
				v = aVolume0;
				for (i = 0; i < aSamples/4; i++)
				{
					float f1 = aBuffer.mData[c] * v; c++; v += vd;
					float f2 = aBuffer.mData[c] * v; c++; v += vd;
					float f3 = aBuffer.mData[c] * v; c++; v += vd;
					float f4 = aBuffer.mData[c] * v; c++; v += vd;

					f1 = (f1 <= -1.65f) ? -0.9862875f : (f1 >= 1.65f) ? 0.9862875f : (0.87f * f1 - 0.1f * f1 * f1 * f1);
					f2 = (f2 <= -1.65f) ? -0.9862875f : (f2 >= 1.65f) ? 0.9862875f : (0.87f * f2 - 0.1f * f2 * f2 * f2);
					f3 = (f3 <= -1.65f) ? -0.9862875f : (f3 >= 1.65f) ? 0.9862875f : (0.87f * f3 - 0.1f * f3 * f3 * f3);
					f4 = (f4 <= -1.65f) ? -0.9862875f : (f4 >= 1.65f) ? 0.9862875f : (0.87f * f4 - 0.1f * f4 * f4 * f4);

					aDestBuffer.mData[d] = f1 * mPostClipScaler; d++;
					aDestBuffer.mData[d] = f2 * mPostClipScaler; d++;
					aDestBuffer.mData[d] = f3 * mPostClipScaler; d++;
					aDestBuffer.mData[d] = f4 * mPostClipScaler; d++;
				}
			}
		}
		else
		{
			c = 0;
			d = 0;
			for (j = 0; j < mChannels; j++)
			{
				v = aVolume0;
				for (i = 0; i < aSamples / 4; i++)
				{
					float f1 = aBuffer.mData[c] * v; c++; v += vd;
					float f2 = aBuffer.mData[c] * v; c++; v += vd;
					float f3 = aBuffer.mData[c] * v; c++; v += vd;
					float f4 = aBuffer.mData[c] * v; c++; v += vd;

					f1 = (f1 <= -1) ? -1 : (f1 >= 1) ? 1 : f1;
					f2 = (f2 <= -1) ? -1 : (f2 >= 1) ? 1 : f2;
					f3 = (f3 <= -1) ? -1 : (f3 >= 1) ? 1 : f3;
					f4 = (f4 <= -1) ? -1 : (f4 >= 1) ? 1 : f4;

					aDestBuffer.mData[d] = f1 * mPostClipScaler; d++;
					aDestBuffer.mData[d] = f2 * mPostClipScaler; d++;
					aDestBuffer.mData[d] = f3 * mPostClipScaler; d++;
					aDestBuffer.mData[d] = f4 * mPostClipScaler; d++;
			}
		}
	}
}
#endif

#define FIXPOINT_FRAC_BITS 20
#define FIXPOINT_FRAC_MUL (1 << FIXPOINT_FRAC_BITS)
#define FIXPOINT_FRAC_MASK ((1 << FIXPOINT_FRAC_BITS) - 1)

	void resample(float *aSrc,
		          float *aSrc1, 
				  float *aDst, 
				  int aSrcOffset,
				  int aDstSampleCount,
				  float aSrcSamplerate, 
				  float aDstSamplerate,
				  int aStepFixed)
	{
#if 0

#elif defined(RESAMPLER_LINEAR)
		int i;
		int pos = aSrcOffset;

		for (i = 0; i < aDstSampleCount; i++, pos += aStepFixed)
		{
			int p = pos >> FIXPOINT_FRAC_BITS;
			int f = pos & FIXPOINT_FRAC_MASK;
#ifdef _DEBUG
			if (p >= SAMPLE_GRANULARITY || p < 0)
			{
				// This should never actually happen
				p = SAMPLE_GRANULARITY - 1;
			}
#endif
			float s1 = aSrc1[SAMPLE_GRANULARITY - 1];
			float s2 = aSrc[p];
			if (p != 0)
			{
				s1 = aSrc[p-1];
			}
			aDst[i] = s1 + (s2 - s1) * f * (1 / (float)FIXPOINT_FRAC_MUL);
		}
#else // Point sample
		int i;
		int pos = aSrcOffset;

		for (i = 0; i < aDstSampleCount; i++, pos += aStepFixed)
		{
			int p = pos >> FIXPOINT_FRAC_BITS;
			aDst[i] = aSrc[p];
		}
#endif
	}

	void Soloud::mixBus(float *aBuffer, unsigned int aSamples, float *aScratch, unsigned int aBus, float aSamplerate, unsigned int aChannels)
	{
		unsigned int i;
		// Clear accumulation buffer
		for (i = 0; i < aSamples * aChannels; i++)
		{
			aBuffer[i] = 0;
		}

		// Accumulate sound sources		
		for (i = 0; i < mActiveVoiceCount; i++)
		{
			AudioSourceInstance *voice = mVoice[mActiveVoice[i]];
			if (voice &&
				voice->mBusHandle == aBus &&
				!(voice->mFlags & AudioSourceInstance::PAUSED) &&
				!(voice->mFlags & AudioSourceInstance::INAUDIBLE))
			{
				unsigned int j, k;
				float step = voice->mSamplerate / aSamplerate;
				// avoid step overflow
				if (step > (1 << (32 - FIXPOINT_FRAC_BITS)))
					step = 0;
				unsigned int step_fixed = (int)floor(step * FIXPOINT_FRAC_MUL);
				unsigned int outofs = 0;
			
				if (voice->mDelaySamples)
				{
					if (voice->mDelaySamples > aSamples)
					{
						outofs = aSamples;
						voice->mDelaySamples -= aSamples;
					}
					else
					{
						outofs = voice->mDelaySamples;
						voice->mDelaySamples = 0;
					}
					
					// Clear scratch where we're skipping
					for (j = 0; j < voice->mChannels; j++)
					{
						memset(aScratch + j * aSamples, 0, sizeof(float) * outofs); 
					}
				}												

				while (step_fixed != 0 && outofs < aSamples)
				{
					if (voice->mLeftoverSamples == 0)
					{
						// Swap resample buffers (ping-pong)
						AudioSourceResampleData * t = voice->mResampleData[0];
						voice->mResampleData[0] = voice->mResampleData[1];
						voice->mResampleData[1] = t;

						// Get a block of source data

						if (voice->hasEnded())
						{
							memset(voice->mResampleData[0]->mBuffer, 0, sizeof(float) * SAMPLE_GRANULARITY * voice->mChannels);
						}
						else
						{
							voice->getAudio(voice->mResampleData[0]->mBuffer, SAMPLE_GRANULARITY);
						}

						
						

						// If we go past zero, crop to zero (a bit of a kludge)
						if (voice->mSrcOffset < SAMPLE_GRANULARITY * FIXPOINT_FRAC_MUL)
						{
							voice->mSrcOffset = 0;
						}
						else
						{
							// We have new block of data, move pointer backwards
							voice->mSrcOffset -= SAMPLE_GRANULARITY * FIXPOINT_FRAC_MUL;
						}

					
						// Run the per-stream filters to get our source data

						for (j = 0; j < FILTERS_PER_STREAM; j++)
						{
							if (voice->mFilter[j])
							{
								voice->mFilter[j]->filter(
									voice->mResampleData[0]->mBuffer,
									SAMPLE_GRANULARITY, 
									voice->mChannels,
									voice->mSamplerate,
									mStreamTime);
							}
						}
					}
					else
					{
						voice->mLeftoverSamples = 0;
					}

					// Figure out how many samples we can generate from this source data.
					// The value may be zero.

					unsigned int writesamples = 0;

					if (voice->mSrcOffset < SAMPLE_GRANULARITY * FIXPOINT_FRAC_MUL)
					{
						writesamples = ((SAMPLE_GRANULARITY * FIXPOINT_FRAC_MUL) - voice->mSrcOffset) / step_fixed + 1;

						// avoid reading past the current buffer..
						if (((writesamples * step_fixed + voice->mSrcOffset) >> FIXPOINT_FRAC_BITS) >= SAMPLE_GRANULARITY)
							writesamples--;
					}


					// If this is too much for our output buffer, don't write that many:
					if (writesamples + outofs > aSamples)
					{
						voice->mLeftoverSamples = (writesamples + outofs) - aSamples;
						writesamples = aSamples - outofs;
					}

					// Call resampler to generate the samples, once per channel
					if (writesamples)
					{
						for (j = 0; j < voice->mChannels; j++)
						{
							resample(voice->mResampleData[0]->mBuffer + SAMPLE_GRANULARITY * j,
								voice->mResampleData[1]->mBuffer + SAMPLE_GRANULARITY * j,
									 aScratch + aSamples * j + outofs, 
									 voice->mSrcOffset,
									 writesamples,
									 voice->mSamplerate,
									 aSamplerate,
									 step_fixed);
						}
					}

					// Keep track of how many samples we've written so far
					outofs += writesamples;

					// Move source pointer onwards (writesamples may be zero)
					voice->mSrcOffset += writesamples * step_fixed;
				}
				
				float pan[MAX_CHANNELS]; // current speaker volume
				float pand[MAX_CHANNELS]; // destination speaker volume
				float pani[MAX_CHANNELS]; // speaker volume increment per sample
				for (k = 0; k < aChannels; k++)
				{
					pan[k] = voice->mCurrentChannelVolume[k];
					pand[k] = voice->mChannelVolume[k] * voice->mOverallVolume;
					pani[k] = (pand[k] - pan[k]) / aSamples;
				}

				int ofs = 0;
				switch (aChannels)
				{
				case 1: // Target is mono. Sum everything. (1->1, 2->1, 4->1, 6->1)
					for (j = 0, ofs = 0; j < voice->mChannels; j++, ofs += aSamples)
					{
						pan[0] = voice->mCurrentChannelVolume[0];
						for (k = 0; k < aSamples; k++)
						{
							pan[0] += pani[0];
							aBuffer[k] += aScratch[ofs + k] * pan[0];
						}
					}
					break;
				case 2:
					switch (voice->mChannels)
					{
					case 6: // 6->2, just sum lefties and righties, add a bit of center, ignore sub?
						for (j = 0; j < aSamples; j++)
						{
							pan[0] += pani[0];
							pan[1] += pani[1];
							float s1 = aScratch[j];
							float s2 = aScratch[aSamples + j];
							float s3 = aScratch[aSamples * 2 + j];
							//float s4 = aScratch[aSamples * 3 + j];
							float s5 = aScratch[aSamples * 4 + j];
							float s6 = aScratch[aSamples * 5 + j];
							aBuffer[j + 0] += 0.3f * (s1 + s3 + s5) * pan[0];
							aBuffer[j + aSamples] += 0.3f * (s2 + s3 + s6) * pan[1];
						}
						break;
					case 4: // 4->2, just sum lefties and righties
						for (j = 0; j < aSamples; j++)
						{
							pan[0] += pani[0];
							pan[1] += pani[1];
							float s1 = aScratch[j];
							float s2 = aScratch[aSamples + j];
							float s3 = aScratch[aSamples * 2 + j];
							float s4 = aScratch[aSamples * 3 + j];
							aBuffer[j + 0] += 0.5f * (s1 + s3) * pan[0];
							aBuffer[j + aSamples] += 0.5f * (s2 + s4) * pan[1];
						}
						break;
					case 2: // 2->2
						for (j = 0; j < aSamples; j++)
						{
							pan[0] += pani[0];
							pan[1] += pani[1];
							float s1 = aScratch[j];
							float s2 = aScratch[aSamples + j];
							aBuffer[j + 0] += s1 * pan[0];
							aBuffer[j + aSamples] += s2 * pan[1];
						}
						break;
					case 1: // 1->2
						for (j = 0; j < aSamples; j++)
						{
							pan[0] += pani[0];
							pan[1] += pani[1];
							float s = aScratch[j];
							aBuffer[j + 0] += s * pan[0];
							aBuffer[j + aSamples] += s * pan[1];
						}
						break;
					}
					break;
				case 4:
					switch (voice->mChannels)
					{
					case 6: // 6->4, add a bit of center, ignore sub?
						for (j = 0; j < aSamples; j++)
						{
							pan[0] += pani[0];
							pan[1] += pani[1];
							pan[2] += pani[2];
							pan[3] += pani[3];
							float s1 = aScratch[j];
							float s2 = aScratch[aSamples + j];
							float s3 = aScratch[aSamples * 2 + j];
							//float s4 = aScratch[aSamples * 3 + j];
							float s5 = aScratch[aSamples * 4 + j];
							float s6 = aScratch[aSamples * 5 + j];
							float c = s3 * 0.7f;
							aBuffer[j + 0] += s1 * pan[0] + c;
							aBuffer[j + aSamples] += s2 * pan[1] + c;
							aBuffer[j + aSamples * 2] += s5 * pan[2];
							aBuffer[j + aSamples * 3] += s6 * pan[3];
						}
						break;
					case 4: // 4->4
						for (j = 0; j < aSamples; j++)
						{
							pan[0] += pani[0];
							pan[1] += pani[1];
							pan[2] += pani[2];
							pan[3] += pani[3];
							float s1 = aScratch[j];
							float s2 = aScratch[aSamples + j];
							float s3 = aScratch[aSamples * 2 + j];
							float s4 = aScratch[aSamples * 3 + j];
							aBuffer[j + 0] += s1 * pan[0];
							aBuffer[j + aSamples] += s2 * pan[1];
							aBuffer[j + aSamples * 2] += s3 * pan[2];
							aBuffer[j + aSamples * 3] += s4 * pan[3];
						}
						break;
					case 2: // 2->4
						for (j = 0; j < aSamples; j++)
						{
							pan[0] += pani[0];
							pan[1] += pani[1];
							pan[2] += pani[2];
							pan[3] += pani[3];
							float s1 = aScratch[j];
							float s2 = aScratch[aSamples + j];
							aBuffer[j + 0] += s1 * pan[0];
							aBuffer[j + aSamples] += s2 * pan[1];
							aBuffer[j + aSamples * 2] += s1 * pan[2];
							aBuffer[j + aSamples * 3] += s2 * pan[3];
						}
						break;
					case 1: // 1->4
						for (j = 0; j < aSamples; j++)
						{
							pan[0] += pani[0];
							pan[1] += pani[1];
							pan[2] += pani[2];
							pan[3] += pani[3];
							float s = aScratch[j];
							aBuffer[j + 0] += s * pan[0];
							aBuffer[j + aSamples] += s * pan[1];
							aBuffer[j + aSamples * 2] += s * pan[2];
							aBuffer[j + aSamples * 3] += s * pan[3];
						}
						break;
					}
					break;
				case 6:
					switch (voice->mChannels)
					{
					case 6: // 6->6
						for (j = 0; j < aSamples; j++)
						{
							pan[0] += pani[0];
							pan[1] += pani[1];
							pan[2] += pani[2];
							pan[3] += pani[3];
							pan[4] += pani[4];
							pan[5] += pani[5];
							float s1 = aScratch[j];
							float s2 = aScratch[aSamples + j];
							float s3 = aScratch[aSamples * 2 + j];
							float s4 = aScratch[aSamples * 3 + j];
							float s5 = aScratch[aSamples * 4 + j];
							float s6 = aScratch[aSamples * 5 + j];
							aBuffer[j + 0] += s1 * pan[0];
							aBuffer[j + aSamples] += s2 * pan[1];
							aBuffer[j + aSamples * 2] += s3 * pan[2];
							aBuffer[j + aSamples * 3] += s4 * pan[3];
							aBuffer[j + aSamples * 4] += s5 * pan[4];
							aBuffer[j + aSamples * 5] += s6 * pan[5];
						}
						break;
					case 4: // 4->6
						for (j = 0; j < aSamples; j++)
						{
							pan[0] += pani[0];
							pan[1] += pani[1];
							pan[2] += pani[2];
							pan[3] += pani[3];
							pan[4] += pani[4];
							pan[5] += pani[5];
							float s1 = aScratch[j];
							float s2 = aScratch[aSamples + j];
							float s3 = aScratch[aSamples * 2 + j];
							float s4 = aScratch[aSamples * 3 + j];
							aBuffer[j + 0] += s1 * pan[0];
							aBuffer[j + aSamples] += s2 * pan[1];
							aBuffer[j + aSamples * 2] += 0.5f * (s1 + s2) * pan[2];
							aBuffer[j + aSamples * 3] += 0.25f * (s1 + s2 + s3 + s4) * pan[3];
							aBuffer[j + aSamples * 4] += s3 * pan[4];
							aBuffer[j + aSamples * 5] += s4 * pan[5];
						}
						break;
					case 2: // 2->6
						for (j = 0; j < aSamples; j++)
						{
							pan[0] += pani[0];
							pan[1] += pani[1];
							pan[2] += pani[2];
							pan[3] += pani[3];
							pan[4] += pani[4];
							pan[5] += pani[5];
							float s1 = aScratch[j];
							float s2 = aScratch[aSamples + j];
							aBuffer[j + 0] += s1 * pan[0];
							aBuffer[j + aSamples] += s2 * pan[1];
							aBuffer[j + aSamples * 2] += 0.5f * (s1 + s2) * pan[2];
							aBuffer[j + aSamples * 3] += 0.5f * (s1 + s2) * pan[3];
							aBuffer[j + aSamples * 4] += s1 * pan[4];
							aBuffer[j + aSamples * 5] += s2 * pan[5];
						}
						break;
					case 1: // 1->6
						for (j = 0; j < aSamples; j++)
						{
							pan[0] += pani[0];
							pan[1] += pani[1];
							pan[2] += pani[2];
							pan[3] += pani[3];
							pan[4] += pani[4];
							pan[5] += pani[5];
							float s = aScratch[j];
							aBuffer[j + 0] += s * pan[0];
							aBuffer[j + aSamples] += s * pan[1];
							aBuffer[j + aSamples * 2] += s * pan[2];
							aBuffer[j + aSamples * 3] += s * pan[3];
							aBuffer[j + aSamples * 4] += s * pan[4];
							aBuffer[j + aSamples * 5] += s * pan[5];
						}
						break;
					}
					break;
				}
				
				for (k = 0; k < aChannels; k++)
					voice->mCurrentChannelVolume[k] = pand[k];

				// clear voice if the sound is over
				if (!(voice->mFlags & AudioSourceInstance::LOOPING) && voice->hasEnded())
				{
					stopVoice(mActiveVoice[i]);
				}
			}
			else
				if (voice &&
					voice->mBusHandle == aBus &&
					!(voice->mFlags & AudioSourceInstance::PAUSED) &&
					(voice->mFlags & AudioSourceInstance::INAUDIBLE) &&
					(voice->mFlags & AudioSourceInstance::INAUDIBLE_TICK))
			{
				// Inaudible but needs ticking. Do minimal work (keep counters up to date and ask audiosource for data)
				float step = voice->mSamplerate / aSamplerate;
				int step_fixed = (int)floor(step * FIXPOINT_FRAC_MUL);
				unsigned int outofs = 0;

				if (voice->mDelaySamples)
				{
					if (voice->mDelaySamples > aSamples)
					{
						outofs = aSamples;
						voice->mDelaySamples -= aSamples;
					}
					else
					{
						outofs = voice->mDelaySamples;
						voice->mDelaySamples = 0;
					}
				}

				while (step_fixed != 0 && outofs < aSamples)
				{
					if (voice->mLeftoverSamples == 0)
					{
						// Swap resample buffers (ping-pong)
						AudioSourceResampleData * t = voice->mResampleData[0];
						voice->mResampleData[0] = voice->mResampleData[1];
						voice->mResampleData[1] = t;

						// Get a block of source data

						if (!voice->hasEnded())
						{
							voice->getAudio(voice->mResampleData[0]->mBuffer, SAMPLE_GRANULARITY);
						}


						// If we go past zero, crop to zero (a bit of a kludge)
						if (voice->mSrcOffset < SAMPLE_GRANULARITY * FIXPOINT_FRAC_MUL)
						{
							voice->mSrcOffset = 0;
						}
						else
						{
							// We have new block of data, move pointer backwards
							voice->mSrcOffset -= SAMPLE_GRANULARITY * FIXPOINT_FRAC_MUL;
						}

						// Skip filters
					}
					else
					{
						voice->mLeftoverSamples = 0;
					}

					// Figure out how many samples we can generate from this source data.
					// The value may be zero.

					unsigned int writesamples = 0;

					if (voice->mSrcOffset < SAMPLE_GRANULARITY * FIXPOINT_FRAC_MUL)
					{
						writesamples = ((SAMPLE_GRANULARITY * FIXPOINT_FRAC_MUL) - voice->mSrcOffset) / step_fixed + 1;

						// avoid reading past the current buffer..
						if (((writesamples * step_fixed + voice->mSrcOffset) >> FIXPOINT_FRAC_BITS) >= SAMPLE_GRANULARITY)
							writesamples--;
					}


					// If this is too much for our output buffer, don't write that many:
					if (writesamples + outofs > aSamples)
					{
						voice->mLeftoverSamples = (writesamples + outofs) - aSamples;
						writesamples = aSamples - outofs;
					}

					// Skip resampler

					// Keep track of how many samples we've written so far
					outofs += writesamples;

					// Move source pointer onwards (writesamples may be zero)
					voice->mSrcOffset += writesamples * step_fixed;
				}

				// clear voice if the sound is over
				if (!(voice->mFlags & AudioSourceInstance::LOOPING) && voice->hasEnded())
				{
					stopVoice(mActiveVoice[i]);
				}
			}
		}
	}

	void Soloud::calcActiveVoices()
	{
		// TODO: consider whether we need to re-evaluate the active voices all the time.
		// It is a must when new voices are started, but otherwise we could get away
		// with postponing it sometimes..

		mActiveVoiceDirty = false;

		// Populate
		unsigned int i, candidates, mustlive;
		candidates = 0;
		mustlive = 0;
		for (i = 0; i < mHighestVoice; i++)
		{
			if (mVoice[i] && (!(mVoice[i]->mFlags & (AudioSourceInstance::INAUDIBLE | AudioSourceInstance::PAUSED)) || (mVoice[i]->mFlags & AudioSourceInstance::INAUDIBLE_TICK)))
			{
				mActiveVoice[candidates] = i;
				candidates++;
				if (mVoice[i]->mFlags & AudioSourceInstance::INAUDIBLE_TICK)
				{
					mActiveVoice[candidates - 1] = mActiveVoice[mustlive];
					mActiveVoice[mustlive] = i;
					mustlive++;
				}
			}
		}

		// Check for early out
		if (candidates <= mMaxActiveVoices)
		{
			// everything is audible, early out
			mActiveVoiceCount = candidates;
			return;
		}

		mActiveVoiceCount = mMaxActiveVoices;

		if (mustlive >= mMaxActiveVoices)
		{
			// Oopsie. Well, nothing to sort, since the "must live" voices already
			// ate all our active voice slots.
			// This is a potentially an error situation, but we have no way to report
			// error from here. And asserting could be bad, too.
			return;
		}

		// If we get this far, there's nothing to it: we'll have to sort the voices to find the most audible.

		// Iterative partial quicksort:
		int left = 0, stack[24], pos = 0, right;
		int len = candidates - mustlive;
		unsigned int *data = mActiveVoice + mustlive;
		int k = mActiveVoiceCount;
		for (;;) 
		{                                 
			for (; left + 1 < len; len++) 
			{                
				if (pos == 24) len = stack[pos = 0]; 
				int pivot = data[left];
				float pivotvol = mVoice[pivot]->mOverallVolume;
				stack[pos++] = len;      
				for (right = left - 1;;) 
				{
					do 
					{
						right++;
					} 
					while (mVoice[data[right]]->mOverallVolume > pivotvol);
					do
					{
						len--;
					}
					while (pivotvol > mVoice[data[len]]->mOverallVolume);
					if (right >= len) break;       
					int temp = data[right];
					data[right] = data[len];
					data[len] = temp;
				}                        
			}
			if (pos == 0) break;         
			if (left >= k) break;
			left = len;                  
			len = stack[--pos];          
		}		
	}

	void Soloud::mix_internal(unsigned int aSamples)
	{
#ifdef FLOATING_POINT_DEBUG
		// This needs to be done in the audio thread as well..
		static int done = 0;
		if (!done)
		{
			unsigned int u;
			u = _controlfp(0, 0);
			u = u & ~(_EM_INVALID | /*_EM_DENORMAL |*/ _EM_ZERODIVIDE | _EM_OVERFLOW /*| _EM_UNDERFLOW  | _EM_INEXACT*/);
			_controlfp(u, _MCW_EM);
			done = 1;
		}
#endif

		float buffertime = aSamples / (float)mSamplerate;
		float globalVolume[2];
		mStreamTime += buffertime;
		mLastClockedTime = 0;

		globalVolume[0] = mGlobalVolume;
		if (mGlobalVolumeFader.mActive)
		{
			mGlobalVolume = mGlobalVolumeFader.get(mStreamTime);
		}
		globalVolume[1] = mGlobalVolume;

		lockAudioMutex();

		// Process faders. May change scratch size.
		int i;
		for (i = 0; i < (signed)mHighestVoice; i++)
		{
			if (mVoice[i] && !(mVoice[i]->mFlags & AudioSourceInstance::PAUSED))
			{
				float volume[2];

				mVoice[i]->mActiveFader = 0;

				if (mGlobalVolumeFader.mActive > 0)
				{
					mVoice[i]->mActiveFader = 1;
				}

				mVoice[i]->mStreamTime += buffertime;

				if (mVoice[i]->mRelativePlaySpeedFader.mActive > 0)
				{
					float speed = mVoice[i]->mRelativePlaySpeedFader.get(mVoice[i]->mStreamTime);
					setVoiceRelativePlaySpeed(i, speed);
				}

				volume[0] = mVoice[i]->mOverallVolume;
				if (mVoice[i]->mVolumeFader.mActive > 0)
				{
					mVoice[i]->mSetVolume = mVoice[i]->mVolumeFader.get(mVoice[i]->mStreamTime);
					mVoice[i]->mActiveFader = 1;
					updateVoiceVolume(i);
					mActiveVoiceDirty = true;
				}
				volume[1] = mVoice[i]->mOverallVolume;

				if (mVoice[i]->mPanFader.mActive > 0)
				{
					float pan = mVoice[i]->mPanFader.get(mVoice[i]->mStreamTime);
					setVoicePan(i, pan);
					mVoice[i]->mActiveFader = 1;
				}

				if (mVoice[i]->mPauseScheduler.mActive)
				{
					mVoice[i]->mPauseScheduler.get(mVoice[i]->mStreamTime);
					if (mVoice[i]->mPauseScheduler.mActive == -1)
					{
						mVoice[i]->mPauseScheduler.mActive = 0;
						setVoicePause(i, 1);
					}
				}

				if (mVoice[i]->mStopScheduler.mActive)
				{
					mVoice[i]->mStopScheduler.get(mVoice[i]->mStreamTime);
					if (mVoice[i]->mStopScheduler.mActive == -1)
					{
						mVoice[i]->mStopScheduler.mActive = 0;
						stopVoice(i);
					}
				}
			}
		}

		if (mActiveVoiceDirty)
			calcActiveVoices();

		// Resize scratch if needed.
		if (mScratchSize < mScratchNeeded)
		{
			mScratchSize = mScratchNeeded;
			mScratch.init(mScratchSize * MAX_CHANNELS);
		}
		
		mixBus(mOutputScratch.mData, aSamples, mScratch.mData, 0, (float)mSamplerate, mChannels);

		for (i = 0; i < FILTERS_PER_STREAM; i++)
		{
			if (mFilterInstance[i])
			{
				mFilterInstance[i]->filter(mOutputScratch.mData, aSamples, mChannels, (float)mSamplerate, mStreamTime);
			}
		}

		unlockAudioMutex();

		clip(mOutputScratch, mScratch, aSamples, globalVolume[0], globalVolume[1]);

		if (mFlags & ENABLE_VISUALIZATION)
		{
			if (aSamples > 255)
			{
				for (i = 0; i < 256; i++)
				{
					int j;
					mVisualizationWaveData[i] = 0;
					for (j = 0; j < (signed)mChannels; j++)
					{
						mVisualizationWaveData[i] += mScratch.mData[i + j * aSamples];
					}
				}
			}
			else
			{
				// Very unlikely failsafe branch
				for (i = 0; i < 256; i++)
				{
					int j;
					mVisualizationWaveData[i] = 0;
					for (j = 0; j < (signed)mChannels; j++)
					{
						mVisualizationWaveData[i] += mScratch.mData[(i % aSamples) + j * aSamples];
					}
				}
			}
		}
	}

	void Soloud::mix(float *aBuffer, unsigned int aSamples)
	{
		mix_internal(aSamples);
		interlace_samples_float(mScratch.mData, aBuffer, aSamples, mChannels);
	}

	void Soloud::mixSigned16(short *aBuffer, unsigned int aSamples)
	{
		mix_internal(aSamples);
		interlace_samples_s16(mScratch.mData, aBuffer, aSamples, mChannels);
	}

	void deinterlace_samples_float(const float *aSourceBuffer, float *aDestBuffer, unsigned int aSamples, unsigned int aChannels)
	{
		// 121212 -> 111222
		unsigned int i, j, c;
		c = 0;
		for (j = 0; j < aChannels; j++)
		{
			for (i = j; i < aSamples; i += aChannels)
			{
				aDestBuffer[c] = aSourceBuffer[i + j];
				c++;
			}
		}
	}

	void interlace_samples_float(const float *aSourceBuffer, float *aDestBuffer, unsigned int aSamples, unsigned int aChannels)
	{
		// 111222 -> 121212
		unsigned int i, j, c;
		c = 0;
		for (j = 0; j < aChannels; j++)
		{
			for (i = j; i < aSamples * aChannels; i += aChannels)
			{
				aDestBuffer[i] = aSourceBuffer[c];
				c++;
			}
		}
	}

	void interlace_samples_s16(const float *aSourceBuffer, short *aDestBuffer, unsigned int aSamples, unsigned int aChannels)
	{
		// 111222 -> 121212
		unsigned int i, j, c;
		c = 0;
		for (j = 0; j < aChannels; j++)
		{
			for (i = j; i < aSamples * aChannels; i += aChannels)
			{
				aDestBuffer[i] = (short)(aSourceBuffer[c] * 0x7fff);
				c++;
			}
		}
	}

	void Soloud::lockAudioMutex()
	{
		if (mAudioThreadMutex)
			Thread::lockMutex(mAudioThreadMutex);
	}

	void Soloud::unlockAudioMutex()
	{
		if (mAudioThreadMutex)
			Thread::unlockMutex(mAudioThreadMutex);
	}

};
//----soloud_audiosource.cpp-----------------------------------------------------------------------
namespace SoLoud
{

	AudioSourceInstance3dData::AudioSourceInstance3dData()
	{
		m3dAttenuationModel = 0;
		m3dAttenuationRolloff = 1;
		m3dDopplerFactor = 1.0;
		m3dMaxDistance = 1000000.0f;
		m3dMinDistance = 0.0f;
		m3dPosition[0] = 0;
		m3dPosition[1] = 0;
		m3dPosition[2] = 0;
		m3dVelocity[0] = 0;
		m3dVelocity[1] = 0;
		m3dVelocity[2] = 0;
		mCollider = 0;
		mColliderData = 0;
		mAttenuator = 0;
	}

	void AudioSourceInstance3dData::init(AudioSource &aSource)
	{
		m3dAttenuationModel = aSource.m3dAttenuationModel;
		m3dAttenuationRolloff = aSource.m3dAttenuationRolloff;
		m3dDopplerFactor = aSource.m3dDopplerFactor;
		m3dMaxDistance = aSource.m3dMaxDistance;
		m3dMinDistance = aSource.m3dMinDistance;
		mCollider = aSource.mCollider;
		mColliderData = aSource.mColliderData;
		mAttenuator = aSource.mAttenuator;
		m3dVolume = 1.0f;
		mDopplerValue = 1.0f;
	}

	AudioSourceResampleData::AudioSourceResampleData()
	{
		mBuffer = 0;
	}
	
	AudioSourceResampleData::~AudioSourceResampleData()
	{
		delete[] mBuffer;
	}

	AudioSourceInstance::AudioSourceInstance()
	{
		mPlayIndex = 0;
		mFlags = 0;
		mPan = 0;
		// Default all volumes to 1.0 so sound behind N mix busses isn't super quiet.
		int i;
		for (i = 0; i < MAX_CHANNELS; i++)
			mChannelVolume[i] = 1.0f;		
		mSetVolume = 1.0f;
		mBaseSamplerate = 44100.0f;
		mSamplerate = 44100.0f;
		mSetRelativePlaySpeed = 1.0f;
		mStreamTime = 0.0f;
		mAudioSourceID = 0;
		mActiveFader = 0;
		mChannels = 1;
		mBusHandle = ~0u;
		mLoopCount = 0;
		for (i = 0; i < FILTERS_PER_STREAM; i++)
		{
			mFilter[i] = NULL;
		}
		for (i = 0; i < MAX_CHANNELS; i++)
		{
			mCurrentChannelVolume[i] = 0;
		}
		// behind pointers because we swap between the two buffers
		mResampleData[0] = new AudioSourceResampleData;
		mResampleData[1] = new AudioSourceResampleData;
		mSrcOffset = 0;
		mLeftoverSamples = 0;
		mDelaySamples = 0;

	}

	AudioSourceInstance::~AudioSourceInstance()
	{
		int i;
		for (i = 0; i < FILTERS_PER_STREAM; i++)
		{
			delete mFilter[i];
		}
		delete mResampleData[0];
		delete mResampleData[1];
	}

	void AudioSourceInstance::init(AudioSource &aSource, int aPlayIndex)
	{
		mPlayIndex = aPlayIndex;
		mBaseSamplerate = aSource.mBaseSamplerate;
		mSamplerate = mBaseSamplerate;
		mChannels = aSource.mChannels;
		mStreamTime = 0.0f;

		if (aSource.mFlags & AudioSource::SHOULD_LOOP)
		{
			mFlags |= AudioSourceInstance::LOOPING;
		}
		if (aSource.mFlags & AudioSource::PROCESS_3D)
		{
			mFlags |= AudioSourceInstance::PROCESS_3D;
		}
		if (aSource.mFlags & AudioSource::LISTENER_RELATIVE)
		{
			mFlags |= AudioSourceInstance::LISTENER_RELATIVE;
		}
		if (aSource.mFlags & AudioSource::INAUDIBLE_KILL)
		{
			mFlags |= AudioSourceInstance::INAUDIBLE_KILL;
		}
		if (aSource.mFlags & AudioSource::INAUDIBLE_TICK)
		{
			mFlags |= AudioSourceInstance::INAUDIBLE_TICK;
		}
	}

	result AudioSourceInstance::rewind()
	{
		return NOT_IMPLEMENTED;
	}

	void AudioSourceInstance::seek(double aSeconds, float *mScratch, unsigned int mScratchSize)
	{
		double offset = aSeconds - mStreamTime;
		if (offset < 0)
		{
			if (rewind() != SO_NO_ERROR)
			{
				// can't do generic seek backwards unless we can rewind.
				return;
			}
			offset = aSeconds;
		}
		int samples_to_discard = (int)floor(mSamplerate * offset);

		while (samples_to_discard)
		{
			int samples = mScratchSize / 2;
			if (samples > samples_to_discard)
				samples = samples_to_discard;
			getAudio(mScratch, samples);
			samples_to_discard -= samples;
		}

		mStreamTime = aSeconds;
	}


	AudioSource::AudioSource() 
	{ 
		int i;
		for (i = 0; i < FILTERS_PER_STREAM; i++)
		{
			mFilter[i] = 0;
		}
		mFlags = 0; 
		mBaseSamplerate = 44100; 
		mAudioSourceID = 0;
		mSoloud = 0;
		mChannels = 1;
		m3dMinDistance = 1;
		m3dMaxDistance = 1000000.0f;
		m3dAttenuationRolloff = 1.0f;
		m3dAttenuationModel = NO_ATTENUATION;
		m3dDopplerFactor = 1.0f;
		mCollider = 0;
		mAttenuator = 0;
		mColliderData = 0;
		mVolume = 1;
	}

	AudioSource::~AudioSource() 
	{
		stop();
	}

	void AudioSource::setVolume(float aVolume)
	{
		mVolume = aVolume;
	}

	void AudioSource::setLooping(bool aLoop)
	{
		if (aLoop)
		{
			mFlags |= SHOULD_LOOP;
		}
		else
		{
			mFlags &= ~SHOULD_LOOP;
		}
	}

	void AudioSource::setSingleInstance(bool aSingleInstance)
	{
		if (aSingleInstance)
		{
			mFlags |= SINGLE_INSTANCE;
		}
		else
		{
			mFlags &= ~SINGLE_INSTANCE;
		}
	}

	void AudioSource::setFilter(unsigned int aFilterId, Filter *aFilter)
	{
		if (aFilterId >= FILTERS_PER_STREAM)
			return;
		mFilter[aFilterId] = aFilter;
	}

	void AudioSource::stop()
	{
		if (mSoloud)
		{
			mSoloud->stopAudioSource(*this);
		}
	}

	void AudioSource::set3dMinMaxDistance(float aMinDistance, float aMaxDistance)
	{
		m3dMinDistance = aMinDistance;
		m3dMaxDistance = aMaxDistance;
	}

	void AudioSource::set3dAttenuation(unsigned int aAttenuationModel, float aAttenuationRolloffFactor)
	{
		m3dAttenuationModel = aAttenuationModel;
		m3dAttenuationRolloff = aAttenuationRolloffFactor;
	}

	void AudioSource::set3dDopplerFactor(float aDopplerFactor)
	{
		m3dDopplerFactor = aDopplerFactor;
	}

	void AudioSource::set3dProcessing(bool aDo3dProcessing)
	{
		if (aDo3dProcessing)
		{
			mFlags |= PROCESS_3D;
		}
		else
		{
			mFlags &= ~PROCESS_3D;
		}
	}

	void AudioSource::set3dListenerRelative(bool aListenerRelative)
	{
		if (aListenerRelative)
		{
			mFlags |= LISTENER_RELATIVE;
		}
		else
		{
			mFlags &= ~LISTENER_RELATIVE;
		}
	}


	void AudioSource::set3dDistanceDelay(bool aDistanceDelay)
	{
		if (aDistanceDelay)
		{
			mFlags |= DISTANCE_DELAY;
		}
		else
		{
			mFlags &= ~DISTANCE_DELAY;
		}
	}

	void AudioSource::set3dCollider(AudioCollider *aCollider, int aUserData)
	{
		mCollider = aCollider;
		mColliderData = aUserData;
	}

	void AudioSource::set3dAttenuator(AudioAttenuator *aAttenuator)
	{
		mAttenuator = aAttenuator;
	}

	void AudioSource::setInaudibleBehavior(bool aMustTick, bool aKill)
	{
		mFlags &= ~(AudioSource::INAUDIBLE_KILL | AudioSource::INAUDIBLE_TICK);
		if (aMustTick)
		{
			mFlags |= AudioSource::INAUDIBLE_TICK;
		}
		if (aKill)
		{
			mFlags |= AudioSource::INAUDIBLE_KILL;
		}
	}


	float AudioSourceInstance::getInfo(unsigned int aInfoKey)
	{
	    return 0;
	}


};
//----soloud_bus.cpp-------------------------------------------------------------------------------
namespace SoLoud
{
	BusInstance::BusInstance(Bus *aParent)
	{
		mParent = aParent;
		mScratchSize = 0;
		mFlags |= PROTECTED | INAUDIBLE_TICK;
	}
	
	void BusInstance::getAudio(float *aBuffer, unsigned int aSamples)
	{
		int handle = mParent->mChannelHandle;
		if (handle == 0) 
		{
			// Avoid reuse of scratch data if this bus hasn't played anything yet
			unsigned int i;
			for (i = 0; i < aSamples * mChannels; i++)
				aBuffer[i] = 0;
			return;
		}
		
		Soloud *s = mParent->mSoloud;
		if (s->mScratchNeeded != mScratchSize)
		{
			mScratchSize = s->mScratchNeeded;
			mScratch.init(mScratchSize * MAX_CHANNELS);
		}
		
		s->mixBus(aBuffer, aSamples, mScratch.mData, handle, mSamplerate, mChannels);

		int i;
		if (mParent->mFlags & AudioSource::VISUALIZATION_DATA)
		{
			if (aSamples > 255)
			{
				for (i = 0; i < 256; i++)
				{
					int j;
					mVisualizationWaveData[i] = 0;
					for (j = 0; j < (signed)mChannels; j++)
						mVisualizationWaveData[i] += aBuffer[i + aSamples * j];
				}
			}
			else
			{
				// Very unlikely failsafe branch
				for (i = 0; i < 256; i++)
				{
					int j;
					mVisualizationWaveData[i] = 0;
					for (j = 0; j < (signed)mChannels; j++)
						mVisualizationWaveData[i] += aBuffer[(i % aSamples) + aSamples * j];
				}
			}
		}
	}

	bool BusInstance::hasEnded()
	{
		// Busses never stop for fear of going under 50mph.
		return 0;
	}

	BusInstance::~BusInstance()
	{
		Soloud *s = mParent->mSoloud;
		int i;
		for (i = 0; i < (signed)s->mHighestVoice; i++)
		{
			if (s->mVoice[i] && s->mVoice[i]->mBusHandle == mParent->mChannelHandle)
			{
				s->stopVoice(i);
			}
		}
	}

	Bus::Bus()
	{
		mChannelHandle = 0;
		mInstance = 0;
		mChannels = 2;
	}
	
	BusInstance * Bus::createInstance()
	{
		if (mChannelHandle)
		{
			// We should be inside the mutex at the moment, so stop using the "internal" functions
			mSoloud->stopVoice(mSoloud->getVoiceFromHandle(mChannelHandle));
			mChannelHandle = 0;
			mInstance = 0;
		}
		mInstance = new BusInstance(this);
		return mInstance;
	}

	void Bus::findBusHandle()
	{
		if (mChannelHandle == 0)
		{
			// Find the channel the bus is playing on to calculate handle..
			int i;
			for (i = 0; mChannelHandle == 0 && i < (signed)mSoloud->mHighestVoice; i++)
			{
				if (mSoloud->mVoice[i] == mInstance)
				{
					mChannelHandle = mSoloud->getHandleFromVoice(i);
				}
			}
		}
	}

	handle Bus::play(AudioSource &aSound, float aVolume, float aPan, bool aPaused)
	{
		if (!mInstance || !mSoloud)
		{
			return 0;
		}

		findBusHandle();

		if (mChannelHandle == 0)
		{
			return 0;
		}
		return mSoloud->play(aSound, aVolume, aPan, aPaused, mChannelHandle);
	}	


	handle Bus::playClocked(time aSoundTime, AudioSource &aSound, float aVolume, float aPan)
	{
		if (!mInstance || !mSoloud)
		{
			return 0;
		}

		findBusHandle();

		if (mChannelHandle == 0)
		{
			return 0;
		}

		return mSoloud->playClocked(aSoundTime, aSound, aVolume, aPan, mChannelHandle);
	}	

	handle Bus::play3d(AudioSource &aSound, float aPosX, float aPosY, float aPosZ, float aVelX, float aVelY, float aVelZ, float aVolume, bool aPaused)
	{
		if (!mInstance || !mSoloud)
		{
			return 0;
		}

		findBusHandle();

		if (mChannelHandle == 0)
		{
			return 0;
		}
		return mSoloud->play3d(aSound, aPosX, aPosY, aPosZ, aVelX, aVelY, aVelZ, aVolume, aPaused, mChannelHandle);
	}

	handle Bus::play3dClocked(time aSoundTime, AudioSource &aSound, float aPosX, float aPosY, float aPosZ, float aVelX, float aVelY, float aVelZ, float aVolume)
	{
		if (!mInstance || !mSoloud)
		{
			return 0;
		}

		findBusHandle();

		if (mChannelHandle == 0)
		{
			return 0;
		}
		return mSoloud->play3dClocked(aSoundTime, aSound, aPosX, aPosY, aPosZ, aVelX, aVelY, aVelZ, aVolume, mChannelHandle);
	}


	void Bus::setFilter(unsigned int aFilterId, Filter *aFilter)
	{
		if (aFilterId >= FILTERS_PER_STREAM)
			return;

		mFilter[aFilterId] = aFilter;

		if (mInstance)
		{
			mSoloud->lockAudioMutex();
			delete mInstance->mFilter[aFilterId];
			mInstance->mFilter[aFilterId] = 0;
		
			if (aFilter)
			{
				mInstance->mFilter[aFilterId] = mFilter[aFilterId]->createInstance();
			}
			mSoloud->unlockAudioMutex();
		}
	}

	result Bus::setChannels(unsigned int aChannels)
	{
		if (aChannels == 0 || aChannels == 3 || aChannels == 5 || aChannels > 6)
			return INVALID_PARAMETER;
		mChannels = aChannels;
		return SO_NO_ERROR;
	}

	void Bus::setVisualizationEnable(bool aEnable)
	{
		if (aEnable)
		{
			mFlags |= AudioSource::VISUALIZATION_DATA;
		}
		else
		{
			mFlags &= ~AudioSource::VISUALIZATION_DATA;
		}
	}
		
	float * Bus::calcFFT()
	{
		if (mInstance && mSoloud)
		{
			mSoloud->lockAudioMutex();
			float temp[1024];
			int i;
			for (i = 0; i < 256; i++)
			{
				temp[i*2] = mInstance->mVisualizationWaveData[i];
				temp[i*2+1] = 0;
				temp[i+512] = 0;
				temp[i+768] = 0;
			}
			mSoloud->unlockAudioMutex();

			SoLoud::FFT::fft1024(temp);

			for (i = 0; i < 256; i++)
			{
				float real = temp[i];
				float imag = temp[i+512];
				mFFTData[i] = (float)sqrt(real*real+imag*imag);
			}
		}

		return mFFTData;
	}

	float * Bus::getWave()
	{
		if (mInstance && mSoloud)
		{
			int i;
			mSoloud->lockAudioMutex();
			for (i = 0; i < 256; i++)
				mWaveData[i] = mInstance->mVisualizationWaveData[i];
			mSoloud->unlockAudioMutex();
		}
		return mWaveData;
	}

};
//----soloud_core_3d.cpp-----------------------------------------------------------------------
// 3d audio operations

namespace SoLoud
{
	struct vec3
	{
		float mX, mY, mZ;

		bool null()
		{
			if (mX == 0 && mY == 0 && mZ == 0)
				return true;
			return false;
		}

		void neg()
		{
			mX = -mX;
			mY = -mY;
			mZ = -mZ;
		}

		float mag()
		{
			return (float)sqrt(mX * mX + mY * mY + mZ * mZ);
		}
		
		void normalize()
		{
			float m = mag();
			if (m == 0)
			{
				mX = mY = mZ = 0;
				return;
			}
			mX /= m;
			mY /= m;
			mZ /= m;
		}
		
		float dot(vec3 a)
		{
			return mX * a.mX + mY * a.mY + mZ * a.mZ;
		}
		
		vec3 sub(vec3 a)
		{
			vec3 r;
			r.mX = mX - a.mX;
			r.mY = mY - a.mY;
			r.mZ = mZ - a.mZ;
			return r;
		}

		vec3 cross(vec3 a)
		{
			vec3 r;

			r.mX = mY * a.mZ - a.mY * mZ;
			r.mY = mZ * a.mX - a.mZ * mX;
			r.mZ = mX * a.mY - a.mX * mY;

			return r;
		}
	};

	struct mat3
	{
		vec3 m[3];

		vec3 mul(vec3 a)
		{
			vec3 r;

			r.mX = m[0].mX * a.mX + m[0].mY * a.mY + m[0].mZ * a.mZ;
			r.mY = m[1].mX * a.mX + m[1].mY * a.mY + m[1].mZ * a.mZ;
			r.mZ = m[2].mX * a.mX + m[2].mY * a.mY + m[2].mZ * a.mZ;

			return r;
		}

		void lookatRH(vec3 at, vec3 up)
		{
			vec3 z = at;
			z.normalize();
			vec3 x = up.cross(z);
			x.normalize();
			vec3 y = z.cross(x);
			m[0] = x;
			m[1] = y;
			m[2] = z;
		}

		void lookatLH(vec3 at, vec3 up)
		{
			vec3 z = at;
			z.normalize();
			vec3 x = up.cross(z);
			x.normalize();
			vec3 y = z.cross(x);
			x.neg();  // flip x
			m[0] = x;
			m[1] = y;
			m[2] = z;
		}
	};

#ifndef MIN
#define MIN(a,b) ((a) < (b)) ? (a) : (b)
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b)) ? (a) : (b)
#endif

	float doppler(vec3 aDeltaPos, vec3 aSrcVel, vec3 aDstVel, float aFactor, float aSoundSpeed)
	{
		float deltamag = aDeltaPos.mag();
		if (deltamag == 0)
			return 1.0f;
		float vls = aDeltaPos.dot(aDstVel) / deltamag;
		float vss = aDeltaPos.dot(aSrcVel) / deltamag;
		float maxspeed = aSoundSpeed / aFactor;
		vss = MIN(vss, maxspeed);
		vls = MIN(vls, maxspeed);
		return (aSoundSpeed - aFactor * vls) / (aSoundSpeed - aFactor * vss);
	}

	float attenuateInvDistance(float aDistance, float aMinDistance, float aMaxDistance, float aRolloffFactor)
	{
		float distance = MAX(aDistance, aMinDistance);
		distance = MIN(distance, aMaxDistance);
		return aMinDistance / (aMinDistance + aRolloffFactor * (distance - aMinDistance));
	}

	float attenuateLinearDistance(float aDistance, float aMinDistance, float aMaxDistance, float aRolloffFactor)
	{
		float distance = MAX(aDistance, aMinDistance);
		distance = MIN(distance, aMaxDistance);
		return 1 - aRolloffFactor * (distance - aMinDistance) / (aMaxDistance - aMinDistance);
	}

	float attenuateExponentialDistance(float aDistance, float aMinDistance, float aMaxDistance, float aRolloffFactor)
	{
		float distance = MAX(aDistance, aMinDistance);
		distance = MIN(distance, aMaxDistance);
		return (float)pow(distance / aMinDistance, -aRolloffFactor);
	}

	void Soloud::update3dVoices(unsigned int *aVoiceArray, unsigned int aVoiceCount)
	{
		vec3 speaker[MAX_CHANNELS];

		int i;
		for (i = 0; i < (signed)mChannels; i++)
		{
			speaker[i].mX = m3dSpeakerPosition[3 * i + 0];
			speaker[i].mY = m3dSpeakerPosition[3 * i + 1];
			speaker[i].mZ = m3dSpeakerPosition[3 * i + 2];
			speaker[i].normalize();
		}

		vec3 lpos, lvel, at, up;
		at.mX = m3dAt[0];
		at.mY = m3dAt[1];
		at.mZ = m3dAt[2];
		up.mX = m3dUp[0];
		up.mY = m3dUp[1];
		up.mZ = m3dUp[2];
		lpos.mX = m3dPosition[0];
		lpos.mY = m3dPosition[1];
		lpos.mZ = m3dPosition[2];
		lvel.mX = m3dVelocity[0];
		lvel.mY = m3dVelocity[1];
		lvel.mZ = m3dVelocity[2];
		mat3 m;
		if (mFlags & LEFT_HANDED_3D)
		{
			m.lookatLH(at, up);
		}
		else
		{
			m.lookatRH(at, up);
		}

		for (i = 0; i < (signed)aVoiceCount; i++)
		{
			AudioSourceInstance3dData * v = &m3dData[aVoiceArray[i]];

			float vol = 1;

			// custom collider
			if (v->mCollider)
			{
				vol *= v->mCollider->collide(this, v, v->mColliderData);
			}

			vec3 pos, vel;
			pos.mX = v->m3dPosition[0];
			pos.mY = v->m3dPosition[1];
			pos.mZ = v->m3dPosition[2];

			vel.mX = v->m3dVelocity[0];
			vel.mY = v->m3dVelocity[1];
			vel.mZ = v->m3dVelocity[2];

			if (!(v->mFlags & AudioSourceInstance::LISTENER_RELATIVE))
			{
				pos = pos.sub(lpos);
			}

			float dist = pos.mag();

			// attenuation

			if (v->mAttenuator)
			{
				vol *= v->mAttenuator->attenuate(dist, v->m3dMinDistance, v->m3dMaxDistance, v->m3dAttenuationRolloff);
			}
			else
			{
				switch (v->m3dAttenuationModel)
				{
				case AudioSource::INVERSE_DISTANCE:
					vol *= attenuateInvDistance(dist, v->m3dMinDistance, v->m3dMaxDistance, v->m3dAttenuationRolloff);
					break;
				case AudioSource::LINEAR_DISTANCE:
					vol *= attenuateLinearDistance(dist, v->m3dMinDistance, v->m3dMaxDistance, v->m3dAttenuationRolloff);
					break;
				case AudioSource::EXPONENTIAL_DISTANCE:
					vol *= attenuateExponentialDistance(dist, v->m3dMinDistance, v->m3dMaxDistance, v->m3dAttenuationRolloff);
					break;
				default:
					//case AudioSource::NO_ATTENUATION:
					break;
				}
			}

			// cone

			// (todo) vol *= conev;

			// doppler
			v->mDopplerValue = doppler(pos, vel, lvel, v->m3dDopplerFactor, m3dSoundSpeed);

			// panning
			pos = m.mul(pos);
			pos.normalize();

			// Apply volume to channels based on speaker vectors
			int j;
			for (j = 0; j < MAX_CHANNELS; j++)
			{
				float speakervol = (speaker[j].dot(pos) + 1) / 2;
				if (speaker[j].null())
					speakervol = 1;
				// Different speaker "focus" calculations to try, if the default "bleeds" too much..
				//speakervol = (speakervol * speakervol + speakervol) / 2;
				//speakervol = speakervol * speakervol;
				v->mChannelVolume[j] = vol * speakervol;
			}

			v->m3dVolume = vol;
		}
	}

	void Soloud::update3dAudio()
	{
		unsigned int voicecount = 0;
		unsigned int voices[VOICE_COUNT];

		// Step 1 - find voices that need 3d processing
		lockAudioMutex();
		int i;
		for (i = 0; i < (signed)mHighestVoice; i++)
		{
			if (mVoice[i] && mVoice[i]->mFlags & AudioSourceInstance::PROCESS_3D)
			{
				voices[voicecount] = i;
				voicecount++;
				m3dData[i].mFlags = mVoice[i]->mFlags;
			}
		}
		unlockAudioMutex();

		// Step 2 - do 3d processing

		update3dVoices(voices, voicecount);

		// Step 3 - update SoLoud voices

		lockAudioMutex();
		for (i = 0; i < (int)voicecount; i++)
		{
			AudioSourceInstance3dData * v = &m3dData[voices[i]];
			AudioSourceInstance * vi = mVoice[voices[i]];
			if (vi)
			{
				updateVoiceRelativePlaySpeed(voices[i]);
				updateVoiceVolume(voices[i]);
				int j;
				for (j = 0; j < MAX_CHANNELS; j++)
				{
					vi->mChannelVolume[j] = v->mChannelVolume[j];
				}

				if (vi->mOverallVolume < 0.01f)
				{
					// Inaudible.
					vi->mFlags |= AudioSourceInstance::INAUDIBLE;

					if (vi->mFlags & AudioSourceInstance::INAUDIBLE_KILL)
					{
						stopVoice(voices[i]);
					}
				}
				else
				{
					vi->mFlags &= ~AudioSourceInstance::INAUDIBLE;
				}
			}
		}

		mActiveVoiceDirty = true;
		unlockAudioMutex();
	}


	handle Soloud::play3d(AudioSource &aSound, float aPosX, float aPosY, float aPosZ, float aVelX, float aVelY, float aVelZ, float aVolume, bool aPaused, unsigned int aBus)
	{
		handle h = play(aSound, aVolume, 0, 1, aBus);
		lockAudioMutex();
		int v = getVoiceFromHandle(h);
		if (v < 0) 
		{
			unlockAudioMutex();
			return h;
		}
		m3dData[v].mHandle = h;
		mVoice[v]->mFlags |= AudioSourceInstance::PROCESS_3D;
		set3dSourceParameters(h, aPosX, aPosY, aPosZ, aVelX, aVelY, aVelZ);

		unlockAudioMutex();

		int samples = 0;
		if (aSound.mFlags & AudioSource::DISTANCE_DELAY)
		{
			vec3 pos;
			pos.mX = aPosX;
			pos.mY = aPosY;
			pos.mZ = aPosZ;
			if (!(mVoice[v]->mFlags & AudioSource::LISTENER_RELATIVE))
			{
				pos.mX -= m3dPosition[0];
				pos.mY -= m3dPosition[1];
				pos.mZ -= m3dPosition[2];
			}
			float dist = pos.mag();
			samples += (int)floor((dist / m3dSoundSpeed) * mSamplerate);
		}

		update3dVoices((unsigned int *)&v, 1);
		updateVoiceRelativePlaySpeed(v);
		int j;
		for (j = 0; j < MAX_CHANNELS; j++)
		{
			mVoice[v]->mChannelVolume[j] = m3dData[v].mChannelVolume[j];
		}

		updateVoiceVolume(v);
		if (mVoice[v]->mOverallVolume < 0.01f)
		{
			// Inaudible.
			mVoice[v]->mFlags |= AudioSourceInstance::INAUDIBLE;

			if (mVoice[v]->mFlags & AudioSourceInstance::INAUDIBLE_KILL)
			{
				stopVoice(v);
			}
		}
		else
		{
			mVoice[v]->mFlags &= ~AudioSourceInstance::INAUDIBLE;
		}
		mActiveVoiceDirty = true;

		setDelaySamples(h, samples);
		setPause(h, aPaused);
		return h;		
	}

	handle Soloud::play3dClocked(time aSoundTime, AudioSource &aSound, float aPosX, float aPosY, float aPosZ, float aVelX, float aVelY, float aVelZ, float aVolume, unsigned int aBus)
	{
		handle h = play(aSound, aVolume, 0, 1, aBus);
		lockAudioMutex();
		int v = getVoiceFromHandle(h);
		if (v < 0) 
		{
			unlockAudioMutex();
			return h;
		}
		m3dData[v].mHandle = h;
		mVoice[v]->mFlags |= AudioSourceInstance::PROCESS_3D;
		set3dSourceParameters(h, aPosX, aPosY, aPosZ, aVelX, aVelY, aVelZ);
		time lasttime = mLastClockedTime;
		if (lasttime == 0) 
			mLastClockedTime = aSoundTime;
		vec3 pos;
		pos.mX = aPosX;
		pos.mY = aPosY;
		pos.mZ = aPosZ;
		unlockAudioMutex();
		int samples = 0;
		if (lasttime != 0)
		{
			samples = (int)floor((aSoundTime - lasttime) * mSamplerate);
		}
		if (aSound.mFlags & AudioSource::DISTANCE_DELAY)
		{
			float dist = pos.mag();
			samples += (int)floor((dist / m3dSoundSpeed) * mSamplerate);
		}

		update3dVoices((unsigned int *)&v, 1);
		updateVoiceRelativePlaySpeed(v);
		int j;
		for (j = 0; j < MAX_CHANNELS; j++)
		{
			mVoice[v]->mChannelVolume[j] = m3dData[v].mChannelVolume[j];
		}

		updateVoiceVolume(v);
		if (mVoice[v]->mOverallVolume < 0.01f)
		{
			// Inaudible.
			mVoice[v]->mFlags |= AudioSourceInstance::INAUDIBLE;

			if (mVoice[v]->mFlags & AudioSourceInstance::INAUDIBLE_KILL)
			{
				stopVoice(v);
			}
		}
		else
		{
			mVoice[v]->mFlags &= ~AudioSourceInstance::INAUDIBLE;
		}
		mActiveVoiceDirty = true;

		setDelaySamples(h, samples);
		setPause(h, 0);
		return h;
	}


	
	result Soloud::set3dSoundSpeed(float aSpeed)
	{
		if (aSpeed <= 0)
			return INVALID_PARAMETER;
		m3dSoundSpeed = aSpeed;
		return SO_NO_ERROR;
	}

	
	float Soloud::get3dSoundSpeed()
	{
		return m3dSoundSpeed;
	}

	
	void Soloud::set3dListenerParameters(float aPosX, float aPosY, float aPosZ, float aAtX, float aAtY, float aAtZ, float aUpX, float aUpY, float aUpZ, float aVelocityX, float aVelocityY, float aVelocityZ)
	{
		m3dPosition[0] = aPosX;
		m3dPosition[1] = aPosY;
		m3dPosition[2] = aPosZ;
		m3dAt[0] = aAtX;
		m3dAt[1] = aAtY;
		m3dAt[2] = aAtZ;
		m3dUp[0] = aUpX;
		m3dUp[1] = aUpY;
		m3dUp[2] = aUpZ;
		m3dVelocity[0] = aVelocityX;
		m3dVelocity[1] = aVelocityY;
		m3dVelocity[2] = aVelocityZ;
	}

	
	void Soloud::set3dListenerPosition(float aPosX, float aPosY, float aPosZ)
	{
		m3dPosition[0] = aPosX;
		m3dPosition[1] = aPosY;
		m3dPosition[2] = aPosZ;
	}

	
	void Soloud::set3dListenerAt(float aAtX, float aAtY, float aAtZ)
	{
		m3dAt[0] = aAtX;
		m3dAt[1] = aAtY;
		m3dAt[2] = aAtZ;
	}

	
	void Soloud::set3dListenerUp(float aUpX, float aUpY, float aUpZ)
	{
		m3dUp[0] = aUpX;
		m3dUp[1] = aUpY;
		m3dUp[2] = aUpZ;
	}

	
	void Soloud::set3dListenerVelocity(float aVelocityX, float aVelocityY, float aVelocityZ)
	{
		m3dVelocity[0] = aVelocityX;
		m3dVelocity[1] = aVelocityY;
		m3dVelocity[2] = aVelocityZ;
	}

	
	void Soloud::set3dSourceParameters(handle aVoiceHandle, float aPosX, float aPosY, float aPosZ, float aVelocityX, float aVelocityY, float aVelocityZ)
	{
		FOR_ALL_VOICES_PRE_3D
			m3dData[ch].m3dPosition[0] = aPosX;
			m3dData[ch].m3dPosition[1] = aPosY;
			m3dData[ch].m3dPosition[2] = aPosZ;
			m3dData[ch].m3dVelocity[0] = aVelocityX;
			m3dData[ch].m3dVelocity[1] = aVelocityY;
			m3dData[ch].m3dVelocity[2] = aVelocityZ;
		FOR_ALL_VOICES_POST_3D
	}

	
	void Soloud::set3dSourcePosition(handle aVoiceHandle, float aPosX, float aPosY, float aPosZ)
	{
		FOR_ALL_VOICES_PRE_3D
			m3dData[ch].m3dPosition[0] = aPosX;
			m3dData[ch].m3dPosition[1] = aPosY;
			m3dData[ch].m3dPosition[2] = aPosZ;
		FOR_ALL_VOICES_POST_3D
	}

	
	void Soloud::set3dSourceVelocity(handle aVoiceHandle, float aVelocityX, float aVelocityY, float aVelocityZ)
	{
		FOR_ALL_VOICES_PRE_3D
			m3dData[ch].m3dVelocity[0] = aVelocityX;
			m3dData[ch].m3dVelocity[1] = aVelocityY;
			m3dData[ch].m3dVelocity[2] = aVelocityZ;
		FOR_ALL_VOICES_POST_3D
	}

	
	void Soloud::set3dSourceMinMaxDistance(handle aVoiceHandle, float aMinDistance, float aMaxDistance)
	{
		FOR_ALL_VOICES_PRE_3D
			m3dData[ch].m3dMinDistance = aMinDistance;
			m3dData[ch].m3dMaxDistance = aMaxDistance;
		FOR_ALL_VOICES_POST_3D
	}

	
	void Soloud::set3dSourceAttenuation(handle aVoiceHandle, unsigned int aAttenuationModel, float aAttenuationRolloffFactor)
	{
		FOR_ALL_VOICES_PRE_3D
			m3dData[ch].m3dAttenuationModel = aAttenuationModel;
			m3dData[ch].m3dAttenuationRolloff = aAttenuationRolloffFactor;
		FOR_ALL_VOICES_POST_3D
	}

	
	void Soloud::set3dSourceDopplerFactor(handle aVoiceHandle, float aDopplerFactor)
	{
		FOR_ALL_VOICES_PRE_3D
			m3dData[ch].m3dDopplerFactor = aDopplerFactor;
		FOR_ALL_VOICES_POST_3D
	}
};
//----soloud_core_basicops.cpp-----------------------------------------------------------------------
// Core "basic" operations - play, stop, etc

namespace SoLoud
{
	handle Soloud::play(AudioSource &aSound, float aVolume, float aPan, bool aPaused, unsigned int aBus)
	{
		if (aSound.mFlags & AudioSource::SINGLE_INSTANCE)
		{
			// Only one instance allowed, stop others
			aSound.stop();
		}

		// Creation of an audio instance may take significant amount of time,
		// so let's not do it inside the audio thread mutex.
		aSound.mSoloud = this;
		SoLoud::AudioSourceInstance *instance = aSound.createInstance();

		lockAudioMutex();
		int ch = findFreeVoice();
		if (ch < 0) 
		{
			unlockAudioMutex();
			delete instance;
			return UNKNOWN_ERROR;
		}
		if (!aSound.mAudioSourceID)
		{
			aSound.mAudioSourceID = mAudioSourceID;
			mAudioSourceID++;
		}
		mVoice[ch] = instance;
		mVoice[ch]->mAudioSourceID = aSound.mAudioSourceID;
		mVoice[ch]->mBusHandle = aBus;
		mVoice[ch]->init(aSound, mPlayIndex);
		m3dData[ch].init(aSound);

		mPlayIndex++;

		// 20 bits, skip the last one (top bits full = voice group)
		if (mPlayIndex == 0xfffff) 
		{
			mPlayIndex = 0;
		}

		if (aPaused)
		{
			mVoice[ch]->mFlags |= AudioSourceInstance::PAUSED;
		}

		setVoicePan(ch, aPan);
		if (aVolume < 0)
		{
			setVoiceVolume(ch, aSound.mVolume);
		}
		else
		{
			setVoiceVolume(ch, aVolume);
		}
        int i;
        for (i = 0; i < MAX_CHANNELS; i++)
        {
            mVoice[ch]->mCurrentChannelVolume[i] = mVoice[ch]->mChannelVolume[i] * mVoice[ch]->mOverallVolume;
        }
		setVoiceRelativePlaySpeed(ch, 1);

        for (i = 0; i < FILTERS_PER_STREAM; i++)
		{
			if (aSound.mFilter[i])
			{
				mVoice[ch]->mFilter[i] = aSound.mFilter[i]->createInstance();
			}
		}

		int scratchneeded = SAMPLE_GRANULARITY * mVoice[ch]->mChannels;

		mVoice[ch]->mResampleData[0]->mBuffer = new float[scratchneeded];
		mVoice[ch]->mResampleData[1]->mBuffer = new float[scratchneeded];

		// First buffer will be overwritten anyway; the second may be referenced by resampler
		memset(mVoice[ch]->mResampleData[0]->mBuffer, 0, sizeof(float) * scratchneeded);
		memset(mVoice[ch]->mResampleData[1]->mBuffer, 0, sizeof(float) * scratchneeded);

		unlockAudioMutex();

		int handle = getHandleFromVoice(ch);
		return handle;
	}

	handle Soloud::playClocked(time aSoundTime, AudioSource &aSound, float aVolume, float aPan, unsigned int aBus)
	{
		handle h = play(aSound, aVolume, aPan, 1, aBus);
		lockAudioMutex();
		time lasttime = mLastClockedTime;
		if (lasttime == 0) 
			mLastClockedTime = aSoundTime;
		unlockAudioMutex();
		int samples = 0;
        if (aSoundTime > lasttime)  //if (lasttime != 0)
		{
			samples = (int)floor((aSoundTime - lasttime) * mSamplerate);
		}
		setDelaySamples(h, samples);
		setPause(h, 0);
		return h;
	}

    handle Soloud::playBackground(AudioSource &aSound, float aVolume, bool aPaused, unsigned int aBus)
    {
        handle h = play(aSound, aVolume, 0.0f, aPaused, aBus);
        setPanAbsolute(h, 1.0f, 1.0f);
        return h;
    }

	void Soloud::seek(handle aVoiceHandle, time aSeconds)
	{
		FOR_ALL_VOICES_PRE
			mVoice[ch]->seek(aSeconds, mScratch.mData, mScratchSize);
		FOR_ALL_VOICES_POST
	}


	void Soloud::stop(handle aVoiceHandle)
	{
		FOR_ALL_VOICES_PRE
			stopVoice(ch);
		FOR_ALL_VOICES_POST
	}

	void Soloud::stopAudioSource(AudioSource &aSound)
	{
		if (aSound.mAudioSourceID)
		{
			lockAudioMutex();
			
			int i;
			for (i = 0; i < (signed)mHighestVoice; i++)
			{
				if (mVoice[i] && mVoice[i]->mAudioSourceID == aSound.mAudioSourceID)
				{
					stopVoice(i);
				}
			}
			unlockAudioMutex();
		}
	}

	void Soloud::stopAll()
	{
		int i;
		lockAudioMutex();
		for (i = 0; i < (signed)mHighestVoice; i++)
		{
			stopVoice(i);
		}
		unlockAudioMutex();
	}
}
//----soloud_core_faderops.cpp---------------------------------------------------------------------
// Core operations related to faders (not including filters)

namespace SoLoud
{
	void Soloud::schedulePause(handle aVoiceHandle, time aTime)
	{
		if (aTime <= 0)
		{
			setPause(aVoiceHandle, 1);
			return;
		}
		FOR_ALL_VOICES_PRE
		mVoice[ch]->mPauseScheduler.set(1, 0, aTime, mVoice[ch]->mStreamTime);
		FOR_ALL_VOICES_POST
	}

	void Soloud::scheduleStop(handle aVoiceHandle, time aTime)
	{
		if (aTime <= 0)
		{
			stop(aVoiceHandle);
			return;
		}
		FOR_ALL_VOICES_PRE
		mVoice[ch]->mStopScheduler.set(1, 0, aTime, mVoice[ch]->mStreamTime);
		FOR_ALL_VOICES_POST
	}

	void Soloud::fadeVolume(handle aVoiceHandle, float aTo, time aTime)
	{
		float from = getVolume(aVoiceHandle);
		if (aTime <= 0 || aTo == from)
		{
			setVolume(aVoiceHandle, aTo);
			return;
		}

		FOR_ALL_VOICES_PRE
		mVoice[ch]->mVolumeFader.set(from, aTo, aTime, mVoice[ch]->mStreamTime);
		FOR_ALL_VOICES_POST
	}

	void Soloud::fadePan(handle aVoiceHandle, float aTo, time aTime)
	{
		float from = getPan(aVoiceHandle);
		if (aTime <= 0 || aTo == from)
		{
			setPan(aVoiceHandle, aTo);
			return;
		}

		FOR_ALL_VOICES_PRE
		mVoice[ch]->mPanFader.set(from, aTo, aTime, mVoice[ch]->mStreamTime);
		FOR_ALL_VOICES_POST
	}

	void Soloud::fadeRelativePlaySpeed(handle aVoiceHandle, float aTo, time aTime)
	{
		float from = getRelativePlaySpeed(aVoiceHandle);
		if (aTime <= 0 || aTo == from)
		{
			setRelativePlaySpeed(aVoiceHandle, aTo);
			return;
		}
		FOR_ALL_VOICES_PRE
		mVoice[ch]->mRelativePlaySpeedFader.set(from, aTo, aTime, mVoice[ch]->mStreamTime);
		FOR_ALL_VOICES_POST
	}

	void Soloud::fadeGlobalVolume(float aTo, time aTime)
	{
		float from = getGlobalVolume();
		if (aTime <= 0 || aTo == from)
		{
			setGlobalVolume(aTo);
			return;
		}
		mStreamTime = 0; // avoid rollover (~6 days)
		mGlobalVolumeFader.set(from, aTo, aTime, mStreamTime);
	}


	void Soloud::oscillateVolume(handle aVoiceHandle, float aFrom, float aTo, time aTime)
	{
		if (aTime <= 0 || aTo == aFrom)
		{
			setVolume(aVoiceHandle, aTo);
			return;
		}

		FOR_ALL_VOICES_PRE
		mVoice[ch]->mVolumeFader.setLFO(aFrom, aTo, aTime, mVoice[ch]->mStreamTime);
		FOR_ALL_VOICES_POST
	}

	void Soloud::oscillatePan(handle aVoiceHandle, float aFrom, float aTo, time aTime)
	{
		if (aTime <= 0 || aTo == aFrom)
		{
			setPan(aVoiceHandle, aTo);
			return;
		}

		FOR_ALL_VOICES_PRE
		mVoice[ch]->mPanFader.setLFO(aFrom, aTo, aTime, mVoice[ch]->mStreamTime);
		FOR_ALL_VOICES_POST
	}

	void Soloud::oscillateRelativePlaySpeed(handle aVoiceHandle, float aFrom, float aTo, time aTime)
	{
		if (aTime <= 0 || aTo == aFrom)
		{
			setRelativePlaySpeed(aVoiceHandle, aTo);
			return;
		}
		
		FOR_ALL_VOICES_PRE
		mVoice[ch]->mRelativePlaySpeedFader.setLFO(aFrom, aTo, aTime, mVoice[ch]->mStreamTime);
		FOR_ALL_VOICES_POST
	}

	void Soloud::oscillateGlobalVolume(float aFrom, float aTo, time aTime)
	{
		if (aTime <= 0 || aTo == aFrom)
		{
			setGlobalVolume(aTo);
			return;
		}
		mStreamTime = 0; // avoid rollover (~6 days)
		mGlobalVolumeFader.setLFO(aFrom, aTo, aTime, mStreamTime);
	}
}
//----soloud_core_filterops.cpp-----------------------------------------------------------------------
// Core operations related to filters

namespace SoLoud
{
	void Soloud::setGlobalFilter(unsigned int aFilterId, Filter *aFilter)
	{
		if (aFilterId >= FILTERS_PER_STREAM)
			return;

		lockAudioMutex();
		delete mFilterInstance[aFilterId];
		mFilterInstance[aFilterId] = 0;
		
		mFilter[aFilterId] = aFilter;
		if (aFilter)
		{
			mFilterInstance[aFilterId] = mFilter[aFilterId]->createInstance();
		}
		unlockAudioMutex();
	}

	float Soloud::getFilterParameter(handle aVoiceHandle, unsigned int aFilterId, unsigned int aAttributeId)
	{
		float ret = INVALID_PARAMETER;
		if (aFilterId >= FILTERS_PER_STREAM)
			return ret;

		if (aVoiceHandle == 0)
		{
			lockAudioMutex();		
			if (mFilterInstance[aFilterId])
			{
				ret = mFilterInstance[aFilterId]->getFilterParameter(aAttributeId);
			}
			unlockAudioMutex();
			return ret;
		}

		int ch = getVoiceFromHandle(aVoiceHandle);
		if (ch == -1) 
		{
			return ret;
		}
		lockAudioMutex();		
		if (mVoice[ch] &&
			mVoice[ch]->mFilter[aFilterId])
		{
			ret = mVoice[ch]->mFilter[aFilterId]->getFilterParameter(aAttributeId);
		}
		unlockAudioMutex();
		
		return ret;
	}

	void Soloud::setFilterParameter(handle aVoiceHandle, unsigned int aFilterId, unsigned int aAttributeId, float aValue)
	{
		if (aFilterId >= FILTERS_PER_STREAM)
			return;

		if (aVoiceHandle == 0)
		{
			lockAudioMutex();		
			if (mFilterInstance[aFilterId])
			{
				mFilterInstance[aFilterId]->setFilterParameter(aAttributeId, aValue);
			}
			unlockAudioMutex();
			return;
		}

		FOR_ALL_VOICES_PRE
		if (mVoice[ch] &&
			mVoice[ch]->mFilter[aFilterId])
		{
			mVoice[ch]->mFilter[aFilterId]->setFilterParameter(aAttributeId, aValue);
		}
		FOR_ALL_VOICES_POST
	}

	void Soloud::fadeFilterParameter(handle aVoiceHandle, unsigned int aFilterId, unsigned int aAttributeId, float aTo, double aTime)
	{
		if (aFilterId >= FILTERS_PER_STREAM)
			return;

		if (aVoiceHandle == 0)
		{
			lockAudioMutex();		
			if (mFilterInstance[aFilterId])
			{
				mFilterInstance[aFilterId]->fadeFilterParameter(aAttributeId, aTo, aTime, mStreamTime);
			}
			unlockAudioMutex();
			return;
		}

		FOR_ALL_VOICES_PRE
		if (mVoice[ch] &&
			mVoice[ch]->mFilter[aFilterId])
		{
			mVoice[ch]->mFilter[aFilterId]->fadeFilterParameter(aAttributeId, aTo, aTime, mStreamTime);
		}
		FOR_ALL_VOICES_POST
	}

	void Soloud::oscillateFilterParameter(handle aVoiceHandle, unsigned int aFilterId, unsigned int aAttributeId, float aFrom, float aTo, double aTime)
	{
		if (aFilterId >= FILTERS_PER_STREAM)
			return;

		if (aVoiceHandle == 0)
		{
			lockAudioMutex();		
			if (mFilterInstance[aFilterId])
			{
				mFilterInstance[aFilterId]->oscillateFilterParameter(aAttributeId, aFrom, aTo, aTime, mStreamTime);
			}
			unlockAudioMutex();
			return;
		}

		FOR_ALL_VOICES_PRE
		if (mVoice[ch] &&
			mVoice[ch]->mFilter[aFilterId])
		{
			mVoice[ch]->mFilter[aFilterId]->oscillateFilterParameter(aAttributeId, aFrom, aTo, aTime, mStreamTime);
		}
		FOR_ALL_VOICES_POST
	}

}

//----soloud_core_getters.cpp-----------------------------------------------------------------------
// Getters - return information about SoLoud state

namespace SoLoud
{
	unsigned int Soloud::getVersion() const
	{
		return SOLOUD_VERSION;
	}

	float Soloud::getPostClipScaler() const
	{
		return mPostClipScaler;
	}

	float Soloud::getGlobalVolume() const
	{
		return mGlobalVolume;
	}

	handle Soloud::getHandleFromVoice(unsigned int aVoice) const
	{
		if (mVoice[aVoice] == 0)
			return 0;
		return (aVoice + 1) | (mVoice[aVoice]->mPlayIndex << 12);
	}

	int Soloud::getVoiceFromHandle(handle aVoiceHandle) const
	{
		// If this is a voice group handle, pick the first handle from the group
		handle *h = voiceGroupHandleToArray(aVoiceHandle);
		if (h != NULL) aVoiceHandle = *h;

		if (aVoiceHandle == 0) 
		{
			return -1;
		}

		int ch = (aVoiceHandle & 0xfff) - 1;
		unsigned int idx = aVoiceHandle >> 12;
		if (mVoice[ch] &&
			(mVoice[ch]->mPlayIndex & 0xfffff) == idx)
		{
			return ch;
		}
		return -1;		
	}

	unsigned int Soloud::getMaxActiveVoiceCount() const
	{
		return mMaxActiveVoices;
	}

	unsigned int Soloud::getActiveVoiceCount()
	{
		lockAudioMutex();
		unsigned int c = mActiveVoiceCount;
		unlockAudioMutex();
		return c;
	}

	unsigned int Soloud::getVoiceCount()
	{
		lockAudioMutex();
		int i;
		int c = 0;
		for (i = 0; i < (signed)mHighestVoice; i++)
		{
			if (mVoice[i]) 
			{
				c++;
			}
		}
		unlockAudioMutex();
		return c;
	}

	bool Soloud::isValidVoiceHandle(handle aVoiceHandle)
	{
		// voice groups are not valid voice handles
		if ((aVoiceHandle & 0xfffff000) == 0xfffff000)
			return 0;

		lockAudioMutex();
		if (getVoiceFromHandle(aVoiceHandle) != -1) 
		{
			unlockAudioMutex();
			return 1;
		}
		unlockAudioMutex();
		return 0;
	}


	bool Soloud::getLooping(handle aVoiceHandle)
	{
		lockAudioMutex();
		int ch = getVoiceFromHandle(aVoiceHandle);
		if (ch == -1)
		{
			unlockAudioMutex();
			return 0;
		}
		bool v = (mVoice[ch]->mFlags & AudioSourceInstance::LOOPING) != 0;
		unlockAudioMutex();
		return v;
	}

	float Soloud::getInfo(handle aVoiceHandle, unsigned int mInfoKey)
	{
		lockAudioMutex();
		int ch = getVoiceFromHandle(aVoiceHandle);
		if (ch == -1)
		{
			unlockAudioMutex();
			return 0;
		}
		float v = mVoice[ch]->getInfo(mInfoKey);
		unlockAudioMutex();
		return v;
	}

	float Soloud::getVolume(handle aVoiceHandle)
	{
		lockAudioMutex();
		int ch = getVoiceFromHandle(aVoiceHandle);
		if (ch == -1) 
		{
			unlockAudioMutex();
			return 0;
		}
		float v = mVoice[ch]->mSetVolume;
		unlockAudioMutex();
		return v;
	}

	float Soloud::getOverallVolume(handle aVoiceHandle)
	{
		lockAudioMutex();
		int ch = getVoiceFromHandle(aVoiceHandle);
		if (ch == -1)
		{
			unlockAudioMutex();
			return 0;
		}
		float v = mVoice[ch]->mOverallVolume;
		unlockAudioMutex();
		return v;
	}

	float Soloud::getPan(handle aVoiceHandle)
	{
		lockAudioMutex();
		int ch = getVoiceFromHandle(aVoiceHandle);
		if (ch == -1) 
		{
			unlockAudioMutex();
			return 0;
		}
		float v = mVoice[ch]->mPan;
		unlockAudioMutex();
		return v;
	}

	time Soloud::getStreamTime(handle aVoiceHandle)
	{
		lockAudioMutex();
		int ch = getVoiceFromHandle(aVoiceHandle);
		if (ch == -1) 
		{
			unlockAudioMutex();
			return 0;
		}
		double v = mVoice[ch]->mStreamTime;
		unlockAudioMutex();
		return v;
	}

	float Soloud::getRelativePlaySpeed(handle aVoiceHandle)
	{
		lockAudioMutex();
		int ch = getVoiceFromHandle(aVoiceHandle);
		if (ch == -1) 
		{
			unlockAudioMutex();
			return 1;
		}
		float v = mVoice[ch]->mSetRelativePlaySpeed;
		unlockAudioMutex();
		return v;
	}

	float Soloud::getSamplerate(handle aVoiceHandle)
	{
		lockAudioMutex();
		int ch = getVoiceFromHandle(aVoiceHandle);
		if (ch == -1) 
		{
			unlockAudioMutex();
			return 0;
		}
		float v = mVoice[ch]->mBaseSamplerate;
		unlockAudioMutex();
		return v;
	}

	bool Soloud::getPause(handle aVoiceHandle)
	{
		lockAudioMutex();
		int ch = getVoiceFromHandle(aVoiceHandle);
		if (ch == -1) 
		{
			unlockAudioMutex();
			return 0;
		}
		int v = !!(mVoice[ch]->mFlags & AudioSourceInstance::PAUSED);
		unlockAudioMutex();
		return v != 0;
	}

	bool Soloud::getProtectVoice(handle aVoiceHandle)
	{
		lockAudioMutex();
		int ch = getVoiceFromHandle(aVoiceHandle);
		if (ch == -1) 
		{
			unlockAudioMutex();
			return 0;
		}
		int v = !!(mVoice[ch]->mFlags & AudioSourceInstance::PROTECTED);
		unlockAudioMutex();
		return v != 0;
	}

	int Soloud::findFreeVoice()
	{
		int i;
		unsigned int lowest_play_index_value = 0xffffffff;
		int lowest_play_index = -1;
		
		// (slowly) drag the highest active voice index down
		if (mHighestVoice > 0 && mVoice[mHighestVoice - 1] == NULL)
			mHighestVoice--;
		
		for (i = 0; i < VOICE_COUNT; i++)
		{
			if (mVoice[i] == NULL)
			{
				if (i+1 > (signed)mHighestVoice)
				{
					mHighestVoice = i + 1;
				}
				return i;
			}
			if (((mVoice[i]->mFlags & AudioSourceInstance::PROTECTED) == 0) && 
				mVoice[i]->mPlayIndex < lowest_play_index_value)
			{
				lowest_play_index_value = mVoice[i]->mPlayIndex;
				lowest_play_index = i;
			}
		}
		stopVoice(lowest_play_index);
		return lowest_play_index;
	}

	unsigned int Soloud::getLoopCount(handle aVoiceHandle)
	{
		lockAudioMutex();
		int ch = getVoiceFromHandle(aVoiceHandle);
		if (ch == -1) 
		{
			unlockAudioMutex();
			return 0;
		}
		int v = mVoice[ch]->mLoopCount;
		unlockAudioMutex();
		return v;
	}

	// Returns current backend ID
	unsigned int Soloud::getBackendId()
	{
		return mBackendID;

	}

	// Returns current backend string
	const char * Soloud::getBackendString()
	{
		return mBackendString;
	}

	// Returns current backend channel count (1 mono, 2 stereo, etc)
	unsigned int Soloud::getBackendChannels()
	{
		return mChannels;
	}

	// Returns current backend sample rate
	unsigned int Soloud::getBackendSamplerate()
	{
		return mSamplerate;
	}

	// Returns current backend buffer size
	unsigned int Soloud::getBackendBufferSize()
	{
		return mBufferSize;
	}

}
//----soloud_core_setters.cpp-----------------------------------------------------------------------
// Setters - set various bits of SoLoud state

namespace SoLoud
{
	void Soloud::setPostClipScaler(float aScaler)
	{
		mPostClipScaler = aScaler;
	}

	void Soloud::setGlobalVolume(float aVolume)
	{
		mGlobalVolumeFader.mActive = 0;
		mGlobalVolume = aVolume;
	}		

	result Soloud::setRelativePlaySpeed(handle aVoiceHandle, float aSpeed)
	{
		result retVal = 0;
		FOR_ALL_VOICES_PRE
			mVoice[ch]->mRelativePlaySpeedFader.mActive = 0;
			retVal = setVoiceRelativePlaySpeed(ch, aSpeed);
			FOR_ALL_VOICES_POST
		return retVal;
	}

	void Soloud::setSamplerate(handle aVoiceHandle, float aSamplerate)
	{
		FOR_ALL_VOICES_PRE
			mVoice[ch]->mBaseSamplerate = aSamplerate;
			updateVoiceRelativePlaySpeed(ch);		
		FOR_ALL_VOICES_POST
	}

	void Soloud::setPause(handle aVoiceHandle, bool aPause)
	{
		FOR_ALL_VOICES_PRE
			setVoicePause(ch, aPause);
		FOR_ALL_VOICES_POST
	}

	result Soloud::setMaxActiveVoiceCount(unsigned int aVoiceCount)
	{
		if (aVoiceCount == 0 || aVoiceCount >= VOICE_COUNT)
			return INVALID_PARAMETER;
		lockAudioMutex();
		mMaxActiveVoices = aVoiceCount;
		unlockAudioMutex();
		return SO_NO_ERROR;
	}

	void Soloud::setPauseAll(bool aPause)
	{
		lockAudioMutex();
		int ch;
		for (ch = 0; ch < (signed)mHighestVoice; ch++)
		{
			setVoicePause(ch, aPause);
		}
		unlockAudioMutex();
	}

	void Soloud::setProtectVoice(handle aVoiceHandle, bool aProtect)
	{
		FOR_ALL_VOICES_PRE
			if (aProtect)
			{
				mVoice[ch]->mFlags |= AudioSourceInstance::PROTECTED;
			}
			else
			{
				mVoice[ch]->mFlags &= ~AudioSourceInstance::PROTECTED;
			}
		FOR_ALL_VOICES_POST
	}

	void Soloud::setPan(handle aVoiceHandle, float aPan)
	{		
		FOR_ALL_VOICES_PRE
			setVoicePan(ch, aPan);
		FOR_ALL_VOICES_POST
	}

	void Soloud::setPanAbsolute(handle aVoiceHandle, float aLVolume, float aRVolume, float aLBVolume, float aRBVolume, float aCVolume, float aSVolume)
	{
		FOR_ALL_VOICES_PRE
			mVoice[ch]->mPanFader.mActive = 0;	
			mVoice[ch]->mChannelVolume[0] = aLVolume;			
			mVoice[ch]->mChannelVolume[1] = aRVolume;
			if (mVoice[ch]->mChannels == 4)
			{
				mVoice[ch]->mChannelVolume[2] = aLBVolume;
				mVoice[ch]->mChannelVolume[3] = aRBVolume;
			}
			if (mVoice[ch]->mChannels == 6)
			{
				mVoice[ch]->mChannelVolume[2] = aCVolume;
				mVoice[ch]->mChannelVolume[3] = aSVolume;
				mVoice[ch]->mChannelVolume[4] = aLBVolume;
				mVoice[ch]->mChannelVolume[5] = aRBVolume;
			}
		FOR_ALL_VOICES_POST
	}

	void Soloud::setInaudibleBehavior(handle aVoiceHandle, bool aMustTick, bool aKill)
	{
		FOR_ALL_VOICES_PRE
			mVoice[ch]->mFlags &= ~(AudioSourceInstance::INAUDIBLE_KILL | AudioSourceInstance::INAUDIBLE_TICK);
			if (aMustTick)
			{
				mVoice[ch]->mFlags |= AudioSourceInstance::INAUDIBLE_TICK;
			}
			if (aKill)
			{
				mVoice[ch]->mFlags |= AudioSourceInstance::INAUDIBLE_KILL;
			}
		FOR_ALL_VOICES_POST
	}

	void Soloud::setLooping(handle aVoiceHandle, bool aLooping)
	{
		FOR_ALL_VOICES_PRE
			if (aLooping)
			{
				mVoice[ch]->mFlags |= AudioSourceInstance::LOOPING;
			}
			else
			{
				mVoice[ch]->mFlags &= ~AudioSourceInstance::LOOPING;
			}
		FOR_ALL_VOICES_POST
	}


	void Soloud::setVolume(handle aVoiceHandle, float aVolume)
	{
		FOR_ALL_VOICES_PRE
			mVoice[ch]->mVolumeFader.mActive = 0;
			setVoiceVolume(ch, aVolume);
		FOR_ALL_VOICES_POST
	}

	void Soloud::setDelaySamples(handle aVoiceHandle, unsigned int aSamples)
	{
		FOR_ALL_VOICES_PRE
			mVoice[ch]->mDelaySamples = aSamples;
		FOR_ALL_VOICES_POST
	}

	void Soloud::setVisualizationEnable(bool aEnable)
	{
		if (aEnable)
		{
			mFlags |= ENABLE_VISUALIZATION;
		}
		else
		{
			mFlags &= ~ENABLE_VISUALIZATION;
		}
	}

	result Soloud::setSpeakerPosition(unsigned int aChannel, float aX, float aY, float aZ)
	{
		if (aChannel >= mChannels)
			return INVALID_PARAMETER;
		m3dSpeakerPosition[3 * aChannel + 0] = aX;
		m3dSpeakerPosition[3 * aChannel + 1] = aY;
		m3dSpeakerPosition[3 * aChannel + 2] = aZ;
		return SO_NO_ERROR;
	}

}

//----soloud_core_voicegroup.cpp-----------------------------------------------------------------------
namespace SoLoud
{
	// Create a voice group. Returns 0 if unable (out of voice groups / out of memory)
	handle Soloud::createVoiceGroup()
	{
		lockAudioMutex();

		unsigned int i;
		// Check if there's any deleted voice groups and re-use if found
		for (i = 0; i < mVoiceGroupCount; i++)
		{
			if (mVoiceGroup[i] == NULL)
			{
				mVoiceGroup[i] = new unsigned int[16];
				if (mVoiceGroup[i] == NULL)
				{
					unlockAudioMutex();
					return 0;
				}
				mVoiceGroup[i][0] = 16;
				mVoiceGroup[i][1] = 0;
				unlockAudioMutex();
				return 0xfffff000 | i;
			}		
		}
		if (mVoiceGroupCount == 4096)
		{
			unlockAudioMutex();
			return 0;
		}
		unsigned int oldcount = mVoiceGroupCount;
		if (mVoiceGroupCount == 0)
		{
			mVoiceGroupCount = 4;
		}
		mVoiceGroupCount *= 2;
		unsigned int **vg = new unsigned int * [mVoiceGroupCount];
		if (vg == NULL)
		{
			mVoiceGroupCount = oldcount;
			unlockAudioMutex();
			return 0;
		}
		for (i = 0; i < oldcount; i++)
		{
			vg[i] = mVoiceGroup[i];
		}

		for (; i < mVoiceGroupCount; i++)
		{
			vg[i] = NULL;
		}

		delete[] mVoiceGroup;
		mVoiceGroup = vg;
		i = oldcount;
		mVoiceGroup[i] = new unsigned int[17];
		if (mVoiceGroup[i] == NULL)
		{
			unlockAudioMutex();
			return 0;
		}
		mVoiceGroup[i][0] = 16;
		mVoiceGroup[i][1] = 0;
		unlockAudioMutex();
		return 0xfffff000 | i;
	}

	// Destroy a voice group. 
	result Soloud::destroyVoiceGroup(handle aVoiceGroupHandle)
	{
		if (!isVoiceGroup(aVoiceGroupHandle))
			return INVALID_PARAMETER;
		int c = aVoiceGroupHandle & 0xfff;
		
		lockAudioMutex();
		delete[] mVoiceGroup[c];
		mVoiceGroup[c] = NULL;
		unlockAudioMutex();
		return SO_NO_ERROR;
	}

	// Add a voice handle to a voice group
	result Soloud::addVoiceToGroup(handle aVoiceGroupHandle, handle aVoiceHandle)
	{
		if (!isVoiceGroup(aVoiceGroupHandle))
			return INVALID_PARAMETER;
		
		// Don't consider adding invalid voice handles as an error, since the voice may just have ended.
		if (!isValidVoiceHandle(aVoiceHandle))
			return SO_NO_ERROR;

		trimVoiceGroup(aVoiceGroupHandle);
		
		int c = aVoiceGroupHandle & 0xfff;
		unsigned int i;

		lockAudioMutex();

		for (i = 1; i < mVoiceGroup[c][0]; i++)
		{
			if (mVoiceGroup[c][i] == aVoiceHandle)
			{
				unlockAudioMutex();
				return SO_NO_ERROR; // already there
			}

			if (mVoiceGroup[c][i] == 0)
			{
				mVoiceGroup[c][i] = aVoiceHandle;
				mVoiceGroup[c][i + 1] = 0;
				
				unlockAudioMutex();
				return SO_NO_ERROR;
			}
		}
		
		// Full group, allocate more memory
		unsigned int * n = new unsigned int[mVoiceGroup[c][0] * 2 + 1];
		if (n == NULL)
		{
			unlockAudioMutex();
			return OUT_OF_MEMORY;
		}
		for (i = 0; i < mVoiceGroup[c][0]; i++)
			n[i] = mVoiceGroup[c][i];
		n[n[0]] = aVoiceHandle;
		n[n[0]+1] = 0;
		n[0] *= 2;
		delete[] mVoiceGroup[c];
		mVoiceGroup[c] = n;
		unlockAudioMutex();
		return SO_NO_ERROR;
	}

	// Is this handle a valid voice group?
	bool Soloud::isVoiceGroup(handle aVoiceGroupHandle)
	{
		if ((aVoiceGroupHandle & 0xfffff000) != 0xfffff000)
			return 0;
		unsigned int c = aVoiceGroupHandle & 0xfff;
		if (c >= mVoiceGroupCount)
			return 0;

		lockAudioMutex();		
		bool res = mVoiceGroup[c] != NULL;		
		unlockAudioMutex();

		return res;
	}

	// Is this voice group empty?
	bool Soloud::isVoiceGroupEmpty(handle aVoiceGroupHandle)
	{
		// If not a voice group, yeah, we're empty alright..
		if (!isVoiceGroup(aVoiceGroupHandle))
			return 1;
		trimVoiceGroup(aVoiceGroupHandle);
		int c = aVoiceGroupHandle & 0xfff;

		lockAudioMutex();
		bool res = mVoiceGroup[c][1] == 0;
		unlockAudioMutex();

		return res;
	}

	// Remove all non-active voices from group
	void Soloud::trimVoiceGroup(handle aVoiceGroupHandle)
	{
		if (!isVoiceGroup(aVoiceGroupHandle))
			return;
		int c = aVoiceGroupHandle & 0xfff;

		lockAudioMutex();
		// empty group
		if (mVoiceGroup[c][1] == 0)
		{
			unlockAudioMutex();
			return;
		}

		unsigned int i;
		for (i = 1; i < mVoiceGroup[c][0]; i++)
		{
			if (mVoiceGroup[c][i] == 0)
			{
				unlockAudioMutex();
				return;
			}
			
			unlockAudioMutex();
			while (!isValidVoiceHandle(mVoiceGroup[c][i])) // function locks mutex, so we need to unlock it before the call
			{
				lockAudioMutex();
				unsigned int j;
				for (j = i; j < mVoiceGroup[c][0] - 1; j++)
				{
					mVoiceGroup[c][j] = mVoiceGroup[c][j + 1];
					if (mVoiceGroup[c][j] == 0)
						break;
				}
				mVoiceGroup[c][mVoiceGroup[c][0] - 1] = 0;				
				if (mVoiceGroup[c][i] == 0)
				{
					unlockAudioMutex();
					return;
				}
			}
		}
		unlockAudioMutex();
	}

	handle *Soloud::voiceGroupHandleToArray(handle aVoiceGroupHandle) const
	{
		if ((aVoiceGroupHandle & 0xfffff000) != 0xfffff000)
			return NULL;
		unsigned int c = aVoiceGroupHandle & 0xfff;
		if (c >= mVoiceGroupCount)
			return NULL;
		if (mVoiceGroup[c] == NULL)
			return NULL;
		return mVoiceGroup[c] + 1;
	}

}

//----soloud_core_voiceops.cpp-----------------------------------------------------------------------
// Direct voice operations (no mutexes - called from other functions)
namespace SoLoud
{
	result Soloud::setVoiceRelativePlaySpeed(unsigned int aVoice, float aSpeed)
	{
		if (aSpeed <= 0.0f)
		{
			return INVALID_PARAMETER;
		}

		if (mVoice[aVoice])
		{
			mVoice[aVoice]->mSetRelativePlaySpeed = aSpeed;
			updateVoiceRelativePlaySpeed(aVoice);
		}

		return 0;
	}

	void Soloud::setVoicePause(unsigned int aVoice, int aPause)
	{
		mActiveVoiceDirty = true;
		if (mVoice[aVoice])
		{
			mVoice[aVoice]->mPauseScheduler.mActive = 0;

			if (aPause)
			{
				mVoice[aVoice]->mFlags |= AudioSourceInstance::PAUSED;
			}
			else
			{
				mVoice[aVoice]->mFlags &= ~AudioSourceInstance::PAUSED;
			}
		}
	}

	void Soloud::setVoicePan(unsigned int aVoice, float aPan)
	{
		if (mVoice[aVoice])
		{
			mVoice[aVoice]->mPan = aPan;
			float l = (float)cos((aPan + 1) * M_PI / 4);
			float r = (float)sin((aPan + 1) * M_PI / 4);
			mVoice[aVoice]->mChannelVolume[0] = l;
			mVoice[aVoice]->mChannelVolume[1] = r;
			if (mVoice[aVoice]->mChannels == 4)
			{
				mVoice[aVoice]->mChannelVolume[2] = l;
				mVoice[aVoice]->mChannelVolume[3] = r;
			}
			if (mVoice[aVoice]->mChannels == 6)
			{
				mVoice[aVoice]->mChannelVolume[2] = 1.0f / (float)sqrt(2.0f);
				mVoice[aVoice]->mChannelVolume[3] = 1;
				mVoice[aVoice]->mChannelVolume[4] = l;
				mVoice[aVoice]->mChannelVolume[5] = r;
			}
		}
	}

	void Soloud::setVoiceVolume(unsigned int aVoice, float aVolume)
	{
		mActiveVoiceDirty = true;
		if (mVoice[aVoice])
		{
			mVoice[aVoice]->mSetVolume = aVolume;
			updateVoiceVolume(aVoice);
		}
	}

	void Soloud::stopVoice(unsigned int aVoice)
	{
		mActiveVoiceDirty = true;
		if (mVoice[aVoice])
		{
			// Delete via temporary variable to avoid recursion
			AudioSourceInstance * v = mVoice[aVoice];
			mVoice[aVoice] = 0;			
			delete v;
		}
	}

	void Soloud::updateVoiceRelativePlaySpeed(unsigned int aVoice)
	{
		mVoice[aVoice]->mOverallRelativePlaySpeed = m3dData[aVoice].mDopplerValue * mVoice[aVoice]->mSetRelativePlaySpeed;
		mVoice[aVoice]->mSamplerate = mVoice[aVoice]->mBaseSamplerate * mVoice[aVoice]->mOverallRelativePlaySpeed;
	}

	void Soloud::updateVoiceVolume(unsigned int aVoice)
	{
		mVoice[aVoice]->mOverallVolume = mVoice[aVoice]->mSetVolume * m3dData[aVoice].m3dVolume;
	}
}
//----soloud_fader.cpp-----------------------------------------------------------------------
namespace SoLoud
{
	Fader::Fader()
	{
		mCurrent = mFrom = mTo = mDelta = 0;
		mTime = mStartTime = mEndTime = 0;
		mActive = 0;
	}

	void Fader::set(float aFrom, float aTo, double aTime, double aStartTime)
	{
		mCurrent = mFrom;
		mFrom = aFrom;
		mTo = aTo;
		mTime = aTime;
		mStartTime = aStartTime;
		mDelta = aTo - aFrom;
		mEndTime = mStartTime + mTime;
		mActive = 1;
	}

	void Fader::setLFO(float aFrom, float aTo, double aTime, double aStartTime)
	{
		mActive = 2;
		mCurrent = 0;
		mFrom = aFrom;
		mTo = aTo;
		mTime = aTime;
		mDelta = (aTo - aFrom) / 2;
		if (mDelta < 0) mDelta = -mDelta;
		mStartTime = aStartTime;
		mEndTime = (float)M_PI * 2 / mTime;
	}

	float Fader::get(double aCurrentTime)
	{
		if (mActive == 2)
		{
			// LFO mode
			if (mStartTime > aCurrentTime)
			{
				// Time rolled over.
				mStartTime = aCurrentTime;
			}
			double t = aCurrentTime - mStartTime;
			return (float)(sin(t * mEndTime) * mDelta + (mFrom + mDelta));
			
		}
		if (mStartTime > aCurrentTime)
		{
			// Time rolled over.
			// Figure out where we were..
			float p = (mCurrent - mFrom) / mDelta; // 0..1
			mFrom = mCurrent;
			mStartTime = aCurrentTime;
			mTime = mTime * (1 - p); // time left
			mDelta = mTo - mFrom;
			mEndTime = mStartTime + mTime;
		}
		if (aCurrentTime > mEndTime)
		{
			mActive = -1;
			return mTo;
		}
		mCurrent = (float)(mFrom + mDelta * ((aCurrentTime - mStartTime) / mTime));
		return mCurrent;
	}
};
//----soloud_file.cpp-----------------------------------------------------------------------
#include <stdio.h>
#include <string.h>

namespace SoLoud
{
	unsigned int File::read8()
	{
		unsigned char d = 0;
		read((unsigned char*)&d, 1);
		return d;
	}

	unsigned int File::read16()
	{
		unsigned short d = 0;
		read((unsigned char*)&d, 2);
		return d;
	}

	unsigned int File::read32()
	{
		unsigned int d = 0;
		read((unsigned char*)&d, 4);
		return d;
	}

DiskFile::DiskFile(FILE *fp):
mFileHandle(fp)
{

}

	unsigned int DiskFile::read(unsigned char *aDst, unsigned int aBytes)
	{
		return (unsigned int)fread(aDst, 1, aBytes, mFileHandle);
	}

	unsigned int DiskFile::length()
	{
		int pos = ftell(mFileHandle);
		fseek(mFileHandle, 0, SEEK_END);
		int len = ftell(mFileHandle);
		fseek(mFileHandle, pos, SEEK_SET);
		return len;
	}

	void DiskFile::seek(int aOffset)
	{
		fseek(mFileHandle, aOffset, SEEK_SET);
	}

	unsigned int DiskFile::pos()
	{
		return ftell(mFileHandle);
	}

	FILE *DiskFile::getFilePtr()
	{
		return mFileHandle;
	}

	DiskFile::~DiskFile()
	{
		if (mFileHandle)
			fclose(mFileHandle);
	}

	DiskFile::DiskFile()
	{
		mFileHandle = 0;
	}

	result DiskFile::open(const char *aFilename)
	{
		if (!aFilename)
			return INVALID_PARAMETER;
        mFileHandle = (FILE*)ImFileOpen(aFilename, "rb");
		if (!mFileHandle)
			return FILE_NOT_FOUND;
		return SO_NO_ERROR;
	}

	int DiskFile::eof()
	{
		return feof(mFileHandle);
	}



	unsigned int MemoryFile::read(unsigned char *aDst, unsigned int aBytes)
	{
		if (mOffset + aBytes >= mDataLength)
			aBytes = mDataLength - mOffset;

		memcpy(aDst, mDataPtr + mOffset, aBytes);
		mOffset += aBytes;

		return aBytes;
	}

	unsigned int MemoryFile::length()
	{
		return mDataLength;
	}

	void MemoryFile::seek(int aOffset)
	{
		if (aOffset >= 0)
			mOffset = aOffset;
		else
			mOffset = mDataLength + aOffset;
		if (mOffset > mDataLength-1)
			mOffset = mDataLength-1;
	}

	unsigned int MemoryFile::pos()
	{
		return mOffset;
	}

	unsigned char * MemoryFile::getMemPtr()
	{
		return mDataPtr;
	}

	MemoryFile::~MemoryFile()
	{
		if (mDataOwned)
			delete[] mDataPtr;
	}

	MemoryFile::MemoryFile()
	{
		mDataPtr = 0;
		mDataLength = 0;
		mOffset = 0;
		mDataOwned = false;
	}

	result MemoryFile::openMem(unsigned char *aData, unsigned int aDataLength, bool aCopy, bool aTakeOwnership)
	{
		if (aData == NULL || aDataLength == 0)
			return INVALID_PARAMETER;

		if (mDataOwned)
			delete[] mDataPtr;
		mDataPtr = 0;
		mOffset = 0;

		mDataLength = aDataLength;

		if (aCopy)
		{
			mDataOwned = true;
			mDataPtr = new unsigned char[aDataLength];
			if (mDataPtr == NULL)
				return OUT_OF_MEMORY;
			memcpy(mDataPtr, aData, aDataLength);
			return SO_NO_ERROR;
		}

		mDataPtr = aData;
		mDataOwned = aTakeOwnership;
		return SO_NO_ERROR;
	}

	result MemoryFile::openToMem(const char *aFile)
	{
		if (!aFile)
			return INVALID_PARAMETER;
		if (mDataOwned)
			delete[] mDataPtr;
		mDataPtr = 0;
		mOffset = 0;

		DiskFile df;
		int res = df.open(aFile);
		if (res != SO_NO_ERROR)
			return res;

		mDataLength = df.length();
		mDataPtr = new unsigned char[mDataLength];
		if (mDataPtr == NULL)
			return OUT_OF_MEMORY;
		df.read(mDataPtr, mDataLength);
		mDataOwned = true;
		return SO_NO_ERROR;
	}

	result MemoryFile::openFileToMem(File *aFile)
	{
		if (mDataOwned)
			delete[] mDataPtr;
		mDataPtr = 0;
		mOffset = 0;

		mDataLength = aFile->length();
		mDataPtr = new unsigned char[mDataLength];
		if (mDataPtr == NULL)
			return OUT_OF_MEMORY;
		aFile->read(mDataPtr, mDataLength);
		mDataOwned = true;
		return SO_NO_ERROR;
	}

	int MemoryFile::eof()
	{
		if (mOffset >= mDataLength)
			return 1;
		return 0;
	}
}

extern "C"
{
	int Soloud_Filehack_fgetc(Soloud_Filehack *f)
	{
		SoLoud::File *fp = (SoLoud::File *)f;
		if (fp->eof())
			return EOF;
		return fp->read8();
	}

	int Soloud_Filehack_fread(void *dst, int s, int c, Soloud_Filehack *f)
	{
		SoLoud::File *fp = (SoLoud::File *)f;
		return fp->read((unsigned char*)dst, s*c) / s;

	}

	int Soloud_Filehack_fseek(Soloud_Filehack *f, int idx, int base)
	{
		SoLoud::File *fp = (SoLoud::File *)f;
		switch (base)
		{
		case SEEK_CUR:
			fp->seek(fp->pos() + idx);
			break;
		case SEEK_END:
			fp->seek(fp->length() + idx);
			break;
		default:
			fp->seek(idx);
		}
		return 0;
	}

	int Soloud_Filehack_ftell(Soloud_Filehack *f)
	{
		SoLoud::File *fp = (SoLoud::File *)f;
		return fp->pos();
	}

	int Soloud_Filehack_fclose(Soloud_Filehack *f)
	{
		SoLoud::File *fp = (SoLoud::File *)f;
		delete fp;
		return 0;
	}

	Soloud_Filehack * Soloud_Filehack_fopen(const char *aFilename, char *aMode)
	{
		SoLoud::DiskFile *df = new SoLoud::DiskFile();
		int res = df->open(aFilename);
		if (res != SoLoud::SO_NO_ERROR)
		{
			delete df;
			df = 0;
		}
		return (Soloud_Filehack*)df;
	}
}
//----soloud_filter.cpp-----------------------------------------------------------------------
namespace SoLoud
{

	Filter::Filter()
	{
	}

	Filter::~Filter()
	{
	}
	
	FilterInstance::FilterInstance()
	{
		mNumParams = 0;
		mParamChanged = 0;
		mParam = 0;
		mParamFader = 0;
	}

	result FilterInstance::initParams(int aNumParams)
	{		
		mNumParams = aNumParams;
		delete[] mParam;
		delete[] mParamFader;
		mParam = new float[mNumParams];
		mParamFader = new Fader[mNumParams];

		if (mParam == NULL || mParamFader == NULL)
		{
			delete[] mParam;
			delete[] mParamFader;
			mParam = NULL;
			mParamFader = NULL;
			mNumParams = 0;
			return OUT_OF_MEMORY;
		}

		unsigned int i;
		for (i = 0; i < mNumParams; i++)
		{
			mParam[i] = 0;
			mParamFader[i].mActive = 0;
		}
		mParam[0] = 1; // set 'wet' to 1

		return 0;
	}

	void FilterInstance::updateParams(double aTime)
	{
		unsigned int i;
		for (i = 0; i < mNumParams; i++)
		{
			if (mParamFader[i].mActive > 0)
			{
				mParamChanged |= 1 << i;
				mParam[i] = mParamFader[i].get(aTime);
			}
		}
	}

	FilterInstance::~FilterInstance()
	{
		delete[] mParam;
		delete[] mParamFader;
	}

	void FilterInstance::setFilterParameter(unsigned int aAttributeId, float aValue)
	{
		if (aAttributeId >= mNumParams)
			return;

		mParamFader[aAttributeId].mActive = 0;
		mParam[aAttributeId] = aValue;
		mParamChanged |= 1 << aAttributeId;
	}

	void FilterInstance::fadeFilterParameter(unsigned int aAttributeId, float aTo, double aTime, double aStartTime)
	{
		if (aAttributeId >= mNumParams || aTime <= 0 || aTo == mParam[aAttributeId])
			return;

		mParamFader[aAttributeId].set(mParam[aAttributeId], aTo, aTime, aStartTime);
	}

	void FilterInstance::oscillateFilterParameter(unsigned int aAttributeId, float aFrom, float aTo, double aTime, double aStartTime)
	{
		if (aAttributeId >= mNumParams || aTime <= 0 || aFrom == aTo)
			return;

		mParamFader[aAttributeId].setLFO(aFrom, aTo, aTime, aStartTime);
	}

	float FilterInstance::getFilterParameter(unsigned int aAttributeId)
	{
		if (aAttributeId >= mNumParams)
			return 0;

		return mParam[aAttributeId];
	}

	void FilterInstance::filter(float *aBuffer, unsigned int aSamples, unsigned int aChannels, float aSamplerate, double aTime)
	{
		unsigned int i;
		for (i = 0; i < aChannels; i++)
		{
			filterChannel(aBuffer + i * aSamples, aSamples, aSamplerate, aTime, i, aChannels);
		}
	}

	void FilterInstance::filterChannel(float *aBuffer, unsigned int aSamples, float aSamplerate, double aTime, unsigned int aChannel, unsigned int aChannels)
	{
	}

};

//----soloud_bassboostfilter.cpp-----------------------------------------------------------------------
namespace SoLoud
{
	BassboostFilterInstance::BassboostFilterInstance(BassboostFilter *aParent)
	{
		mParent = aParent;
		initParams(2);
		mParam[BOOST] = aParent->mBoost;
	}

	void BassboostFilterInstance::fftFilterChannel(float *aFFTBuffer, unsigned int aSamples, float aSamplerate, time aTime, unsigned int aChannel, unsigned int aChannels)
	{
		unsigned int i;
		for (i = 0; i < 2; i++)
		{
			aFFTBuffer[i] *= mParam[BOOST];
		}
	}

	result BassboostFilter::setParams(float aBoost)
	{
		if (aBoost < 0)
			return INVALID_PARAMETER;
		mBoost = aBoost;
		return SO_NO_ERROR;
	}

	BassboostFilter::BassboostFilter()
	{
		mBoost = 2;
	}

	FilterInstance *BassboostFilter::createInstance()
	{
		return new BassboostFilterInstance(this);
	}
}
//----soloud_biquadresonantfilter.cpp-----------------------------------------------------------------------
namespace SoLoud
{
	void BiquadResonantFilterInstance::calcBQRParams()
	{
		mDirty = 0;

		float omega = (float)((2.0f * M_PI * mParam[FREQUENCY]) / mParam[SAMPLERATE]);
		float sin_omega = (float)sin(omega);
		float cos_omega = (float)cos(omega);
		float alpha = sin_omega / (2.0f * mParam[RESONANCE]);
		float scalar = 1.0f / (1.0f + alpha);

		mActive = 1;

		switch (mFilterType)
		{
		case BiquadResonantFilter::NONE:
			mActive = 0;
			break;
		case BiquadResonantFilter::LOWPASS:
			mA0 = 0.5f * (1.0f - cos_omega) * scalar;
			mA1 = (1.0f - cos_omega) * scalar;
			mA2 = mA0;
			mB1 = -2.0f * cos_omega * scalar;
			mB2 = (1.0f - alpha) * scalar;
			break;
		case BiquadResonantFilter::HIGHPASS:
			mA0 = 0.5f * (1.0f + cos_omega) * scalar;
			mA1 = -(1.0f + cos_omega) * scalar;
			mA2 = mA0;
			mB1 = -2.0f * cos_omega * scalar;
			mB2 = (1.0f - alpha) * scalar;
			break;
		case BiquadResonantFilter::BANDPASS:
			mA0 = alpha * scalar;
			mA1 = 0;
			mA2 = -mA0;
			mB1 = -2.0f * cos_omega * scalar;
			mB2 = (1.0f - alpha) * scalar;
			break;
		}
	}


	BiquadResonantFilterInstance::BiquadResonantFilterInstance(BiquadResonantFilter *aParent)
	{
		int i;
		for (i = 0; i < 2; i++)
		{
			mState[i].mX1 = 0;
			mState[i].mY1 = 0;
			mState[i].mX2 = 0;
			mState[i].mY2 = 0;
		}

		mParent = aParent;
		mFilterType = aParent->mFilterType;

		initParams(4);
		
		mParam[SAMPLERATE] = aParent->mSampleRate;
		mParam[RESONANCE] = aParent->mResonance;
		mParam[FREQUENCY] = aParent->mFrequency;
		mParam[WET] = 1;

		calcBQRParams();
	}

	void BiquadResonantFilterInstance::filterChannel(float *aBuffer, unsigned int aSamples, float aSamplerate, double aTime, unsigned int aChannel, unsigned int aChannels)
	{
		if (!mActive)
			return;

		if (aChannel == 0)
		{
			updateParams(aTime);

			if (mParamChanged & ((1 << FREQUENCY) | (1 << RESONANCE) | (1 << SAMPLERATE)))
			{
				calcBQRParams();
			}
			mParamChanged = 0;
		}

		float x;
		unsigned int i;
		int c = 0;

		BQRStateData &s = mState[aChannel];

		for (i = 0; i < aSamples; i +=2, c++)
		{
			// Generate outputs by filtering inputs.
			x = aBuffer[c];
			s.mY2 = (mA0 * x) + (mA1 * s.mX1) + (mA2 * s.mX2) - (mB1 * s.mY1) - (mB2 * s.mY2);
			aBuffer[c] += (s.mY2 - aBuffer[c]) * mParam[WET];

			c++;

			// Permute filter operations to reduce data movement.
			// Just substitute variables instead of doing mX1=x, etc.
			s.mX2 = aBuffer[c];
			s.mY1 = (mA0 * s.mX2) + (mA1 * x) + (mA2 * s.mX1) - (mB1 * s.mY2) - (mB2 * s.mY1);
			aBuffer[c] += (s.mY1 - aBuffer[c]) * mParam[WET];

			// Only move a little data.
			s.mX1 = s.mX2;
			s.mX2 = x;
		}

		// Apply a small impulse to filter to prevent arithmetic underflow,
		// which can cause the FPU to interrupt the CPU.
		s.mY1 += (float) 1.0E-26;		
	}


	BiquadResonantFilterInstance::~BiquadResonantFilterInstance()
	{
	}

	BiquadResonantFilter::BiquadResonantFilter()
	{
		setParams(LOWPASS, 44100, 1000, 2);
	}

	result BiquadResonantFilter::setParams(int aType, float aSampleRate, float aFrequency, float aResonance)
	{
		if (aType < 0 || aType > 3 || aSampleRate <= 0 || aFrequency <= 0 || aResonance <= 0)
			return INVALID_PARAMETER;

		mFilterType = aType;
		mSampleRate = aSampleRate;
		mFrequency = aFrequency;
		mResonance = aResonance;

		return 0;
	}

	BiquadResonantFilter::~BiquadResonantFilter()
	{
	}


	BiquadResonantFilterInstance *BiquadResonantFilter::createInstance()
	{
		return new BiquadResonantFilterInstance(this);
	}
}
//----soloud_dcremovalfilter.cpp-----------------------------------------------------------------------
namespace SoLoud
{
	DCRemovalFilterInstance::DCRemovalFilterInstance(DCRemovalFilter *aParent)
	{
		mParent = aParent;
		mBuffer = 0;
		mBufferLength = 0;
		mTotals = 0;
		mOffset = 0;
		initParams(1);

	}

	void DCRemovalFilterInstance::filter(float *aBuffer, unsigned int aSamples, unsigned int aChannels, float aSamplerate, double aTime)
	{
		updateParams(aTime);

		if (mBuffer == 0)
		{
			mBufferLength = (int)ceil(mParent->mLength * aSamplerate);
			mBuffer = new float[mBufferLength * aChannels];
			mTotals = new float[aChannels];
			unsigned int i;
			for (i = 0; i < aChannels; i++)
			{
			    mTotals[i] = 0;
			}
			for (i = 0; i < mBufferLength * aChannels; i++)
			{
				mBuffer[i] = 0;
			}
		}

		unsigned int i, j;
		int prevofs = (mOffset + mBufferLength - 1) % mBufferLength;
		for (i = 0; i < aSamples; i++)
		{
			for (j = 0; j < aChannels; j++)
			{
				int chofs = j * mBufferLength;
				int bchofs = j * aSamples;
								
				float n = aBuffer[i + bchofs];
				mTotals[j] -= mBuffer[mOffset + chofs];
				mTotals[j] += n;
				mBuffer[mOffset + chofs] = n;
			    
			    n -= mTotals[j] / mBufferLength;
			    
				aBuffer[i + bchofs] += (n - aBuffer[i + bchofs]) * mParam[0];
			}
			prevofs = mOffset;
			mOffset = (mOffset + 1) % mBufferLength;
		}
	}

	DCRemovalFilterInstance::~DCRemovalFilterInstance()
	{
		delete[] mBuffer;
		delete[] mTotals;
	}

	DCRemovalFilter::DCRemovalFilter()
	{
		mLength = 0.1f;
	}

	result DCRemovalFilter::setParams(float aLength)
	{
		if (aLength <= 0)
			return INVALID_PARAMETER;

        mLength = aLength;
		
		return 0;
	}


	FilterInstance *DCRemovalFilter::createInstance()
	{
		return new DCRemovalFilterInstance(this);
	}
}
//----soloud_echofilter.cpp-----------------------------------------------------------------------
namespace SoLoud
{
	EchoFilterInstance::EchoFilterInstance(EchoFilter *aParent)
	{
		mParent = aParent;
		mBuffer = 0;
		mBufferLength = 0;
		mOffset = 0;
		initParams(1);

	}

	void EchoFilterInstance::filter(float *aBuffer, unsigned int aSamples, unsigned int aChannels, float aSamplerate, double aTime)
	{
		updateParams(aTime);

		if (mBuffer == 0)
		{
			mBufferLength = (int)ceil(mParent->mDelay * aSamplerate);
			mBuffer = new float[mBufferLength * aChannels];
			unsigned int i;
			for (i = 0; i < mBufferLength * aChannels; i++)
			{
				mBuffer[i] = 0;
			}
		}

		float decay = mParent->mDecay;
		unsigned int i, j;
		int prevofs = (mOffset + mBufferLength - 1) % mBufferLength;
		for (i = 0; i < aSamples; i++)
		{
			for (j = 0; j < aChannels; j++)
			{
				int chofs = j * mBufferLength;
				int bchofs = j * aSamples;
				
				mBuffer[mOffset + chofs] = mParent->mFilter * mBuffer[prevofs + chofs] + (1 - mParent->mFilter) * mBuffer[mOffset + chofs];
				
				float n = aBuffer[i + bchofs] + mBuffer[mOffset + chofs] * decay;
				mBuffer[mOffset + chofs] = n;

				aBuffer[i + bchofs] += (n - aBuffer[i + bchofs]) * mParam[0];
			}
			prevofs = mOffset;
			mOffset = (mOffset + 1) % mBufferLength;
		}
	}

	EchoFilterInstance::~EchoFilterInstance()
	{
		delete[] mBuffer;
	}

	EchoFilter::EchoFilter()
	{
		mDelay = 0.3f;
		mDecay = 0.7f;
		mFilter = 0.0f;
	}

	result EchoFilter::setParams(float aDelay, float aDecay, float aFilter)
	{
		if (aDelay <= 0 || aDecay <= 0 || aFilter < 0 || aFilter >= 1.0f)
			return INVALID_PARAMETER;

		mDecay = aDecay;
		mDelay = aDelay;
		mFilter = aFilter;
		
		return 0;
	}


	FilterInstance *EchoFilter::createInstance()
	{
		return new EchoFilterInstance(this);
	}
}
//----soloud_fftfilter.cpp-----------------------------------------------------------------------
namespace SoLoud
{
	FFTFilterInstance::FFTFilterInstance()
	{
		mParent = 0;
		mInputBuffer = 0;
		mMixBuffer = 0;
		mTemp = 0;
		int i;
		for (i = 0; i < MAX_CHANNELS; i++)
			mOffset[i] = 0;
	}

	FFTFilterInstance::FFTFilterInstance(FFTFilter *aParent)
	{
		mParent = aParent;
		mInputBuffer = 0;
		mMixBuffer = 0;
		mTemp = 0;
		int i;
		for (i = 0; i < MAX_CHANNELS; i++)
			mOffset[i] = 0;
		initParams(1);
	}

	void FFTFilterInstance::filterChannel(float *aBuffer, unsigned int aSamples, float aSamplerate, double aTime, unsigned int aChannel, unsigned int aChannels)
	{
		if (aChannel == 0)
		{
			updateParams(aTime);
		}

		if (mInputBuffer == 0)
		{
			mInputBuffer = new float[512 * aChannels];
			mMixBuffer = new float[512 * aChannels];
			mTemp = new float[256];
			memset(mInputBuffer, 0x2f, sizeof(float) * 512 * aChannels);
			memset(mMixBuffer, 0, sizeof(float) * 512 * aChannels);
		}

		float * b = mTemp;

		int i;
		unsigned int ofs = 0;
		unsigned int chofs = 512 * aChannel;
		unsigned int bofs = mOffset[aChannel];
		
		while (ofs < aSamples)
		{
			for (i = 0; i < 128; i++)
			{
				mInputBuffer[chofs + ((bofs + i + 128) & 511)] = aBuffer[ofs + i];
				mMixBuffer[chofs + ((bofs + i + 128) & 511)] = 0;
			}
			
			for (i = 0; i < 256; i++)
			{
				b[i] = mInputBuffer[chofs + ((bofs + i) & 511)];
			}
			FFT::fft256(b);

			// do magic
			fftFilterChannel(b, 128, aSamplerate, aTime, aChannel, aChannels);
			
			FFT::ifft256(b);

			for (i = 0; i < 256; i++)
			{
				mMixBuffer[chofs + ((bofs + i) & 511)] += b[i] * (128 - abs(128 - i)) * (1.0f / 128.0f);
			}			
			
			for (i = 0; i < 128; i++)
			{
				aBuffer[ofs + i] += (mMixBuffer[chofs + ((bofs + i) & 511)] - aBuffer[ofs + i]) * mParam[0];
			}
			ofs += 128;
			bofs += 128;
		}
		mOffset[aChannel] = bofs;
	}

	void FFTFilterInstance::fftFilterChannel(float *aFFTBuffer, unsigned int aSamples, float aSamplerate, time aTime, unsigned int aChannel, unsigned int aChannels)
	{
		unsigned int i;
		for (i = 4; i < aSamples; i++)
		{
			aFFTBuffer[i - 4] = aFFTBuffer[i];
			aFFTBuffer[i + aSamples - 4] = aFFTBuffer[i + aSamples];
		}
		for (i = 0; i < 4; i++)
		{
			aFFTBuffer[aSamples - 4 + i] = 0;
			aFFTBuffer[aSamples + aSamples - 4 + i] = 0;
		}
	}

	FFTFilterInstance::~FFTFilterInstance()
	{
		delete[] mTemp;
		delete[] mInputBuffer;
		delete[] mMixBuffer;
	}

	FFTFilter::FFTFilter()
	{
	}

	FilterInstance *FFTFilter::createInstance()
	{
		return new FFTFilterInstance(this);
	}
}
//----soloud_flangerfilter.cpp-----------------------------------------------------------------------
namespace SoLoud
{
	FlangerFilterInstance::FlangerFilterInstance(FlangerFilter *aParent)
	{
		mParent = aParent;
		mBuffer = 0;
		mBufferLength = 0;
		mOffset = 0;
		mIndex = 0;
		initParams(3);
		mParam[FlangerFilter::WET] = 1;
		mParam[FlangerFilter::FREQ] = mParent->mFreq;
		mParam[FlangerFilter::DELAY] = mParent->mDelay;
	}

	void FlangerFilterInstance::filter(float *aBuffer, unsigned int aSamples, unsigned int aChannels, float aSamplerate, double aTime)
	{
		updateParams(aTime);

		if (mBufferLength < mParam[FlangerFilter::DELAY] * aSamplerate)
		{
			delete[] mBuffer;
			mBufferLength = (int)ceil(mParam[FlangerFilter::DELAY] * aSamplerate) * aChannels;
			mBuffer = new float[mBufferLength];
			if (mBuffer == NULL)
			{
				mBufferLength = 0;
				return;
			}
			memset(mBuffer, 0, sizeof(float) * mBufferLength * aChannels);
		}

		unsigned int i, j;
		int maxsamples = (int)ceil(mParam[FlangerFilter::DELAY] * aSamplerate);
		double inc = mParam[FlangerFilter::FREQ] * M_PI * 2 / aSamplerate;
		for (i = 0; i < aChannels; i++)
		{
			int mbofs = i * mBufferLength;
			int abofs = i * aSamples;
			for (j = 0; j < aSamples; j++, abofs++)
			{
				int delay = (int)floor(maxsamples * (1 + cos(mIndex))) / 2;
				mIndex += inc;
				mBuffer[mbofs + mOffset % mBufferLength] = aBuffer[abofs];
				float n = 0.5f * (aBuffer[abofs] + mBuffer[mbofs + (mBufferLength - delay + mOffset) % mBufferLength]);
				mOffset++;
				aBuffer[abofs] += (n - aBuffer[abofs]) * mParam[FlangerFilter::WET];
			}
			mOffset -= aSamples;
		}
		mOffset += aSamples;
		mOffset %= mBufferLength;
	}

	FlangerFilterInstance::~FlangerFilterInstance()
	{
		delete[] mBuffer;
	}

	FlangerFilter::FlangerFilter()
	{
		mDelay = 0.005f;
		mFreq = 10;
	}

	result FlangerFilter::setParams(float aDelay, float aFreq)
	{
		if (aDelay <= 0 || aFreq <= 0)
			return INVALID_PARAMETER;

		mDelay = aDelay;
		mFreq = aFreq;
		
		return 0;
	}


	FilterInstance *FlangerFilter::createInstance()
	{
		return new FlangerFilterInstance(this);
	}
}
//----soloud_lofifilter.cpp-----------------------------------------------------------------------
namespace SoLoud
{

	LofiFilterInstance::LofiFilterInstance(LofiFilter *aParent)
	{
		mParent = aParent;
		initParams(3);
		mParam[SAMPLERATE] = aParent->mSampleRate;
		mParam[BITDEPTH] = aParent->mBitdepth;
		mChannelData[0].mSample = 0;
		mChannelData[0].mSamplesToSkip = 0;
		mChannelData[1].mSample = 0;
		mChannelData[1].mSamplesToSkip = 0;
	}

	void LofiFilterInstance::filterChannel(float *aBuffer, unsigned int aSamples, float aSamplerate, double aTime, unsigned int aChannel, unsigned int aChannels)
	{
		updateParams(aTime);

		unsigned int i;
		for (i = 0; i < aSamples; i++)
		{
			if (mChannelData[aChannel].mSamplesToSkip <= 0)
			{
				mChannelData[aChannel].mSamplesToSkip += (aSamplerate / mParam[SAMPLERATE]) - 1;
                float q = (float)pow(2.f, mParam[BITDEPTH]);
				mChannelData[aChannel].mSample = (float)floor(q*aBuffer[i])/q;
			}
			else
			{
				mChannelData[aChannel].mSamplesToSkip--;
			}
			aBuffer[i] += (mChannelData[aChannel].mSample - aBuffer[i]) * mParam[WET];
		}

	}

	LofiFilterInstance::~LofiFilterInstance()
	{
	}

	LofiFilter::LofiFilter()
	{
		setParams(4000, 3);
	}

	result LofiFilter::setParams(float aSampleRate, float aBitdepth)
	{
		if (aSampleRate <= 0 || aBitdepth <= 0)
			return INVALID_PARAMETER;

		mSampleRate = aSampleRate;
		mBitdepth = aBitdepth;
		return 0;
	}

	LofiFilter::~LofiFilter()
	{
	}


	LofiFilterInstance *LofiFilter::createInstance()
	{
		return new LofiFilterInstance(this);
	}
}

#ifndef NO_IMGUISOLOUD_WAV
//----soloud_file_hack_on.h---------------------------------------------------------------------------------------
#define IMGUISOLOUD_FILE_HACK_ON // basically SoLoud uses a custom stb_vorbis.c (please see it) that is wrapped around this. I'm now trying to refactor the code to use "default" stb_vorbis.c.
#ifdef IMGUISOLOUD_FILE_HACK_ON	// Probably meaningless, but I'm just condensing code in a single blob now...
// This is a "hack" header to fool third party code to use our File stuff instead
// of stdio FILE* stuff.
// You can use soloud_file_hack_off.h to undef the stuff defined here.
#ifndef SEEK_SET
#error soloud_file_hack_on must be included after stdio, otherwise the #define hacks will break stdio.
#endif
typedef void* Soloud_Filehack;
#ifdef __cplusplus
extern "C" {
#endif
extern int Soloud_Filehack_fgetc(Soloud_Filehack *f);
extern int Soloud_Filehack_fread(void *dst, int s, int c, Soloud_Filehack *f);
extern int Soloud_Filehack_fseek(Soloud_Filehack *f, int idx, int base);
extern int Soloud_Filehack_ftell(Soloud_Filehack *f);
extern int Soloud_Filehack_fclose(Soloud_Filehack *f);
extern Soloud_Filehack * Soloud_Filehack_fopen(const char *aFilename, char *aMode);
#ifdef __cplusplus
}
#endif
#define FILE Soloud_Filehack
#define fgetc Soloud_Filehack_fgetc
#define fread Soloud_Filehack_fread
#define fseek Soloud_Filehack_fseek
#define ftell Soloud_Filehack_ftell
#define fclose Soloud_Filehack_fclose
#define fopen Soloud_Filehack_fopen
#endif // IMGUISOLOUD_FILE_HACK_ON

//----stb_vorbis.c-------------------------------------------------------------------
#ifdef _MSC_VER
#   pragma warning(disable:4996) // allows usage of strncpy, strcpy, strcat, sprintf, fopen
#else //_MSC_VER
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wwrite-strings"
#   pragma GCC diagnostic ignored "-Wunused-value"
#   pragma GCC diagnostic ignored "-Wtype-limits"
#endif //_MSC_VER
//#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"
#ifdef _MSC_VER
#   pragma warning(default:4996)
#else // _MSC_VER
#   pragma GCC diagnostic pop
#endif //_MSC_VER//-----------------------------------------------------------------------------------


//----soloud_file_hack_off.h-----------------------------------------------------------------------------------------------
#ifdef IMGUISOLOUD_FILE_HACK_ON
// See soloud_file_hack_on.h
#undef FILE
#undef fgetc
#undef fread
#undef fseek
#undef ftell
#undef fclose
#undef fopen
#endif //IMGUISOLOUD_FILE_HACK_ON


//----../src/audiosource/wav/soloud_wav.cpp-----------------------------------------------------------------------
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
namespace SoLoud
{
	WavInstance::WavInstance(Wav *aParent)
	{
		mParent = aParent;
		mOffset = 0;
	}

	void WavInstance::getAudio(float *aBuffer, unsigned int aSamples)
	{		
		if (mParent->mData == NULL)
			return;

		// Buffer size may be bigger than samples, and samples may loop..

		unsigned int written = 0;
		unsigned int maxwrite = (aSamples > mParent->mSampleCount) ?  mParent->mSampleCount : aSamples;
		unsigned int channels = mChannels;

		while (written < aSamples)
		{
			unsigned int copysize = maxwrite;
			if (copysize + mOffset > mParent->mSampleCount)
			{
				copysize = mParent->mSampleCount - mOffset;
			}

			if (copysize + written > aSamples)
			{
				copysize = aSamples - written;
			}

			unsigned int i;
			for (i = 0; i < channels; i++)
			{
				memcpy(aBuffer + i * aSamples + written, mParent->mData + mOffset + i * mParent->mSampleCount, sizeof(float) * copysize);
			}

			written += copysize;
			mOffset += copysize;				
		
			if (copysize != maxwrite)
			{
				if (mFlags & AudioSourceInstance::LOOPING)
				{
					if (mOffset == mParent->mSampleCount)
					{
						mOffset = 0;
						mLoopCount++;
					}
				}
				else
				{
					for (i = 0; i < channels; i++)
					{
						memset(aBuffer + copysize + i * aSamples, 0, sizeof(float) * (aSamples - written));
					}
					mOffset += aSamples - written;
					written = aSamples;
				}
			}
		}
	}

	result WavInstance::rewind()
	{
		mOffset = 0;
		mStreamTime = 0;
		return 0;
	}

	bool WavInstance::hasEnded()
	{
		if (!(mFlags & AudioSourceInstance::LOOPING) && mOffset >= mParent->mSampleCount)
		{
			return 1;
		}
		return 0;
	}

	Wav::Wav()
	{
		mData = NULL;
		mSampleCount = 0;
	}
	
	Wav::~Wav()
	{
		stop();
		delete[] mData;
	}

#define MAKEDWORD(a,b,c,d) (((d) << 24) | ((c) << 16) | ((b) << 8) | (a))

    result Wav::loadwav(File *aReader)
	{
		/*int wavsize =*/ aReader->read32();
		if (aReader->read32() != MAKEDWORD('W','A','V','E'))
		{
			return FILE_LOAD_FAILED;
		}
        //if (aReader->read32() != MAKEDWORD('f','m','t',' '))
        int chunk = aReader->read32();
        if (chunk == MAKEDWORD('J','U','N','K'))
        {
            int size = aReader->read32();
            if (size & 1)
                size += 1;
            int i;
            for (i = 0; i < size; i++)
                aReader->read8();
            chunk = aReader->read32();
        }
        if (chunk != MAKEDWORD('f','m','t',' '))
		{
			return FILE_LOAD_FAILED;
		}
		int subchunk1size = aReader->read32();
		int audioformat = aReader->read16();
		int channels = aReader->read16();
		int samplerate = aReader->read32();
		/*int byterate =*/ aReader->read32();
		/*int blockalign =*/ aReader->read16();
		int bitspersample = aReader->read16();

		if (audioformat != 1 ||
			subchunk1size != 16 ||
			(bitspersample != 8 && bitspersample != 16))
		{
			return FILE_LOAD_FAILED;
		}
		
        chunk = aReader->read32();
		
		if (chunk == MAKEDWORD('L','I','S','T'))
		{
			int size = aReader->read32();
			int i;
			for (i = 0; i < size; i++)
				aReader->read8();
			chunk = aReader->read32();
		}
		
		if (chunk != MAKEDWORD('d','a','t','a'))
		{
			return FILE_LOAD_FAILED;
		}

		int readchannels = 1;

		if (channels > 1)
		{
			readchannels = 2;
			mChannels = 2;
		}

		int subchunk2size = aReader->read32();
		
		int samples = (subchunk2size / (bitspersample / 8)) / channels;
		
		mData = new float[samples * readchannels];
		
		int i, j;
		if (bitspersample == 8)
		{
			for (i = 0; i < samples; i++)
			{
				for (j = 0; j < channels; j++)
				{
					if (j == 0)
					{
						mData[i] = ((signed)aReader->read8() - 128) / (float)0x80;
					}
					else
					{
						if (readchannels > 1 && j == 1)
						{
							mData[i + samples] = ((signed)aReader->read8() - 128) / (float)0x80;
						}
						else
						{
							aReader->read8();
						}
					}
				}
			}
		}
		else
		if (bitspersample == 16)
		{
			for (i = 0; i < samples; i++)
			{
				for (j = 0; j < channels; j++)
				{
					if (j == 0)
					{
						mData[i] = ((signed short)aReader->read16()) / (float)0x8000;
					}
					else
					{
						if (readchannels > 1 && j == 1)
						{
							mData[i + samples] = ((signed short)aReader->read16()) / (float)0x8000;
						}
						else
						{
							aReader->read16();
						}
					}
				}
			}
		}
		mBaseSamplerate = (float)samplerate;
		mSampleCount = samples;

		return 0;
	}

	result Wav::loadogg(stb_vorbis *aVorbis)
	{
        stb_vorbis_info info = stb_vorbis_get_info(aVorbis);
		mBaseSamplerate = (float)info.sample_rate;
        int samples = stb_vorbis_stream_length_in_samples(aVorbis);

		int readchannels = 1;
		if (info.channels > 1)
		{
			readchannels = 2;
			mChannels = 2;
		}
		mData = new float[samples * readchannels];
		mSampleCount = samples;
		samples = 0;
		while(1)
		{
			float **outputs;
            int n = stb_vorbis_get_frame_float(aVorbis, NULL, &outputs);
			if (n == 0)
            {
				break;
            }
			if (readchannels == 1)
			{
				memcpy(mData + samples, outputs[0],sizeof(float) * n);
			}
			else
			{
				memcpy(mData + samples, outputs[0],sizeof(float) * n);
				memcpy(mData + samples + mSampleCount, outputs[1],sizeof(float) * n);
			}
			samples += n;
		}
        stb_vorbis_close(aVorbis);

		return 0;
	}

    result Wav::testAndLoadFile(File *aReader)
    {
		delete[] mData;
		mData = 0;
		mSampleCount = 0;
        int tag = aReader->read32();
		if (tag == MAKEDWORD('O','g','g','S')) 
        {
		 	aReader->seek(0);
			int e = 0;
			stb_vorbis *v = 0;
			v = stb_vorbis_open_file((Soloud_Filehack*)aReader, 0, &e, 0);

			if (0 != v)
            {
				return loadogg(v);
            }
			return FILE_LOAD_FAILED;
		} 
        else if (tag == MAKEDWORD('R','I','F','F')) 
        {
			return loadwav(aReader);
		}
		return FILE_LOAD_FAILED;
    }

	result Wav::load(const char *aFilename)
	{
		DiskFile dr;
		int res = dr.open(aFilename);
		if (res != SO_NO_ERROR)
        {
			return res;
        }
		return testAndLoadFile(&dr);
	}

	result Wav::loadMem(unsigned char *aMem, unsigned int aLength, bool aCopy, bool aTakeOwnership)
	{
		if (aMem == NULL || aLength == 0)
			return INVALID_PARAMETER;

		MemoryFile dr;
        dr.openMem(aMem, aLength, aCopy, aTakeOwnership);
		return testAndLoadFile(&dr);
	}

	result Wav::loadFile(File *aFile)
	{
		return testAndLoadFile(aFile);
	}

	AudioSourceInstance *Wav::createInstance()
	{
		return new WavInstance(this);
	}

	double Wav::getLength()
	{
		if (mBaseSamplerate == 0)
			return 0;
		return mSampleCount / mBaseSamplerate;
	}
};
//----../src/audiosource/speech/soloud_wavstream.cpp-----------------------------------------------------------------------
namespace SoLoud
{
	WavStreamInstance::WavStreamInstance(WavStream *aParent)
	{
		mParent = aParent;
		mOffset = 0;
		mOgg = 0;
		mFile = 0;
		if (aParent->mMemFile)
		{
			MemoryFile *mf = new MemoryFile();
			mFile = mf;
			mf->openMem(aParent->mMemFile->getMemPtr(), aParent->mMemFile->length(), false, false);
		}
		else
		if (aParent->mFilename)
		{
			DiskFile *df = new DiskFile;
			mFile = df;
			df->open(aParent->mFilename);
		}
		else
		if (aParent->mStreamFile)
		{
			mFile = aParent->mStreamFile;
			mFile->seek(0); // stb_vorbis assumes file offset to be at start of ogg
		}
		else
		{
			return;
		}
		
		if (mFile)
		{
			if (mParent->mOgg)
			{
				int e;

				mOgg = stb_vorbis_open_file((Soloud_Filehack *)mFile, 0, &e, 0);

				if (!mOgg)
				{
					if (mFile != mParent->mStreamFile)
						delete mFile;
					mFile = 0;
				}
				mOggFrameSize = 0;
				mOggFrameOffset = 0;
				mOggOutputs = 0;
			}
			else
			{		
				mFile->seek(aParent->mDataOffset);
			}
		}
	}

	WavStreamInstance::~WavStreamInstance()
	{
		if (mOgg)
		{
			stb_vorbis_close(mOgg);
		}
		if (mFile != mParent->mStreamFile)
		{
			delete mFile;
		}
	}

	static void getWavData(File * aFile, float * aBuffer, int aSamples, int aPitch, int aChannels, int aSrcChannels, int aBits)
	{
		int i, j;
		if (aBits == 8)
		{
			for (i = 0; i < aSamples; i++)
			{
				for (j = 0; j < aSrcChannels; j++)
				{
					if (j == 0)
					{
						aBuffer[i] = ((signed)aFile->read8() - 128) / (float)0x80;
					}
					else
					{
						if (aChannels > 1 && j == 1)
						{
							aBuffer[i + aPitch] = ((signed)aFile->read8() - 128) / (float)0x80;
						}
						else
						{
							aFile->read8();
						}
					}
				}
			}
		}
		else
		if (aBits == 16)
		{
			for (i = 0; i < aSamples; i++)
			{
				for (j = 0; j < aSrcChannels; j++)
				{
					if (j == 0)
					{
						aBuffer[i] = ((signed short)aFile->read16()) / (float)0x8000;
					}
					else
					{
						if (aChannels > 1 && j == 1)
						{
							aBuffer[i + aPitch] = ((signed short)aFile->read16()) / (float)0x8000;
						}
						else
						{
							aFile->read16();
						}
					}
				}
			}
		}
	}

	static int getOggData(float **aOggOutputs, float *aBuffer, int aSamples, int aPitch, int aFrameSize, int aFrameOffset, int aChannels)
	{			
		if (aFrameSize <= 0)
			return 0;

		int samples = aSamples;
		if (aFrameSize - aFrameOffset < samples)
		{
			samples = aFrameSize - aFrameOffset;
		}

		if (aChannels == 1)
		{
			memcpy(aBuffer, aOggOutputs[0] + aFrameOffset, sizeof(float) * samples);
		}
		else
		{
			memcpy(aBuffer, aOggOutputs[0] + aFrameOffset, sizeof(float) * samples);
			memcpy(aBuffer + aPitch, aOggOutputs[1] + aFrameOffset, sizeof(float) * samples);
		}
		return samples;
	}

	void WavStreamInstance::getAudio(float *aBuffer, unsigned int aSamples)
	{			
		unsigned int channels = mChannels;

		if (mFile == NULL)
			return;

		if (mOgg)
		{
			unsigned int offset = 0;			
			if (mOggFrameOffset < mOggFrameSize)
			{
				int b = getOggData(mOggOutputs, aBuffer, aSamples, aSamples, mOggFrameSize, mOggFrameOffset, channels);
				mOffset += b;
				offset += b;
				mOggFrameOffset += b;
			}

			while (offset < aSamples)
			{
				mOggFrameSize = stb_vorbis_get_frame_float(mOgg, NULL, &mOggOutputs);
				mOggFrameOffset = 0;
				int b;
				b = getOggData(mOggOutputs, aBuffer + offset, aSamples - offset, aSamples, mOggFrameSize, mOggFrameOffset, channels);
				mOffset += b;
				offset += b;
				mOggFrameOffset += b;
				if (mOffset >= mParent->mSampleCount)
				{
					if (mFlags & AudioSourceInstance::LOOPING)
					{
						stb_vorbis_seek_start(mOgg);
						mOffset = aSamples - offset;
						mLoopCount++;
					}
					else
					{
						unsigned int i;
						for (i = 0; i < channels; i++)
							memset(aBuffer + offset + i * aSamples, 0, sizeof(float) * (aSamples - offset));
						mOffset += aSamples - offset;
						offset = aSamples;
					}
				}
			}
		}
		else
		{
			unsigned int copysize = aSamples;
			if (copysize + mOffset > mParent->mSampleCount)
			{
				copysize = mParent->mSampleCount - mOffset;
			}

			getWavData(mFile, aBuffer, copysize, aSamples, channels, mParent->mChannels, mParent->mBits);
		
			if (copysize != aSamples)
			{
				if (mFlags & AudioSourceInstance::LOOPING)
				{
					mFile->seek(mParent->mDataOffset);
					getWavData(mFile, aBuffer + copysize, aSamples - copysize, aSamples, channels, mParent->mChannels, mParent->mBits);
					mOffset = aSamples - copysize;
					mLoopCount++;
				}
				else
				{					
					unsigned int i;
					for (i = 0; i < channels; i++)
						memset(aBuffer + copysize + i * aSamples, 0, sizeof(float) * (aSamples - copysize));
						
					mOffset += aSamples;
				}
			}
			else
			{
				mOffset += aSamples;
			}
		}
	}

	result WavStreamInstance::rewind()
	{
		if (mOgg)
		{
			stb_vorbis_seek_start(mOgg);
		}
		else
		if (mFile)
		{
			mFile->seek(mParent->mDataOffset);
		}
		mOffset = 0;
		mStreamTime = 0;
		return 0;
	}

	bool WavStreamInstance::hasEnded()
	{
		if (mOffset >= mParent->mSampleCount && !(mFlags & AudioSourceInstance::LOOPING))
		{
			return 1;
		}
		return 0;
	}

	WavStream::WavStream()
	{
		mFilename = 0;
		mSampleCount = 0;
		mOgg = 0;
		mDataOffset = 0;
		mBits = 0;
		mMemFile = 0;
		mStreamFile = 0;
	}
	
	WavStream::~WavStream()
	{
		stop();
		delete[] mFilename;
		delete mMemFile;
	}
	
#define MAKEDWORD(a,b,c,d) (((d) << 24) | ((c) << 16) | ((b) << 8) | (a))

	result WavStream::loadwav(File * fp)
	{
		fp->seek(4);
		int wavsize = fp->read32();
		if (fp->read32() != MAKEDWORD('W', 'A', 'V', 'E'))
		{
			return FILE_LOAD_FAILED;
		}
        int chunk = fp->read32();
        if (chunk == MAKEDWORD('J', 'U', 'N', 'K'))
        {
            int size = fp->read32();
            if (size & 1)
            {
                size += 1;
            }
            int i;
            for (i = 0; i < size; i++)
                fp->read8();
            chunk = fp->read32();
        }
        if (chunk != MAKEDWORD('f', 'm', 't', ' '))
		{
			return FILE_LOAD_FAILED;
		}
		int subchunk1size = fp->read32();
		int audioformat = fp->read16();
		int channels = fp->read16();
		int samplerate = fp->read32();
		int byterate = fp->read32();
		int blockalign = fp->read16();
		int bitspersample = fp->read16();

		if (audioformat != 1 ||
			subchunk1size != 16 ||
			(bitspersample != 8 && bitspersample != 16))
		{
			return FILE_LOAD_FAILED;
		}
		
        chunk = fp->read32();
		
		if (chunk == MAKEDWORD('L','I','S','T'))
		{
			int size = fp->read32();
			int i;
			for (i = 0; i < size; i++)
				fp->read8();
			chunk = fp->read32();
		}
		
		if (chunk != MAKEDWORD('d','a','t','a'))
		{
			return FILE_LOAD_FAILED;
		}

		int readchannels = 1;

		mChannels = channels;

		if (channels > 1)
		{
			readchannels = 2;
			mChannels = 2;
		}

		int subchunk2size = fp->read32();
		
		int samples = (subchunk2size / (bitspersample / 8)) / channels;
		
		mDataOffset = fp->pos();
		mBits = bitspersample;
		mBaseSamplerate = (float)samplerate;
		mSampleCount = samples;
		mOgg = 0;

		return 0;
	}

	result WavStream::loadogg(File * fp)
	{
		fp->seek(0);
		int e;
		stb_vorbis *v;
		v = stb_vorbis_open_file((Soloud_Filehack *)fp, 0, &e, 0);
		if (v == NULL)
			return FILE_LOAD_FAILED;
		stb_vorbis_info info = stb_vorbis_get_info(v);
		mChannels = 1;
		if (info.channels > 1)
		{
			mChannels = 2;
		}
		mBaseSamplerate = (float)info.sample_rate;
		int samples = stb_vorbis_stream_length_in_samples(v);
		stb_vorbis_close(v);
		mOgg = 1;

		mSampleCount = samples;

		return 0;
	}

	result WavStream::load(const char *aFilename)
	{
		delete[] mFilename;
		delete mMemFile;
		mMemFile = 0;
		mFilename = 0;
		mSampleCount = 0;
		DiskFile fp;
		int res = fp.open(aFilename);
		if (res != SO_NO_ERROR)
			return res;
		
		int len = (int)strlen(aFilename);
		mFilename = new char[len+1];		
		memcpy(mFilename, aFilename, len);
		mFilename[len] = 0;
		
		res = parse(&fp);

		if (res != SO_NO_ERROR)
		{
			delete[] mFilename;
			mFilename = 0;
			return res;
		}

		return 0;
	}

	result WavStream::loadMem(unsigned char *aData, unsigned int aDataLen, bool aCopy, bool aTakeOwnership)
	{
		delete[] mFilename;
		delete mMemFile;
		mStreamFile = 0;
		mMemFile = 0;
		mFilename = 0;
		mSampleCount = 0;

		if (aData == NULL || aDataLen == 0)
			return INVALID_PARAMETER;

		MemoryFile *mf = new MemoryFile();
		int res = mf->openMem(aData, aDataLen, aCopy, aTakeOwnership);
		if (res != SO_NO_ERROR)
		{
			delete mf;
			return res;
		}

		res = parse(mf);

		if (res != SO_NO_ERROR)
		{
			delete mf;
			return res;
		}

		mMemFile = mf;

		return 0;
	}

	result WavStream::loadToMem(const char *aFilename)
	{
		DiskFile df;
		int res = df.open(aFilename);
		if (res == SO_NO_ERROR)
		{
			res = loadFileToMem(&df);
		}
		return res;
	}

	result WavStream::loadFile(File *aFile)
	{
		delete[] mFilename;
		delete mMemFile;
		mStreamFile = 0;
		mMemFile = 0;
		mFilename = 0;
		mSampleCount = 0;

		int res = parse(aFile);

		if (res != SO_NO_ERROR)
		{
			return res;
		}

		mStreamFile = aFile;

		return 0;
	}

	result WavStream::loadFileToMem(File *aFile)
	{
		delete[] mFilename;
		delete mMemFile;
		mStreamFile = 0;
		mMemFile = 0;
		mFilename = 0;
		mSampleCount = 0;

		MemoryFile *mf = new MemoryFile();
		int res = mf->openFileToMem(aFile);
		if (res != SO_NO_ERROR)
		{
			delete mf;
			return res;
		}

		res = parse(mf);

		if (res != SO_NO_ERROR)
		{
			delete mf;
			return res;
		}

		mMemFile = mf;

		return res;
	}


	result WavStream::parse(File *aFile)
	{
		int tag = aFile->read32();
		int res = SO_NO_ERROR;
		if (tag == MAKEDWORD('O', 'g', 'g', 'S'))
		{
			res = loadogg(aFile);
		}
		else
		if (tag == MAKEDWORD('R', 'I', 'F', 'F'))
		{
			res = loadwav(aFile);
		}
		else
		{
			res = FILE_LOAD_FAILED;
		}
		return res;
	}

	AudioSourceInstance *WavStream::createInstance()
	{
		return new WavStreamInstance(this);
	}

	double WavStream::getLength()
	{
		if (mBaseSamplerate == 0)
			return 0;
		return mSampleCount / mBaseSamplerate;
	}

};
#endif //NO_IMGUISOLOUD_WAV

//----../src/audiosource/modplug/soloud_modplug.cpp-----------------------------------------------------------------------------------------------
#ifdef YES_IMGUISOLOUD_MODPLUG
#ifdef WITH_MODPLUG
//---------../ext/libmodplug/src/modplug.h-------------------------------------------
//#include <modplug.h>                  // Should we amalgamate this too ?
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-compare"
#endif
#   ifdef MAX_CHANNELS
#       define SOLOUD_MAX_CHANNELS MAX_CHANNELS
#   undef MAX_CHANNELS
#   endif //MAX_CHANNELS
#define HAVE_SETENV
#define HAVE_SINF
#include "imguimodplug.cpp.inl"			// Sure! Should we scope it with a namespace ? Not now...
#undef HAVE_SINF
#undef HAVE_SETENV
#   ifdef SOLOUD_MAX_CHANNELS
#       undef MAX_CHANNELS
#       define  MAX_CHANNELS SOLOUD_MAX_CHANNELS
#       undef SOLOUD_MAX_CHANNELS
#   endif //SOLOUD_MAX_CHANNELS
#ifdef __clang__
#pragma clang diagnostic pop
#endif
//---------end ./ext/libmodplug/...--------------------------------------------------
#endif //WITH_MODPLUG
//----../src/audiosource/modplug/soloud_modplug.cpp-----------------------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
namespace SoLoud
{

	ModplugInstance::ModplugInstance(Modplug *aParent)
	{
#ifdef WITH_MODPLUG
		mParent = aParent;
		ModPlugFile* mpf = ModPlug_Load((const void*)mParent->mData, mParent->mDataLen);
		mModplugfile = (void*)mpf;
		mPlaying = mpf != NULL;		
#endif
	}

	void ModplugInstance::getAudio(float *aBuffer, unsigned int aSamples)
	{
#ifdef WITH_MODPLUG
		if (mModplugfile == NULL)
			return;
		int buf[1024];
		int s = aSamples;
		unsigned int outofs = 0;
		
		while (s && mPlaying)
		{
			int samples = 512;
			if (s < samples) s = samples;
			int res = ModPlug_Read((ModPlugFile *)mModplugfile, (void*)&buf[0], sizeof(int) * 2 * samples);
			int samples_in_buffer = res / (sizeof(int) * 2);
			if (s != samples_in_buffer) mPlaying = 0;

			int i;
			for (i = 0; i < samples_in_buffer; i++)
			{
				aBuffer[outofs] = buf[i*2+0] / (float)0x7fffffff;
				aBuffer[outofs + aSamples] = buf[i*2+1] / (float)0x7fffffff;
				outofs++;
			}
			s -= samples_in_buffer;
		}

		if (outofs < aSamples)
		{
			// TODO: handle looping
			unsigned int i;
			for (i = outofs; i < aSamples; i++)
				aBuffer[i] = aBuffer[i + aSamples] = 0;
		}
#endif		
	}

	bool ModplugInstance::hasEnded()
	{
#ifdef WITH_MODPLUG
		return !mPlaying;
#else
		return 1;
#endif
	}

	ModplugInstance::~ModplugInstance()
	{
#ifdef WITH_MODPLUG
		if (mModplugfile)
		{
			ModPlug_Unload((ModPlugFile*)mModplugfile);
		}
		mModplugfile = 0;
#endif
	}

	result Modplug::loadMem(unsigned char *aMem, unsigned int aLength, bool aCopy, bool aTakeOwnership)
	{
		MemoryFile mf;
		int res = mf.openMem(aMem, aLength, aCopy, aTakeOwnership);
		if (res != SO_NO_ERROR)
			return res;
		return loadFile(&mf);
	}

	result Modplug::load(const char *aFilename)
	{
		DiskFile df;
		int res = df.open(aFilename);
		if (res != SO_NO_ERROR)
			return res;
		return loadFile(&df);
	}

	result Modplug::loadFile(File *aFile)
	{
#ifdef WITH_MODPLUG
		
		if (mData)
		{
			delete[] mData;
		}

		mDataLen = aFile->length();
		mData = new char[mDataLen];
		if (!mData)
		{
			mData = 0;
			mDataLen = 0;
			return OUT_OF_MEMORY;
		}
		aFile->read((unsigned char*)mData, mDataLen);

		ModPlugFile* mpf = ModPlug_Load((const void*)mData, mDataLen);
		if (!mpf)
		{
			delete[] mData;
			mDataLen = 0;
			return FILE_LOAD_FAILED;
		}
		mEstimatedDuration = ModPlug_GetLength(mpf) / 1000.0f;
		ModPlug_Unload(mpf);
		return 0;
#else
		return NOT_IMPLEMENTED;
#endif
	}

	float Modplug::getLength()
	{
		return mEstimatedDuration;
	}

	Modplug::Modplug()
	{
#ifdef WITH_MODPLUG
		mBaseSamplerate = 44100;
		mChannels = 2;
		mData = 0;
		mDataLen = 0;

		ModPlug_Settings mps;
		ModPlug_GetSettings(&mps);
		mps.mChannels = 2;
		mps.mBits = 32;
		mps.mFrequency = 44100;
		mps.mResamplingMode = MODPLUG_RESAMPLE_LINEAR;
		mps.mStereoSeparation = 128;
		mps.mMaxMixChannels = 64;
		mps.mLoopCount = -1;
		mps.mFlags = MODPLUG_ENABLE_OVERSAMPLING;
		ModPlug_SetSettings(&mps);
#endif
	}

	Modplug::~Modplug()
	{
		stop();
#ifdef WITH_MODPLUG
		delete[] mData;
		mData = 0;
		mDataLen = 0;
#endif
	}

	AudioSourceInstance * Modplug::createInstance() 
	{
		return new ModplugInstance(this);
	}

    const char* Modplug::SupportedExtensions = ".669;.abc;.amf;.ams;.dbm;.dmf;.dsm;.far;.it;.j2b;.mdl;.med;.mid;.mod;.mt2;.mtm;.okt;.pat;.psm;.ptm;.s3m;.stm;.ult;.umx;.xm";

} // namespace SoLoud
#endif //YES_IMGUISOLOUD_MODPLUG

//----../src/audiosource/monotone/soloud_monotone.cpp-----------------------------------------------------------------------------------------------
#ifdef  YES_IMGUISOLOUD_MONOTONE
namespace SoLoud
{

	MonotoneInstance::MonotoneInstance(Monotone *aParent)
	{
		mParent = aParent;
		mOrder = 0;
		mRow = 0;
		mTempo = 4;
		mSampleCount = 0;
		mNextChannel = 0;
		mRowTick = 0;
		int i;
		for (i = 0; i < 12; i++)
		{
			mOutput[i].mSamplePos = 0;
			mOutput[i].mSamplePosInc = 0;
			mOutput[i].mEnabled = i < mParent->mHardwareChannels && i < mParent->mSong.mTotalTracks;			
			mChannel[i].mEnabled = i < mParent->mSong.mTotalTracks;
			mChannel[i].mActive = 0;
			mChannel[i].mArpCounter = 0;
			mChannel[i].mLastNote = 0;
			mChannel[i].mPortamentoToNote = 0;
			mChannel[i].mArp = 0;
			mChannel[i].mVibrato = 0;
			mChannel[i].mVibratoIndex = 0;
			mChannel[i].mVibratoDepth = 1;
			mChannel[i].mVibratoSpeed = 1;
		}
	}

	void MonotoneInstance::getAudio(float *aBuffer, unsigned int aSamples)
	{
		int samplesPerTick = (int)floor(mSamplerate / 60);
		unsigned int i;
		for (i = 0; i < 12; i++)
		{
			mOutput[i].mEnabled = i < (unsigned int)mParent->mHardwareChannels && i < (unsigned int)mParent->mSong.mTotalTracks;
		}
		for (i = 0; i < aSamples; i++)
		{
			if ((mSampleCount % samplesPerTick) == 0)
			{
				// new tick
				mRowTick++;
				if (mRowTick >= mTempo)
				{
					mRowTick = 0;
					// Process row
					int patternjump = mOrder + 1;
					int rowjump = 0;
					int dojump = 0;
					int pattern = mParent->mSong.mOrder[mOrder];
					int j;
					for (j = 0; j < mParent->mSong.mTotalTracks; j++)
					{
						unsigned int d = mParent->mSong.mPatternData[(pattern * 64 + mRow) * mParent->mSong.mTotalTracks + j];
						unsigned int note = (d >> 9) & 127;
						unsigned int effect = (d >> 6) & 7;
						unsigned int effectdata = (d)& 63;
						unsigned int effectdata1 = (d >> 3) & 7;
						unsigned int effectdata2 = (d >> 0) & 7;

						// by default, effects are off, and have to be set on every row.
						mChannel[j].mPortamento = 0;
						mChannel[j].mArp = 0;
						mChannel[j].mVibrato = 0;
						
						int oldhz = mChannel[j].mFreq[0];

						if (note == 127)
						{
							// noteEnd
							mChannel[j].mActive = 0;
							mChannel[j].mFreq[0] = 0;
							mChannel[j].mFreq[1] = 0;
							mChannel[j].mFreq[2] = 0;
							mChannel[j].mPortamento = 0;
							mChannel[j].mLastNote = 0;
						}
						else
						if (note != 0)
						{
							mChannel[j].mActive = 1;
							mChannel[j].mFreq[0] = mParent->mNotesHz[note * 8];
							mChannel[j].mFreq[1] = mChannel[j].mFreq[0];
							mChannel[j].mFreq[2] = mChannel[j].mFreq[0];
							mChannel[j].mPortamento = 0;
							mChannel[j].mLastNote = note;
							mChannel[j].mVibratoIndex = 0;
						}
						else
						if (note == 0)
						{
							note = mChannel[j].mLastNote;
						}

						switch (effect)
						{
						case 0x0:
							// arp
							mChannel[j].mFreq[1] = mParent->mNotesHz[(note + effectdata1) * 8];
							mChannel[j].mFreq[2] = mParent->mNotesHz[(note + effectdata2) * 8];
							if (effectdata1 || effectdata2)
								mChannel[j].mArp = 1;
							break;
						case 0x1:
							// portamento up
							mChannel[j].mPortamento = effectdata;
							break;
						case 0x2:
							// portamento down
							mChannel[j].mPortamento = -(signed)effectdata;
							break;
						case 0x3:
							// portamento to note
							mChannel[j].mPortamentoToNote = mParent->mNotesHz[note * 8];
							if (oldhz != mChannel[j].mPortamentoToNote)
							{
								mChannel[j].mFreq[0] = oldhz;
								mChannel[j].mPortamento = effectdata;
								if (oldhz > mChannel[j].mPortamentoToNote)
									mChannel[j].mPortamento *= -1;							
							}
							else
							{
								mChannel[j].mPortamentoToNote = 0;
							}
							break;
						case 0x4:
							// vibrato
							mChannel[j].mVibrato = 1;
							if (effectdata2 != 0) mChannel[j].mVibratoDepth = effectdata2;
							if (effectdata1 != 0) mChannel[j].mVibratoSpeed = effectdata1;
							break;
						case 0x5:
							// pattern jump
							patternjump = effectdata;
							dojump = 1;
							break;
						case 0x6:
							// row jump
							rowjump = effectdata;
							dojump = 1;
							break;
						case 0x7:
							// set speed
							mTempo = effectdata;
							break;
						}
					}					
					
					mRow++;

					if (dojump)
					{
						mRow = rowjump;
						mOrder = patternjump;
					}

					if (mRow == 64)
					{
						mRow = 0;
						mOrder++;
						if (mParent->mSong.mOrder[mOrder] == 0xff)
							mOrder = 0;
					}
				}

				int j;

				// per tick events
				for (j = 0; j < mParent->mSong.mTotalTracks; j++)
				{
					if (mChannel[j].mActive)
					{
						if (mChannel[j].mVibrato)
						{
							mChannel[j].mFreq[0] = mParent->mNotesHz[mChannel[j].mLastNote * 8 + (mParent->mVibTable[mChannel[j].mVibratoIndex] * mChannel[j].mVibratoDepth) / 64];
							mChannel[j].mVibratoIndex += mChannel[j].mVibratoSpeed;
							mChannel[j].mVibratoIndex %= 32;
						}
						if (mChannel[j].mPortamento && mRowTick != 0)
						{
							mChannel[j].mFreq[0] += mChannel[j].mPortamento;
							if (mChannel[j].mPortamentoToNote)
							{
								if ((mChannel[j].mPortamento > 0 && mChannel[j].mFreq[0] >= mChannel[j].mPortamentoToNote) ||
   									(mChannel[j].mPortamento < 0 && mChannel[j].mFreq[0] <= mChannel[j].mPortamentoToNote))
								{
									mChannel[j].mFreq[0] = mChannel[j].mPortamentoToNote;
									mChannel[j].mPortamentoToNote = 0;
								}
							}
						}
					}
				}

				// Channel fill

				int gotit = 0;
				int tries = 0;

				for (j = 0; j < mParent->mHardwareChannels; j++)
					mOutput[j].mSamplePosInc = 0;

				while (gotit < mParent->mHardwareChannels && tries < mParent->mSong.mTotalTracks)
				{
					if (mChannel[mNextChannel].mActive)
					{
						if (mChannel[mNextChannel].mArp)
						{
							mOutput[gotit].mSamplePosInc = 1.0f / (mSamplerate / mChannel[mNextChannel].mFreq[mChannel[mNextChannel].mArpCounter]);
							mChannel[mNextChannel].mArpCounter++;
							mChannel[mNextChannel].mArpCounter %= 3;
						}
						else
						{
							mOutput[gotit].mSamplePosInc = 1.0f / (mSamplerate / mChannel[mNextChannel].mFreq[0]);
						}
						gotit++;
					}
					mNextChannel++;
					mNextChannel %= mParent->mSong.mTotalTracks;
					tries++;
				}								
			}
			
			aBuffer[i] = 0;
			int j;
			switch (mParent->mWaveform)
			{
			case Monotone::SAW:
				for (j = 0; j < 12; j++)
				{
					if (mOutput[j].mEnabled)
					{
						float bleh = mOutput[j].mSamplePos + mOutput[j].mSamplePosInc;
						mOutput[j].mSamplePos = bleh - (long)bleh;
						// saw:
						aBuffer[i] += ((mOutput[j].mSamplePos) - 0.5f) * 0.5f;
					}
				}
				break;
			case Monotone::SIN:
				for (j = 0; j < 12; j++)
				{
					if (mOutput[j].mEnabled)
					{
						float bleh = mOutput[j].mSamplePos + mOutput[j].mSamplePosInc;
						mOutput[j].mSamplePos = bleh - (long)bleh;
						// sin: 
						aBuffer[i] += (float)sin(mOutput[j].mSamplePos * M_PI * 2) * 0.5f;
					}
				}
				break;
			case Monotone::SAWSIN:
				for (j = 0; j < 12; j++)
				{
					if (mOutput[j].mEnabled)
					{
						float bleh = mOutput[j].mSamplePos + mOutput[j].mSamplePosInc;
						mOutput[j].mSamplePos = bleh - (long)bleh;
						// sawsin:
						bleh = ((mOutput[j].mSamplePos) - 0.5f);
						bleh *= (float)sin(mOutput[j].mSamplePos * M_PI * 2);
						aBuffer[i] += bleh;
					}
				}
				break;
			case Monotone::SQUARE:
			default:
				for (j = 0; j < 12; j++)
				{
					if (mOutput[j].mEnabled)
					{
						float bleh = mOutput[j].mSamplePos + mOutput[j].mSamplePosInc;
						mOutput[j].mSamplePos = bleh - (long)bleh;
						// square:
						aBuffer[i] += (mOutput[j].mSamplePos > 0.5f) ? 0.25f : -0.25f;
					}
				}
				break;
			}

			mSampleCount++;
		}
	}

	bool MonotoneInstance::hasEnded()
	{
		return 0;
	}

	Monotone::Monotone()
	{
		int i;
		float temphz = 27.5f;
		int IBO = 12; // Intervals Between Octaves
		int IBN = 8; // Intervals Between Notes
		float interval = 1.00724641222f;//exp(ln(2)/(IBO*IBN));
		int maxnote = 3 + (8 * IBO) + 1;

		mNotesHz[0] = 440;
		mNotesHz[1 * IBN] = (int)floor(temphz + 0.5f);

		for (i = (1 * IBN) - 1; i > 1; i--)
		{
			temphz = temphz / interval;
			if (temphz < 19) temphz = 19; // orig limitation, we could go lower though
			mNotesHz[i] = (int)floor(temphz + 0.5f);
		}
		temphz = 27.5f;
		for (i = (1 * IBN) + 1; i < maxnote * IBN; i++)
		{
			temphz = temphz * interval;
			mNotesHz[i] = (int)floor(temphz + 0.5f);
		}

		for (i = 0; i < 32; i++)
			mVibTable[i] = (int)floor(0.5 + 64 * sin(i * M_PI / 32 * 2));

		mSong.mTitle = 0;
		mSong.mComment = 0;
		mSong.mPatternData = 0;

		mBaseSamplerate = 44100;
		mChannels = 1;

		mHardwareChannels = 1;
		mWaveform = SQUARE;
	}

	void Monotone::clear()
	{
		stop();

		delete[] mSong.mTitle;
		delete[] mSong.mComment;
		delete[] mSong.mPatternData;

		mSong.mTitle = 0;
		mSong.mComment = 0;
		mSong.mPatternData = 0;
	}

	Monotone::~Monotone()
	{
		stop();
		clear();
	}

	static char * mystrdup(const char *src)
	{
		int len = (int)strlen(src);
		char * res = new char[len + 1];
		memcpy(res, src, len);
		res[len] = 0;
		return res;
	}

	result Monotone::setParams(int aHardwareChannels, int aWaveform)
	{
		if (aHardwareChannels <= 0 || aWaveform < 0)
			return INVALID_PARAMETER;
		mHardwareChannels = aHardwareChannels;
		mWaveform = aWaveform;
		return SO_NO_ERROR;
	}
	
	result Monotone::loadMem(unsigned char *aMem, unsigned int aLength, bool aCopy, bool aTakeOwnership)
	{
		MemoryFile mf;
		int res = mf.openMem(aMem, aLength, aCopy, aTakeOwnership);
		if (res != SO_NO_ERROR)
			return res;
		return loadFile(&mf);
	}

	result Monotone::load(const char *aFilename)
	{
		DiskFile df;
		int res = df.open(aFilename);
		if (res != SO_NO_ERROR)
			return res;
		return loadFile(&df);
	}

	result Monotone::loadFile(File *aFile)
	{
		if (aFile == NULL)
			return INVALID_PARAMETER;
		clear();
		int i;
		unsigned char temp[200];
		aFile->read(temp, 9);
		char magic[] = "\bMONOTONE";
		for (i = 0; i < 9; i++)
		{
			if (temp[i] != magic[i])
			{
				return FILE_LOAD_FAILED;
			}
		}
		aFile->read(temp, 41);
		temp[temp[0] + 1] = 0; // pascal -> asciiz: pascal strings have length as first byte
		mSong.mTitle = mystrdup((char*)temp + 1);
		aFile->read(temp, 41);
		temp[temp[0] + 1] = 0; // pascal -> asciiz: pascal strings have length as first byte
		mSong.mComment = mystrdup((char*)temp + 1);
		aFile->read(temp, 4);
		mSong.mVersion = temp[0];
		mSong.mTotalPatterns = temp[1];
		mSong.mTotalTracks = temp[2];
		mSong.mCellSize = temp[3];
		if (mSong.mVersion != 1 || mSong.mCellSize != 2)
		{
			return FILE_LOAD_FAILED;
		}
		aFile->read(mSong.mOrder, 256);
		int totalnotes = 64 * mSong.mTotalPatterns * mSong.mTotalTracks;
		mSong.mPatternData = new unsigned int[totalnotes];
		for (i = 0; i < totalnotes; i++)
		{
			aFile->read(temp, 2);
			unsigned int datavalue = temp[0] | (temp[1] << 8);
			mSong.mPatternData[i] = datavalue;
			//unsigned int note = (datavalue >> 9) & 127;
			//unsigned int effect = (datavalue >> 6) & 7;
			//unsigned int effectdata = (datavalue)& 63;
			//unsigned int effectdata1 = (datavalue >> 3) & 7;
			//unsigned int effectdata2 = (datavalue >> 0) & 7;
		}

		return SO_NO_ERROR;
	}


	AudioSourceInstance * Monotone::createInstance() 
	{
		return new MonotoneInstance(this);
	}

};
#endif //YES_IMGUISOLOUD_MONOTONE

//----../src/audiosource/sfrx/soloud_sfrx.cpp-----------------------------------------------------------------------------------------------
#ifdef YES_IMGUISOLOUD_SFXR
namespace SoLoud
{

	Prg::Prg()
	{		
	}

	void Prg::srand(int aSeed)
	{
		index = 0;
		int i;
		for (i = 0; i < 16; i++)
			state[i] = aSeed + i * aSeed + i;
	}

	// WELL512 implementation, public domain by Chris Lomont
	unsigned int Prg::rand()
	{
		unsigned int a, b, c, d;
		a = state[index];
		c = state[(index+13)&15];
		b = a^c^(a<<16)^(c<<15);
		c = state[(index+9)&15];
		c ^= (c>>11);
		a = state[index] = b^c;
		d = a^((a<<5)&0xDA442D24UL);
		index = (index + 15)&15;
		a = state[index];
		state[index] = a^b^d^(a<<2)^(b<<18)^(c<<28);
		return state[index];
	}

	SfxrInstance::SfxrInstance(Sfxr *aParent)
	{
		mParent = aParent;
		mParams = aParent->mParams;
		mRand.srand(0x792352);
		resetSample(false);
		playing_sample = 1;
	}

#define frnd(x) ((float)(mRand.rand()%10001)/10000*(x))

	void SfxrInstance::getAudio(float *aBuffer, unsigned int aSamples)
	{
		float *buffer = aBuffer;
		unsigned int i;
		for(i = 0; i < aSamples; i++)
		{
			if(!playing_sample)
			{
				*aBuffer = 0;
				aBuffer++;
				continue;
			}

			rep_time++;
			if(rep_limit!=0 && rep_time>=rep_limit)
			{
				rep_time=0;
				resetSample(true);
			}

			// frequency envelopes/arpeggios
			arp_time++;
			if(arp_limit!=0 && arp_time>=arp_limit)
			{
				arp_limit=0;
				fperiod*=arp_mod;
			}
			fslide+=fdslide;
			fperiod*=fslide;
			if(fperiod>fmaxperiod)
			{
				fperiod=fmaxperiod;
				if(mParams.p_freq_limit>0.0f)
				{
					if (mFlags & LOOPING)
					{
						resetSample(false);
					}
					else
					{
						playing_sample=false;
					}
				}
			}
			float rfperiod=(float)fperiod;
			if(vib_amp>0.0f)
			{
				vib_phase+=vib_speed;
				rfperiod=(float)(fperiod*(1.0+sin(vib_phase)*vib_amp));
			}
			period=(int)rfperiod;
			if(period<8) period=8;
			square_duty+=square_slide;
			if(square_duty<0.0f) square_duty=0.0f;
			if(square_duty>0.5f) square_duty=0.5f;		
			// volume envelope
			env_time++;
			if(env_time>env_length[env_stage])
			{
				env_time=0;
				env_stage++;
				if(env_stage==3)
				{
					if (mFlags & LOOPING)
					{
						resetSample(false);
					}
					else
					{
						playing_sample=false;
					}
				}
			}
			if (env_stage == 0)
			{
				if (env_length[0])
					env_vol = (float)env_time / env_length[0];
				else
					env_vol = 0;
			}
			if (env_stage == 1)
			{
				if (env_length[1])
					env_vol = 1.0f + (float)pow(1.0f - (float)env_time / env_length[1], 1.0f)*2.0f*mParams.p_env_punch;
				else
					env_vol = 0;
			}
			if (env_stage == 2)
			{
				if (env_length[2])
					env_vol = 1.0f - (float)env_time / env_length[2];
				else
					env_vol = 0;
			}

			// phaser step
			fphase+=fdphase;
			iphase=abs((int)fphase);
			if(iphase>1023) iphase=1023;

			if(flthp_d!=0.0f)
			{
				flthp*=flthp_d;
				if(flthp<0.00001f) flthp=0.00001f;
				if(flthp>0.1f) flthp=0.1f;
			}

			float ssample=0.0f;
			for(int si=0;si<8;si++) // 8x supersampling
			{
				float sample=0.0f;
				phase++;
				if(phase>=period)
				{
					//				phase=0;
					phase%=period;
					if(mParams.wave_type==3)
						for(int i=0;i<32;i++)
							noise_buffer[i]=frnd(2.0f)-1.0f;
				}
				// base waveform
				float fp=(float)phase/period;
				switch(mParams.wave_type)
				{
				case 0: // square
					if(fp<square_duty)
						sample=0.5f;
					else
						sample=-0.5f;
					break;
				case 1: // sawtooth
					sample=1.0f-fp*2;
					break;
				case 2: // sine
					sample=(float)sin(fp*2*M_PI);
					break;
				case 3: // noise
					sample=noise_buffer[phase*32/period];
					break;
				}
				// lp filter
				float pp=fltp;
				fltw*=fltw_d;
				if(fltw<0.0f) fltw=0.0f;
				if(fltw>0.1f) fltw=0.1f;
				if(mParams.p_lpf_freq!=1.0f)
				{
					fltdp+=(sample-fltp)*fltw;
					fltdp-=fltdp*fltdmp;
				}
				else
				{
					fltp=sample;
					fltdp=0.0f;
				}
				fltp+=fltdp;
				// hp filter
				fltphp+=fltp-pp;
				fltphp-=fltphp*flthp;
				sample=fltphp;
				// phaser
				phaser_buffer[ipp&1023]=sample;
				sample+=phaser_buffer[(ipp-iphase+1024)&1023];
				ipp=(ipp+1)&1023;
				// final accumulation and envelope application
				ssample+=sample*env_vol;
			}
			ssample=ssample/8*mParams.master_vol;

			ssample*=2.0f*mParams.sound_vol;

			if(buffer!=NULL)
			{
				if(ssample>1.0f) ssample=1.0f;
				if(ssample<-1.0f) ssample=-1.0f;
				*buffer++=ssample;
			}
		}

	}

	bool SfxrInstance::hasEnded()
	{
		return !playing_sample;
	}

	void SfxrInstance::resetSample(bool aRestart)
	{
		if(!aRestart)
			phase=0;
		fperiod=100.0/(mParams.p_base_freq*mParams.p_base_freq+0.001);
		period=(int)fperiod;
		fmaxperiod=100.0/(mParams.p_freq_limit*mParams.p_freq_limit+0.001);
		fslide=1.0-pow((double)mParams.p_freq_ramp, 3.0)*0.01;
		fdslide=-pow((double)mParams.p_freq_dramp, 3.0)*0.000001;
		square_duty=0.5f-mParams.p_duty*0.5f;
		square_slide=-mParams.p_duty_ramp*0.00005f;
		if(mParams.p_arp_mod>=0.0f)
			arp_mod=1.0-pow((double)mParams.p_arp_mod, 2.0)*0.9;
		else
			arp_mod=1.0+pow((double)mParams.p_arp_mod, 2.0)*10.0;
		arp_time=0;
		arp_limit=(int)(pow(1.0f-mParams.p_arp_speed, 2.0f)*20000+32);
		if(mParams.p_arp_speed==1.0f)
			arp_limit=0;
		if(!aRestart)
		{
			// reset filter
			fltp=0.0f;
			fltdp=0.0f;
			fltw=(float)pow(mParams.p_lpf_freq, 3.0f)*0.1f;
			fltw_d=1.0f+mParams.p_lpf_ramp*0.0001f;
			fltdmp=5.0f/(1.0f+(float)pow(mParams.p_lpf_resonance, 2.0f)*20.0f)*(0.01f+fltw);
			if(fltdmp>0.8f) fltdmp=0.8f;
			fltphp=0.0f;
			flthp=(float)pow(mParams.p_hpf_freq, 2.0f)*0.1f;
			flthp_d=(float)(1.0+mParams.p_hpf_ramp*0.0003f);
			// reset vibrato
			vib_phase=0.0f;
			vib_speed=(float)pow(mParams.p_vib_speed, 2.0f)*0.01f;
			vib_amp=mParams.p_vib_strength*0.5f;
			// reset envelope
			env_vol=0.0f;
			env_stage=0;
			env_time=0;
			env_length[0]=(int)(mParams.p_env_attack*mParams.p_env_attack*100000.0f);
			env_length[1]=(int)(mParams.p_env_sustain*mParams.p_env_sustain*100000.0f);
			env_length[2]=(int)(mParams.p_env_decay*mParams.p_env_decay*100000.0f);

			fphase=(float)pow(mParams.p_pha_offset, 2.0f)*1020.0f;
			if(mParams.p_pha_offset<0.0f) fphase=-fphase;
			fdphase=(float)pow(mParams.p_pha_ramp, 2.0f)*1.0f;
			if(mParams.p_pha_ramp<0.0f) fdphase=-fdphase;
			iphase=abs((int)fphase);
			ipp=0;
			for(int i=0;i<1024;i++)
				phaser_buffer[i]=0.0f;

			for(int i=0;i<32;i++)
				noise_buffer[i]=frnd(2.0f)-1.0f;

			rep_time=0;
			rep_limit=(int)(pow(1.0f-mParams.p_repeat_speed, 2.0f)*20000+32);
			if(mParams.p_repeat_speed==0.0f)
				rep_limit=0;
		}
	}


#define rnd(n) (mRand.rand()%(n+1))
#undef frnd
#define frnd(x) ((float)(mRand.rand()%10001)/10000*(x))


	result Sfxr::loadPreset(int aPresetNo, int aRandSeed)
	{
		if (aPresetNo < 0 || aPresetNo > 6)
			return INVALID_PARAMETER;

		resetParams();
		mRand.srand(aRandSeed);
		switch(aPresetNo)
		{
		case 0: // pickup/coin
			mParams.p_base_freq=0.4f+frnd(0.5f);
			mParams.p_env_attack=0.0f;
			mParams.p_env_sustain=frnd(0.1f);
			mParams.p_env_decay=0.1f+frnd(0.4f);
			mParams.p_env_punch=0.3f+frnd(0.3f);
			if(rnd(1))
			{
				mParams.p_arp_speed=0.5f+frnd(0.2f);
				mParams.p_arp_mod=0.2f+frnd(0.4f);
			}
			break;
		case 1: // laser/shoot
			mParams.wave_type=rnd(2);
			if(mParams.wave_type==2 && rnd(1))
				mParams.wave_type=rnd(1);
			mParams.p_base_freq=0.5f+frnd(0.5f);
			mParams.p_freq_limit=mParams.p_base_freq-0.2f-frnd(0.6f);
			if(mParams.p_freq_limit<0.2f) mParams.p_freq_limit=0.2f;
			mParams.p_freq_ramp=-0.15f-frnd(0.2f);
			if(rnd(2)==0)
			{
				mParams.p_base_freq=0.3f+frnd(0.6f);
				mParams.p_freq_limit=frnd(0.1f);
				mParams.p_freq_ramp=-0.35f-frnd(0.3f);
			}
			if(rnd(1))
			{
				mParams.p_duty=frnd(0.5f);
				mParams.p_duty_ramp=frnd(0.2f);
			}
			else
			{
				mParams.p_duty=0.4f+frnd(0.5f);
				mParams.p_duty_ramp=-frnd(0.7f);
			}
			mParams.p_env_attack=0.0f;
			mParams.p_env_sustain=0.1f+frnd(0.2f);
			mParams.p_env_decay=frnd(0.4f);
			if(rnd(1))
				mParams.p_env_punch=frnd(0.3f);
			if(rnd(2)==0)
			{
				mParams.p_pha_offset=frnd(0.2f);
				mParams.p_pha_ramp=-frnd(0.2f);
			}
			if(rnd(1))
				mParams.p_hpf_freq=frnd(0.3f);
			break;
		case 2: // explosion
			mParams.wave_type=3;
			if(rnd(1))
			{
				mParams.p_base_freq=0.1f+frnd(0.4f);
				mParams.p_freq_ramp=-0.1f+frnd(0.4f);
			}
			else
			{
				mParams.p_base_freq=0.2f+frnd(0.7f);
				mParams.p_freq_ramp=-0.2f-frnd(0.2f);
			}
			mParams.p_base_freq*=mParams.p_base_freq;
			if(rnd(4)==0)
				mParams.p_freq_ramp=0.0f;
			if(rnd(2)==0)
				mParams.p_repeat_speed=0.3f+frnd(0.5f);
			mParams.p_env_attack=0.0f;
			mParams.p_env_sustain=0.1f+frnd(0.3f);
			mParams.p_env_decay=frnd(0.5f);
			if(rnd(1)==0)
			{
				mParams.p_pha_offset=-0.3f+frnd(0.9f);
				mParams.p_pha_ramp=-frnd(0.3f);
			}
			mParams.p_env_punch=0.2f+frnd(0.6f);
			if(rnd(1))
			{
				mParams.p_vib_strength=frnd(0.7f);
				mParams.p_vib_speed=frnd(0.6f);
			}
			if(rnd(2)==0)
			{
				mParams.p_arp_speed=0.6f+frnd(0.3f);
				mParams.p_arp_mod=0.8f-frnd(1.6f);
			}
			break;
		case 3: // powerup
			if(rnd(1))
				mParams.wave_type=1;
			else
				mParams.p_duty=frnd(0.6f);
			if(rnd(1))
			{
				mParams.p_base_freq=0.2f+frnd(0.3f);
				mParams.p_freq_ramp=0.1f+frnd(0.4f);
				mParams.p_repeat_speed=0.4f+frnd(0.4f);
			}
			else
			{
				mParams.p_base_freq=0.2f+frnd(0.3f);
				mParams.p_freq_ramp=0.05f+frnd(0.2f);
				if(rnd(1))
				{
					mParams.p_vib_strength=frnd(0.7f);
					mParams.p_vib_speed=frnd(0.6f);
				}
			}
			mParams.p_env_attack=0.0f;
			mParams.p_env_sustain=frnd(0.4f);
			mParams.p_env_decay=0.1f+frnd(0.4f);
			break;
		case 4: // hit/hurt
			mParams.wave_type=rnd(2);
			if(mParams.wave_type==2)
				mParams.wave_type=3;
			if(mParams.wave_type==0)
				mParams.p_duty=frnd(0.6f);
			mParams.p_base_freq=0.2f+frnd(0.6f);
			mParams.p_freq_ramp=-0.3f-frnd(0.4f);
			mParams.p_env_attack=0.0f;
			mParams.p_env_sustain=frnd(0.1f);
			mParams.p_env_decay=0.1f+frnd(0.2f);
			if(rnd(1))
				mParams.p_hpf_freq=frnd(0.3f);
			break;
		case 5: // jump
			mParams.wave_type=0;
			mParams.p_duty=frnd(0.6f);
			mParams.p_base_freq=0.3f+frnd(0.3f);
			mParams.p_freq_ramp=0.1f+frnd(0.2f);
			mParams.p_env_attack=0.0f;
			mParams.p_env_sustain=0.1f+frnd(0.3f);
			mParams.p_env_decay=0.1f+frnd(0.2f);
			if(rnd(1))
				mParams.p_hpf_freq=frnd(0.3f);
			if(rnd(1))
				mParams.p_lpf_freq=1.0f-frnd(0.6f);
			break;
		case 6: // blip/select
			mParams.wave_type=rnd(1);
			if(mParams.wave_type==0)
				mParams.p_duty=frnd(0.6f);
			mParams.p_base_freq=0.2f+frnd(0.4f);
			mParams.p_env_attack=0.0f;
			mParams.p_env_sustain=0.1f+frnd(0.1f);
			mParams.p_env_decay=frnd(0.2f);
			mParams.p_hpf_freq=0.1f;
			break;
		}
		return 0;
	}
	
	void Sfxr::resetParams()
	{
		mParams.wave_type=0;

		mParams.p_base_freq=0.3f;
		mParams.p_freq_limit=0.0f;
		mParams.p_freq_ramp=0.0f;
		mParams.p_freq_dramp=0.0f;
		mParams.p_duty=0.0f;
		mParams.p_duty_ramp=0.0f;

		mParams.p_vib_strength=0.0f;
		mParams.p_vib_speed=0.0f;
		mParams.p_vib_delay=0.0f;

		mParams.p_env_attack=0.0f;
		mParams.p_env_sustain=0.3f;
		mParams.p_env_decay=0.4f;
		mParams.p_env_punch=0.0f;

		mParams.filter_on=false;
		mParams.p_lpf_resonance=0.0f;
		mParams.p_lpf_freq=1.0f;
		mParams.p_lpf_ramp=0.0f;
		mParams.p_hpf_freq=0.0f;
		mParams.p_hpf_ramp=0.0f;
	
		mParams.p_pha_offset=0.0f;
		mParams.p_pha_ramp=0.0f;

		mParams.p_repeat_speed=0.0f;

		mParams.p_arp_speed=0.0f;
		mParams.p_arp_mod=0.0f;

		mParams.master_vol=0.05f;
		mParams.sound_vol=0.5f;
	}

	result Sfxr::loadParamsMem(unsigned char *aMem, unsigned int aLength, bool aCopy, bool aTakeOwnership)
	{
		MemoryFile mf;
		int res = mf.openMem(aMem, aLength, aCopy, aTakeOwnership);
		if (res != SO_NO_ERROR)
			return res;
		return loadParamsFile(&mf);
	}

	result Sfxr::loadParams(const char *aFilename)
	{
		DiskFile df;
		int res = df.open(aFilename);
		if (res != SO_NO_ERROR)
			return res;
		return loadParamsFile(&df);
	}

	result Sfxr::loadParamsFile(File *aFile)
	{
		int version=0;
		aFile->read((unsigned char*)&version, sizeof(int));
		if(version!=100 && version!=101 && version!=102)
		{
			return FILE_LOAD_FAILED;
		}

		aFile->read((unsigned char*)&mParams.wave_type, sizeof(int));


		mParams.sound_vol=0.5f;
		if(version==102)
			aFile->read((unsigned char*)&mParams.sound_vol, sizeof(float));

		aFile->read((unsigned char*)&mParams.p_base_freq, sizeof(float));
		aFile->read((unsigned char*)&mParams.p_freq_limit, sizeof(float));
		aFile->read((unsigned char*)&mParams.p_freq_ramp, sizeof(float));
		if(version>=101)
			aFile->read((unsigned char*)&mParams.p_freq_dramp, sizeof(float));
		aFile->read((unsigned char*)&mParams.p_duty, sizeof(float));
		aFile->read((unsigned char*)&mParams.p_duty_ramp, sizeof(float));

		aFile->read((unsigned char*)&mParams.p_vib_strength, sizeof(float));
		aFile->read((unsigned char*)&mParams.p_vib_speed, sizeof(float));
		aFile->read((unsigned char*)&mParams.p_vib_delay, sizeof(float));

		aFile->read((unsigned char*)&mParams.p_env_attack, sizeof(float));
		aFile->read((unsigned char*)&mParams.p_env_sustain, sizeof(float));
		aFile->read((unsigned char*)&mParams.p_env_decay, sizeof(float));
		aFile->read((unsigned char*)&mParams.p_env_punch, sizeof(float));

		aFile->read((unsigned char*)&mParams.filter_on, sizeof(bool));
		aFile->read((unsigned char*)&mParams.p_lpf_resonance, sizeof(float));
		aFile->read((unsigned char*)&mParams.p_lpf_freq, sizeof(float));
		aFile->read((unsigned char*)&mParams.p_lpf_ramp, sizeof(float));
		aFile->read((unsigned char*)&mParams.p_hpf_freq, sizeof(float));
		aFile->read((unsigned char*)&mParams.p_hpf_ramp, sizeof(float));
	
		aFile->read((unsigned char*)&mParams.p_pha_offset, sizeof(float));
		aFile->read((unsigned char*)&mParams.p_pha_ramp, sizeof(float));

		aFile->read((unsigned char*)&mParams.p_repeat_speed, sizeof(float));

		if(version>=101)
		{
			aFile->read((unsigned char*)&mParams.p_arp_speed, sizeof(float));
			aFile->read((unsigned char*)&mParams.p_arp_mod, sizeof(float));
		}

		return 0;
	}

	Sfxr::~Sfxr()
	{
		stop();
	}

	Sfxr::Sfxr()
	{
		resetParams();
		mBaseSamplerate = 44100;
	}


	AudioSourceInstance * Sfxr::createInstance() 
	{
		return new SfxrInstance(this);
	}

};
#endif //YES_IMGUISOLOUD_SFXR

//----../src/audiosource/tedsid/soloud_tedsid.cpp-----------------------------------------------------------------------------------------------
#ifdef YES_IMGUISOLOUD_TEDSID
/*
The TED / SID support is based on tedplay (c) 2012 Attila Grosz, used under Unlicense http://unlicense.org/
*/
//----../src/audiosource/ted.h-----------------------------------------------------------------------------------------------
class TED 
{
public: 
    unsigned int    masterVolume;
    int             Volume;
    int             Snd1Status;
    int             Snd2Status;
    int             SndNoiseStatus;
    int             DAStatus;
    unsigned short  Freq1;
    unsigned short  Freq2;
    int             NoiseCounter;
    int             FlipFlop[2];
    int             dcOutput[2];
    int             oscCount[2];
    int             OscReload[2];
    int             waveForm[2];
    int             oscStep;
    int             sampleRate;
    unsigned char   noise[256]; // 0-8
    unsigned int channelMask[3];
    int             vol;

    TED();
    void enableChannel(unsigned int channel, bool enable);
    void setFreq(unsigned int channel, int freq);
    void oscillatorReset();
    void oscillatorInit();
    void writeSoundReg(unsigned int reg, unsigned char value);
    void storeToBuffer(short *buffer, unsigned int count);
    unsigned int waveSquare(unsigned int channel);
    unsigned int waveSawTooth(unsigned int channel);
    unsigned int waveTriangle(unsigned int channel);
    unsigned int getWaveSample(unsigned int channel, unsigned int wave);
    void renderSound(unsigned int nrsamples, short *buffer);
    void setMasterVolume(unsigned int shift);
    void selectWaveForm(unsigned int channel, unsigned int wave);
    void setplaybackSpeed(unsigned int speed);
    unsigned int getTimeSinceLastReset();
    void setSampleRate(unsigned int value);
    void setFilterOrder(unsigned int value);
    void initFilter(unsigned int sampleRate_, unsigned int filterOrder_);
};
//----../src/audiosource/ted.cpp-----------------------------------------------------------------------------------------------
#define PRECISION 0
#define OSCRELOADVAL (0x3FF << PRECISION)
#define TED_SOUND_CLOCK (221680)

TED::TED()
{
    sampleRate = TED_SOUND_CLOCK;
	masterVolume = 8;
	Volume = 8;
	Snd1Status = 0;
	Snd2Status = 0;
	SndNoiseStatus = 0;
	DAStatus = 0;
	Freq1 = 0;
	Freq2 = 0;
	NoiseCounter = 0;
	FlipFlop[0] = FlipFlop[1] = 0;
	dcOutput[0] = dcOutput[1] = 0;
	oscCount[0] = oscCount[1] = 0;
	OscReload[0] = OscReload[1] = 0;
	waveForm[0] = waveForm[1] = 0;
	oscStep = 0;
	memset(noise, 0, sizeof(noise));
	channelMask[0] = channelMask[1] = 0;
	vol = 0;
}

void TED::enableChannel(unsigned int channel, bool enable)
{
	channelMask[channel % 3] = enable ? -1 : 0;
}

inline void TED::setFreq(unsigned int channel, int freq)
{
	dcOutput[channel] = (freq == 0x3FE) ? 1 : 0;
	OscReload[channel] = ((freq + 1)&0x3FF) << PRECISION;
}

void TED::oscillatorReset()
{
	FlipFlop[0] = dcOutput[0] = 0;
	FlipFlop[1] = dcOutput[1] = 0;
	oscCount[0] = 0;
	oscCount[1] = 0;
	NoiseCounter = 0;
	Freq1 = Freq2 = 0;
	DAStatus = Snd1Status = Snd2Status = 0;
}

// call only once!
void TED::oscillatorInit()
{
	oscillatorReset();
	/* initialise im with 0xa8 */
	int im = 0xa8;
    for (unsigned int i = 0; i<256; i++) {
		noise[i] = im & 1;
		im = (im<<1)+(1^((im>>7)&1)^((im>>5)&1)^((im>>4)&1)^((im>>1)&1));
    }
	oscStep = (1 << PRECISION) << 0;

	// set player specific parameters
	waveForm[0] = waveForm[1] = 1;
	masterVolume = 8;
	enableChannel(0, true);
	enableChannel(1, true);
	enableChannel(2, true);
}

void TED::writeSoundReg(unsigned int reg, unsigned char value)
{

	switch (reg) {
		case 0:
			Freq1 = (Freq1 & 0x300) | value;
			setFreq(0, Freq1);
			break;
		case 1:
			Freq2 = (Freq2 & 0x300) | value;
			setFreq(1, Freq2);
			break;
		case 2:
			Freq2 = (Freq2 & 0xFF) | (value << 8);
			setFreq(1, Freq2);
			break;
		case 3:
			if ((DAStatus = value & 0x80)) {
				FlipFlop[0] = 1;
				FlipFlop[1] = 1;
				oscCount[0] = OscReload[0];
				oscCount[1] = OscReload[1];
				NoiseCounter = 0xFF;
			}
			Volume = value & 0x0F;
			if (Volume > 8) Volume = 8;
			Volume = (Volume << 8) * masterVolume / 10;
			Snd1Status = value & 0x10;
			Snd2Status = value & 0x20;
			SndNoiseStatus = value & 0x40;
			break;
		case 4:
			Freq1 = (Freq1 & 0xFF) | (value << 8);
			setFreq(0, Freq1);
			break;
	}
}

inline unsigned int TED::waveSquare(unsigned int channel)
{
	return Volume;
}

inline unsigned int TED::waveSawTooth(unsigned int channel)
{
	unsigned int mod;

#if 0
	int msb = OSCRELOADVAL + 1 - OscReload[channel];
	int diff = 2 * msb - int(FlipFlop[channel]) * msb - int(oscCount[channel]) + int(OscReload[channel]);
	//if (diff < 0) diff = 0;
	//if (oscCount[channel] >= 0x3fa) diff = 0;
	mod = (Volume * diff) / (2 * msb);
#else
	int diff = int(oscCount[channel]) - int(OscReload[channel]);
	if (diff < 0) diff = 0;
	mod = (Volume * diff) / (OSCRELOADVAL + 1 - OscReload[channel]);
#endif
	return mod;
}

inline unsigned int TED::waveTriangle(unsigned int channel)
{
	unsigned int mod;
	int msb;

#if 0
	msb = OSCRELOADVAL + 1 - OscReload[channel];
	int diff = FlipFlop[channel] ? int(oscCount[channel]) - int(OscReload[channel]) 
		: int(OSCRELOADVAL) - int(oscCount[channel]);
	//if (diff < 0) diff = 0;
	//if (oscCount[channel] >= 0x3fa) diff = 0;
	mod = (3 * Volume * diff / msb / 2);
#else
	/*
		msb = (OscReload[channel] + OSCRELOADVAL) / 2;
	int diff = oscCount[channel] < msb ? oscCount[channel] - OscReload[channel] : OSCRELOADVAL - oscCount[channel];
	mod = (2 * diff * Volume / (OSCRELOADVAL - OscReload[channel] + 1));
	if (mod > Volume) mod = Volume;
	*/
	msb = (OscReload[channel] + OSCRELOADVAL) / 2;
	mod = oscCount[channel] < msb ? oscCount[channel] : (oscCount[channel] - msb);
	mod = (mod * Volume / msb);
#endif
	return mod;
}

inline unsigned int TED::getWaveSample(unsigned int channel, unsigned int wave)
{
	unsigned int sm;

	switch (wave) {
		default:
		case 1: // square
			return waveSquare(channel);
			break;
		case 2: // sawtooth
			return waveSawTooth(channel);
			break;
		case 4: // triangle
			return waveTriangle(channel);
			break;

		// combined waveforms á la SID
		case 3: // square + sawtooth
			sm = waveSawTooth(channel) + waveSquare(channel);
			return sm /= 2;
			break;
		case 5: // square + triangle
			sm = waveTriangle(channel) + waveSquare(channel);
			return sm /= 2;
			break;
		case 6: // sawtooth + triangle
			sm = waveTriangle(channel) + waveSawTooth(channel);
			return sm /= 2;
			break;
		case 7: // square + sawtooth + triangle
			sm = waveTriangle(channel) + waveSawTooth(channel) + waveSquare(channel);
			return sm /= 3;
			break;
	}
}

void TED::renderSound(unsigned int nrsamples, short *buffer)
{
	// Calculate the buffer...
	if (DAStatus) {// digi?
		short sample = 0;//audiohwspec->silence;
		if (Snd1Status) sample = Volume;
		if (Snd2Status) sample += Volume;
		for (;nrsamples--;) {
			*buffer++ = sample & channelMask[2];
		}
	} else {
		unsigned int result;
		for (;nrsamples--;) {
			// Channel 1
			if (dcOutput[0]) {
				FlipFlop[0] = 1;
			} else if ((oscCount[0] += oscStep) >= OSCRELOADVAL) {
				FlipFlop[0] ^= 1;
				oscCount[0] = OscReload[0] + (oscCount[0] - OSCRELOADVAL);
			}
			// Channel 2
			if (dcOutput[1]) {
				FlipFlop[1] = 1;
			} else if ((oscCount[1] += oscStep) >= OSCRELOADVAL) {
				NoiseCounter = (NoiseCounter + 1) & 0xFF;
				FlipFlop[1] ^= 1;
				oscCount[1] = OscReload[1] + (oscCount[1] - OSCRELOADVAL);
			}
			result = (Snd1Status && FlipFlop[0]) ? (getWaveSample(0, waveForm[0]) & channelMask[0]) : 0;
			if (Snd2Status && FlipFlop[1]) {
				result += getWaveSample(1, waveForm[1]) & channelMask[1];
			} else if (SndNoiseStatus && noise[NoiseCounter] & channelMask[2]) {
				result += Volume;
			}
			*buffer++ = result;
		}   // for
	}
}
//----../src/audiosource/sid.h-----------------------------------------------------------------------------------------------
#define _SID_H

#define SOUND_FREQ_PAL_C64 985248
#define TED_SOUND_CLOCK (221680)

enum { 
	SID6581 = 0,
	SID8580,
	SID8580DB,
	SID6581R1
};

// SID class
class SIDsound 
{
public:
	SIDsound(unsigned int model, unsigned int chnlDisableMask);
	virtual ~SIDsound();
	virtual void reset();
	virtual void setReplayFreq() {
		calcEnvelopeTable();
	};
	void setModel(unsigned int model);
	void setFrequency(unsigned int sid_frequency);
	void setSampleRate(unsigned int sampleRate_);
	void calcEnvelopeTable();
	unsigned char read(unsigned int adr);
	void write(unsigned int adr, unsigned char byte);
	void calcSamples(short *buf, long count);
	void enableDisableChannel(unsigned int ch, bool enabled) {
		voice[ch].disabled = !enabled;
	}

private:

	// SIDsound waveforms
	enum {
		WAVE_NONE, WAVE_TRI, WAVE_SAW, WAVE_TRISAW, WAVE_PULSE, 
		WAVE_TRIPULSE, WAVE_SAWPULSE, WAVE_TRISAWPULSE,	WAVE_NOISE
	};
	// Envelope Genarator states
	enum {
		EG_FROZEN, EG_ATTACK, EG_DECAY, EG_RELEASE
	};
	// Filter types
	enum { 
		FILTER_NONE, FILTER_LP, FILTER_BP, FILTER_LPBP, FILTER_HP, FILTER_NOTCH, FILTER_HPBP, FILTER_ALL
	};
	// Class for SID voices
	class SIDVoice {
	public:
		int wave;				// the selected waveform
		int egState;			// state of the EG
		SIDVoice *modulatedBy;	// the voice that modulates this one
		SIDVoice *modulatesThis;// the voice that is modulated by this one

		unsigned int accu;		// accumulator of the waveform generator, 8.16 fixed
		unsigned int accPrev;	// previous accu value (for ring modulation)
		unsigned int add;		// added to the accumulator for each sample
		unsigned int shiftReg;	// shift register for noise waveform

		unsigned short freq;	// voice frequency
		unsigned short pw;		// pulse-width value

		unsigned int envAttackAdd;
		unsigned int envDecaySub;
		unsigned int envSustainLevel;
		unsigned int envReleaseSub;
		unsigned int envCurrLevel;
		unsigned int envCounter;
		unsigned int envExpCounter;
		unsigned int envCounterCompare;

		unsigned int gate;		// EG gate flag
		unsigned int ring;		// ring modulation flag
		unsigned int test;		// test flag
		unsigned int filter;	// voice filtered flag
		unsigned int muted;		// voice muted flag (only for 3rd voice)
		bool		disabled;	// voice disabled

		// This bit is set for the modulating voice, 
		// not for the modulated one (compared to the real one)
		unsigned int sync; // sync modulation flag
	};
	int volume;			// SID Master volume
	unsigned int sidBaseFreq;	// SID base frequency
	unsigned int sidCyclesPerSampleInt;
	unsigned int clockDeltaRemainder;
	int dcMixer; // different for 6581 and 8580 (constant level output for digi)
	int dcVoice;
	int dcWave;
	int dcDigiBlaster;
	//int extIn;
	//
	unsigned int clock();
	// Wave generator functions
	inline static int waveTriangle(SIDVoice &v);
	inline static int waveSaw(SIDVoice &v);
	inline static int wavePulse(SIDVoice &v);
	inline static int waveTriSaw(SIDVoice &v);
	inline static int waveTriPulse(SIDVoice &v);
	inline static int waveSawPulse(SIDVoice &v);
	inline static int waveTriSawPulse(SIDVoice &v);
	inline static int waveNoise(SIDVoice &v);
	inline static int getWaveSample(SIDVoice &v);
	inline void updateShiftReg(SIDVoice &v);
	// Envelope
	inline int doEnvelopeGenerator(unsigned int cycles, SIDVoice &v);
	static const unsigned int RateCountPeriod[16]; // Factors for A/D/S/R Timing
	static const unsigned char envGenDRdivisors[256]; // For exponential approximation of D/R
	/*static*/ unsigned int masterVolume;
	// voice array for the 3 channels
	SIDVoice voice[3];
	// filter stuff
	unsigned char	filterType; // filter type
	unsigned int	filterCutoff;	// SID filter frequency
	unsigned char	filterResonance;	// filter resonance (0..15)
	double cutOffFreq[2048];	// filter cutoff frequency register
	int resonanceCoeffDiv1024;		// filter resonance * 1024
	int w0;					// filter cutoff freq
	void setResonance();
	void setFilterCutoff();
	int filterOutput(unsigned int cycles, int Vi);
	int Vhp; // highpass
	int Vbp; // bandpass
	int Vlp; // lowpass
	//
	unsigned char lastByteWritten;// Last value written to the SID
	int model_;
	bool enableDigiBlaster;
	unsigned int sampleRate;
};

/*
	Wave outputs
*/
inline int SIDsound::waveTriangle(SIDVoice &v)
{
	unsigned int msb = (v.ring ? v.accu ^ v.modulatedBy->accu : v.accu)
		& 0x8000000;
	// triangle wave 15 bit only
 	return ((msb ? ~v.accu : v.accu) >> 15) & 0xFFE;
}

inline int SIDsound::waveSaw(SIDVoice &v)
{
	return (v.accu >> 16) & 0xFFF;
}

inline int SIDsound::wavePulse(SIDVoice &v)
{
	// square wave starts high
	return (v.test | ((v.accu >> 16) >= v.pw ? 0xFFF : 0x000));
}

inline int SIDsound::waveTriSaw(SIDVoice &v)
{
	unsigned int sm = (waveTriangle(v)) & (waveSaw(v));
	return (sm>>1) & (sm<<1);
}

inline int SIDsound::waveTriPulse(SIDVoice &v)
{
	unsigned int sm = (waveTriangle(v)) & (wavePulse(v));
	return (sm>>1) & (sm<<1);
}

inline int SIDsound::waveSawPulse(SIDVoice &v)
{
	return (waveSaw(v)) & (wavePulse(v));
}

inline int SIDsound::waveTriSawPulse(SIDVoice &v)
{
	unsigned int sm = (waveTriangle(v)) & (waveSaw(v)) & (wavePulse(v));
	return (sm>>1) & (sm<<1);
}
//----../src/audiosource/sid.cpp-----------------------------------------------------------------------------------------------
//  Issues:
//  - Filter cutoff frequencies not 100% accurate
//  - Combined waveforms of the 6581 incorrect (SID card used 8580 anyway)
//  - filter distortion not emulated
//  - no joystick or paddle support
//  - probably many more
#include <memory.h>
#define DIGIBLASTER_MULT 14

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

// Hack to store master volume
//unsigned int SIDsound::masterVolume = 0;

//
//	Random number generator for noise waveform
//

// Test a bit. Returns 1 if bit is set.
inline static long bit(long val, unsigned int bitnr) 
{
	return (val >> bitnr) & 1;
}

inline void SIDsound::updateShiftReg(SIDVoice &v)
{
	unsigned int shiftReg = v.shiftReg;
	unsigned int bit22 = bit(shiftReg,22);
	unsigned int bit17 = bit(shiftReg,17);

	// Shift 1 bit left
	shiftReg = ((shiftReg) << 1);// & 0x7fffff;

	// Feed bit 0 
	v.shiftReg = shiftReg | (bit22 ^ bit17);
}

inline int SIDsound::waveNoise(SIDVoice &v)
{
	unsigned int shiftReg = v.shiftReg;
	// Pick out bits to make output value, left shift by 4
	return 
		(bit(shiftReg,22) << 11) |
		(bit(shiftReg,20) << 10) |
		(bit(shiftReg,16) << 9) |
		(bit(shiftReg,13) << 8) |
		(bit(shiftReg,11) << 7) |
		(bit(shiftReg, 7) << 6) |
		(bit(shiftReg, 4) << 5) |
		(bit(shiftReg, 2) << 4);
};

void SIDsound::setModel(unsigned int model) 
{
	int i;

	switch (model) {
		case SID8580DB:
		case SID8580:
			for ( i=0; i<2048; i++) {
				double x = i / 8.0;
				//double cf = 12500.0 * i / 2048.0; // specs and YAPE
				// approximate with a 3-degree polynomial
				//double cf = 0.0003*x*x*x + 0.0882*x*x + 44.49*x - 38.409;
				// approximate with a 2-degree polynomial
				//double cf = -0.0177*x*x + 55.261*x - 55.518; // CSG 8580R4
				double cf = -0.0156*x*x + 48.473*x - 45.074; // 8580R5
				cutOffFreq[i] = cf <= 0 ? 0 : cf;
			}
			dcWave = 0x800;
			dcMixer = 0;
			dcVoice = 0;
			break;

		case SID6581: // R4 actually
			for (i=0; i<1024; i++) {
				cutOffFreq[i] = (tanh(((double)i/1.5 - 1024.0)/1024.0*M_PI) + tanh(M_PI))
					* (6000.0 - 220.0) + 220.0;
			}
			for (; i<1056; i++) {
				double x = ((double)i - 1024.0) / (1056.0 - 1003.);
				cutOffFreq[i] = x*(1315.0 - 1003.0) + 1003.0;
			}
			for (; i<2048; i++) {
				double x = ((double)i - 1056.0) / (2048.0 - 1056.0);
				cutOffFreq[i] = //(tanh (((double)i - 2048.0)/1024.0*M_PI) + tanh(M_PI))
					//* (20163.0 - 1315.0) + 1315.0;
					(20163.0 - 1315.0) * x + 1315.0;
			}
			dcWave = 0x380;
			dcMixer = -0xFFF*0xFF/18 >> 7;
			dcVoice = 0x800*0xFF;
			break;

		case SID6581R1: // 6581 R1
			for (i=0; i<1024; i++) { 
				cutOffFreq[i] = (tanh(((double)i-1024.0)/1024.0*M_PI) + tanh(M_PI))
					* (6000.0 - 220.0) + 220.0;
			}
			for (; i<2048; i++) { 
				cutOffFreq[i] = (tanh (((double)i-2048.0)/1024.0*M_PI) + tanh(M_PI))
					* (18000.0 - 4600.0) + 4600.0;
			}
			dcWave = 0x380;
			dcMixer = -0xFFF*0xFF/18 >> 7;
			dcVoice = 0x800*0xFF;
			break;
	}
	setFilterCutoff();
	model_ = model;
}

// Static data members
const unsigned int SIDsound::RateCountPeriod[16] = {
	0x7F00,0x0006,0x003C,0x0330,0x20C0,0x6755,0x3800,0x500E,
	0x1212,0x0222,0x1848,0x59B8,0x3840,0x77E2,0x7625,0x0A93
};

const unsigned char SIDsound::envGenDRdivisors[256] = {
	30,30,30,30,30,30,16,16,16,16,16,16,16,16,8,8, 
	8,8,8,8,8,8,8,8,8,8,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

void SIDsound::calcEnvelopeTable()
{
	// number of SIDsound envelope clocks per sample (0x1FFFFF)
	const double deltaSampleCyclesFloat = ((double) sidBaseFreq * 256.0) / (double)sampleRate;
	sidCyclesPerSampleInt = (unsigned int) (deltaSampleCyclesFloat + 0.5);
}

void SIDsound::setFrequency(unsigned int sid_frequency)
{
	switch (sid_frequency) {
		case 0:
			sidBaseFreq = TED_SOUND_CLOCK * 4; // 312 * 114 * 50 / 2;
			break;
		default:
			sidBaseFreq = SOUND_FREQ_PAL_C64;
			break;
	}
	calcEnvelopeTable();
}

void SIDsound::setSampleRate(unsigned int sampleRate_)
{
	sampleRate = sampleRate_;
	calcEnvelopeTable();
}

SIDsound::SIDsound(unsigned int model, unsigned int chnlDisableMask) : enableDigiBlaster(false)
{
	unsigned int i;
	masterVolume = 0;

	// Link voices together
	for (i=0; i<3; i++) {
		voice[i].modulatedBy = &voice[(i+2)%3]; // previous voice
		voice[i].modulatesThis = &voice[(i+1)%3]; // next voice
		voice[i].disabled = !!((chnlDisableMask >> i) & 1);
	}

	filterCutoff = 0;
	setModel(model);
	setFrequency(0);
	reset();
}

void SIDsound::reset(void)
{
	volume = masterVolume;

	lastByteWritten = 0;

	for (int v=0; v<3; v++) {
		voice[v].wave = WAVE_NONE;
		voice[v].egState = EG_FROZEN;
		voice[v].accu = voice[v].add = 0;
		voice[v].freq = voice[v].pw = 0;
		voice[v].envCurrLevel = voice[v].envSustainLevel = 0;
		voice[v].gate = voice[v].ring = voice[v].test = 0;
		voice[v].filter = voice[v].sync = false;
		voice[v].muted = 0;
		// Initial value of internal shift register
		voice[v].shiftReg = 0x7FFFFC;
		voice[v].envExpCounter = 0;
		voice[v].envAttackAdd = voice[v].envDecaySub = voice[v].envReleaseSub = 0;
		voice[v].envCounterCompare = RateCountPeriod[0];
		voice[v].envCounter = 0x7fff;
	}

	filterType = FILTER_NONE;
	filterCutoff = filterResonance = 0;

	Vhp = Vbp = Vlp = 0;
	setFilterCutoff();
	setResonance();

	dcDigiBlaster = 0;
	clockDeltaRemainder = 0;
}

inline int SIDsound::getWaveSample(SIDVoice &v)
{
	switch (v.wave) {
		case WAVE_TRI:
			return waveTriangle(v);
		case WAVE_SAW:
			return waveSaw(v);
		case WAVE_PULSE:
			return wavePulse(v);
		case WAVE_TRISAW:
			return waveTriSaw(v);
		case WAVE_TRIPULSE:
			return waveTriPulse(v);
		case WAVE_SAWPULSE:
			return waveSawPulse(v);
		case WAVE_TRISAWPULSE:
			return waveTriSawPulse(v);
		case WAVE_NOISE:
			return waveNoise(v);
		default:
			return 0x800;
	}
}

unsigned char SIDsound::read(unsigned int adr)
{
	switch(adr) {
		case 0x19:
		case 0x1A:
			// POTX/POTY paddle AD converters (unemulated)
			lastByteWritten = 0;
			return 0xFF;

		// Voice 3 (only) oscillator readout
		case 0x1B:
			lastByteWritten = 0;
			return (unsigned char)(getWaveSample(voice[2])>>0); // 4?

		// Voice 3 EG readout
		case 0x1C:
			return (unsigned char)(voice[2].envCurrLevel);

		case 0x1E: // Digiblaster DAC readout
			if (enableDigiBlaster && model_ == SID8580)
			{
				return (unsigned char) (dcDigiBlaster >> DIGIBLASTER_MULT);
			}
			return lastByteWritten;

		default:
			// Write-only registers return the last value written
			return lastByteWritten;
	}
}

void SIDsound::write(unsigned int adr, unsigned char value)
{	
	lastByteWritten = value;

	SIDVoice &v = voice[adr/7];
	switch (adr) {
		case 0:
		case 7:
		case 14:
			v.freq = (unsigned short)((v.freq & 0xff00) | value);
			v.add = (unsigned int)(((double)v.freq 
				* sidBaseFreq) * 16.0 / sampleRate + 0.5);
			break;

		case 1:
		case 8:
		case 15:
			v.freq = (unsigned short)((v.freq & 0xff) | (value << 8));
			v.add = (unsigned int)(((double)v.freq 
				* sidBaseFreq) * 16.0 / sampleRate + 0.5);
			break;

		case 2:
		case 9:
		case 16:
			v.pw = (unsigned short)((v.pw & 0x0f00) | value);
			break;

		case 3:
		case 10:
		case 17:
			v.pw = (unsigned short)((v.pw & 0xff) | ((value & 0xf) << 8));
			break;

		case 4:
		case 11:
		case 18:
			if ((value & 1) != (unsigned char) v.gate) {
				if (value & 1) {
					// gate on
					v.egState = EG_ATTACK;
					v.envCounterCompare = v.envAttackAdd;
				} else {
					// gate off
#if 00
					if (v.egState != EG_FROZEN)
#endif
						v.egState = EG_RELEASE;
					v.envCounterCompare = v.envReleaseSub;
				}
				v.gate = value & 1;
			}
			v.modulatedBy->sync = value & 2;
			v.ring = value & 4;
			if ((value & 8) && !v.test) {
				v.accu = 0; //(model_ >= SID8580) ? 0 : 0;
				unsigned int bit19 = (v.shiftReg >> 19) & 1;
				v.shiftReg = (v.shiftReg & 0x7ffffd) | ((bit19^1) << 1);
				v.test = 0xFFF;
			} else if (v.test && !(value & 8)) {
				unsigned int bit0 = ((v.shiftReg >> 22) ^ (v.shiftReg >> 17)) & 0x1;
				v.shiftReg <<= 1;
				v.shiftReg &= 0x7fffff;
				v.shiftReg |= bit0;
				v.test = 0x000;
			}
			v.wave = (value >> 4) & 0x0F;
			if (v.wave > 8) {
				v.shiftReg &= 0x7fffff^(1<<22)^(1<<20)^(1<<16)^(1<<13)^(1<<11)^(1<<7)^(1<<4)^(1<<2);
			}
			break;

		case 5:
		case 12:
		case 19:
			v.envAttackAdd = value >> 4;
			v.envDecaySub = value & 0x0F;
			if (v.egState == EG_ATTACK)
				v.envCounterCompare = v.envAttackAdd;
			else if (v.egState == EG_DECAY)
				v.envCounterCompare = v.envDecaySub;
			break;

		case 6:
		case 13:
		case 20:
			v.envSustainLevel = (value >> 4) * 0x11;
			v.envReleaseSub = value & 0x0F;
			if (v.egState == EG_RELEASE)
				v.envCounterCompare = v.envReleaseSub;
			break;

		case 21:
			if ((unsigned int)(value&7) != (filterCutoff&7)) {
				filterCutoff = (value&7)|(filterCutoff&0x7F8);
				setFilterCutoff();
			}
			break;

		case 22:
			filterCutoff = (value<<3)|(filterCutoff&7);
			setFilterCutoff();
			break;

		case 23:
			voice[0].filter = value & 1;
			voice[1].filter = value & 2;
			voice[2].filter = value & 4;
			filterResonance = (unsigned char)(value >> 4);
			setResonance();
			break;

		case 24:
			volume = value & 0x0F;
			voice[2].muted = value & 0x80;
			filterType = (unsigned char)((value >> 4) & 7);
			break;

		case 30: // Digiblaster DAC
			if (enableDigiBlaster && model_ == SID8580)
			{
				dcDigiBlaster = (value ^ 0x00) << DIGIBLASTER_MULT;
			}
			break;

		case 31: // Digiblaster ADC
			break;
	}
}

inline void SIDsound::setFilterCutoff()
{
	const double freqDomainDivCoeff = 2 * M_PI * 1.048576;
	w0 = int(cutOffFreq[filterCutoff] * freqDomainDivCoeff);
	// Limit cutoff to Nyquist frq to keep the sample based filter stable
	const double NyquistFrq = double(sampleRate) / 2;
	const double maxCutOff = NyquistFrq > 16000.0 ? 16000.0 : NyquistFrq;
	const int w0MaxDt = int(maxCutOff * freqDomainDivCoeff); // 16000
	if (w0 > w0MaxDt) w0 = w0MaxDt;
}

inline void SIDsound::setResonance()
{
	resonanceCoeffDiv1024 = (int) (1024.0/(0.707 + 1.9 * (double) filterResonance / 15.0) + 0.5); // 2.3
}

inline unsigned int SIDsound::clock()
{
	unsigned int count = sidCyclesPerSampleInt >> 8;
	unsigned int tmp = sidCyclesPerSampleInt & 0xFF;
	unsigned int newCount = clockDeltaRemainder + tmp;
	
	if (newCount >= 0x100) {
		clockDeltaRemainder = newCount & 0xFF;
		count++;
	} else {
		clockDeltaRemainder = newCount;
	}
	return count;
}

// simplified version of http://bel.fi/~alankila/c64-sw/index-cpp.html
inline int SIDsound::filterOutput(unsigned int cycles, int Vi)
{
	int w0deltaTime = w0 >> 6;
	Vi >>= 7;
	unsigned int count = cycles;

	do {
		int dVlp = (w0deltaTime * Vbp >> 14);
		Vlp -= dVlp;
		int dVbp = (w0deltaTime * Vhp >> 14);
		Vbp -= dVbp;
		Vhp = (Vbp * resonanceCoeffDiv1024 >> 10) - Vlp - Vi;
	} while (--count);

	int Vf;

	switch (filterType) {
		default:
		case FILTER_NONE:
			Vf = 0;
			break;
		case FILTER_LP:
			Vf = Vlp;
			break;
		case FILTER_BP:
			Vf = Vbp;
			break;
		case FILTER_LPBP:
			Vf = Vlp + Vbp;
			break;
		case FILTER_HP:
			Vf = Vhp;
			break;
		case FILTER_NOTCH:
			Vf = Vlp + Vhp;
			break;
		case FILTER_HPBP:
			Vf = Vbp + Vhp;
			break;
		case FILTER_ALL:
			Vf = Vlp + Vbp + Vhp;
			break;
	}
	return Vf << 7;
}

// Envelope based on:
// http://blog.kevtris.org/?p=13
inline int SIDsound::doEnvelopeGenerator(unsigned int cycles, SIDVoice &v)
{
	unsigned int count = cycles;

	do {
		unsigned int LFSR = v.envCounter;
		if (LFSR != RateCountPeriod[v.envCounterCompare]) {
			const unsigned int feedback = ((LFSR >> 14) ^ (LFSR >> 13)) & 1;
			LFSR = ((LFSR << 1) | feedback) & 0x7FFF;
			v.envCounter = LFSR;
		} else {
			// LFSR = 0x7fff reset LFSR
			v.envCounter = 0x7fff;

			if (v.egState == EG_ATTACK || ++v.envExpCounter == envGenDRdivisors[v.envCurrLevel]) {

				v.envExpCounter = 0;

				switch (v.egState) {

				case EG_ATTACK:
					// According to Bob Yannes, Attack is linear...
					if ( ((++v.envCurrLevel) & 0xFF) == 0xFF) {
						v.egState = EG_DECAY;
						v.envCounterCompare = v.envDecaySub;
					}
					break;

				case EG_DECAY:
					if (v.envCurrLevel != v.envSustainLevel) {
						--v.envCurrLevel &= 0xFF;
						if (!v.envCurrLevel)
							v.egState = EG_FROZEN;
					}
					break;

				case EG_RELEASE:
					v.envCurrLevel = (v.envCurrLevel - 1) & 0xFF;
					if (!v.envCurrLevel)
						v.egState = EG_FROZEN;
					break;

				case EG_FROZEN:
					v.envCurrLevel = 0;
					break;
				}
			}
		}
	} while (--count);

	return v.envCurrLevel & 0xFF; // envelope is 8 bits
}

void SIDsound::calcSamples(short *buf, long accu)
{
	for (;accu--;) {
		// Outputs for normal and filtered sounds
		int sumFilteredOutput = 0;
		int sumOutput = 0;
		int sample;

		const unsigned int cyclesToDo = clock();
		// Loop for the three voices
		int j = 2;
		do {
			SIDVoice &v = voice[j];
			int envelope = doEnvelopeGenerator(cyclesToDo, v);
			// Waveform generator
			if (!v.test) {
#if 1
				unsigned int accPrev = v.accu;
				// Update accumulator
				v.accu += v.add;
				// FIXME Apply ring modulation.
				if (v.sync && !(accPrev & 0x8000000) && (v.accu & 0x8000000)
           			)
#else
				v.accPrev = v.accu;
				// Update accumulator if test bit not set
				v.accu += v.add;
				unsigned int accPrev = v.accPrev;
				if (v.sync && !(v.accPrev & 0x8000000) && (v.accu & 0x8000000)
    				&& !( v.modulatedBy->sync && !(v.modulatedBy->accPrev & 0x800000) && 
        			(v.modulatedBy->accu & 0x800000)) 
           			)
#endif
					v.modulatesThis->accu = 0;
				if (v.freq) {
					unsigned int accNext = accPrev;
					unsigned int freq = v.freq << 4;
					do {
						accNext += freq;
						// noise shift register is updating even when waveform is not selected
						if (!(accPrev & 0x0800000) && (accNext & 0x0800000))
							updateShiftReg(v);
						accPrev = accNext;
					} while ( accNext < v.accu );
				}
				// real accu is 24 bit but we use FP integer arithmetic
				v.accu &= 0xFFFFFFF;
			}
			int output = v.disabled ? 0x0800 : getWaveSample(v);

			if (v.filter)
				sumFilteredOutput += (output - dcWave) * envelope + dcVoice;
			else {
				if (v.muted)
					sumOutput += (0x0800 - dcWave) * envelope + dcVoice;
				else
					sumOutput += (output - dcWave) * envelope + dcVoice;
			}
		} while (j--);

		int accu = (sumOutput + filterOutput(cyclesToDo, sumFilteredOutput) 
			+ dcMixer + dcDigiBlaster) * volume;

#if 1
		sample = accu >> 12;
#else
		unsigned int interPolationFac = (clockDeltaRemainder - sidCyclesPerSampleInt) & 0xFF;
		accu >>= 7;
		sample = (prevAccu * (0xFF ^ interPolationFac) + accu * (interPolationFac)) >> 12;
		prevAccu = accu;
#endif

		*buf++ = (short) sample;
	}
}

SIDsound::~SIDsound()
{
	masterVolume = volume;
}
//----../src/audiosource/tedsid/soloud_tedsid.cpp-----------------------------------------------------------------------------------------------
namespace SoLoud
{

	TedSidInstance::TedSidInstance(TedSid *aParent)
	{
		mParent = aParent;
		mSampleCount = 0;
		mSID = new SIDsound(mParent->mModel, 0);
		mSID->setFrequency(0);
		mSID->setSampleRate(TED_SOUND_CLOCK);		
		mSID->setFrequency(1);

		mTED = new TED();
		mTED->oscillatorInit();

		mNextReg = 100; // NOP
		mNextVal = 0;
		int i;
		for (i = 0; i < 128; i++)
			mRegValues[i] = 0;
	}

	void TedSidInstance::getAudio(float *aBuffer, unsigned int aSamples)
	{
		unsigned int i;
		for (i = 0; i < aSamples; i++)
		{
		    tick();
			short sample;
			mSID->calcSamples(&sample, 1);
			short tedsample = 0;
			mTED->renderSound(1, &tedsample);
			aBuffer[i] = (sample + tedsample) / 8192.0f;
			mSampleCount--;
		}
	}
	
	void TedSidInstance::tick()
	{
	    if (mParent->mFile == 0)
	        return;

		while (mSampleCount == 0)
		{
			mRegValues[mNextReg] = mNextVal;
			if (mNextReg < 64)
			{
				mSID->write(mNextReg, mNextVal);
			}
			else
			if (mNextReg < 64 + 5)
			{
				mTED->writeSoundReg(mNextReg - 64, mNextVal);
			}
//			mSampleCount = mParent->mFile->read16();
			mNextVal = mParent->mFile->read8();
			mNextReg = mParent->mFile->read8();
			if (mNextReg & 0x80)
			{
				// timestamp!
				mSampleCount = ((int)(mNextReg & 0x7f) << 8) | mNextVal;
				mNextVal = mParent->mFile->read8();
				mNextReg = mParent->mFile->read8();
			}
			if (mParent->mFile->eof())
				mParent->mFile->seek(8);
		}
	}

	float TedSidInstance::getInfo(unsigned int aInfoKey)
	{
		return (float)mRegValues[aInfoKey & 127];
	}

	bool TedSidInstance::hasEnded()
	{
		return 0;
	}

	TedSidInstance::~TedSidInstance()
	{
		delete mSID;
		delete mTED;
	}

	TedSid::TedSid()
	{
		mBaseSamplerate = TED_SOUND_CLOCK;
		mChannels = 1;
		mFile = 0;
		mFileOwned = false;
	}

	TedSid::~TedSid()
	{
		stop();
		if (mFileOwned)
			delete mFile;
	}

	result TedSid::loadMem(unsigned char *aMem, unsigned int aLength, bool aCopy, bool aTakeOwnership)
	{
		if (!aMem || aLength == 0)
			return INVALID_PARAMETER;
		MemoryFile *mf = new MemoryFile;
		if (!mf)
			return OUT_OF_MEMORY;
		int res = mf->openMem(aMem, aLength, aCopy, aTakeOwnership);
		if (res != SO_NO_ERROR)
		{
			delete mf;
			return res;
		}
		res = loadFile(mf);
		if (res != SO_NO_ERROR)
		{
			delete mf;
			return res;
		}
		mFileOwned = aCopy || aTakeOwnership;

		return SO_NO_ERROR;
	}

	result TedSid::load(const char *aFilename)
	{
		if (!aFilename)
			return INVALID_PARAMETER;
		DiskFile *df = new DiskFile;
		if (!df) return OUT_OF_MEMORY;
		int res = df->open(aFilename);
		if (res != SO_NO_ERROR)
			return res;
		res = loadFile(df);
		if (res != SO_NO_ERROR)
		{
			delete df;
			return res;
		}
		mFileOwned = true;				
		return SO_NO_ERROR;
	}

	result TedSid::loadToMem(const char *aFilename)
	{
		if (!aFilename)
			return INVALID_PARAMETER;
		MemoryFile *mf = new MemoryFile;
		if (!mf) return OUT_OF_MEMORY;
		int res = mf->openToMem(aFilename);
		if (res != SO_NO_ERROR)
		{
			delete mf;
			return res;
		}
		res = loadFile(mf);
		if (res != SO_NO_ERROR)
		{
			delete mf;
			return res;
		}
		mFileOwned = true;
		return SO_NO_ERROR;
	}

	result TedSid::loadFileToMem(File *aFile)
	{
		if (!aFile)
			return INVALID_PARAMETER;
		MemoryFile *mf = new MemoryFile;
		if (!mf) return OUT_OF_MEMORY;
		int res = mf->openFileToMem(aFile);
		if (res != SO_NO_ERROR)
		{
			delete mf;
			return res;
		}
		res = loadFile(mf);
		if (res != SO_NO_ERROR)
		{
			delete mf;
			return res;
		}
		mFileOwned = true;
		return SO_NO_ERROR;
	}

	result TedSid::loadFile(File *aFile)
	{
		if (aFile == NULL)
			return INVALID_PARAMETER;
		if (mFileOwned)
			delete mFile;
		// Expect a file wih header and at least one reg write
		if (aFile->length() < 4+4+2+2) return FILE_LOAD_FAILED;

		aFile->seek(0);
		if (aFile->read8() != 'D') return FILE_LOAD_FAILED;
		if (aFile->read8() != 'u') return FILE_LOAD_FAILED;
		if (aFile->read8() != 'm') return FILE_LOAD_FAILED;
		if (aFile->read8() != 'p') return FILE_LOAD_FAILED;
		if (aFile->read8() != 0) return FILE_LOAD_FAILED;
		mModel = aFile->read8();
		aFile->seek(8);

		mFile = aFile;
		mFileOwned = false;


		return SO_NO_ERROR;
	}


	AudioSourceInstance * TedSid::createInstance() 
	{
		return new TedSidInstance(this);
	}

};
#endif //YES_IMGUISOLOUD_TEDSID

//----../src/audiosource/vic/soloud_vic.cpp-----------------------------------------------------------------------------------------------
#ifdef YES_IMGUISOLOUD_VIC
namespace SoLoud
{

	VicInstance::VicInstance(Vic *aParent)
	{
		m_parent = aParent;

		for(int i = 0; i < 4; i++)
			m_phase[i] = 0;

		m_noisePos = 0;
	}

	VicInstance::~VicInstance()
	{
	}

	void VicInstance::getAudio(float *aBuffer, unsigned int aSamples)
	{
		unsigned int phaseAdder[4] = { 0, 0, 0, 0 };
		for(int i = 0; i < 4; i++)
		{
			unsigned char reg = m_parent->getRegister(i);
			if(reg >= 128)
			{
				float freq = m_parent->m_clocks[i] / (float)(reg < 255 ? 255 - reg : 1);
				phaseAdder[i] = (unsigned int)(freq * 65536.0f / 44100.0f + 0.5f);
			}
		}

		for(int i = 0; i < (signed)aSamples; i++)
		{
			float s = 0.0f;

			// square waves
			for(int v = 0; v < 3; v++)
			{
				if(phaseAdder[v] != 0)
				{
					s += (m_phase[v] < 32768 ? 0.5f : -0.5f);
					m_phase[v] = (m_phase[v] + phaseAdder[v]) & 65535;
				}
			}

			// noise
			if(phaseAdder[3] != 0)
			{
				s += (float)m_parent->m_noise[m_noisePos] / 255.0f - 0.5f;

				m_phase[3] += phaseAdder[3];

				if(m_phase[3] >= 32768)
				{
					m_noisePos = (m_noisePos + 1) & 8191;
					m_phase[3] &= 32767;
				}
			}

			aBuffer[i] = s / 4.0f;
		}
	}

	bool VicInstance::hasEnded()
	{
		return false;
	}

	Vic::Vic()
	{
		mBaseSamplerate = 44100;
		setModel(PAL);

		for(int i = 0; i < MAX_REGS; i++)
			m_regs[i] = 0;

		// Galois LFSR (source: https://en.wikipedia.org/wiki/Linear_feedback_shift_register)
	    unsigned short lfsr = 0xACE1u;
		for(int i = 0; i < 8192; i++)
		{
		    unsigned lsb = lfsr & 1;
		    lfsr >>= 1;
            //lfsr ^= (-lsb) & 0xB400u;
            lfsr ^= (unsigned)(-(signed)lsb) & 0xB400u;
		    m_noise[i] = (lfsr & 0xff) ^ (lfsr >> 8);
		}
	}

	Vic::~Vic()
	{
		stop();
	}

	void Vic::setModel(int model)
	{
		m_model = model;

		switch(model)
		{
		case PAL:
			m_clocks[0] = 4329.0f;
			m_clocks[1] = 8659.0f;
			m_clocks[2] = 17320.0f;
			m_clocks[3] = 34640.0f;
			break;

		case NTSC:
			m_clocks[0] = 3995.0f;
			m_clocks[1] = 7990.0f;
			m_clocks[2] = 15980.0f;
			m_clocks[3] = 31960.0f;
			break;
		}
	}

	int Vic::getModel() const
	{
		return m_model;
	}

	AudioSourceInstance * Vic::createInstance() 
	{
		return new VicInstance(this);
	}

};
#endif //YES_IMGUISOLOUD_VIC

//----demos/piano/soloud_basicwave.cpp--------------------------------------------------------------------------------
#ifndef NO_IMGUISOLOUD_BASICWAVE
inline static float my_fabs(float x)    {return (x<0) ? (-x) : x;}

namespace SoLoud
{

    BasicwaveInstance::BasicwaveInstance(Basicwave *aParent)
    {
        mParent = aParent;
        mOffset = 0;
    }

    void BasicwaveInstance::getAudio(float *aBuffer, unsigned int aSamples)
    {
        unsigned int i;
        switch (mParent->mWaveform)
        {
            case Basicwave::SINE:
                for (i = 0; i < aSamples; i++)
                {
                    aBuffer[i] = (float)sin(mParent->mFreq * mOffset * M_PI * 2);
                    mOffset++;
                }
                break;
            case Basicwave::SAW:
                for (i = 0; i < aSamples; i++)
                {
                    aBuffer[i] = (1 - (float)fmod(mParent->mFreq * mOffset, 1)) * 2 - 1;
                    mOffset++;
                }
                break;
            case Basicwave::INVERSESAW:
                for (i = 0; i < aSamples; i++)
                {
                    aBuffer[i] = ((float)fmod(mParent->mFreq * mOffset, 1)) * 2 - 1;
                    mOffset++;
                }
                break;
            case Basicwave::SQUARE:
                for (i = 0; i < aSamples; i++)
                {
                    aBuffer[i] = ((float)fmod(mParent->mFreq * mOffset, 1.0f) > 0.5f) ? -1.0f : 1.0f;
                    mOffset++;
                }
                break;
            case Basicwave::TRIANGLE:
                for (i = 0; i < aSamples; i++)
                {
                    aBuffer[i] = my_fabs(0.5f - (float)fmod(mParent->mFreq * mOffset, 1)) * 4 - 1;
                    mOffset++;
                }
                break;
        }
    }

    bool BasicwaveInstance::hasEnded()
    {
        // This audio source never ends.
        return 0;
    }

    Basicwave::Basicwave()
    {
        setSamplerate(44100);
        mWaveform = SQUARE;
    }

    Basicwave::~Basicwave()
    {
        stop();
    }

    void Basicwave::setSamplerate(float aSamplerate)
    {
        mBaseSamplerate = aSamplerate;
        mFreq = (float)(440 / mBaseSamplerate);
    }

    void Basicwave::setWaveform(int aWaveform)
    {
        mWaveform = aWaveform;
    }

    AudioSourceInstance * Basicwave::createInstance()
    {
        return new BasicwaveInstance(this);
    }

};
#endif //NO_IMGUISOLOUD_BASICWAVE

//----../src/audiosource/speech/soloud_speech.cpp-----------------------------------------------------------------------
#ifdef YES_IMGUISOLOUD_SPEECH
//----../src/audiosource/speech/resonator.cpp-----------------------------------------------------------------------
#ifndef PI
#define PI 3.1415926535897932384626433832795f
#endif

/* Convert formant freqencies and bandwidth into resonator difference equation coefficents
	*/
void resonator::initResonator(
	int aFrequency,                       /* Frequency of resonator in Hz  */
	int aBandwidth,                      /* Bandwidth of resonator in Hz  */
	int aSamplerate)
{
	float arg = (-PI / aSamplerate) * aBandwidth;
	float r = (float)exp(arg);  
	mC = -(r * r);             
	arg = (-2.0f * PI / aSamplerate) * aFrequency;
	mB = r * (float)cos(arg) * 2.0f;   
	mA = 1.0f - mB - mC;    
}

/* Convert formant freqencies and bandwidth into anti-resonator difference equation constants
	*/
void resonator::initAntiresonator(
	int aFrequency,                       /* Frequency of resonator in Hz  */
	int aBandwidth,                      /* Bandwidth of resonator in Hz  */
	int aSamplerate)
{
	initResonator(aFrequency, aBandwidth, aSamplerate); /* First compute ordinary resonator coefficients */
	/* Now convert to antiresonator coefficients */
	mA = 1.0f / mA;             /* a'=  1/a */
	mB *= -mA;                  /* b'= -b/a */
	mC *= -mA;                  /* c'= -c/a */
}

/* Generic resonator function */
float resonator::resonate(float input)
{
	float x = mA * input + mB * mP1 + mC * mP2;
	mP2 = mP1;
	mP1 = x;
	return x;
}

/* Generic anti-resonator function
	Same as resonator except that a,b,c need to be set with initAntiresonator()
	and we save inputs in p1/p2 rather than outputs.
	There is currently only one of these - "mNasalZero"
*/
/*  Output = (mNasalZero.a * input) + (mNasalZero.b * oldin1) + (mNasalZero.c * oldin2) */

float resonator::antiresonate(float input)
{
	float x = mA * input + mB * mP1 + mC * mP2;
	mP2 = mP1;
	mP1 = input;
	return x;
}

resonator::resonator()
{
	mA = mB = mC = mP1 = mP2 = 0;
}

resonator::~resonator()
{
}

void resonator::setGain(float aG)
{
	mA *= aG;
}
//----../src/audiosource/speech/darray.cpp-----------------------------------------------------------------------
darray::darray()
{
	mAllocChunk = 128;
	mAllocated = mUsed = 0;
	mData = NULL;
}

void darray::clear()
{
	free(mData);
	mAllocChunk = 128;
	mAllocated = mUsed = 0;
	mData = NULL;
}

darray::~darray()
{
	clear();
}

char * darray::getDataInPos(int aPosition)
{
	if (aPosition < mAllocated && aPosition < mUsed)
		return mData + aPosition;

	if (aPosition >= mAllocated)
	{
		int newsize = mAllocated;

		while (newsize <= aPosition)
		{
			newsize += mAllocChunk;
			mAllocChunk *= 2;
		}

		char *newdata = (char*)realloc(mData, newsize);
		if (!newdata)
		{
			free(mData);
			mData = NULL;
			mAllocated = mUsed = 0;
			return NULL;
		}
        else memset(newdata + mAllocated, 0, newsize - mAllocated); //
        //memset(newdata,0,newsize);	// nope

		mData = newdata;
		mAllocated = newsize;			
	}

	if (aPosition >= mUsed)
	{
		mUsed = aPosition + 1;
	}

	return mData + aPosition;
}

void darray::put(int aData)
{
	char *s = getDataInPos(mUsed);

	*s = aData;
}
//----../src/audiosource/speech/klatt.cpp--[&& Elements.def]-----------------------------------------------------------
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces" // warning : suggest braces aronud initialization of subobject
#endif

#ifndef PI
#define PI 3.1415926535897932384626433832795f
#endif

#ifndef NULL
#define NULL 0
#endif

class Interp
{
public:
	float mSteady;
	float mFixed;
	char  mProportion;
	char  mExtDelay;
	char  mIntDelay;
};


enum Eparm_e
{
  ELM_FN, ELM_F1, ELM_F2, ELM_F3, 
  ELM_B1, ELM_B2, ELM_B3, ELM_AN, 
  ELM_A1, ELM_A2, ELM_A3, ELM_A4, 
  ELM_A5, ELM_A6, ELM_AB, ELM_AV, 
  ELM_AVC, ELM_ASP, ELM_AF, 
  ELM_COUNT
};
 
class Element
{
public:
	  const char *mName; // unused
	  const char mRK;
	  const char mDU;
	  const char mUD;
	  unsigned char mFont; // unused
	  const char  *mDict; // unused
	  const char  *mIpa; // unused
	  int   mFeat; // only ELM_FEATURE_VWL
	  Interp mInterpolator[ELM_COUNT];
 };

enum ELEMENT_FEATURES
{
	ELM_FEATURE_ALV = 0x00000001,
	ELM_FEATURE_APR = 0x00000002,
	ELM_FEATURE_BCK = 0x00000004,
	ELM_FEATURE_BLB = 0x00000008,
	ELM_FEATURE_CNT = 0x00000010,
	ELM_FEATURE_DNT = 0x00000020,
	ELM_FEATURE_FNT = 0x00000040,
	ELM_FEATURE_FRC = 0x00000080,
	ELM_FEATURE_GLT = 0x00000100,
	ELM_FEATURE_HGH = 0x00000200,
	ELM_FEATURE_LAT = 0x00000400,
	ELM_FEATURE_LBD = 0x00000800,
	ELM_FEATURE_LBV = 0x00001000,
	ELM_FEATURE_LMD = 0x00002000,
	ELM_FEATURE_LOW = 0x00004000,
	ELM_FEATURE_MDL = 0x00008000,
	ELM_FEATURE_NAS = 0x00010000,
	ELM_FEATURE_PAL = 0x00020000,
	ELM_FEATURE_PLA = 0x00040000,
	ELM_FEATURE_RND = 0x00080000,
	ELM_FEATURE_RZD = 0x00100000,
	ELM_FEATURE_SMH = 0x00200000,
	ELM_FEATURE_STP = 0x00400000,
	ELM_FEATURE_UMD = 0x00800000,
	ELM_FEATURE_UNR = 0x01000000,
	ELM_FEATURE_VCD = 0x02000000,
	ELM_FEATURE_VEL = 0x04000000,
	ELM_FEATURE_VLS = 0x08000000,
	ELM_FEATURE_VWL = 0x10000000
};

enum ELEMENTS 
{
	ELM_END = 0,	
	ELM_Q,	ELM_P,	ELM_PY,	ELM_PZ,	ELM_T,	ELM_TY,	
	ELM_TZ,	ELM_K,	ELM_KY,	ELM_KZ,	ELM_B,	ELM_BY,	ELM_BZ,	
	ELM_D,	ELM_DY,	ELM_DZ,	ELM_G,	ELM_GY,	ELM_GZ,	ELM_M,	
	ELM_N,	ELM_NG,	ELM_F,	ELM_TH,	ELM_S,	ELM_SH,	ELM_X,
	ELM_H,	ELM_V,	ELM_QQ,	ELM_DH,	ELM_DI,	ELM_Z,	ELM_ZZ,
	ELM_ZH,	ELM_CH,	ELM_CI,	ELM_J,	ELM_JY,	ELM_L,	ELM_LL,
	ELM_RX,	ELM_R,	ELM_W,	ELM_Y,	ELM_I,	ELM_E,	ELM_AA,
	ELM_U,	ELM_O,	ELM_OO,	ELM_A,	ELM_EE,	ELM_ER,	ELM_AR,
	ELM_AW,	ELM_UU,	ELM_AI,	ELM_IE,	ELM_OI,	ELM_OU,	ELM_OV,
	ELM_OA,	ELM_IA,	ELM_IB,	ELM_AIR,ELM_OOR,ELM_OR
};

#define PHONEME_COUNT 52
#define AMP_ADJ 14
#define StressDur(e,s) (s,((e->mDU + e->mUD)/2))




class PhonemeToElements 
{
public:
	int mKey;
	char mData[8];
};

/* Order is important - 2 byte phonemes first, otherwise
   the search function will fail*/
static PhonemeToElements phoneme_to_elements[PHONEME_COUNT] = 
{
	/* mKey, count, 0-7 elements */
/* tS */ 0x5374, 2, ELM_CH, ELM_CI, 0, 0, 0, 0, 0,
/* dZ */ 0x5a64, 4, ELM_J, ELM_JY, ELM_QQ, ELM_JY, 0, 0, 0,
/* rr */ 0x7272, 3, ELM_R, ELM_QQ, ELM_R, 0, 0, 0, 0,
/* eI */ 0x4965, 2, ELM_AI, ELM_I, 0, 0, 0, 0, 0,
/* aI */ 0x4961, 2, ELM_IE, ELM_I, 0, 0, 0, 0, 0,
/* oI */ 0x496f, 2, ELM_OI, ELM_I, 0, 0, 0, 0, 0,
/* aU */ 0x5561, 2, ELM_OU, ELM_OV, 0, 0, 0, 0, 0,
/* @U */ 0x5540, 2, ELM_OA, ELM_OV, 0, 0, 0, 0, 0,
/* I@ */ 0x4049, 2, ELM_IA, ELM_IB, 0, 0, 0, 0, 0,
/* e@ */ 0x4065, 2, ELM_AIR, ELM_IB, 0, 0, 0, 0, 0,
/* U@ */ 0x4055, 2, ELM_OOR, ELM_IB, 0, 0, 0, 0, 0,
/* O@ */ 0x404f, 2, ELM_OR, ELM_IB, 0, 0, 0, 0, 0,
/* oU */ 0x556f, 2, ELM_OI, ELM_OV, 0, 0, 0, 0, 0,
/*    */ 0x0020, 1, ELM_Q, 0, 0, 0, 0, 0, 0,
/* p  */ 0x0070, 3, ELM_P, ELM_PY, ELM_PZ, 0, 0, 0, 0,
/* t  */ 0x0074, 3, ELM_T, ELM_TY, ELM_TZ, 0, 0, 0, 0,
/* k  */ 0x006b, 3, ELM_K, ELM_KY, ELM_KZ, 0, 0, 0, 0,
/* b  */ 0x0062, 3, ELM_B, ELM_BY, ELM_BZ, 0, 0, 0, 0,
/* d  */ 0x0064, 3, ELM_D, ELM_DY, ELM_DZ, 0, 0, 0, 0,
/* g  */ 0x0067, 3, ELM_G, ELM_GY, ELM_GZ, 0, 0, 0, 0,
/* m  */ 0x006d, 1, ELM_M, 0, 0, 0, 0, 0, 0,
/* n  */ 0x006e, 1, ELM_N, 0, 0, 0, 0, 0, 0,
/* N  */ 0x004e, 1, ELM_NG, 0, 0, 0, 0, 0, 0,
/* f  */ 0x0066, 1, ELM_F, 0, 0, 0, 0, 0, 0,
/* T  */ 0x0054, 1, ELM_TH, 0, 0, 0, 0, 0, 0,
/* s  */ 0x0073, 1, ELM_S, 0, 0, 0, 0, 0, 0,
/* S  */ 0x0053, 1, ELM_SH, 0, 0, 0, 0, 0, 0,
/* h  */ 0x0068, 1, ELM_H, 0, 0, 0, 0, 0, 0,
/* v  */ 0x0076, 3, ELM_V, ELM_QQ, ELM_V, 0, 0, 0, 0,
/* D  */ 0x0044, 3, ELM_DH, ELM_QQ, ELM_DI, 0, 0, 0, 0,
/* z  */ 0x007a, 3, ELM_Z, ELM_QQ, ELM_ZZ, 0, 0, 0, 0,
/* Z  */ 0x005a, 3, ELM_ZH, ELM_QQ, ELM_ZH, 0, 0, 0, 0,
/* l  */ 0x006c, 1, ELM_L, 0, 0, 0, 0, 0, 0,
/* r  */ 0x0072, 1, ELM_R, 0, 0, 0, 0, 0, 0,
/* R  */ 0x0052, 1, ELM_RX, 0, 0, 0, 0, 0, 0,
/* w  */ 0x0077, 1, ELM_W, 0, 0, 0, 0, 0, 0,
/* x  */ 0x0078, 1, ELM_X, 0, 0, 0, 0, 0, 0,
/* %  */ 0x0025, 1, ELM_QQ, 0, 0, 0, 0, 0, 0,
/* j  */ 0x006a, 1, ELM_Y, 0, 0, 0, 0, 0, 0,
/* I  */ 0x0049, 1, ELM_I, 0, 0, 0, 0, 0, 0,
/* e  */ 0x0065, 1, ELM_E, 0, 0, 0, 0, 0, 0,
/* &  */ 0x0026, 1, ELM_AA, 0, 0, 0, 0, 0, 0,
/* V  */ 0x0056, 1, ELM_U, 0, 0, 0, 0, 0, 0,
/* 0  */ 0x0030, 1, ELM_O, 0, 0, 0, 0, 0, 0,
/* U  */ 0x0055, 1, ELM_OO, 0, 0, 0, 0, 0, 0,
/* @  */ 0x0040, 1, ELM_A, 0, 0, 0, 0, 0, 0,
/* i  */ 0x0069, 1, ELM_EE, 0, 0, 0, 0, 0, 0,
/* 3  */ 0x0033, 1, ELM_ER, 0, 0, 0, 0, 0, 0,
/* A  */ 0x0041, 1, ELM_AR, 0, 0, 0, 0, 0, 0,
/* O  */ 0x004f, 1, ELM_AW, 0, 0, 0, 0, 0, 0,
/* u  */ 0x0075, 1, ELM_UU, 0, 0, 0, 0, 0, 0,
/* o  */ 0x006f, 1, ELM_OI, 0, 0, 0, 0, 0, 0,
};

static Element gElement[] =
{
/*mName, mRK, mDU, mUD, mFont, mDict, mIpa, mFeat, interpolators*/
/* (mSteady, mFixed, mProportion, mExtDelay, mIntDelay) */
{"END", 31, 5, 5,0x00,NULL,NULL,0,
 {
  {   270,    135,  50,  3,  3}, /* ELM_FN       0 */
  {   490,      0, 100,  0,  0}, /* ELM_F1       0 */
  {  1480,      0, 100,  0,  0}, /* ELM_F2       0 */
  {  2500,      0, 100,  0,  0}, /* ELM_F3       0 */
  {    60,      0, 100,  0,  0}, /* ELM_B1       0 */
  {    90,      0, 100,  0,  0}, /* ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* ELM_B3       0 */
  {   -30,  -10.5, 100,  3,  0}, /* ELM_AN   -10.5 */
  {   -30,  -10.5, 100,  3,  0}, /* ELM_A1   -10.5 */
  {   -30,  -10.5, 100,  3,  0}, /* ELM_A2   -10.5 */
  {   -30,  -10.5, 100,  3,  0}, /* ELM_A3   -10.5 */
  {   -30,  -10.5, 100,  3,  0}, /* ELM_A4   -10.5 */
  {   -30,      0, 100,  3,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  3,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  3,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"Q",   29, 6, 6,0x00,NULL,NULL,0,
 {
  {   270,    135,  50,  3,  3}, /* ELM_FN       0 */
  {   490,      0, 100,  3,  3}, /* ELM_F1       0 */
  {  1480,      0, 100,  3,  3}, /* ELM_F2       0 */
  {  2500,      0, 100,  3,  3}, /* ELM_F3       0 */
  {    60,      0, 100,  3,  3}, /* ELM_B1       0 */
  {    90,      0, 100,  3,  3}, /* ELM_B2       0 */
  {   150,      0, 100,  3,  3}, /* ELM_B3       0 */
  {   -30,  -10.5, 100,  3,  0}, /* ELM_AN   -10.5 */
  {   -30,  -10.5, 100,  3,  0}, /* ELM_A1   -10.5 */
  {   -30,  -10.5, 100,  3,  0}, /* ELM_A2   -10.5 */
  {   -30,  -10.5, 100,  3,  0}, /* ELM_A3   -10.5 */
  {   -30,  -10.5, 100,  3,  0}, /* ELM_A4   -10.5 */
  {   -30,      0, 100,  3,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  3,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  3,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"P",   23, 8, 8,0x70,"p","p",ELM_FEATURE_BLB|ELM_FEATURE_STP|ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,    110,  50,  2,  2}, /* ELM_F1      15 */
  {   760,    350,  50,  2,  2}, /* ELM_F2     -30 */
  {  2500,      0, 100,  0,  2}, /* ELM_F3       0 */
  {    60,     30,  50,  2,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  2,  2}, /* ELM_B2       0 */
  {   150,      0, 100,  0,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"PY",  29, 1, 1,0x70,"p","p",ELM_FEATURE_BLB|ELM_FEATURE_STP|ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,      0, 100,  0,  0}, /* ELM_F1       0 */
  {   760,      0, 100,  0,  0}, /* ELM_F2       0 */
  {  2500,      0, 100,  0,  0}, /* ELM_F3       0 */
  {    60,      0, 100,  0,  0}, /* ELM_B1       0 */
  {    90,      0, 100,  0,  0}, /* ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {  24.5,      0, 100,  0,  0}, /* ELM_A1       0 */
  {    49,      0, 100,  0,  0}, /* ELM_A2       0 */
  { 43.75,      0, 100,  0,  0}, /* ELM_A3       0 */
  {  38.5,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"PZ",  23, 2, 2,0x70,"p","p",ELM_FEATURE_BLB|ELM_FEATURE_STP|ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,    110,  50,  2,  2}, /* ELM_F1      15 */
  {   760,    350,  50,  2,  2}, /* ELM_F2     -30 */
  {  2500,      0, 100,  2,  2}, /* ELM_F3       0 */
  {    60,     30,  50,  2,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  2,  2}, /* ELM_B2       0 */
  {   150,      0, 100,  2,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {  24.5,      0, 100,  0,  0}, /* ELM_A1       0 */
  {  38.5,      0, 100,  0,  0}, /* ELM_A2       0 */
  { 33.25,      0, 100,  0,  0}, /* ELM_A3       0 */
  {    28,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"T",   23, 6, 6,0x74,"t","t",ELM_FEATURE_ALV|ELM_FEATURE_STP|ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,    110,  50,  2,  2}, /* ELM_F1      15 */
  {  1780,    950,  50,  2,  2}, /* ELM_F2      60 */
  {  2680,   2680,   0,  0,  2}, /* ELM_F3       0 */
  {    60,     30,  50,  2,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  2,  2}, /* ELM_B2       0 */
  {   150,    150,   0,  0,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"TY",  29, 1, 1,0x74,"t","t",ELM_FEATURE_ALV|ELM_FEATURE_STP|ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,      0, 100,  0,  0}, /* ELM_F1       0 */
  {  1780,      0, 100,  0,  0}, /* ELM_F2       0 */
  {  2680,      0, 100,  0,  0}, /* ELM_F3       0 */
  {    60,      0, 100,  0,  0}, /* ELM_B1       0 */
  {    90,      0, 100,  0,  0}, /* ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A2       0 */
  {  38.5,      0, 100,  0,  0}, /* ELM_A3       0 */
  { 50.75,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"TZ",  23, 2, 2,0x74,"t","t",ELM_FEATURE_ALV|ELM_FEATURE_STP|ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,    110,  50,  2,  1}, /* ELM_F1      15 */
  {  1780,    950,  50,  2,  1}, /* ELM_F2      60 */
  {  2680,   2680,   0,  2,  0}, /* ELM_F3       0 */
  {    60,     30,  50,  2,  1}, /* ELM_B1       0 */
  {    90,     45,  50,  2,  1}, /* ELM_B2       0 */
  {   150,    150,   0,  2,  0}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A2       0 */
  {    28,      0, 100,  0,  0}, /* ELM_A3       0 */
  { 40.25,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"K",   23, 8, 8,0x6B,"k","k",ELM_FEATURE_STP|ELM_FEATURE_VEL|ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,    110,  50,  3,  3}, /* ELM_F1      15 */
  {  1480,   1550,  50,  3,  3}, /* ELM_F2     810 */
  {  2620,   1580,  50,  3,  3}, /* ELM_F3     270 */
  {    60,     30,  50,  3,  3}, /* ELM_B1       0 */
  {    90,     45,  50,  3,  3}, /* ELM_B2       0 */
  {   150,     75,  50,  3,  3}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"KY",  29, 1, 1,0x6B,"k","k",ELM_FEATURE_STP|ELM_FEATURE_VEL|ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,      0, 100,  0,  0}, /* ELM_F1       0 */
  {  1480,      0, 100,  0,  0}, /* ELM_F2       0 */
  {  2620,      0, 100,  0,  0}, /* ELM_F3       0 */
  {    60,      0, 100,  0,  0}, /* ELM_B1       0 */
  {    90,      0, 100,  0,  0}, /* ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A1       0 */
  { 50.75,      0, 100,  0,  0}, /* ELM_A2       0 */
  { 50.75,      0, 100,  0,  0}, /* ELM_A3       0 */
  { 29.75,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"KZ",  23, 4, 4,0x6B,"k","k",ELM_FEATURE_STP|ELM_FEATURE_VEL|ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,    110,  50,  3,  3}, /* ELM_F1      15 */
  {  1480,   1550,  50,  3,  3}, /* ELM_F2     810 */
  {  2620,   1580,  50,  3,  3}, /* ELM_F3     270 */
  {    60,     30,  50,  3,  3}, /* ELM_B1       0 */
  {    90,     45,  50,  3,  3}, /* ELM_B2       0 */
  {   150,     75,  50,  3,  3}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A1       0 */
  { 40.25,      0, 100,  0,  0}, /* ELM_A2       0 */
  { 40.25,      0, 100,  0,  0}, /* ELM_A3       0 */
  { 19.25,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"B",   26,12,12,0x62,"b","b",ELM_FEATURE_BLB|ELM_FEATURE_STP|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,    110,  50,  2,  2}, /* ELM_F1      15 */
  {   760,    350,  50,  2,  2}, /* ELM_F2     -30 */
  {  2500,      0, 100,  0,  2}, /* ELM_F3       0 */
  {    60,     30,  50,  2,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  2,  2}, /* ELM_B2       0 */
  {   150,      0, 100,  0,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {  24.5,      0, 100,  0,  0}, /* ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"BY",  29, 1, 1,0x62,"b","b",ELM_FEATURE_BLB|ELM_FEATURE_STP|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,      0, 100,  0,  0}, /* ELM_F1       0 */
  {   760,      0, 100,  0,  0}, /* ELM_F2       0 */
  {  2500,      0, 100,  0,  0}, /* ELM_F3       0 */
  {    60,      0, 100,  0,  0}, /* ELM_B1       0 */
  {    90,      0, 100,  0,  0}, /* ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {  24.5,      0, 100,  0,  0}, /* ELM_A1       0 */
  {    49,      0, 100,  0,  0}, /* ELM_A2       0 */
  { 43.25,      0, 100,  0,  0}, /* ELM_A3       0 */
  {  38.5,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"BZ",  26, 0, 0,0x62,"b","b",ELM_FEATURE_BLB|ELM_FEATURE_STP|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,    110,  50,  2,  0}, /* ELM_F1      15 */
  {   760,    350,  50,  2,  0}, /* ELM_F2     -30 */
  {  2500,      0, 100,  0,  0}, /* ELM_F3       0 */
  {    60,     30,  50,  2,  0}, /* ELM_B1       0 */
  {    90,     45,  50,  2,  0}, /* ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"D",   26, 8, 8,0x64,"d","d",ELM_FEATURE_ALV|ELM_FEATURE_STP|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,    110,  50,  2,  2}, /* ELM_F1      15 */
  {  1780,    950,  50,  2,  2}, /* ELM_F2      60 */
  {  2680,   2680,   0,  2,  2}, /* ELM_F3       0 */
  {    60,     30,  50,  2,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  2,  2}, /* ELM_B2       0 */
  {   150,    150,   0,  2,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {  31.5,      0, 100,  0,  0}, /* ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"DY",  29, 1, 1,0x64,"d","d",ELM_FEATURE_ALV|ELM_FEATURE_STP|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,      0, 100,  0,  0}, /* ELM_F1       0 */
  {  1780,      0, 100,  0,  0}, /* ELM_F2       0 */
  {  2680,      0, 100,  0,  0}, /* ELM_F3       0 */
  {    60,      0, 100,  0,  0}, /* ELM_B1       0 */
  {    90,      0, 100,  0,  0}, /* ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {  38.5,      0, 100,  0,  0}, /* ELM_A1       0 */
  {  38.5,      0, 100,  0,  0}, /* ELM_A2       0 */
  {    35,      0, 100,  0,  0}, /* ELM_A3       0 */
  {  45.5,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"DZ",  26, 1, 1,0x64,"d","d",ELM_FEATURE_ALV|ELM_FEATURE_STP|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,    110,  50,  2,  0}, /* ELM_F1      15 */
  {  1780,    950,  50,  2,  0}, /* ELM_F2      60 */
  {  2680,   2680,   0,  2,  0}, /* ELM_F3       0 */
  {    60,     30,  50,  2,  0}, /* ELM_B1       0 */
  {    90,     45,  50,  2,  0}, /* ELM_B2       0 */
  {   150,    150,   0,  2,  0}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {  38.5,      0, 100,  0,  0}, /* ELM_A1       0 */
  {    28,      0, 100,  0,  0}, /* ELM_A2       0 */
  {  24.5,      0, 100,  0,  0}, /* ELM_A3       0 */
  {    35,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},


{"G",   26,12,12,0x67,"g","g",ELM_FEATURE_STP|ELM_FEATURE_VCD|ELM_FEATURE_VEL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,    110,  50,  3,  3}, /* ELM_F1      15 */
  {  1480,   1550,  50,  3,  3}, /* ELM_F2     810 */
  {  2620,   1580,  50,  3,  3}, /* ELM_F3     270 */
  {    60,     30,  50,  3,  3}, /* ELM_B1       0 */
  {    90,     45,  50,  3,  3}, /* ELM_B2       0 */
  {   150,     75,  50,  3,  3}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {    35,      0, 100,  0,  0}, /* ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"GY",  29, 1, 1,0x67,"g","g",ELM_FEATURE_STP|ELM_FEATURE_VCD|ELM_FEATURE_VEL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,      0, 100,  0,  0}, /* ELM_F1       0 */
  {  1480,      0, 100,  0,  0}, /* ELM_F2       0 */
  {  2620,      0, 100,  0,  0}, /* ELM_F3       0 */
  {    60,      0, 100,  0,  0}, /* ELM_B1       0 */
  {    90,      0, 100,  0,  0}, /* ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {    35,      0, 100,  0,  0}, /* ELM_A1       0 */
  {  45.5,      0, 100,  0,  0}, /* ELM_A2       0 */
  { 40.25,      0, 100,  0,  0}, /* ELM_A3       0 */
  {  24.5,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"GZ",  26, 2, 2,0x67,"g","g",ELM_FEATURE_STP|ELM_FEATURE_VCD|ELM_FEATURE_VEL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,    110,  50,  3,  2}, /* ELM_F1      15 */
  {  1480,   1550,  50,  3,  2}, /* ELM_F2     810 */
  {  2620,   1580,  50,  3,  2}, /* ELM_F3     270 */
  {    60,     30,  50,  3,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* ELM_B2       0 */
  {   150,     75,  50,  3,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {    35,      0, 100,  0,  0}, /* ELM_A1       0 */
  {    35,      0, 100,  0,  0}, /* ELM_A2       0 */
  { 29.75,      0, 100,  0,  0}, /* ELM_A3       0 */
  {    14,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"M",   15, 8, 8,0x6D,"m","m",ELM_FEATURE_BLB|ELM_FEATURE_NAS,
 {
  {   360,    360,   0,  3,  0}, /* ELM_FN       0 */
  {   480,    480,   0,  3,  0}, /* ELM_F1       0 */
  {  1000,    350,  50,  3,  0}, /* ELM_F2    -150 */
  {  2200,      0, 100,  5,  0}, /* ELM_F3       0 */
  {    40,     20,  50,  3,  0}, /* ELM_B1       0 */
  {   175,     87,  50,  3,  0}, /* ELM_B2    -0.5 */
  {   120,      0, 100,  5,  0}, /* ELM_B3       0 */
  {    42,     21,  50,  3,  0}, /* ELM_AN       0 */
  {    26,    -10, 100,  3,  0}, /* ELM_A1     -10 */
  {    30,    -10, 100,  3,  0}, /* ELM_A2     -10 */
  {    33,    -10, 100,  3,  0}, /* ELM_A3     -10 */
  {   -30,    -10, 100,  3,  0}, /* ELM_A4     -10 */
  {   -30,      0, 100,  3,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  3,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  3,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  2,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  2,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  2,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  2,  0}  /* ELM_AF       0 */
 }
},

{"N",   15, 8, 8,0x6E,"n","n",ELM_FEATURE_ALV|ELM_FEATURE_NAS,
 {
  {   450,    450,   0,  3,  0}, /* ELM_FN       0 */
  {   480,    480,   0,  3,  0}, /* ELM_F1       0 */
  {  1780,    950,  50,  3,  3}, /* ELM_F2      60 */
  {  2620,   2680,   0,  3,  0}, /* ELM_F3      60 */
  {    40,     20,  50,  3,  0}, /* ELM_B1       0 */
  {   300,    150,  50,  3,  3}, /* ELM_B2       0 */
  {   260,    130,  50,  3,  0}, /* ELM_B3       0 */
  {    42,     21,  50,  3,  0}, /* ELM_AN       0 */
  {    35,    -10, 100,  3,  0}, /* ELM_A1     -10 */
  {    35,    -10, 100,  3,  0}, /* ELM_A2     -10 */
  {    35,    -10, 100,  3,  0}, /* ELM_A3     -10 */
  {    20,    -10, 100,  3,  0}, /* ELM_A4     -10 */
  {   -30,      0, 100,  3,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  3,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  3,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  2,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  2,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  2,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  2,  0}  /* ELM_AF       0 */
 }
},

{"NG",  15, 8, 8,0x4E,"N","N",ELM_FEATURE_NAS|ELM_FEATURE_VEL,
 {
  {   360,    360,   0,  3,  0}, /* ELM_FN       0 */
  {   480,    480,   0,  3,  0}, /* ELM_F1       0 */
  {   820,   1550,  50,  5,  3}, /* ELM_F2    1140 */
  {  2800,   1580,  50,  3,  3}, /* ELM_F3     180 */
  {   160,     80,   0,  5,  0}, /* ELM_B1     -80 */
  {   150,     75,  50,  5,  3}, /* ELM_B2       0 */
  {   100,     50,  50,  3,  0}, /* ELM_B3       0 */
  {    42,     21,  50,  3,  3}, /* ELM_AN       0 */
  {    20,      0, 100,  3,  0}, /* ELM_A1       0 */
  {    30,      0, 100,  3,  0}, /* ELM_A2       0 */
  {    35,      0, 100,  3,  0}, /* ELM_A3       0 */
  {     0,      0, 100,  3,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  3,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  3,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  3,  0}, /* ELM_AB       0 */
  {    52,     26,  50,  2,  0}, /* ELM_AV       0 */
  {    56,     28,  50,  2,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  2,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  2,  0}  /* ELM_AF       0 */
 }
},

{"F",   18,12,12,0x66,"f","f",ELM_FEATURE_FRC|ELM_FEATURE_LBD|ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   400,    170,  50,  3,  2}, /* ELM_F1     -30 */
  {  1420,    350,  50,  3,  2}, /* ELM_F2    -360 */
  {  2560,    980,  50,  3,  2}, /* ELM_F3    -300 */
  {    60,     30,  50,  3,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* ELM_B2       0 */
  {   150,     75,  50,  3,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A1       0 */
  {     0,      0, 100,  0,  0}, /* ELM_A2       0 */
  {     0,      0, 100,  0,  0}, /* ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {    54,     27,  50,  0,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    32,     16,  50,  0,  0}, /* ELM_ASP      0 */
  {    54,     30,  50,  0,  0}  /* ELM_AF       3 */
 }
},

{"TH",  18,15,15,0x54,"T","T",ELM_FEATURE_DNT|ELM_FEATURE_FRC|ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   400,    170,  50,  3,  2}, /* ELM_F1     -30 */
  {  1780,   1190,  50,  3,  2}, /* ELM_F2     300 */
  {  2680,   2680,   0,  3,  2}, /* ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* ELM_B2       0 */
  {   150,    150,   0,  3,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A1       0 */
  { 26.25,      0, 100,  0,  0}, /* ELM_A2       0 */
  {    28,      0, 100,  0,  0}, /* ELM_A3       0 */
  { 22.75,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"S",   18,12,12,0x73,"s","s",ELM_FEATURE_ALV|ELM_FEATURE_FRC|ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   400,    170,  50,  3,  2}, /* ELM_F1     -30 */
  {  1720,    950,  50,  3,  2}, /* ELM_F2      90 */
  {  2620,      0, 100,  3,  2}, /* ELM_F3       0 */
  {   200,    100,  50,  3,  2}, /* ELM_B1       0 */
  {    96,     48,  50,  3,  2}, /* ELM_B2       0 */
  {   220,      0, 100,  3,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A1       0 */
  {    28,      0, 100,  0,  0}, /* ELM_A2       0 */
  {    28,      0, 100,  0,  0}, /* ELM_A3       0 */
  { 40.25,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    32,     16,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"SH",  18,12,12,0x53,"S","S",ELM_FEATURE_FRC|ELM_FEATURE_PLA|ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   400,    170,  50,  3,  2}, /* ELM_F1     -30 */
  {  2200,   1190,  50,  3,  2}, /* ELM_F2      90 */
  {  2560,      0, 100,  3,  2}, /* ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* ELM_B2       0 */
  {   150,      0, 100,  3,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A1       0 */
  {  31.5,      0, 100,  0,  0}, /* ELM_A2       0 */
  {    42,      0, 100,  0,  0}, /* ELM_A3       0 */
  {  31.5,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"X",   18,12,12,0x78,"x","x",ELM_FEATURE_FRC|ELM_FEATURE_VEL|ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,    110,  50,  3,  3}, /* ELM_F1      15 */
  {  1480,   1550,  50,  3,  3}, /* ELM_F2     810 */
  {  2620,   1580,  50,  3,  3}, /* ELM_F3     270 */
  {    60,     30,  50,  3,  3}, /* ELM_B1       0 */
  {    90,     45,  50,  3,  3}, /* ELM_B2       0 */
  {   150,     75,  50,  3,  3}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A1       0 */
  { 40.25,      0, 100,  0,  0}, /* ELM_A2       0 */
  { 40.25,      0, 100,  0,  0}, /* ELM_A3       0 */
  { 19.25,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"H",    9,10,10,0x68,"h","h",ELM_FEATURE_APR|ELM_FEATURE_GLT,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   490,      0, 100,  0,  7}, /* ELM_F1       0 */
  {  1480,      0, 100,  0,  7}, /* ELM_F2       0 */
  {  2500,      0, 100,  0,  7}, /* ELM_F3       0 */
  {    60,      0, 100,  0,  7}, /* ELM_B1       0 */
  {    90,      0, 100,  0,  7}, /* ELM_B2       0 */
  {   150,      0, 100,  0,  7}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  7}, /* ELM_AN       0 */
  {    35,    -14, 100,  0,  7}, /* ELM_A1     -14 */
  { 36.75,    -14, 100,  0,  7}, /* ELM_A2     -14 */
  { 26.25,     -7, 100,  0,  7}, /* ELM_A3      -7 */
  { 22.75,   -3.5, 100,  0,  7}, /* ELM_A4    -3.5 */
  {   -30,      0, 100,  0,  7}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  7}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  7}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"V",   20, 4, 4,0x76,"v","v",ELM_FEATURE_FRC|ELM_FEATURE_LBD|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   280,    170,  50,  3,  2}, /* ELM_F1      30 */
  {  1420,    350,  50,  3,  2}, /* ELM_F2    -360 */
  {  2560,    980,  50,  3,  2}, /* ELM_F3    -300 */
  {    60,     30,  50,  3,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* ELM_B2       0 */
  {   150,     75,  50,  3,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  { 29.75,      0, 100,  0,  0}, /* ELM_A1       0 */
  { 40.25,      0, 100,  0,  0}, /* ELM_A2       0 */
  { 36.75,      0, 100,  0,  0}, /* ELM_A3       0 */
  { 33.25,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"QQ",  30, 0, 0,0x5A,"Z","Z",ELM_FEATURE_FRC|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   280,      0, 100,  0,  0}, /* ELM_F1       0 */
  {  1420,      0, 100,  0,  0}, /* ELM_F2       0 */
  {  2560,      0, 100,  0,  0}, /* ELM_F3       0 */
  {    60,      0, 100,  0,  0}, /* ELM_B1       0 */
  {    90,      0, 100,  0,  0}, /* ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  { 29.75,      0, 100,  0,  0}, /* ELM_A1       0 */
  { 40.25,      0, 100,  0,  0}, /* ELM_A2       0 */
  { 36.75,      0, 100,  0,  0}, /* ELM_A3       0 */
  { 33.25,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"DH",  20, 4, 4,0x54,"D","D",ELM_FEATURE_DNT|ELM_FEATURE_FRC|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   280,    170,  50,  3,  2}, /* ELM_F1      30 */
  {  1600,   1190,  50,  3,  2}, /* ELM_F2     390 */
  {  2560,      0, 100,  3,  2}, /* ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* ELM_B2       0 */
  {   150,      0, 100,  3,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  { 29.75,      0, 100,  0,  0}, /* ELM_A1       0 */
  {  31.5,      0, 100,  0,  0}, /* ELM_A2       0 */
  { 26.25,      0, 100,  0,  0}, /* ELM_A3       0 */
  {    28,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {    54,     27,  50,  0,  0}, /* ELM_AB       0 */
  {    36,     18,  50,  0,  0}, /* ELM_AV       0 */
  {    54,     27,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"DI",  20, 4, 4,0x54,"D","D",ELM_FEATURE_DNT|ELM_FEATURE_FRC|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   280,    170,  50,  3,  2}, /* ELM_F1      30 */
  {  1600,   1190,  50,  3,  2}, /* ELM_F2     390 */
  {  2560,      0, 100,  3,  2}, /* ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* ELM_B2       0 */
  {   150,      0, 100,  3,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  { 29.75,      0, 100,  0,  0}, /* ELM_A1       0 */
  {  31.5,      0, 100,  0,  0}, /* ELM_A2       0 */
  { 26.25,      0, 100,  0,  0}, /* ELM_A3       0 */
  {    28,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"Z",   20, 4, 4,0x7A,"z","z",ELM_FEATURE_ALV|ELM_FEATURE_FRC|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   280,    170,  50,  3,  2}, /* ELM_F1      30 */
  {  1720,    950,  50,  3,  2}, /* ELM_F2      90 */
  {  2560,      0, 100,  3,  2}, /* ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* ELM_B2       0 */
  {   150,      0, 100,  3,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  { 29.75,      0, 100,  0,  0}, /* ELM_A1       0 */
  {  24.5,      0, 100,  0,  0}, /* ELM_A2       0 */
  {  24.5,      0, 100,  0,  0}, /* ELM_A3       0 */
  { 36.75,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    40,     20,  50,  0,  0}, /* ELM_AV       0 */
  {    54,     27,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"ZZ",  20, 4, 4,0x7A,"z","z",ELM_FEATURE_ALV|ELM_FEATURE_FRC|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   280,    170,  50,  3,  2}, /* ELM_F1      30 */
  {  1720,    950,  50,  3,  2}, /* ELM_F2      90 */
  {  2560,      0, 100,  3,  2}, /* ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* ELM_B2       0 */
  {   150,      0, 100,  3,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  { 29.75,      0, 100,  0,  0}, /* ELM_A1       0 */
  {  24.5,      0, 100,  0,  0}, /* ELM_A2       0 */
  {  24.5,      0, 100,  0,  0}, /* ELM_A3       0 */
  { 36.75,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"ZH",  20, 4, 4,0x5A,"Z","Z",ELM_FEATURE_FRC|ELM_FEATURE_PLA|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   280,    170,  50,  3,  2}, /* ELM_F1      30 */
  {  2020,   1190,  50,  3,  2}, /* ELM_F2     180 */
  {  2560,      0, 100,  3,  2}, /* ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* ELM_B2       0 */
  {   150,      0, 100,  3,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  { 29.75,      0, 100,  0,  0}, /* ELM_A1       0 */
  { 26.25,      0, 100,  0,  0}, /* ELM_A2       0 */
  { 36.75,      0, 100,  0,  0}, /* ELM_A3       0 */
  { 26.25,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"CH",  23, 4, 4,0x74,"t","t",ELM_FEATURE_ALV|ELM_FEATURE_STP|ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,    110,  50,  2,  2}, /* ELM_F1      15 */
  {  1780,    950,  50,  2,  2}, /* ELM_F2      60 */
  {  2680,   2680,   0,  2,  2}, /* ELM_F3       0 */
  {    60,     30,  50,  2,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  2,  2}, /* ELM_B2       0 */
  {   150,    150,   0,  2,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"CI",  18, 8, 8,0x53,"S","S",ELM_FEATURE_FRC|ELM_FEATURE_PLA|ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   400,    170,  50,  3,  2}, /* ELM_F1     -30 */
  {  2020,   1190,  50,  3,  2}, /* ELM_F2     180 */
  {  2560,      0, 100,  3,  2}, /* ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* ELM_B2       0 */
  {   150,      0, 100,  3,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A1       0 */
  {  31.5,      0, 100,  0,  0}, /* ELM_A2       0 */
  {    42,      0, 100,  0,  0}, /* ELM_A3       0 */
  {  31.5,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"J",   26, 4, 4,0x64,"d","d",ELM_FEATURE_ALV|ELM_FEATURE_STP|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,    110,  50,  2,  2}, /* ELM_F1      15 */
  {  1780,    950,  50,  2,  2}, /* ELM_F2      60 */
  {  2680,   2680,   0,  2,  2}, /* ELM_F3       0 */
  {    60,     30,  50,  2,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  2,  2}, /* ELM_B2       0 */
  {   150,    150,   0,  2,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  {  31.5,      0, 100,  0,  0}, /* ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"JY",  20, 3, 3,0x5A,"Z","Z",ELM_FEATURE_FRC|ELM_FEATURE_PLA|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   280,    170,  50,  3,  2}, /* ELM_F1      30 */
  {  2020,   1190,  50,  3,  2}, /* ELM_F2     180 */
  {  2560,      0, 100,  3,  2}, /* ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* ELM_B2       0 */
  {   150,      0, 100,  3,  2}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  { 29.75,      0, 100,  0,  0}, /* ELM_A1       0 */
  { 26.25,      0, 100,  0,  0}, /* ELM_A2       0 */
  { 36.75,      0, 100,  0,  0}, /* ELM_A3       0 */
  { 26.25,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"L",   11, 8, 8,0x6C,"l","l",ELM_FEATURE_ALV|ELM_FEATURE_LAT|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   460,    230,  50,  6,  0}, /* ELM_F1       0 */
  {  1480,    710,  50,  6,  0}, /* ELM_F2     -30 */
  {  2500,   1220,  50,  6,  0}, /* ELM_F3     -30 */
  {    60,     30,  50,  6,  0}, /* ELM_B1       0 */
  {    90,     45,  50,  6,  0}, /* ELM_B2       0 */
  {   150,     75,  50,  6,  0}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  { 36.75,      0, 100,  0,  0}, /* ELM_A1       0 */
  { 26.25,      0, 100,  0,  0}, /* ELM_A2       0 */
  { 26.25,      0, 100,  0,  0}, /* ELM_A3       0 */
  {    21,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"LL",  11, 8, 8,0x6C,"l","l",ELM_FEATURE_ALV|ELM_FEATURE_LAT|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   460,    230,  50,  6,  0}, /* ELM_F1       0 */
  {   940,    470,  50,  6,  0}, /* ELM_F2       0 */
  {  2500,   1220,  50,  6,  0}, /* ELM_F3     -30 */
  {    60,     30,  50,  6,  0}, /* ELM_B1       0 */
  {    90,     45,  50,  6,  0}, /* ELM_B2       0 */
  {   150,     75,  50,  6,  0}, /* ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AN       0 */
  { 36.75,      0, 100,  0,  0}, /* ELM_A1       0 */
  { 26.25,      0, 100,  0,  0}, /* ELM_A2       0 */
  { 26.25,      0, 100,  0,  0}, /* ELM_A3       0 */
  {    21,      0, 100,  0,  0}, /* ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"RX",  10,10,10,0xD5,"R","<ELM_FEATURE_RZD>",ELM_FEATURE_RZD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   490,      0, 100,  0,  5}, /* ELM_F1       0 */
  {  1180,      0, 100,  0,  5}, /* ELM_F2       0 */
  {  1600,   1600,   0,  5,  5}, /* ELM_F3       0 */
  {    60,     30,  50,  0,  5}, /* ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* ELM_B2       0 */
  {    70,     35,  50,  5,  5}, /* ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* ELM_AN       0 */
  {    42,     21,  50,  5,  5}, /* ELM_A1       0 */
  {    35,   17.5,  50,  5,  5}, /* ELM_A2       0 */
  {    35,   17.5,  50,  5,  5}, /* ELM_A3       0 */
  {   -30,      0,  50,  5,  5}, /* ELM_A4      15 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_AB       0 */
  {    50,     25,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"R",   10,11,11,0xA8,"r","r",ELM_FEATURE_ALV|ELM_FEATURE_APR,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   490,      0, 100,  0,  5}, /* ELM_F1       0 */
  {  1180,    590,  50,  5,  5}, /* ELM_F2       0 */
  {  1600,    740,  50,  5,  5}, /* ELM_F3     -60 */
  {    60,      0, 100,  0,  5}, /* ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* ELM_AN       0 */
  {    42,     21,  50,  5,  5}, /* ELM_A1       0 */
  {    35,   17.5,  50,  5,  5}, /* ELM_A2       0 */
  {    35,   17.5,  50,  5,  5}, /* ELM_A3       0 */
  {   -30,      0,  50,  5,  5}, /* ELM_A4      15 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"W",   10, 8, 8,0x77,"w","w",ELM_FEATURE_APR|ELM_FEATURE_LBV|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   190,     50,  50,  4,  4}, /* ELM_F1     -45 */
  {   760,    350,  50,  4,  4}, /* ELM_F2     -30 */
  {  2020,    980,  50,  4,  4}, /* ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* ELM_AN       0 */
  { 43.75,     21,  50,  4,  4}, /* ELM_A1  -0.875 */
  {    28,     14,  50,  4,  4}, /* ELM_A2       0 */
  {    21,   10.5,  50,  4,  4}, /* ELM_A3       0 */
  {   -30,      0,  50,  4,  4}, /* ELM_A4      15 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"Y",   10, 7, 7,0x6A,"j","j",ELM_FEATURE_APR|ELM_FEATURE_PAL|ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   250,    110,  50,  4,  4}, /* ELM_F1     -15 */
  {  2500,   1190,  50,  4,  4}, /* ELM_F2     -60 */
  {  2980,   1460,  50,  4,  4}, /* ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A1  -0.875 */
  { 33.25,   17.5,  50,  4,  4}, /* ELM_A2   0.875 */
  {  38.5,   17.5,  50,  4,  4}, /* ELM_A3   -1.75 */
  {  31.5,     14,  50,  4,  4}, /* ELM_A4   -1.75 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"I",    2, 8, 6,0x49,"I","I",ELM_FEATURE_FNT|ELM_FEATURE_SMH|ELM_FEATURE_UNR|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   400,    170,  50,  4,  4}, /* ELM_F1     -30 */
  {  2080,   1070,  50,  4,  4}, /* ELM_F2      30 */
  {  2560,   1340,  50,  4,  4}, /* ELM_F3      60 */
  {    60,     30,  50,  4,  4}, /* ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A1  -0.875 */
  { 36.75,   17.5,  50,  4,  4}, /* ELM_A2  -0.875 */
  {    35,   17.5,  50,  4,  4}, /* ELM_A3       0 */
  { 29.75,     14,  50,  4,  4}, /* ELM_A4  -0.875 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"E",    2, 8, 4,0x45,"e","E",ELM_FEATURE_FNT|ELM_FEATURE_LMD|ELM_FEATURE_UNR|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   640,    350,  50,  4,  4}, /* ELM_F1      30 */
  {  2020,   1070,  50,  4,  4}, /* ELM_F2      60 */
  {  2500,   1220,  50,  4,  4}, /* ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A1  -0.875 */
  {    42,     21,  50,  4,  4}, /* ELM_A2       0 */
  {  38.5,   17.5,  50,  4,  4}, /* ELM_A3   -1.75 */
  {  31.5,     14,  50,  4,  4}, /* ELM_A4   -1.75 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"AA",   2,10, 5,0x51,"&","&",ELM_FEATURE_FNT|ELM_FEATURE_LOW|ELM_FEATURE_UNR|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   790,    410,  50,  4,  4}, /* ELM_F1      15 */
  {  1780,    950,  50,  4,  4}, /* ELM_F2      60 */
  {  2500,   1220,  50,  4,  4}, /* ELM_F3     -30 */
  {   130,     65,  50,  4,  4}, /* ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A1  -0.875 */
  { 47.25,   24.5,  50,  4,  4}, /* ELM_A2   0.875 */
  {  38.5,   17.5,  50,  4,  4}, /* ELM_A3   -1.75 */
  {  31.5,     14,  50,  4,  4}, /* ELM_A4   -1.75 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"U",    2, 9, 6,0xC3,"V","V",ELM_FEATURE_BCK|ELM_FEATURE_LMD|ELM_FEATURE_UNR|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   700,    350,  50,  4,  4}, /* ELM_F1       0 */
  {  1360,    710,  50,  4,  4}, /* ELM_F2      30 */
  {  2500,   1220,  50,  4,  4}, /* ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A1  -0.875 */
  { 43.75,     21,  50,  4,  4}, /* ELM_A2  -0.875 */
  {  31.5,     14,  50,  4,  4}, /* ELM_A3   -1.75 */
  {  24.5,   10.5,  50,  4,  4}, /* ELM_A4   -1.75 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"O",    2, 9, 6,0x81,"0","A.",ELM_FEATURE_BCK|ELM_FEATURE_LOW|ELM_FEATURE_RND|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   610,    290,  50,  4,  4}, /* ELM_F1     -15 */
  {   880,    470,  50,  4,  4}, /* ELM_F2      30 */
  {  2500,   1220,  50,  4,  4}, /* ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A1  -0.875 */
  { 47.25,   24.5,  50,  4,  4}, /* ELM_A2   0.875 */
  { 22.75,   10.5,  50,  4,  4}, /* ELM_A3  -0.875 */
  { 15.75,      7,  50,  4,  4}, /* ELM_A4  -0.875 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"OO",   2, 6, 4,0x55,"U","U",ELM_FEATURE_BCK|ELM_FEATURE_RND|ELM_FEATURE_SMH|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   370,    170,  50,  4,  4}, /* ELM_F1     -15 */
  {  1000,    470,  50,  4,  4}, /* ELM_F2     -30 */
  {  2500,   1220,  50,  4,  4}, /* ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A1  -0.875 */
  {    42,     21,  50,  4,  4}, /* ELM_A2       0 */
  {    28,     14,  50,  4,  4}, /* ELM_A3       0 */
  { 22.75,   10.5,  50,  4,  4}, /* ELM_A4  -0.875 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"A",    2, 4, 4,0xAB,"@","@",ELM_FEATURE_CNT|ELM_FEATURE_MDL|ELM_FEATURE_UNR|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   490,    230,  50,  4,  4}, /* ELM_F1     -15 */
  {  1480,    710,  50,  4,  4}, /* ELM_F2     -30 */
  {  2500,   1220,  50,  4,  4}, /* ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A1  -0.875 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A2  -0.875 */
  { 33.25,   17.5,  50,  4,  4}, /* ELM_A3   0.875 */
  { 26.25,     14,  50,  4,  4}, /* ELM_A4   0.875 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"EE",   2,11, 7,0x69,"i","i",ELM_FEATURE_FNT|ELM_FEATURE_HGH|ELM_FEATURE_UNR|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   250,    110,  50,  4,  4}, /* ELM_F1     -15 */
  {  2320,   1190,  50,  4,  4}, /* ELM_F2      30 */
  {  3200,   1580,  50,  4,  4}, /* ELM_F3     -20 */
  {    60,     30,  50,  4,  4}, /* ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A1  -0.875 */
  { 33.25,   17.5,  50,  4,  4}, /* ELM_A2   0.875 */
  { 36.75,   17.5,  50,  4,  4}, /* ELM_A3  -0.875 */
  {  31.5,     14,  50,  4,  4}, /* ELM_A4   -1.75 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"ER",   2,16,16,0xCE,"3","V\"",ELM_FEATURE_CNT|ELM_FEATURE_LMD|ELM_FEATURE_UNR|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   580,    290,  50,  4,  4}, /* ELM_F1       0 */
  {  1420,    710,  50,  4,  4}, /* ELM_F2       0 */
  {  2500,   1220,  50,  4,  4}, /* ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A1  -0.875 */
  {  45.5,     21,  50,  4,  4}, /* ELM_A2   -1.75 */
  { 33.25,   17.5,  50,  4,  4}, /* ELM_A3   0.875 */
  { 26.25,     14,  50,  4,  4}, /* ELM_A4   0.875 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"AR",   2,15,15,0x41,"A","A",ELM_FEATURE_BCK|ELM_FEATURE_LOW|ELM_FEATURE_UNR|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   790,    410,  50,  4,  4}, /* ELM_F1      15 */
  {   880,    470,  50,  4,  4}, /* ELM_F2      30 */
  {  2500,   1220,  50,  4,  4}, /* ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A1  -0.875 */
  {    49,   24.5,  50,  4,  4}, /* ELM_A2       0 */
  { 29.75,     14,  50,  4,  4}, /* ELM_A3  -0.875 */
  { 22.75,   10.5,  50,  4,  4}, /* ELM_A4  -0.875 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"AW",   2,16,10,0x8D,"O","O",ELM_FEATURE_BCK|ELM_FEATURE_LMD|ELM_FEATURE_RND|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   490,    230,  50,  4,  4}, /* ELM_F1     -15 */
  {   820,    470,  50,  4,  4}, /* ELM_F2      60 */
  {  2500,   1220,  50,  4,  4}, /* ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A1  -0.875 */
  {  45.5,     21,  50,  4,  4}, /* ELM_A2   -1.75 */
  { 22.75,   10.5,  50,  4,  4}, /* ELM_A3  -0.875 */
  {  17.5,      7,  50,  4,  4}, /* ELM_A4   -1.75 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"UU",   2,14, 9,0x75,"u","u",ELM_FEATURE_BCK|ELM_FEATURE_HGH|ELM_FEATURE_RND|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   250,    110,  50,  4,  4}, /* ELM_F1     -15 */
  {   880,    470,  50,  4,  4}, /* ELM_F2      30 */
  {  2200,   1100,  50,  4,  4}, /* ELM_F3       0 */
  {    60,     30,  50,  4,  4}, /* ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A1  -0.875 */
  {  38.5,   17.5,  50,  4,  4}, /* ELM_A2   -1.75 */
  {  17.5,      7,  50,  4,  4}, /* ELM_A3   -1.75 */
  {  10.5,    3.5,  50,  4,  4}, /* ELM_A4   -1.75 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"AI",   2, 9, 6,0x45,"e","E",ELM_FEATURE_FNT|ELM_FEATURE_LMD|ELM_FEATURE_UNR|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   640,    290,  50,  5,  5}, /* ELM_F1     -30 */
  {  1600,    830,  50,  5,  5}, /* ELM_F2      30 */
  {  2500,   1220,  50,  5,  5}, /* ELM_F3     -30 */
  {    60,     30,  50,  5,  5}, /* ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* ELM_A1  -0.875 */
  {  45.5,     21,  50,  5,  5}, /* ELM_A2   -1.75 */
  {    35,   17.5,  50,  5,  5}, /* ELM_A3       0 */
  { 29.75,     14,  50,  5,  5}, /* ELM_A4  -0.875 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"IE",   2, 9, 6,0x61,"a","a",ELM_FEATURE_CNT|ELM_FEATURE_LOW|ELM_FEATURE_UNR|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   790,    410,  50,  5,  5}, /* ELM_F1      15 */
  {   880,    470,  50,  5,  5}, /* ELM_F2      30 */
  {  2500,   1220,  50,  5,  5}, /* ELM_F3     -30 */
  {    60,     30,  50,  5,  5}, /* ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* ELM_A1  -0.875 */
  {    49,   24.5,  50,  5,  5}, /* ELM_A2       0 */
  { 29.75,     14,  50,  5,  5}, /* ELM_A3  -0.875 */
  { 22.75,   10.5,  50,  5,  5}, /* ELM_A4  -0.875 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"OI",   2, 9, 6,0x6F,"o","o",ELM_FEATURE_BCK|ELM_FEATURE_RND|ELM_FEATURE_UMD|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   490,    230,  50,  5,  5}, /* ELM_F1     -15 */
  {   820,    350,  50,  5,  5}, /* ELM_F2     -60 */
  {  2500,   1220,  50,  5,  5}, /* ELM_F3     -30 */
  {    60,     30,  50,  5,  5}, /* ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* ELM_A1  -0.875 */
  {  45.5,     21,  50,  5,  5}, /* ELM_A2   -1.75 */
  { 22.75,   10.5,  50,  5,  5}, /* ELM_A3  -0.875 */
  {  17.5,      7,  50,  5,  5}, /* ELM_A4   -1.75 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"OU",   2, 9, 6,0x61,"a","a",ELM_FEATURE_CNT|ELM_FEATURE_LOW|ELM_FEATURE_UNR|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   790,    410,  50,  5,  5}, /* ELM_F1      15 */
  {  1300,    590,  50,  5,  5}, /* ELM_F2     -60 */
  {  2500,   1220,  50,  5,  5}, /* ELM_F3     -30 */
  {    60,     30,  50,  5,  5}, /* ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* ELM_A1  -0.875 */
  { 47.25,   24.5,  50,  5,  5}, /* ELM_A2   0.875 */
  {    35,   17.5,  50,  5,  5}, /* ELM_A3       0 */
  {    28,     14,  50,  5,  5}, /* ELM_A4       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"OV",   2, 8, 6,0x55,"U","U",ELM_FEATURE_BCK|ELM_FEATURE_RND|ELM_FEATURE_SMH|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   370,    170,  50,  4,  4}, /* ELM_F1     -15 */
  {  1000,    470,  50,  4,  4}, /* ELM_F2     -30 */
  {  2500,   1220,  50,  4,  4}, /* ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A1  -0.875 */
  {    42,     21,  50,  4,  4}, /* ELM_A2       0 */
  {    28,     14,  50,  4,  4}, /* ELM_A3       0 */
  { 22.75,   10.5,  50,  4,  4}, /* ELM_A4  -0.875 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"OA",   2, 9, 6,0xAB,"@","@",ELM_FEATURE_CNT|ELM_FEATURE_MDL|ELM_FEATURE_UNR|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   490,    230,  50,  5,  5}, /* ELM_F1     -15 */
  {  1480,    710,  50,  5,  5}, /* ELM_F2     -30 */
  {  2500,   1220,  50,  5,  5}, /* ELM_F3     -30 */
  {    60,     30,  50,  5,  5}, /* ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* ELM_A1  -0.875 */
  { 50.75,   24.5,  50,  5,  5}, /* ELM_A2  -0.875 */
  { 33.25,   17.5,  50,  5,  5}, /* ELM_A3   0.875 */
  { 26.25,     14,  50,  5,  5}, /* ELM_A4   0.875 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"IA",   2, 9, 6,0x49,"I","I",ELM_FEATURE_FNT|ELM_FEATURE_SMH|ELM_FEATURE_UNR|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   310,    170,  50,  5,  5}, /* ELM_F1      15 */
  {  2200,   1070,  50,  5,  5}, /* ELM_F2     -30 */
  {  2920,   1460,  50,  5,  5}, /* ELM_F3       0 */
  {    60,     30,  50,  5,  5}, /* ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* ELM_A1  -0.875 */
  {    35,   17.5,  50,  5,  5}, /* ELM_A2       0 */
  { 36.75,   17.5,  50,  5,  5}, /* ELM_A3  -0.875 */
  {  31.5,     14,  50,  5,  5}, /* ELM_A4   -1.75 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"IB",   2, 8, 6,0x51,"@","@",ELM_FEATURE_FNT|ELM_FEATURE_LOW|ELM_FEATURE_UNR|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   490,    230,  50,  4,  4}, /* ELM_F1     -15 */
  {  1480,    710,  50,  4,  4}, /* ELM_F2     -30 */
  {  2500,   1220,  50,  4,  4}, /* ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A1  -0.875 */
  { 50.75,   24.5,  50,  4,  4}, /* ELM_A2  -0.875 */
  { 33.25,   17.5,  50,  4,  4}, /* ELM_A3   0.875 */
  { 26.25,     14,  50,  4,  4}, /* ELM_A4   0.875 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"AIR",  2, 9, 6,0x45,"e","E",ELM_FEATURE_FNT|ELM_FEATURE_LMD|ELM_FEATURE_UNR|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   640,    350,  50,  5,  5}, /* ELM_F1      30 */
  {  2020,   1070,  50,  5,  5}, /* ELM_F2      60 */
  {  2500,   1220,  50,  5,  5}, /* ELM_F3     -30 */
  {    60,     30,  50,  5,  5}, /* ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* ELM_A1  -0.875 */
  {    42,     21,  50,  5,  5}, /* ELM_A2       0 */
  {  38.5,   17.5,  50,  5,  5}, /* ELM_A3   -1.75 */
  {  31.5,     14,  50,  5,  5}, /* ELM_A4   -1.75 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"OOR",  2, 9, 6,0x55,"U","U",ELM_FEATURE_BCK|ELM_FEATURE_RND|ELM_FEATURE_SMH|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   370,    170,  50,  5,  5}, /* ELM_F1     -15 */
  {  1000,    470,  50,  5,  5}, /* ELM_F2     -30 */
  {  2500,   1220,  50,  5,  5}, /* ELM_F3     -30 */
  {    60,     30,  50,  5,  5}, /* ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* ELM_A1  -0.875 */
  {    42,     21,  50,  5,  5}, /* ELM_A2       0 */
  {    28,     14,  50,  5,  5}, /* ELM_A3       0 */
  { 22.75,      7,  50,  5,  5}, /* ELM_A4  -4.375 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
},

{"OR",   2, 9, 6,0x8D,"O","O",ELM_FEATURE_BCK|ELM_FEATURE_LMD|ELM_FEATURE_RND|ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* ELM_FN       0 */
  {   490,    230,  50,  5,  5}, /* ELM_F1     -15 */
  {   820,    470,  50,  5,  5}, /* ELM_F2      60 */
  {  2500,   1220,  50,  5,  5}, /* ELM_F3     -30 */
  {    60,     30,  50,  5,  5}, /* ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* ELM_A1  -0.875 */
  {  45.5,     21,  50,  5,  5}, /* ELM_A2   -1.75 */
  { 22.75,   10.5,  50,  5,  5}, /* ELM_A3  -0.875 */
  {  17.5,      7,  50,  5,  5}, /* ELM_A4   -1.75 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* ELM_AF       0 */
 }
} 

};

static short clip(float input)
{
	int temp = (int)input;
	/* clip on boundaries of 16-bit word */

	if (temp < -32767)
	{
		//assert?
		temp = -32767;
	}
	else
	if (temp > 32767)
	{
		//assert?
		temp = 32767;
	}

	return (short)(temp);
}

/* Convert from decibels to a linear scale factor */
static float DBtoLIN(int dB)
{
	/*
	* Convertion table, db to linear, 87 dB --> 32767
	*                                 86 dB --> 29491 (1 dB down = 0.5**1/6)
	*                                 ...
	*                                 81 dB --> 16384 (6 dB down = 0.5)
	*                                 ...
	*                                  0 dB -->     0
	*
	* The just noticeable difference for a change in intensity of a vowel
	*   is approximately 1 dB.  Thus all amplitudes are quantized to 1 dB
	*   steps.
	*/

	static const float amptable[88] =
	{
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 6.0, 7.0,
		8.0, 9.0, 10.0, 11.0, 13.0,
		14.0, 16.0, 18.0, 20.0, 22.0,
		25.0, 28.0, 32.0, 35.0, 40.0,
		45.0, 51.0, 57.0, 64.0, 71.0,
		80.0, 90.0, 101.0, 114.0, 128.0,
		142.0, 159.0, 179.0, 202.0, 227.0,
		256.0, 284.0, 318.0, 359.0, 405.0,
		455.0, 512.0, 568.0, 638.0, 719.0,
		811.0, 911.0, 1024.0, 1137.0, 1276.0,
		1438.0, 1622.0, 1823.0, 2048.0, 2273.0,
		2552.0, 2875.0, 3244.0, 3645.0, 4096.0,
		4547.0, 5104.0, 5751.0, 6488.0, 7291.0,
		8192.0, 9093.0, 10207.0, 11502.0, 12976.0,
		14582.0, 16384.0, 18350.0, 20644.0, 23429.0,
		26214.0, 29491.0, 32767.0
	};

	// Check limits or argument (can be removed in final product)
	if (dB < 0)
	{
		dB = 0;
	}
	else
	if (dB >= 88)
	{
		dB = 87;
	}

	return amptable[dB] * 0.001f;
}



klatt_frame::klatt_frame() :
	mF0FundamentalFreq(1330),	mVoicingAmpdb(60),				mFormant1Freq(500),  
	mFormant1Bandwidth(60),		mFormant2Freq(1500),			mFormant2Bandwidth(90),
	mFormant3Freq(2800),		mFormant3Bandwidth(150),		mFormant4Freq(3250), 
	mFormant4Bandwidth(200),	mFormant5Freq(3700),			mFormant5Bandwidth(200),  
	mFormant6Freq(4990),		mFormant6Bandwidth(500),		mNasalZeroFreq(270),  
	mNasalZeroBandwidth(100),	mNasalPoleFreq(270),			mNasalPoleBandwidth(100), 
	mAspirationAmpdb(0),		mNoSamplesInOpenPeriod(30),		mVoicingBreathiness(0),      
	mVoicingSpectralTiltdb(10), mFricationAmpdb(0),				mSkewnessOfAlternatePeriods(0),   
	mFormant1Ampdb(0),			mFormant1ParallelBandwidth(80), mFormant2Ampdb(0),      
	mFormant2ParallelBandwidth(200), mFormant3Ampdb(0),			mFormant3ParallelBandwidth(350),
	mFormant4Ampdb(0),			mFormant4ParallelBandwidth(500), mFormant5Ampdb(0),      
	mFormant5ParallelBandwidth(600), mFormant6Ampdb(0),			mFormant6ParallelBandwidth(800),    
	mParallelNasalPoleAmpdb(0), mBypassFricationAmpdb(0),       mPalallelVoicingAmpdb(0),   
	mOverallGaindb(62)  
{
};


klatt::klatt() : 
	mF0Flutter(0), 
	mSampleRate(0), 
	mNspFr(0),
	mF0FundamentalFreq(0),
	mVoicingAmpdb(0),
	mSkewnessOfAlternatePeriods(0),
	mTimeCount(0), 
	mNPer(0),
	mT0(0),
	mNOpen(0),
	mNMod(0),
	mAmpVoice(0),
	mAmpBypas(0),
	mAmpAspir(0),
	mAmpFrica(0),
	mAmpBreth(0),
	mSkew(0),
	mNatglotA(0),
	mNatglotB(0),
	mVWave(0),
	mVLast(0),
	mNLast(0),
	mGlotLast(0),
	mDecay(0),
	mOneMd(0),
	mElementCount(0),
	mElement(0),
	mElementIndex(0),
	mLastElement(0),
	mTStress(0),
	mNTStress(0),
	mTop(0)
{
}

/*
function FLUTTER

This function adds F0 flutter, as specified in:

"Analysis, synthesis and perception of voice quality variations among
female and male talkers" D.H. Klatt and L.C. Klatt JASA 87(2) February 1990.
Flutter is added by applying a quasi-random element constructed from three
slowly varying sine waves.
*/
void klatt::flutter(klatt_frame *pars)
{
	int original_f0 = pars->mF0FundamentalFreq / 10;
	float fla = (float) mF0Flutter / 50;
	float flb = (float) original_f0 / 100;
	float flc = (float)sin(2 * PI * 12.7 * mTimeCount);
	float fld = (float)sin(2 * PI * 7.1 * mTimeCount);
	float fle = (float)sin(2 * PI * 4.7 * mTimeCount);
	float delta_f0 = fla * flb * (flc + fld + fle) * 10;
	mF0FundamentalFreq += (int) delta_f0;
}

/* Vwave is the differentiated glottal flow waveform, there is a weak
spectral zero around 800 Hz, magic constants a,b reset pitch-synch
*/

float klatt::natural_source(int aNper)
{
	// See if glottis open 

	if (aNper < mNOpen)
	{
		float lgtemp;
		mNatglotA -= mNatglotB;
		mVWave += mNatglotA;
		lgtemp = mVWave * 0.028f;        /* function of samp_rate ? */
		return (lgtemp);
	}
	else
	{
		// Glottis closed 
		mVWave = 0.0;
		return (0.0);
	}
}

/* Reset selected parameters pitch-synchronously */

void klatt::pitch_synch_par_reset(klatt_frame *frame, int ns)
{
	/*
	* Constant natglot[] controls shape of glottal pulse as a function
	* of desired duration of open phase N0
	* (Note that N0 is specified in terms of 40,000 samples/sec of speech)
	*
	*    Assume voicing waveform V(t) has form: k1 t**2 - k2 t**3
	*
	*    If the radiation characterivative, a temporal derivative
	*      is folded in, and we go from continuous time to discrete
	*      integers n:  dV/dt = mVWave[n]
	*                         = sum over i=1,2,...,n of { a - (i * b) }
	*                         = a n  -  b/2 n**2
	*
	*      where the  constants a and b control the detailed shape
	*      and amplitude of the voicing waveform over the open
	*      potion of the voicing cycle "mNOpen".
	*
	*    Let integral of dV/dt have no net dc flow --> a = (b * mNOpen) / 3
	*
	*    Let maximum of dUg(n)/dn be constant --> b = gain / (mNOpen * mNOpen)
	*      meaning as mNOpen gets bigger, V has bigger peak proportional to n
	*
	*    Thus, to generate the table below for 40 <= mNOpen <= 263:
	*
	*      natglot[mNOpen - 40] = 1920000 / (mNOpen * mNOpen)
	*/
	static const short natglot[224] =
	{
		1200, 1142, 1088, 1038, 991, 948, 907, 869, 833, 799,
		768, 738, 710, 683, 658, 634, 612, 590, 570, 551,
		533, 515, 499, 483, 468, 454, 440, 427, 415, 403,
		391, 380, 370, 360, 350, 341, 332, 323, 315, 307,
		300, 292, 285, 278, 272, 265, 259, 253, 247, 242,
		237, 231, 226, 221, 217, 212, 208, 204, 199, 195,
		192, 188, 184, 180, 177, 174, 170, 167, 164, 161,
		158, 155, 153, 150, 147, 145, 142, 140, 137, 135,
		133, 131, 128, 126, 124, 122, 120, 119, 117, 115,
		113, 111, 110, 108, 106, 105, 103, 102, 100, 99,
		97, 96, 95, 93, 92, 91, 90, 88, 87, 86,
		85, 84, 83, 82, 80, 79, 78, 77, 76, 75,
		75, 74, 73, 72, 71, 70, 69, 68, 68, 67,
		66, 65, 64, 64, 63, 62, 61, 61, 60, 59,
		59, 58, 57, 57, 56, 56, 55, 55, 54, 54,
		53, 53, 52, 52, 51, 51, 50, 50, 49, 49,
		48, 48, 47, 47, 46, 46, 45, 45, 44, 44,
		43, 43, 42, 42, 41, 41, 41, 41, 40, 40,
		39, 39, 38, 38, 38, 38, 37, 37, 36, 36,
		36, 36, 35, 35, 35, 35, 34, 34, 33, 33,
		33, 33, 32, 32, 32, 32, 31, 31, 31, 31,
		30, 30, 30, 30, 29, 29, 29, 29, 28, 28,
		28, 28, 27, 27
	};

	if (mF0FundamentalFreq > 0)
	{
		mT0 = (40 * mSampleRate) / mF0FundamentalFreq;

		/* Period in samp*4 */
		mAmpVoice = DBtoLIN(mVoicingAmpdb);

		/* Duration of period before amplitude modulation */
		mNMod = mT0;

		if (mVoicingAmpdb > 0)
		{
			mNMod >>= 1;
		}

		/* Breathiness of voicing waveform */

		mAmpBreth = DBtoLIN(frame->mVoicingBreathiness) * 0.1f;

		/* Set open phase of glottal period */
		/* where  40 <= open phase <= 263 */

		mNOpen = 4 * frame->mNoSamplesInOpenPeriod;

		if (mNOpen >= (mT0 - 1))
		{
			mNOpen = mT0 - 2;
		}

		if (mNOpen < 40)
		{
			mNOpen = 40;                  /* F0 max = 1000 Hz */
		}

		/* Reset a & b, which determine shape of "natural" glottal waveform */

		mNatglotB = natglot[mNOpen - 40];
		mNatglotA = (mNatglotB * mNOpen) * .333f;

		/* Reset width of "impulsive" glottal pulse */

		int temp;
		float temp1;

		temp = mSampleRate / mNOpen;
		mCritDampedGlotLowPassFilter.initResonator(0L, temp, mSampleRate);

		/* Make gain at F1 about constant */

		temp1 = mNOpen * .00833f;
		mCritDampedGlotLowPassFilter.setGain(temp1 * temp1);

		/* Truncate skewness so as not to exceed duration of closed phase
		of glottal period */

		temp = mT0 - mNOpen;

		if (mSkewnessOfAlternatePeriods > temp)
		{
			mSkewnessOfAlternatePeriods = temp;
		}

		if (mSkew >= 0)
		{
			mSkew = mSkewnessOfAlternatePeriods;                /* Reset mSkew to requested mSkewnessOfAlternatePeriods */
		}
		else
		{
			mSkew = -mSkewnessOfAlternatePeriods;
		}

		/* Add skewness to closed portion of voicing period */

		mT0 = mT0 + mSkew;
		mSkew = -mSkew;
	}
	else
	{
		mT0 = 4;                        /* Default for f0 undefined */
		mAmpVoice = 0.0;
		mNMod = mT0;
		mAmpBreth = 0.0;
		mNatglotA = 0.0;
		mNatglotB = 0.0;
	}

	/* Reset these pars pitch synchronously or at update rate if f0=0 */

	if ((mT0 != 4) || (ns == 0))
	{
		/* Set one-pole ELM_FEATURE_LOW-pass filter that tilts glottal source */
		mDecay = (0.033f * frame->mVoicingSpectralTiltdb);	/* Function of samp_rate ? */

		if (mDecay > 0.0f)
		{
			mOneMd = 1.0f - mDecay;
		}
		else
		{
			mOneMd = 1.0f;
		}
	}
}


/* Get variable parameters from host computer,
initially also get definition of fixed pars
*/

void klatt::frame_init(klatt_frame *frame)
{
	int mOverallGaindb;                       /* Overall gain, 60 dB is unity  0 to   60  */
	float amp_parF1;                 /* mFormant1Ampdb converted to linear gain  */
	float amp_parFN;                 /* mParallelNasalPoleAmpdb converted to linear gain  */
	float amp_parF2;                 /* mFormant2Ampdb converted to linear gain  */
	float amp_parF3;                 /* mFormant3Ampdb converted to linear gain  */
	float amp_parF4;                 /* mFormant4Ampdb converted to linear gain  */
	float amp_parF5;                 /* mFormant5Ampdb converted to linear gain  */
	float amp_parF6;                 /* mFormant6Ampdb converted to linear gain  */

	/* Read  speech frame definition into temp store
       and move some parameters into active use immediately
       (voice-excited ones are updated pitch synchronously
       to avoid waveform glitches).
	 */

	mF0FundamentalFreq = frame->mF0FundamentalFreq;
	mVoicingAmpdb = frame->mVoicingAmpdb - 7;

	if (mVoicingAmpdb < 0) mVoicingAmpdb = 0;

	mAmpAspir = DBtoLIN(frame->mAspirationAmpdb) * .05f;
	mAmpFrica = DBtoLIN(frame->mFricationAmpdb) * 0.25f;
	mSkewnessOfAlternatePeriods = frame->mSkewnessOfAlternatePeriods;

	/* Fudge factors (which comprehend affects of formants on each other?)
       with these in place ALL_PARALLEL should sound as close as
	   possible to CASCADE_PARALLEL.
	   Possible problem feeding in Holmes's amplitudes given this.
	*/
	amp_parF1 = DBtoLIN(frame->mFormant1Ampdb) * 0.4f;	/* -7.96 dB */
	amp_parF2 = DBtoLIN(frame->mFormant2Ampdb) * 0.15f;	/* -16.5 dB */
	amp_parF3 = DBtoLIN(frame->mFormant3Ampdb) * 0.06f;	/* -24.4 dB */
	amp_parF4 = DBtoLIN(frame->mFormant4Ampdb) * 0.04f;	/* -28.0 dB */
	amp_parF5 = DBtoLIN(frame->mFormant5Ampdb) * 0.022f;	/* -33.2 dB */
	amp_parF6 = DBtoLIN(frame->mFormant6Ampdb) * 0.03f;	/* -30.5 dB */
	amp_parFN = DBtoLIN(frame->mParallelNasalPoleAmpdb) * 0.6f;	/* -4.44 dB */
	mAmpBypas = DBtoLIN(frame->mBypassFricationAmpdb) * 0.05f;	/* -26.0 db */

	// Set coeficients of nasal resonator and zero antiresonator 
	mNasalPole.initResonator(frame->mNasalPoleFreq, frame->mNasalPoleBandwidth, mSampleRate);

	mNasalZero.initAntiresonator(frame->mNasalZeroFreq, frame->mNasalZeroBandwidth, mSampleRate);

	// Set coefficients of parallel resonators, and amplitude of outputs 
	mParallelFormant1.initResonator(frame->mFormant1Freq, frame->mFormant1ParallelBandwidth, mSampleRate);
	mParallelFormant1.setGain(amp_parF1);

	mParallelResoNasalPole.initResonator(frame->mNasalPoleFreq, frame->mNasalPoleBandwidth, mSampleRate);
	mParallelResoNasalPole.setGain(amp_parFN);

	mParallelFormant2.initResonator(frame->mFormant2Freq, frame->mFormant2ParallelBandwidth, mSampleRate);
	mParallelFormant2.setGain(amp_parF2);

	mParallelFormant3.initResonator(frame->mFormant3Freq, frame->mFormant3ParallelBandwidth, mSampleRate);
	mParallelFormant3.setGain(amp_parF3);

	mParallelFormant4.initResonator(frame->mFormant4Freq, frame->mFormant4ParallelBandwidth, mSampleRate);
	mParallelFormant4.setGain(amp_parF4);

	mParallelFormant5.initResonator(frame->mFormant5Freq, frame->mFormant5ParallelBandwidth, mSampleRate);
	mParallelFormant5.setGain(amp_parF5);

	mParallelFormant6.initResonator(frame->mFormant6Freq, frame->mFormant6ParallelBandwidth, mSampleRate);
	mParallelFormant6.setGain(amp_parF6);


	/* fold overall gain into output resonator */
	mOverallGaindb = frame->mOverallGaindb - 3;

	if (mOverallGaindb <= 0)
		mOverallGaindb = 57;

	/* output ELM_FEATURE_LOW-pass filter - resonator with freq 0 and BW = globals->mSampleRate
	Thus 3db point is globals->mSampleRate/2 i.e. Nyquist limit.
	Only 3db down seems rather mild...
	*/
	mOutputLowPassFilter.initResonator(0L, (int) mSampleRate, mSampleRate);
	mOutputLowPassFilter.setGain(DBtoLIN(mOverallGaindb));
}

/*
function PARWAV

CONVERT FRAME OF PARAMETER DATA TO A WAVEFORM CHUNK
Synthesize globals->mNspFr samples of waveform and store in jwave[].
*/

void klatt::parwave(klatt_frame *frame, short int *jwave)
{
	/* Output of cascade branch, also final output  */

	/* Initialize synthesizer and get specification for current speech
	frame from host microcomputer */

	frame_init(frame);

	if (mF0Flutter != 0)
	{
		mTimeCount++;                  /* used for f0 flutter */
		flutter(frame);       /* add f0 flutter */
	}

	/* MAIN LOOP, for each output sample of current frame: */

	int ns;
	for (ns = 0; ns < mNspFr; ns++)
	{
		static unsigned int seed = 5; /* Fixed staring value */
		float noise;
		int n4;
		float sourc;                   /* Sound source if all-parallel config used  */
		float glotout;                 /* Output of glottal sound source  */
		float par_glotout;             /* Output of parallelglottal sound sourc  */
		float voice = 0;               /* Current sample of voicing waveform  */
		float frics;                   /* Frication sound source  */
		float aspiration;              /* Aspiration sound source  */
		int nrand;                    /* Varible used by random number generator  */

		/* Our own code like rand(), but portable
		whole upper 31 bits of seed random
		assumes 32-bit unsigned arithmetic
		with untested code to handle larger.
		*/
		seed = seed * 1664525 + 1;

		if (8 * sizeof(unsigned int) > 32)
			seed &= 0xFFFFFFFF;

		/* Shift top bits of seed up to top of int then back down to LS 14 bits */
		/* Assumes 8 bits per sizeof unit i.e. a "byte" */
		nrand = (((int) seed) << (8 * sizeof(int) - 32)) >> (8 * sizeof(int) - 14);

		/* Tilt down noise spectrum by soft ELM_FEATURE_LOW-pass filter having
		*    a pole near the origin in the z-plane, i.e.
		*    output = input + (0.75 * lastoutput) */

		noise = nrand + (0.75f * mNLast);	/* Function of samp_rate ? */

		mNLast = noise;

		/* Amplitude modulate noise (reduce noise amplitude during
		second half of glottal period) if voicing simultaneously present
		*/

		if (mNPer > mNMod)
		{
			noise *= 0.5f;
		}

		/* Compute frication noise */
		sourc = frics = mAmpFrica * noise;

		/* Compute voicing waveform : (run glottal source simulation at
		4 times normal sample rate to minimize quantization noise in
		period of female voice)
		*/

		for (n4 = 0; n4 < 4; n4++)
		{
			/* use a more-natural-shaped source waveform with excitation
			occurring both upon opening and upon closure, stronest at closure */
			voice = natural_source(mNPer);

			/* Reset period when counter 'mNPer' reaches mT0 */

			if (mNPer >= mT0)
			{
				mNPer = 0;
				pitch_synch_par_reset(frame, ns);
			}

			/* Low-pass filter voicing waveform before downsampling from 4*globals->mSampleRate */
			/* to globals->mSampleRate samples/sec.  Resonator f=.09*globals->mSampleRate, bw=.06*globals->mSampleRate  */

			voice = mDownSampLowPassFilter.resonate(voice);	/* in=voice, out=voice */

			/* Increment counter that keeps track of 4*globals->mSampleRate samples/sec */
			mNPer++;
		}

		/* Tilt spectrum of voicing source down by soft ELM_FEATURE_LOW-pass filtering, amount
		of tilt determined by mVoicingSpectralTiltdb
		*/
		voice = (voice * mOneMd) + (mVLast * mDecay);

		mVLast = voice;

		/* Add breathiness during glottal open phase */
		if (mNPer < mNOpen)
		{
			/* Amount of breathiness determined by parameter mVoicingBreathiness */
			/* Use nrand rather than noise because noise is ELM_FEATURE_LOW-passed */
			voice += mAmpBreth * nrand;
		}

		/* Set amplitude of voicing */
		glotout = mAmpVoice * voice;

		/* Compute aspiration amplitude and add to voicing source */
		aspiration = mAmpAspir * noise;

		glotout += aspiration;

		par_glotout = glotout;

		/* NIS - rsynth "hack"
		As Holmes' scheme is weak at nasals and (physically) nasal cavity
		is "back near glottis" feed glottal source through nasal resonators
		Don't think this is quite right, but improves things a bit
		*/
		par_glotout = mNasalZero.antiresonate(par_glotout);
		par_glotout = mNasalPole.resonate(par_glotout);
		/* And just use mParallelFormant1 NOT mParallelResoNasalPole */		
		float out = mParallelFormant1.resonate(par_glotout);
		/* Sound sourc for other parallel resonators is frication
		plus first difference of voicing waveform.
		*/
		sourc += (par_glotout - mGlotLast);
		mGlotLast = par_glotout;

		/* Standard parallel vocal tract
		Formants F6,F5,F4,F3,F2, outputs added with alternating sign
		*/
		out = mParallelFormant6.resonate(sourc) - out;
		out = mParallelFormant5.resonate(sourc) - out;
		out = mParallelFormant4.resonate(sourc) - out;
		out = mParallelFormant3.resonate(sourc) - out;
		out = mParallelFormant2.resonate(sourc) - out;
		out = mAmpBypas * sourc - out;
		out = mOutputLowPassFilter.resonate(out);

		*jwave++ = clip(out); /* Convert back to integer */
	}
}



static char * phoneme_to_element_lookup(char *s, void ** data)
{
	int key8 = *s;
	int key16 = key8 + (s[1] << 8);
	if (s[1] == 0) key16 = -1; // avoid key8==key16
	int i;
	for (i = 0; i < PHONEME_COUNT; i++)
	{
		if (phoneme_to_elements[i].mKey == key16)
		{
			*data = &phoneme_to_elements[i].mData;
			return s+2;
		}
		if (phoneme_to_elements[i].mKey == key8)
		{
			*data = &phoneme_to_elements[i].mData;
			return s+1;
		}
	}
	// should never happen
	*data = NULL;
	return s+1;
}



int klatt::phone_to_elm(char *aPhoneme, int aCount, darray *aElement)
{
	int stress = 0;
	char *s = aPhoneme;
	int t = 0;
	char *limit = s + aCount;

	while (s < limit && *s)
	{
		char *e = NULL;
		s = phoneme_to_element_lookup(s, (void**)&e);

		if (e)
		{
			int n = *e++;

			while (n-- > 0)
			{
				int x = *e++;
				Element * p = &gElement[x];
				/* This works because only vowels have mUD != mDU,
				and we set stress just before a vowel
				*/
				aElement->put(x);

				if (!(p->mFeat & ELM_FEATURE_VWL))
					stress = 0;

                int stressdur = StressDur(p,stress);

                t += stressdur;

				aElement->put(stressdur);
				aElement->put(stress);
			}
		}

		else
		{
			char ch = *s++;

			switch (ch)
			{

			case '\'':                /* Primary stress */
				stress = 3;
				break;

			case ',':                 /* Secondary stress */
				stress = 2;
				break;

			case '+':                 /* Tertiary stress */
				stress = 1;
				break;

			case '-':                 /* hyphen in input */
				break;

			default:
//				fprintf(stderr, "Ignoring %c in '%.*s'\n", ch, aCount, aPhoneme);
				break;
			}
		}
	}

	return t;
}



/* 'a' is dominant element, 'b' is dominated
    ext is flag to say to use external times from 'a' rather
    than internal i.e. ext != 0 if 'a' is NOT current element.
 */

static void set_trans(Slope *t, Element * a, Element * b,int ext, int e)
{
	int i;

	for (i = 0; i < ELM_COUNT; i++)
	{
		t[i].mTime = ((ext) ? a->mInterpolator[i].mExtDelay : a->mInterpolator[i].mIntDelay);

		if (t[i].mTime)
		{
			t[i].mValue = a->mInterpolator[i].mFixed + (a->mInterpolator[i].mProportion * b->mInterpolator[i].mSteady) * 0.01f; // mProportion is in scale 0..100, so *0.01.
		}
		else
		{
			t[i].mValue = b->mInterpolator[i].mSteady;
		}
	}
}

static float lerp(float a, float b, int t, int d)
{
	if (t <= 0)
	{
		return a;
	}

	if (t >= d)
	{
		return b;
	}

	float f = (float)t / (float)d;
	return a + (b - a) * f;
}

static float interpolate(Slope *aStartSlope, Slope *aEndSlope, float aMidValue, int aTime, int aDuration)
{
	int steadyTime = aDuration - (aStartSlope->mTime + aEndSlope->mTime);

	if (steadyTime >= 0)
	{
		// Interpolate to a midpoint, stay there for a while, then interpolate to end

		if (aTime < aStartSlope->mTime)
		{
			// interpolate to the first value
			return lerp(aStartSlope->mValue, aMidValue, aTime, aStartSlope->mTime);
		}
		// reached midpoint

		aTime -= aStartSlope->mTime;

		if (aTime <= steadyTime)
		{
			// still at steady state
			return aMidValue;  
		}

		// interpolate to the end
		return lerp(aMidValue, aEndSlope->mValue, aTime - steadyTime, aEndSlope->mTime);
	}
	else
	{
		// No steady state
		float f = 1.0f - ((float) aTime / (float) aDuration);
		float sp = lerp(aStartSlope->mValue, aMidValue, aTime, aStartSlope->mTime);
		float ep = lerp(aEndSlope->mValue, aMidValue, aDuration - aTime, aEndSlope->mTime);
		return f * sp + ((float) 1.0 - f) * ep;
	}
}



void klatt::initsynth(int aElementCount,unsigned char *aElement)
{
	mElement = aElement;
	mElementCount = aElementCount;
	mElementIndex = 0;
	mLastElement = &gElement[0];
	mTStress = 0;
	mNTStress = 0;
	mKlattFramePars.mF0FundamentalFreq = 1330;
	mTop = 1.1f * mKlattFramePars.mF0FundamentalFreq;
	mKlattFramePars.mNasalPoleFreq = (int)mLastElement->mInterpolator[ELM_FN].mSteady;
	mKlattFramePars.mFormant1ParallelBandwidth = mKlattFramePars.mFormant1Bandwidth = 60;
	mKlattFramePars.mFormant2ParallelBandwidth = mKlattFramePars.mFormant2Bandwidth = 90;
	mKlattFramePars.mFormant3ParallelBandwidth = mKlattFramePars.mFormant3Bandwidth = 150;
//	mKlattFramePars.mFormant4ParallelBandwidth = (default)

	// Set stress attack/decay slope 
	mStressS.mTime = 40;
	mStressE.mTime = 40;
	mStressE.mValue = 0.0;
}

int klatt::synth(int aSampleCount, short *aSamplePointer)
{
	short *samp = aSamplePointer;

	if (mElementIndex >= mElementCount)
		return -1;

	Element * currentElement = &gElement[mElement[mElementIndex++]];
	int dur = mElement[mElementIndex++];
	mElementIndex++; // skip stress 

	// Skip zero length elements which are only there to affect
	// boundary values of adjacent elements		

	if (dur > 0)
	{
		Element * ne = (mElementIndex < mElementCount) ? &gElement[mElement[mElementIndex]] : &gElement[0];
		Slope start[ELM_COUNT];
		Slope end[ELM_COUNT];
		int t;

		if (currentElement->mRK > mLastElement->mRK)
		{
			set_trans(start, currentElement, mLastElement, 0, 's');
			// we dominate last 
		}
		else
		{
			set_trans(start, mLastElement, currentElement, 1, 's');
			// last dominates us 
		}

		if (ne->mRK > currentElement->mRK)
		{
			set_trans(end, ne, currentElement, 1, 'e');
			// next dominates us 
		}
		else
		{
			set_trans(end, currentElement, ne, 0, 'e');
			// we dominate next 
		}

		for (t = 0; t < dur; t++, mTStress++)
		{
			float base = mTop * 0.8f; // 3 * top / 5 
			float tp[ELM_COUNT];
			int j;

			if (mTStress == mNTStress)
			{
				int j = mElementIndex;
				mStressS = mStressE;
				mTStress = 0;
				mNTStress = dur;

				while (j <= mElementCount)
				{
					Element * e   = (j < mElementCount) ? &gElement[mElement[j++]] : &gElement[0];
					int du = (j < mElementCount) ? mElement[j++] : 0;
					int s  = (j < mElementCount) ? mElement[j++] : 3;

					if (s || e->mFeat & ELM_FEATURE_VWL)
					{
						int d = 0;

						if (s)
							mStressE.mValue = (float) s / 3;
						else
							mStressE.mValue = (float) 0.1;

						do
						{
							d += du;
							e = (j < mElementCount) ? &gElement[mElement[j++]] : &gElement[0];
							du = mElement[j++];
						}

						while ((e->mFeat & ELM_FEATURE_VWL) && mElement[j++] == s);

						mNTStress += d / 2;

						break;
					}

					mNTStress += du;
				}
			}

			for (j = 0; j < ELM_COUNT; j++)
			{
				tp[j] = interpolate(&start[j], &end[j], (float) currentElement->mInterpolator[j].mSteady, t, dur);
			}

			// Now call the synth for each frame 

			mKlattFramePars.mF0FundamentalFreq = (int)(base + (mTop - base) * interpolate(&mStressS, &mStressE, (float) 0, mTStress, mNTStress));
			mKlattFramePars.mVoicingAmpdb = mKlattFramePars.mPalallelVoicingAmpdb = (int)tp[ELM_AV];
			mKlattFramePars.mFricationAmpdb = (int)tp[ELM_AF];
			mKlattFramePars.mNasalZeroFreq = (int)tp[ELM_FN];
			mKlattFramePars.mAspirationAmpdb = (int)tp[ELM_ASP];
			mKlattFramePars.mVoicingBreathiness = (int)tp[ELM_AVC];
			mKlattFramePars.mFormant1ParallelBandwidth = mKlattFramePars.mFormant1Bandwidth = (int)tp[ELM_B1];
			mKlattFramePars.mFormant2ParallelBandwidth = mKlattFramePars.mFormant2Bandwidth = (int)tp[ELM_B2];
			mKlattFramePars.mFormant3ParallelBandwidth = mKlattFramePars.mFormant3Bandwidth = (int)tp[ELM_B3];
			mKlattFramePars.mFormant1Freq = (int)tp[ELM_F1];
			mKlattFramePars.mFormant2Freq = (int)tp[ELM_F2];
			mKlattFramePars.mFormant3Freq = (int)tp[ELM_F3];

			// AMP_ADJ + is a kludge to get amplitudes up to klatt-compatible levels
				
				
			//pars.mParallelNasalPoleAmpdb  = AMP_ADJ + tp[ELM_AN];
				
			mKlattFramePars.mBypassFricationAmpdb = AMP_ADJ + (int)tp[ELM_AB];
			mKlattFramePars.mFormant5Ampdb = AMP_ADJ + (int)tp[ELM_A5];
			mKlattFramePars.mFormant6Ampdb = AMP_ADJ + (int)tp[ELM_A6];
			mKlattFramePars.mFormant1Ampdb = AMP_ADJ + (int)tp[ELM_A1];
			mKlattFramePars.mFormant2Ampdb = AMP_ADJ + (int)tp[ELM_A2];
			mKlattFramePars.mFormant3Ampdb = AMP_ADJ + (int)tp[ELM_A3];
			mKlattFramePars.mFormant4Ampdb = AMP_ADJ + (int)tp[ELM_A4];

			parwave(&mKlattFramePars, samp);

			samp += mNspFr;

			// Declination of f0 envelope 0.25Hz / cS 
			mTop -= 0.5;
		}
	}

	mLastElement = currentElement;

	return (int)(samp - aSamplePointer);
}


void klatt::init()
{
    mSampleRate = 11025;
    mF0Flutter = 0;

	int FLPhz = (950 * mSampleRate) / 10000;
	int BLPhz = (630 * mSampleRate) / 10000;
	mNspFr = (int)(mSampleRate * 10) / 1000;

	mDownSampLowPassFilter.initResonator(FLPhz, BLPhz, mSampleRate);

	mNPer = 0;                        /* LG */
	mT0 = 0;                          /* LG */

	mVLast = 0;                       /* Previous output of voice  */
	mNLast = 0;                       /* Previous output of random number generator  */
	mGlotLast = 0;                    /* Previous value of glotout  */
}
#ifdef __clang__
#   pragma clang diagnostic pop
#endif
//----../src/audiosource/speech/tts.cpp-----------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

static const char *ASCII[] =
{
	"null", "", "", "",
	"", "", "", "",
	"", "", "", "",
	"", "", "", "",
	"", "", "", "",
	"", "", "", "",
	"", "", "", "",
	"", "", "", "",
	"space", "exclamation mark", "double quote", "hash",
	"dollar", "percent", "ampersand", "quote",
	"open parenthesis", "close parenthesis", "asterisk", "plus",
	"comma", "minus", "full stop", "slash",
	"zero", "one", "two", "three",
	"four", "five", "six", "seven",
	"eight", "nine", "colon", "semi colon",
	"less than", "equals", "greater than", "question mark",
#ifndef ALPHA_IN_DICT
	"at", "ay", "bee", "see",
	"dee", "e", "eff", "gee",
	"aych", "i", "jay", "kay",
	"ell", "em", "en", "ohe",
	"pee", "kju", "are", "es",
	"tee", "you", "vee", "double you",
	"eks", "why", "zed", "open bracket",
#else                             /* ALPHA_IN_DICT */
	"at", "A", "B", "C",
	"D", "E", "F", "G",
	"H", "I", "J", "K",
	"L", "M", "N", "O",
	"P", "Q", "R", "S",
	"T", "U", "V", "W",
	"X", "Y", "Z", "open bracket",
#endif                            /* ALPHA_IN_DICT */
	"back slash", "close bracket", "circumflex", "underscore",
#ifndef ALPHA_IN_DICT
	"back quote", "ay", "bee", "see",
	"dee", "e", "eff", "gee",
	"aych", "i", "jay", "kay",
	"ell", "em", "en", "ohe",
	"pee", "kju", "are", "es",
	"tee", "you", "vee", "double you",
	"eks", "why", "zed", "open brace",
#else                             /* ALPHA_IN_DICT */
	"back quote", "A", "B", "C",
	"D", "E", "F", "G",
	"H", "I", "J", "K",
	"L", "M", "N", "O",
	"P", "Q", "R", "S",
	"T", "U", "V", "W",
	"X", "Y", "Z", "open brace",
#endif                            /* ALPHA_IN_DICT */
	"vertical bar", "close brace", "tilde", "delete",
	NULL
};

/* Context definitions */
static const char Anything[] = "";
/* No context requirement */

static const char Nothing[] = " ";
/* Context is beginning or end of word */

static const char Silent[] = "";
/* No phonemes */


#define LEFT_PART       0
#define MATCH_PART      1
#define RIGHT_PART      2
#define OUT_PART        3

typedef const char *Rule[4];
/* Rule is an array of 4 character pointers */


/*0 = Punctuation */
/*
**      LEFT_PART       MATCH_PART      RIGHT_PART      OUT_PART
*/


static Rule punct_rules[] =
{
	{Anything, " ", Anything, " "},
	{Anything, "-", Anything, ""},
	{".", "'S", Anything, "z"},
	{"#:.E", "'S", Anything, "z"},
	{"#", "'S", Anything, "z"},
	{Anything, "'", Anything, ""},
	{Anything, ",", Anything, " "},
	{Anything, ".", Anything, " "},
	{Anything, "?", Anything, " "},
	{Anything, "!", Anything, " "},
	{Anything, 0, Anything, Silent},
};

static Rule A_rules[] =
{
	{Anything, "A", Nothing, "@"},
	{Nothing, "ARE", Nothing, "0r"},
	{Nothing, "AR", "O", "@r"},
	{Anything, "AR", "#", "er"},
	{"^", "AS", "#", "eIs"},
	{Anything, "A", "WA", "@"},
	{Anything, "AW", Anything, "O"},
	{" :", "ANY", Anything, "eni"},
	{Anything, "A", "^+#", "eI"},
	{"#:", "ALLY", Anything, "@li"},
	{Nothing, "AL", "#", "@l"},
	{Anything, "AGAIN", Anything, "@gen"},
	{"#:", "AG", "E", "IdZ"},
	{Anything, "A", "^+:#", "&"},
	{" :", "A", "^+ ", "eI"},
	{Anything, "A", "^%", "eI"},
	{Nothing, "ARR", Anything, "@r"},
	{Anything, "ARR", Anything, "&r"},
	{" :", "AR", Nothing, "0r"},
	{Anything, "AR", Nothing, "3"},
	{Anything, "AR", Anything, "0r"},
	{Anything, "AIR", Anything, "er"},
	{Anything, "AI", Anything, "eI"},
	{Anything, "AY", Anything, "eI"},
	{Anything, "AU", Anything, "O"},
	{"#:", "AL", Nothing, "@l"},
	{"#:", "ALS", Nothing, "@lz"},
	{Anything, "ALK", Anything, "Ok"},
	{Anything, "AL", "^", "Ol"},
	{" :", "ABLE", Anything, "eIb@l"},
	{Anything, "ABLE", Anything, "@b@l"},
	{Anything, "ANG", "+", "eIndZ"},
	{"^", "A", "^#", "eI"},
	{Anything, "A", Anything, "&"},
	{Anything, 0, Anything, Silent},
};

static Rule B_rules[] =
{
	{Nothing, "BE", "^#", "bI"},
	{Anything, "BEING", Anything, "biIN"},
	{Nothing, "BOTH", Nothing, "b@UT"},
	{Nothing, "BUS", "#", "bIz"},
	{Anything, "BUIL", Anything, "bIl"},
	{Anything, "B", Anything, "b"},
	{Anything, 0, Anything, Silent},
};

static Rule C_rules[] =
{
	{Nothing, "CH", "^", "k"},
	{"^E", "CH", Anything, "k"},
	{Anything, "CH", Anything, "tS"},
	{" S", "CI", "#", "saI"},
	{Anything, "CI", "A", "S"},
	{Anything, "CI", "O", "S"},
	{Anything, "CI", "EN", "S"},
	{Anything, "C", "+", "s"},
	{Anything, "CK", Anything, "k"},
	{Anything, "COM", "%", "kVm"},
	{Anything, "C", Anything, "k"},
	{Anything, 0, Anything, Silent},
};

static Rule D_rules[] =
{
	{"#:", "DED", Nothing, "dId"},
	{".E", "D", Nothing, "d"},
	{"#:^E", "D", Nothing, "t"},
	{Nothing, "DE", "^#", "dI"},
	{Nothing, "DO", Nothing, "mDU"},
	{Nothing, "DOES", Anything, "dVz"},
	{Nothing, "DOING", Anything, "duIN"},
	{Nothing, "DOW", Anything, "daU"},
	{Anything, "DU", "A", "dZu"},
	{Anything, "D", Anything, "d"},
	{Anything, 0, Anything, Silent},
};

static Rule E_rules[] =
{
	{"#:", "E", Nothing, ""},
	{"':^", "E", Nothing, ""},
	{" :", "E", Nothing, "i"},
	{"#", "ED", Nothing, "d"},
	{"#:", "E", "D ", ""},
	{Anything, "EV", "ER", "ev"},
	{Anything, "E", "^%", "i"},
	{Anything, "ERI", "#", "iri"},
	{Anything, "ERI", Anything, "erI"},
	{"#:", "ER", "#", "3"},
	{Anything, "ER", "#", "er"},
	{Anything, "ER", Anything, "3"},
	{Nothing, "EVEN", Anything, "iven"},
	{"#:", "E", "W", ""},
	{"T", "EW", Anything, "u"},
	{"S", "EW", Anything, "u"},
	{"R", "EW", Anything, "u"},
	{"D", "EW", Anything, "u"},
	{"L", "EW", Anything, "u"},
	{"Z", "EW", Anything, "u"},
	{"N", "EW", Anything, "u"},
	{"J", "EW", Anything, "u"},
	{"TH", "EW", Anything, "u"},
	{"CH", "EW", Anything, "u"},
	{"SH", "EW", Anything, "u"},
	{Anything, "EW", Anything, "ju"},
	{Anything, "E", "O", "i"},
	{"#:S", "ES", Nothing, "Iz"},
	{"#:C", "ES", Nothing, "Iz"},
	{"#:G", "ES", Nothing, "Iz"},
	{"#:Z", "ES", Nothing, "Iz"},
	{"#:X", "ES", Nothing, "Iz"},
	{"#:J", "ES", Nothing, "Iz"},
	{"#:CH", "ES", Nothing, "Iz"},
	{"#:SH", "ES", Nothing, "Iz"},
	{"#:", "E", "S ", ""},
	{"#:", "ELY", Nothing, "li"},
	{"#:", "EMENT", Anything, "ment"},
	{Anything, "EFUL", Anything, "fUl"},
	{Anything, "EE", Anything, "i"},
	{Anything, "EARN", Anything, "3n"},
	{Nothing, "EAR", "^", "3"},
	{Anything, "EAD", Anything, "ed"},
	{"#:", "EA", Nothing, "i@"},
	{Anything, "EA", "SU", "e"},
	{Anything, "EA", Anything, "i"},
	{Anything, "EIGH", Anything, "eI"},
	{Anything, "EI", Anything, "i"},
	{Nothing, "EYE", Anything, "aI"},
	{Anything, "EY", Anything, "i"},
	{Anything, "EU", Anything, "ju"},
	{Anything, "E", Anything, "e"},
	{Anything, 0, Anything, Silent},
};

static Rule F_rules[] =
{
	{Anything, "FUL", Anything, "fUl"},
	{Anything, "F", Anything, "f"},
	{Anything, 0, Anything, Silent},
};

static Rule G_rules[] =
{
	{Anything, "GIV", Anything, "gIv"},
	{Nothing, "G", "I^", "g"},
	{Anything, "GE", "T", "ge"},
	{"SU", "GGES", Anything, "gdZes"},
	{Anything, "GG", Anything, "g"},
	{" B#", "G", Anything, "g"},
	{Anything, "G", "+", "dZ"},
	{Anything, "GREAT", Anything, "greIt"},
	{"#", "GH", Anything, ""},
	{Anything, "G", Anything, "g"},
	{Anything, 0, Anything, Silent},
};

static Rule H_rules[] =
{
	{Nothing, "HAV", Anything, "h&v"},
	{Nothing, "HERE", Anything, "hir"},
	{Nothing, "HOUR", Anything, "aU3"},
	{Anything, "HOW", Anything, "haU"},
	{Anything, "H", "#", "h"},
	{Anything, "H", Anything, ""},
	{Anything, 0, Anything, Silent},
};

static Rule I_rules[] =
{
	{Nothing, "IAIN", Nothing, "I@n"},
	{Nothing, "ING", Nothing, "IN"},
	{Nothing, "IN", Anything, "In"},
	{Nothing, "I", Nothing, "aI"},
	{Anything, "IN", "D", "aIn"},
	{Anything, "IER", Anything, "i3"},
	{"#:R", "IED", Anything, "id"},
	{Anything, "IED", Nothing, "aId"},
	{Anything, "IEN", Anything, "ien"},
	{Anything, "IE", "T", "aIe"},
	{" :", "I", "%", "aI"},
	{Anything, "I", "%", "i"},
	{Anything, "IE", Anything, "i"},
	{Anything, "I", "^+:#", "I"},
	{Anything, "IR", "#", "aIr"},
	{Anything, "IZ", "%", "aIz"},
	{Anything, "IS", "%", "aIz"},
	{Anything, "I", "D%", "aI"},
	{"+^", "I", "^+", "I"},
	{Anything, "I", "T%", "aI"},
	{"#:^", "I", "^+", "I"},
	{Anything, "I", "^+", "aI"},
	{Anything, "IR", Anything, "3"},
	{Anything, "IGH", Anything, "aI"},
	{Anything, "ILD", Anything, "aIld"},
	{Anything, "IGN", Nothing, "aIn"},
	{Anything, "IGN", "^", "aIn"},
	{Anything, "IGN", "%", "aIn"},
	{Anything, "IQUE", Anything, "ik"},
	{"^", "I", "^#", "aI"},
	{Anything, "I", Anything, "I"},
	{Anything, 0, Anything, Silent},
};

static Rule J_rules[] =
{
	{Anything, "J", Anything, "dZ"},
	{Anything, 0, Anything, Silent},
};

static Rule K_rules[] =
{
	{Nothing, "K", "N", ""},
	{Anything, "K", Anything, "k"},
	{Anything, 0, Anything, Silent},
};

static Rule L_rules[] =
{
	{Anything, "LO", "C#", "l@U"},
	{"L", "L", Anything, ""},
	{"#:^", "L", "%", "@l"},
	{Anything, "LEAD", Anything, "lid"},
	{Anything, "L", Anything, "l"},
	{Anything, 0, Anything, Silent},
};

static Rule M_rules[] =
{
	{Anything, "MOV", Anything, "muv"},
	{"#", "MM", "#", "m"},
	{Anything, "M", Anything, "m"},
	{Anything, 0, Anything, Silent},
};

static Rule N_rules[] =
{
	{"E", "NG", "+", "ndZ"},
	{Anything, "NG", "R", "Ng"},
	{Anything, "NG", "#", "Ng"},
	{Anything, "NGL", "%", "Ng@l"},
	{Anything, "NG", Anything, "N"},
	{Anything, "NK", Anything, "Nk"},
	{Nothing, "NOW", Nothing, "naU"},
	{"#", "NG", Nothing, "Ng"},
	{Anything, "N", Anything, "n"},
	{Anything, 0, Anything, Silent},
};

static Rule O_rules[] =
{
	{Anything, "OF", Nothing, "@v"},
	{Anything, "OROUGH", Anything, "3@U"},
	{"#:", "OR", Nothing, "3"},
	{"#:", "ORS", Nothing, "3z"},
	{Anything, "OR", Anything, "Or"},
	{Nothing, "ONE", Anything, "wVn"},
	{Anything, "OW", Anything, "@U"},
	{Nothing, "OVER", Anything, "@Uv3"},
	{Anything, "OV", Anything, "Vv"},
	{Anything, "O", "^%", "@U"},
	{Anything, "O", "^EN", "@U"},
	{Anything, "O", "^I#", "@U"},
	{Anything, "OL", "D", "@Ul"},
	{Anything, "OUGHT", Anything, "Ot"},
	{Anything, "OUGH", Anything, "Vf"},
	{Nothing, "OU", Anything, "aU"},
	{"H", "OU", "S#", "aU"},
	{Anything, "OUS", Anything, "@s"},
	{Anything, "OUR", Anything, "Or"},
	{Anything, "OULD", Anything, "Ud"},
	{"^", "OU", "^L", "V"},
	{Anything, "OUP", Anything, "up"},
	{Anything, "OU", Anything, "aU"},
	{Anything, "OY", Anything, "oI"},
	{Anything, "OING", Anything, "@UIN"},
	{Anything, "OI", Anything, "oI"},
	{Anything, "OOR", Anything, "Or"},
	{Anything, "OOK", Anything, "Uk"},
	{Anything, "OOD", Anything, "Ud"},
	{Anything, "OO", Anything, "u"},
	{Anything, "O", "E", "@U"},
	{Anything, "O", Nothing, "@U"},
	{Anything, "OA", Anything, "@U"},
	{Nothing, "ONLY", Anything, "@Unli"},
	{Nothing, "ONCE", Anything, "wVns"},
	{Anything, "ON'T", Anything, "@Unt"},
	{"C", "O", "N", "0"},
	{Anything, "O", "NG", "O"},
	{" :^", "O", "N", "V"},
	{"I", "ON", Anything, "@n"},
	{"#:", "ON", Nothing, "@n"},
	{"#^", "ON", Anything, "@n"},
	{Anything, "O", "ST ", "@U"},
	{Anything, "OF", "^", "Of"},
	{Anything, "OTHER", Anything, "VD3"},
	{Anything, "OSS", Nothing, "Os"},
	{"#:^", "OM", Anything, "Vm"},
	{Anything, "O", Anything, "0"},
	{Anything, 0, Anything, Silent},
};

static Rule P_rules[] =
{
	{Anything, "PH", Anything, "f"},
	{Anything, "PEOP", Anything, "pip"},
	{Anything, "POW", Anything, "paU"},
	{Anything, "PUT", Nothing, "pUt"},
	{Anything, "P", Anything, "p"},
	{Anything, 0, Anything, Silent},
};

static Rule Q_rules[] =
{
	{Anything, "QUAR", Anything, "kwOr"},
	{Anything, "QU", Anything, "kw"},
	{Anything, "Q", Anything, "k"},
	{Anything, 0, Anything, Silent},
};

static Rule R_rules[] =
{
	{Nothing, "RE", "^#", "ri"},
	{Anything, "R", Anything, "r"},
	{Anything, 0, Anything, Silent},
};

static Rule S_rules[] =
{
	{Anything, "SH", Anything, "S"},
	{"#", "SION", Anything, "Z@n"},
	{Anything, "SOME", Anything, "sVm"},
	{"#", "SUR", "#", "Z3"},
	{Anything, "SUR", "#", "S3"},
	{"#", "SU", "#", "Zu"},
	{"#", "SSU", "#", "Su"},
	{"#", "SED", Nothing, "zd"},
	{"#", "S", "#", "z"},
	{Anything, "SAID", Anything, "sed"},
	{"^", "SION", Anything, "S@n"},
	{Anything, "S", "S", ""},
	{".", "S", Nothing, "z"},
	{"#:.E", "S", Nothing, "z"},
	{"#:^##", "S", Nothing, "z"},
	{"#:^#", "S", Nothing, "s"},
	{"U", "S", Nothing, "s"},
	{" :#", "S", Nothing, "z"},
	{Nothing, "SCH", Anything, "sk"},
	{Anything, "S", "C+", ""},
	{"#", "SM", Anything, "zm"},
	{"#", "SN", "'", "z@n"},
	{Anything, "S", Anything, "s"},
	{Anything, 0, Anything, Silent},
};

static Rule T_rules[] =
{
	{Nothing, "THE", Nothing, "D@"},
	{Anything, "TO", Nothing, "tu"},
	{Anything, "THAT", Nothing, "D&t"},
	{Nothing, "THIS", Nothing, "DIs"},
	{Nothing, "THEY", Anything, "DeI"},
	{Nothing, "THERE", Anything, "Der"},
	{Anything, "THER", Anything, "D3"},
	{Anything, "THEIR", Anything, "Der"},
	{Nothing, "THAN", Nothing, "D&n"},
	{Nothing, "THEM", Nothing, "Dem"},
	{Anything, "THESE", Nothing, "Diz"},
	{Nothing, "THEN", Anything, "Den"},
	{Anything, "THROUGH", Anything, "Tru"},
	{Anything, "THOSE", Anything, "D@Uz"},
	{Anything, "THOUGH", Nothing, "D@U"},
	{Nothing, "THUS", Anything, "DVs"},
	{Anything, "TH", Anything, "T"},
	{"#:", "TED", Nothing, "tId"},
	{"S", "TI", "#N", "tS"},
	{Anything, "TI", "O", "S"},
	{Anything, "TI", "A", "S"},
	{Anything, "TIEN", Anything, "S@n"},
	{Anything, "TUR", "#", "tS3"},
	{Anything, "TU", "A", "tSu"},
	{Nothing, "TWO", Anything, "tu"},
	{Anything, "T", Anything, "t"},
	{Anything, 0, Anything, Silent},
};

static Rule U_rules[] =
{
	{Nothing, "UN", "I", "jun"},
	{Nothing, "UN", Anything, "Vn"},
	{Nothing, "UPON", Anything, "@pOn"},
	{"T", "UR", "#", "Ur"},
	{"S", "UR", "#", "Ur"},
	{"R", "UR", "#", "Ur"},
	{"D", "UR", "#", "Ur"},
	{"L", "UR", "#", "Ur"},
	{"Z", "UR", "#", "Ur"},
	{"N", "UR", "#", "Ur"},
	{"J", "UR", "#", "Ur"},
	{"TH", "UR", "#", "Ur"},
	{"CH", "UR", "#", "Ur"},
	{"SH", "UR", "#", "Ur"},
	{Anything, "UR", "#", "jUr"},
	{Anything, "UR", Anything, "3"},
	{Anything, "U", "^ ", "V"},
	{Anything, "U", "^^", "V"},
	{Anything, "UY", Anything, "aI"},
	{" G", "U", "#", ""},
	{"G", "U", "%", ""},
	{"G", "U", "#", "w"},
	{"#N", "U", Anything, "ju"},
	{"T", "U", Anything, "u"},
	{"S", "U", Anything, "u"},
	{"R", "U", Anything, "u"},
	{"D", "U", Anything, "u"},
	{"L", "U", Anything, "u"},
	{"Z", "U", Anything, "u"},
	{"N", "U", Anything, "u"},
	{"J", "U", Anything, "u"},
	{"TH", "U", Anything, "u"},
	{"CH", "U", Anything, "u"},
	{"SH", "U", Anything, "u"},
	{Anything, "U", Anything, "ju"},
	{Anything, 0, Anything, Silent},
};

static Rule V_rules[] =
{
	{Anything, "VIEW", Anything, "vju"},
	{Anything, "V", Anything, "v"},
	{Anything, 0, Anything, Silent},
};

static Rule W_rules[] =
{
	{Nothing, "WERE", Anything, "w3"},
	{Anything, "WA", "S", "w0"},
	{Anything, "WA", "T", "w0"},
	{Anything, "WHERE", Anything, "hwer"},
	{Anything, "WHAT", Anything, "hw0t"},
	{Anything, "WHOL", Anything, "h@Ul"},
	{Anything, "WHO", Anything, "hu"},
	{Anything, "WH", Anything, "hw"},
	{Anything, "WAR", Anything, "wOr"},
	{Anything, "WOR", "^", "w3"},
	{Anything, "WR", Anything, "r"},
	{Anything, "W", Anything, "w"},
	{Anything, 0, Anything, Silent},
};

static Rule X_rules[] =
{
	{Anything, "X", Anything, "ks"},
	{Anything, 0, Anything, Silent},
};

static Rule Y_rules[] =
{
	{Anything, "YOUNG", Anything, "jVN"},
	{Nothing, "YOU", Anything, "ju"},
	{Nothing, "YES", Anything, "jes"},
	{Nothing, "Y", Anything, "j"},
	{"#:^", "Y", Nothing, "i"},
	{"#:^", "Y", "I", "i"},
	{" :", "Y", Nothing, "aI"},
	{" :", "Y", "#", "aI"},
	{" :", "Y", "^+:#", "I"},
	{" :", "Y", "^#", "aI"},
	{Anything, "Y", Anything, "I"},
	{Anything, 0, Anything, Silent},
};

static Rule Z_rules[] =
{
	{Anything, "Z", Anything, "z"},
	{Anything, 0, Anything, Silent},
};

static Rule *Rules[] =
{
	punct_rules,
	A_rules, B_rules, C_rules, D_rules, E_rules, F_rules, G_rules,
	H_rules, I_rules, J_rules, K_rules, L_rules, M_rules, N_rules,
	O_rules, P_rules, Q_rules, R_rules, S_rules, T_rules, U_rules,
	V_rules, W_rules, X_rules, Y_rules, Z_rules
};


static const char *Cardinals[] =
{
	"zero", "one", "two", "three", "four", 
	"five", "six", "seven", "eight", "nine", 
	"ten", "eleven", "twelve", "thirteen", "fourteen", 
	"fifteen", "sixteen", "seventeen", "eighteen", "nineteen"
};


static const char *Twenties[] =
{
	"twenty", "thirty", "forty", "fifty",
	"sixty", "seventy", "eighty", "ninety"
};


static const char *Ordinals[] =
{
	"zeroth", "first", "second", "third", "fourth", 
	"fifth", "sixth", "seventh","eighth", "ninth",
	"tenth", "eleventh", "twelfth", "thirteenth", "fourteenth", 
	"fifteenth", "sixteenth", "seventeenth", "eighteenth", "nineteenth"
};


static const char *Ord_twenties[] =
{
	"twentieth", "thirtieth", "fortieth", "fiftieth",
	"sixtieth", "seventieth", "eightieth", "ninetieth"
};


/*
** Translate a number to phonemes.  This version is for CARDINAL numbers.
**       Note: this is recursive.
*/
static int xlate_cardinal(int value, darray *phone)
{
	int nph = 0;

	if (value < 0)
	{
		nph += xlate_string("minus", phone);
		value = (-value);

		if (value < 0)                 /* Overflow!  -32768 */
		{
			nph += xlate_string("a lot", phone);
			return nph;
		}
	}

	if (value >= 1000000000L)
		/* Billions */
	{
		nph += xlate_cardinal(value / 1000000000L, phone);
		nph += xlate_string("billion", phone);
		value = value % 1000000000;

		if (value == 0)
			return nph;                   /* Even billion */

		if (value < 100)
			nph += xlate_string("and", phone);

		/* as in THREE BILLION AND FIVE */
	}

	if (value >= 1000000L)
		/* Millions */
	{
		nph += xlate_cardinal(value / 1000000L, phone);
		nph += xlate_string("million", phone);
		value = value % 1000000L;

		if (value == 0)
			return nph;                   /* Even million */

		if (value < 100)
			nph += xlate_string("and", phone);

		/* as in THREE MILLION AND FIVE */
	}

	/* Thousands 1000..1099 2000..99999 */
	/* 1100 to 1999 is eleven-hunderd to ninteen-hunderd */

	if ((value >= 1000L && value <= 1099L) || value >= 2000L)
	{
		nph += xlate_cardinal(value / 1000L, phone);
		nph += xlate_string("thousand", phone);
		value = value % 1000L;

		if (value == 0)
			return nph;                   /* Even thousand */

		if (value < 100)
			nph += xlate_string("and", phone);

		/* as in THREE THOUSAND AND FIVE */
	}

	if (value >= 100L)
	{
		nph += xlate_string(Cardinals[value / 100], phone);
		nph += xlate_string("hundred", phone);
		value = value % 100;

		if (value == 0)
			return nph;                   /* Even hundred */
	}

	if (value >= 20)
	{
		nph += xlate_string(Twenties[(value - 20) / 10], phone);
		value = value % 10;

		if (value == 0)
			return nph;                   /* Even ten */
	}

	nph += xlate_string(Cardinals[value], phone);

	return nph;
}

/*
** Translate a number to phonemes.  This version is for ORDINAL numbers.
**       Note: this is recursive.
*/
static int xlate_ordinal(int value, darray *phone)
{
	int nph = 0;

	if (value < 0)
	{
		nph += xlate_string("minus", phone);
		value = (-value);

		if (value < 0)                 /* Overflow!  -32768 */
		{
			nph += xlate_string("a lot", phone);
			return nph;
		}
	}

	if (value >= 1000000000L)
		/* Billions */
	{
		nph += xlate_cardinal(value / 1000000000L, phone);
		value = value % 1000000000;

		if (value == 0)
		{
			nph += xlate_string("billionth", phone);
			return nph;                  /* Even billion */
		}

		nph += xlate_string("billion", phone);

		if (value < 100)
			nph += xlate_string("and", phone);

		/* as in THREE BILLION AND FIVE */
	}

	if (value >= 1000000L)
		/* Millions */
	{
		nph += xlate_cardinal(value / 1000000L, phone);
		value = value % 1000000L;

		if (value == 0)
		{
			nph += xlate_string("millionth", phone);
			return nph;                  /* Even million */
		}

		nph += xlate_string("million", phone);

		if (value < 100)
			nph += xlate_string("and", phone);

		/* as in THREE MILLION AND FIVE */
	}

	/* Thousands 1000..1099 2000..99999 */
	/* 1100 to 1999 is eleven-hunderd to ninteen-hunderd */

	if ((value >= 1000L && value <= 1099L) || value >= 2000L)
	{
		nph += xlate_cardinal(value / 1000L, phone);
		value = value % 1000L;

		if (value == 0)
		{
			nph += xlate_string("thousandth", phone);
			return nph;                  /* Even thousand */
		}

		nph += xlate_string("thousand", phone);

		if (value < 100)
			nph += xlate_string("and", phone);

		/* as in THREE THOUSAND AND FIVE */
	}

	if (value >= 100L)
	{
		nph += xlate_string(Cardinals[value / 100], phone);
		value = value % 100;

		if (value == 0)
		{
			nph += xlate_string("hundredth", phone);
			return nph;                  /* Even hundred */
		}

		nph += xlate_string("hundred", phone);
	}

	if (value >= 20)
	{
		if ((value % 10) == 0)
		{
			nph += xlate_string(Ord_twenties[(value - 20) / 10], phone);
			return nph;                  /* Even ten */
		}

		nph += xlate_string(Twenties[(value - 20) / 10], phone);

		value = value % 10;
	}

	nph += xlate_string(Ordinals[value], phone);

	return nph;
}


static int isvowel(int chr)
{
	return (chr == 'A' || chr == 'E' || chr == 'I' ||
		chr == 'O' || chr == 'U');
}

static int isconsonant(int chr)
{
	return (isupper(chr) && !isvowel(chr));
}

static int leftmatch(
	const char *pattern,                    /* first char of pattern to match in text */
	const char *context)                     /* last char of text to be matched */

{
	const char *pat;
	const char *text;
	int count;

	if (*pattern == '\0')
		/* null string matches any context */
	{
		return 1;
	}

	/* point to last character in pattern string */
	count = (int)strlen(pattern);

	pat = pattern + (count - 1);

	text = context;

	for (; count > 0; pat--, count--)
	{
		/* First check for simple text or space */
		if (isalpha(*pat) || *pat == '\'' || *pat == ' ')
		{
			if (*pat != *text)
				return 0;
			else
			{
				text--;
				continue;
			}
		}

		switch (*pat)
		{

		case '#':                   /* One or more vowels */

			if (!isvowel(*text))
				return 0;

			text--;

			while (isvowel(*text))
				text--;

			break;

		case ':':                   /* Zero or more consonants */
			while (isconsonant(*text))
				text--;

			break;

		case '^':                   /* One consonant */
			if (!isconsonant(*text))
				return 0;

			text--;

			break;

		case '.':                   /* B, D, V, G, J, L, M, N, R, W, Z */
			if (*text != 'B' && *text != 'D' && *text != 'V'
				&& *text != 'G' && *text != 'J' && *text != 'L'
				&& *text != 'M' && *text != 'N' && *text != 'R'
				&& *text != 'W' && *text != 'Z')
				return 0;

			text--;

			break;

		case '+':                   /* E, I or Y (front vowel) */
			if (*text != 'E' && *text != 'I' && *text != 'Y')
				return 0;

			text--;

			break;

		case '%':

		default:
			fprintf(stderr, "Bad char in left rule: '%c'\n", *pat);

			return 0;
		}
	}

	return 1;
}

static int rightmatch(
	const char *pattern,                    /* first char of pattern to match in text */
	const char *context)                     /* last char of text to be matched */
{
	const char *pat;
	const char *text;

	if (*pattern == '\0')
		/* null string matches any context */
		return 1;

	pat = pattern;

	text = context;

	for (pat = pattern; *pat != '\0'; pat++)
	{
		/* First check for simple text or space */
		if (isalpha(*pat) || *pat == '\'' || *pat == ' ')
		{
			if (*pat != *text)
				return 0;
			else
			{
				text++;
				continue;
			}
		}

		switch (*pat)
		{

		case '#':                   /* One or more vowels */

			if (!isvowel(*text))
				return 0;

			text++;

			while (isvowel(*text))
				text++;

			break;

		case ':':                   /* Zero or more consonants */
			while (isconsonant(*text))
				text++;

			break;

		case '^':                   /* One consonant */
			if (!isconsonant(*text))
				return 0;

			text++;

			break;

		case '.':                   /* B, D, V, G, J, L, M, N, R, W, Z */
			if (*text != 'B' && *text != 'D' && *text != 'V'
				&& *text != 'G' && *text != 'J' && *text != 'L'
				&& *text != 'M' && *text != 'N' && *text != 'R'
				&& *text != 'W' && *text != 'Z')
				return 0;

			text++;

			break;

		case '+':                   /* E, I or Y (front vowel) */
			if (*text != 'E' && *text != 'I' && *text != 'Y')
				return 0;

			text++;

			break;

		case '%':                   /* ER, E, ES, ED, ING, ELY (a suffix) */
			if (*text == 'E')
			{
				text++;

				if (*text == 'L')
				{
					text++;

					if (*text == 'Y')
					{
						text++;
						break;
					}

					else
					{
						text--;               /* Don't gobble L */
						break;
					}
				}

				else
					if (*text == 'R' || *text == 'S' || *text == 'D')
						text++;

				break;
			}

			else
				if (*text == 'I')
				{
					text++;

					if (*text == 'N')
					{
						text++;

						if (*text == 'G')
						{
							text++;
							break;
						}
					}

					return 0;
				}

				else
					return 0;

		default:
			fprintf(stderr, "Bad char in right rule:'%c'\n", *pat);

			return 0;
		}
	}

	return 1;
}

static void phone_cat(darray *arg, const char *s)
{
	char ch;

	while ((ch = *s++))
		arg->put(ch);
}


static int find_rule(darray *arg, char *word, int index, Rule *rules)
{
	for (;;)                         /* Search for the rule */
	{
		Rule *rule;
		const char *left,
			*match,
			*right,
			*output;
		int remainder;
		rule = rules++;
		match = (*rule)[1];

		if (match == 0)
			/* bad symbol! */
		{
			fprintf(stderr, "Error: Can't find rule for: '%c' in \"%s\"\n",
				word[index], word);
			return index + 1;            /* Skip it! */
		}

		for (remainder = index; *match != '\0'; match++, remainder++)
		{
			if (*match != word[remainder])
				break;
		}

		if (*match != '\0')
			continue;                     /* found missmatch */

		left = (*rule)[0];

		right = (*rule)[2];

		if (!leftmatch(left, &word[index - 1]))
			continue;

		if (!rightmatch(right, &word[remainder]))
			continue;

		output = (*rule)[3];

		phone_cat(arg, output);

		return remainder;
	}
}

static void guess_word(darray *arg, char *word)
{
	int index;                       /* Current position in word */
	int type;                        /* First letter of match part */
	index = 1;                       /* Skip the initial blank */

	do
	{
		if (isupper(word[index]))
			type = word[index] - 'A' + 1;
		else
			type = 0;

		index = find_rule(arg, word, index, Rules[type]);
	}

	while (word[index] != '\0');
}


static int NRL(const char *s, int n, darray *phone)
{
	int old = phone->getSize();
	char *word = (char *) malloc(n + 3);
	char *d = word;
	*d++ = ' ';

	while (n-- > 0)
	{
		char ch = *s++;

		if (islower(ch))
			ch = toupper(ch);

		*d++ = ch;
	}

	*d++ = ' '; // kinda unnecessary

	*d = '\0';
	guess_word(phone, word);
	free(word);
	return phone->getSize() - old;
}


static int spell_out(const char *word, int n, darray *phone)
{
	int nph = 0;

	while (n-- > 0)
	{
		nph += xlate_string(ASCII[*word++ & 0x7F], phone);
	}

	return nph;
}

static int suspect_word(const char *s, int n)
{
	int i = 0;
	int seen_lower = 0;
	int seen_upper = 0;
	int seen_vowel = 0;
	int last = 0;

	for (i = 0; i < n; i++)
	{
		char ch = *s++;

		if (i && last != '-' && isupper(ch))
			seen_upper = 1;

		if (islower(ch))
		{
			seen_lower = 1;
			ch = toupper(ch);
		}

		if (ch == 'A' || ch == 'E' || ch == 'I' || ch == 'O' || ch == 'U' || ch == 'Y')
			seen_vowel = 1;

		last = ch;
	}

	return !seen_vowel || (seen_upper && seen_lower) || !seen_lower;
}

static int xlate_word(const char *word, int n, darray *phone)
{
	int nph = 0;

	if (*word != '[')
	{
		if (suspect_word(word, n))
			return spell_out(word, n, phone);
		else
		{
			nph += NRL(word, n, phone);
		}
	}

	else
	{
		if ((++word)[(--n) - 1] == ']')
			n--;

		while (n-- > 0)
		{
			phone->put(*word++);
			nph++;
		}
	}

	phone->put(' ');

	return nph + 1;
}


int xlate_string(const char *string, darray *phone)
{
	int nph = 0;
	const char *s = string;
	char ch;

	while (isspace(ch = *s))
		s++;

	while ((ch = *s))
	{
		const char *word = s;

		if (isalpha(ch))
		{
			while (isalpha(ch = *s) || ((ch == '\'' || ch == '-' || ch == '.') && isalpha(s[1])))
				s++;

			if (!ch || isspace(ch) || ispunct(ch) || (isdigit(ch) && !suspect_word(word, (int)(s - word))))
				nph += xlate_word(word, (int)(s - word), phone);
			else
			{
				while ((ch = *s) && !isspace(ch) && !ispunct(ch))
					s++;

				nph += spell_out(word, (int)(s - word), phone);
			}
		}

		else
			if (isdigit(ch) || (ch == '-' && isdigit(s[1])))
			{
				int sign = (ch == '-') ? -1 : 1;
				int value = 0;

				if (sign < 0)
					ch = *++s;

				while (isdigit(ch = *s))
				{
					value = value * 10 + ch - '0';
					s++;
				}

				if (ch == '.' && isdigit(s[1]))
				{
					word = ++s;
					nph += xlate_cardinal(value * sign, phone);
					nph += xlate_string("point", phone);

					while (isdigit(ch = *s))
						s++;

					nph += spell_out(word, (int)(s - word), phone);
				}

				else
				{
					/* check for ordinals, date, time etc. can go in here */
					nph += xlate_cardinal(value * sign, phone);
				}
			}

			else
				if (ch == '[' && strchr(s, ']'))
				{
					const char *word = s;

					while (*s && *s++ != ']')
						/* nothing */
						;

					nph += xlate_word(word, (int)(s - word), phone);
				}

				else
					if (ispunct(ch))
					{
						switch (ch)
						{

						case '!':

						case '?':

						case '.':
							s++;
							phone->put(' ');
							break;

						case '"':                 /* change pitch ? */

						case ':':

						case '-':

						case ';':

						case ',':

						case '(':

						case ')':
							s++;
							phone->put(' ');
							break;

						case '[':
							{
								const char *e = strchr(s, ']');

								if (e)
								{
									s++;

									while (s < e)
										phone->put(*s++);

									s = e + 1;

									break;
								}
							}

						default:
							nph += spell_out(word, 1, phone);
							s++;
							break;
						}
					}

					else
					{
						while ((ch = *s) && !isspace(ch))
							s++;

						nph += spell_out(word, (int)(s - word), phone);
					}

					while (isspace(ch = *s))
						s++;
	}

	return nph;
}
//----../src/audiosource/speech/soloud_speech.cpp-----------------------------------------------------------------------
namespace SoLoud
{
	SpeechInstance::SpeechInstance(Speech *aParent)
	{
		mParent = aParent;			
		mSynth.init();
		mSample = new short[mSynth.mNspFr * 100];
		mSynth.initsynth(mParent->mElement.getSize(), (unsigned char *)mParent->mElement.getData());
		mOffset = 10;
		mSampleCount = 10;
	}

	SpeechInstance::~SpeechInstance(){
	    delete[] mSample;
	}

	static void writesamples(short * aSrc, float * aDst, int aCount)
	{
		int i;
		for (i = 0; i < aCount; i++)
		{
			aDst[i] = aSrc[i] * (1 / (float)0x8000);
		}
	}

	void SpeechInstance::getAudio(float *aBuffer, unsigned int aSamples)
	{
		unsigned int samples_out = 0;
		if (mSampleCount > mOffset)
		{
			unsigned int copycount = mSampleCount - mOffset;
			if (copycount > aSamples) 
			{
				copycount = aSamples;
			}
			writesamples(mSample + mOffset, aBuffer, copycount);
			mOffset += copycount;
			samples_out += copycount;
		}

		while (mSampleCount >= 0 && samples_out < aSamples)
		{
			mOffset = 0;
			mSampleCount = mSynth.synth(mSynth.mNspFr, mSample);
			if (mSampleCount > 0)
			{
				unsigned int copycount = mSampleCount;
				if (copycount > aSamples - samples_out)
				{
					copycount = aSamples - samples_out;
				}
				writesamples(mSample, aBuffer + samples_out, copycount);
				mOffset += copycount;
				samples_out += copycount;				
			}
			else
			if (mSampleCount < 0 && mFlags & AudioSourceInstance::LOOPING)
			{
				mSynth.init();
				mSynth.initsynth(mParent->mElement.getSize(), (unsigned char *)mParent->mElement.getData());
				mOffset = 10;
				mSampleCount = 10;
				mLoopCount++;
			}
		}

		if (mSampleCount < 0 && aSamples > samples_out)
		{
			memset(aBuffer + samples_out, 0, sizeof(float) * (aSamples - samples_out));				
		}
	}

	result SpeechInstance::rewind()
	{
		mSynth.init();
		mSynth.initsynth(mParent->mElement.getSize(), (unsigned char *)mParent->mElement.getData());
		mOffset = 10;
		mSampleCount = 10;
		mStreamTime = 0;
		return 0;
	}

	bool SpeechInstance::hasEnded()
	{
			
		if (mSampleCount < 0)
			return 1;				
		return 0;
	}	

	result Speech::setText(const char *aText)
	{
		if (aText == NULL)
			return INVALID_PARAMETER;

		stop();
		mElement.clear();
		darray phone;
		xlate_string(aText, &phone);
		mFrames = klatt::phone_to_elm(phone.getData(), phone.getSize(), &mElement);
		return 0;
	}

	Speech::Speech()
	{
		mBaseSamplerate = 11025;
		mFrames = 0;
	}

	Speech::~Speech()
	{
		stop();
	}

	AudioSourceInstance *Speech::createInstance()
	{
		return new SpeechInstance(this);
	}	
};
#endif // YES_IMGUISOLOUD_SPEECH

//----soloud_alsa.cpp-----------------------------------------------------------------------
#if !defined(WITH_ALSA)

namespace SoLoud
{
    result alsa_init(Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer)
	{
		return NOT_IMPLEMENTED;
	}
};

#else

#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

namespace SoLoud
{
    struct ALSAData
    {
        float *buffer;
        short *sampleBuffer;
        snd_pcm_t *alsaDeviceHandle;
        Soloud *soloud;
        int samples;
        int channels;
        bool audioProcessingDone;
        Thread::ThreadHandle threadHandle;
    };


    static void alsaThread(void *aParam)
    {
        
        ALSAData *data = static_cast<ALSAData*>(aParam);
        while (!data->audioProcessingDone) 
        {            
            data->soloud->mix(data->buffer, data->samples);            
            for (int i=0;i<data->samples*data->channels;++i)
            {
                data->sampleBuffer[i] = static_cast<short>(floor(data->buffer[i] 
                                                          * static_cast<float>(0x7fff)));
            }
            if (snd_pcm_writei(data->alsaDeviceHandle, data->sampleBuffer, data->samples) == -EPIPE)
                snd_pcm_prepare(data->alsaDeviceHandle);
                
        }
        
    }

    static void alsaCleanup(Soloud *aSoloud)
    {
        if (0 == aSoloud->mBackendData)
        {
            return;
        }
        ALSAData *data = static_cast<ALSAData*>(aSoloud->mBackendData);
        data->audioProcessingDone = true;
        if (data->threadHandle)
        {
            Thread::wait(data->threadHandle);
            Thread::release(data->threadHandle);
        }
        snd_pcm_drain(data->alsaDeviceHandle);
        snd_pcm_close(data->alsaDeviceHandle);
        if (0 != data->sampleBuffer)
        {
            delete[] data->sampleBuffer;
        }
        if (0 != data->buffer)
        {
            delete[] data->buffer;
        }
        delete data;
        aSoloud->mBackendData = 0;
    }

    result alsa_init(Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer, unsigned int aChannels)
    {
        ALSAData *data = new ALSAData;
        memset(data, 0, sizeof(ALSAData));
        aSoloud->mBackendData = data;
        aSoloud->mBackendCleanupFunc = alsaCleanup;
        data->samples = aBuffer;
        data->channels = 2;
        data->soloud = aSoloud;

        int rc;
        snd_pcm_t *handle;
        rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
        if (rc < 0)
        {
            return UNKNOWN_ERROR;
        }
        
        data->alsaDeviceHandle = handle;
        
        snd_pcm_hw_params_t *params;
        snd_pcm_hw_params_alloca(&params);
        snd_pcm_hw_params_any(handle, params);

        snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
        snd_pcm_hw_params_set_channels(handle, params, 2);
        snd_pcm_hw_params_set_buffer_size(handle, params, aBuffer);
        
        unsigned int val = aSamplerate;
        int dir = 0;
        rc = snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);
        if (rc < 0)
        {
            return UNKNOWN_ERROR;
        }

        rc = snd_pcm_hw_params(handle, params);
        if (rc < 0) 
        {
            return UNKNOWN_ERROR;
        }

        snd_pcm_hw_params_get_rate(params, &val, &dir);
        aSamplerate = val;
        snd_pcm_hw_params_get_channels(params, &val);
        data->channels = val;

        data->buffer = new float[data->samples*data->channels];
        data->sampleBuffer = new short[data->samples*data->channels];
        aSoloud->postinit(aSamplerate, data->samples * data->channels, aFlags, 2);
        data->threadHandle = Thread::createThread(alsaThread, data);
        if (0 == data->threadHandle)
        {
            return UNKNOWN_ERROR;
        }
        aSoloud->mBackendString = "ALSA";
        return 0;
    }
};
#endif
//----soloud_coreaudio.cpp-----------------------------------------------------------------------
#if !defined(WITH_COREAUDIO)

namespace SoLoud
{
	result coreaudio_init(SoLoud::Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer)
	{
		return NOT_IMPLEMENTED;
	}
}
#else

#include <AudioToolbox/AudioToolbox.h>

#define NUM_BUFFERS 2

static AudioQueueRef audioQueue = 0;

namespace SoLoud
{
	void soloud_coreaudio_deinit(SoLoud::Soloud *aSoloud)
	{
		AudioQueueStop(audioQueue, true);
		AudioQueueDispose(audioQueue, false);
	}
	
	static void coreaudio_mutex_lock(void *mutex)
	{
		Thread::lockMutex(mutex);
	}

	static void coreaudio_mutex_unlock(void *mutex)
	{
		Thread::unlockMutex(mutex);
	}

	static void coreaudio_fill_buffer(void *context, AudioQueueRef queue, AudioQueueBufferRef buffer)
	{
		SoLoud::Soloud *aSoloud = (SoLoud::Soloud*)context;
		aSoloud->mixSigned16((short*)buffer->mAudioData, buffer->mAudioDataByteSize / 4);
		AudioQueueEnqueueBuffer(queue, buffer, 0, NULL);
	}

	result coreaudio_init(SoLoud::Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer, unsigned int aChannels)
	{
		aSoloud->postinit(aSamplerate, aBuffer, aFlags, 2);
		aSoloud->mBackendCleanupFunc = soloud_coreaudio_deinit;

		AudioStreamBasicDescription audioFormat;
		audioFormat.mSampleRate = aSamplerate;
		audioFormat.mFormatID = kAudioFormatLinearPCM;
		audioFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
		audioFormat.mBytesPerPacket = 4;
		audioFormat.mFramesPerPacket = 1;
		audioFormat.mBytesPerFrame = 4;
		audioFormat.mChannelsPerFrame = 2;
		audioFormat.mBitsPerChannel = 16;
		audioFormat.mReserved = 0;

		// create the audio queue
		OSStatus result = AudioQueueNewOutput(&audioFormat, coreaudio_fill_buffer, aSoloud, NULL, NULL, 0, &audioQueue);
		if(result)
		{
			//printf("AudioQueueNewOutput failed (%d)\n", result);
			return UNKNOWN_ERROR;
		}

		// allocate and prime audio buffers
		for(int i = 0; i < NUM_BUFFERS; ++i)
		{
			AudioQueueBufferRef buffer;
			result = AudioQueueAllocateBuffer(audioQueue, aBuffer * 4, &buffer);
			if(result)
			{
				//printf("AudioQueueAllocateBuffer failed (%d)\n", result);
				return UNKNOWN_ERROR;
			}
			buffer->mAudioDataByteSize = aBuffer * 4;
			memset(buffer->mAudioData, 0, buffer->mAudioDataByteSize);
			AudioQueueEnqueueBuffer(audioQueue, buffer, 0, NULL);
		}

		// start playback
		result = AudioQueueStart(audioQueue, NULL);
		if(result)
		{
			//printf("AudioQueueStart failed (%d)\n", result);
			return UNKNOWN_ERROR;
		}

        aSoloud->mBackendString = "CoreAudio";
		return 0;
	}	
};
#endif
//----soloud_null.cpp-----------------------------------------------------------------------
#if !defined(WITH_NULL)

namespace SoLoud
{
    result null_init(Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer)
	{
		return NOT_IMPLEMENTED;
	}
};

#else

namespace SoLoud
{
    static void nullCleanup(Soloud *aSoloud)
    {
    }

    result null_init(Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer, unsigned int aChannels)
    {
		if (aChannels == 0 || aChannels == 3 || aChannels == 5 || aChannels > 6 || aBuffer < SAMPLE_GRANULARITY)
			return INVALID_PARAMETER;
        aSoloud->mBackendData = 0;
        aSoloud->mBackendCleanupFunc = nullCleanup;

        aSoloud->postinit(aSamplerate, aBuffer, aFlags, aChannels);
        aSoloud->mBackendString = "null driver";
        return SO_NO_ERROR;
    }
};
#endif
//----soloud_openal.cpp-----------------------------------------------------------------------
#if !defined(WITH_OPENAL)

namespace SoLoud
{
	result openal_init(SoLoud::Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer)
	{
		return NOT_IMPLEMENTED;
	}
}
#else

#ifdef __APPLE__
#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#else
#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"
#endif

#define NUM_BUFFERS 2

static ALCdevice* device = NULL;
static ALCcontext* context = NULL;
static ALenum format = 0;
static ALuint buffers[NUM_BUFFERS];
static ALuint source = 0;
static int frequency = 0;
static volatile int threadrun = 0;
static int buffersize = 0;
static short* bufferdata = 0;

extern "C"
{
	int dll_al_found();
	ALCdevice* dll_alc_OpenDevice(const ALCchar *devicename);
	void dll_alc_CloseDevice(ALCdevice *device);
	ALCcontext* dll_alc_CreateContext(ALCdevice *device, const ALCint* attrlist);
	void dll_alc_DestroyContext(ALCcontext *context);
	ALCboolean dll_alc_MakeContextCurrent(ALCcontext *context);
	void dll_al_GetSourcei(ALuint source, ALenum param, ALint *value);
	void dll_al_SourceQueueBuffers(ALuint source, ALsizei nb, const ALuint *buffers);
	void dll_al_SourceUnqueueBuffers(ALuint source, ALsizei nb, ALuint *buffers);
	void dll_al_BufferData(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq);
	void dll_al_SourcePlay(ALuint source);
	void dll_al_SourceStop(ALuint source);
	void dll_al_GenBuffers(ALsizei n, ALuint *buffers);
	void dll_al_DeleteBuffers(ALsizei n, ALuint *buffers);
	void dll_al_GenSources(ALsizei n, ALuint *sources);
	void dll_al_DeleteSources(ALsizei n, ALuint *sources);
}

namespace SoLoud
{
	void soloud_openal_deinit(SoLoud::Soloud *aSoloud)
	{
		threadrun++;
		while (threadrun == 1)
		{
			Thread::sleep(10);
		}

		dll_al_SourceStop(source);
		dll_al_DeleteSources(1, &source);
		dll_al_DeleteBuffers(NUM_BUFFERS, buffers);

		dll_alc_MakeContextCurrent(NULL);
		dll_alc_DestroyContext(context);
		dll_alc_CloseDevice(device);
		
		free(bufferdata);

		device = NULL;
		context = NULL;
		format = 0;
		source = 0;
		frequency = 0;
		threadrun = 0;
		buffersize = 0;
		bufferdata = 0;
	}
	
	static void openal_mutex_lock(void * mutex)
	{
		Thread::lockMutex(mutex);
	}

	static void openal_mutex_unlock(void * mutex)
	{
		Thread::unlockMutex(mutex);
	}

	static void openal_iterate(SoLoud::Soloud *aSoloud)
	{
		ALuint buffer = 0;
		ALint buffersProcessed = 0;
		ALint state;
		dll_al_GetSourcei(source, AL_BUFFERS_PROCESSED, &buffersProcessed);

		while (buffersProcessed--) 
		{
			aSoloud->mixSigned16(bufferdata,buffersize);

			dll_al_SourceUnqueueBuffers(source, 1, &buffer);

			dll_al_BufferData(buffer, format, bufferdata, buffersize*4, frequency);

			dll_al_SourceQueueBuffers(source, 1, &buffer);
		}

		dll_al_GetSourcei(source, AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING)
			dll_al_SourcePlay(source);
	}

	static void openal_thread(void *aParam)
	{
		Soloud *soloud = (Soloud *)aParam;
		while (threadrun == 0)
		{
			openal_iterate(soloud);
			Thread::sleep(1);
		}
		threadrun++;
	}

	result openal_init(SoLoud::Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer, unsigned int aChannels)
	{
		if (!dll_al_found())
			return DLL_NOT_FOUND;

		aSoloud->postinit(aSamplerate,aBuffer,aFlags,2);
		aSoloud->mBackendCleanupFunc = soloud_openal_deinit;

		device = dll_alc_OpenDevice(NULL);
		context = dll_alc_CreateContext(device, NULL);
		dll_alc_MakeContextCurrent(context);
		format = AL_FORMAT_STEREO16;
		dll_al_GenBuffers(NUM_BUFFERS, buffers);
		dll_al_GenSources(1, &source);
		buffersize = aBuffer;
		bufferdata = (short*)malloc(buffersize*2*2);

		frequency = aSamplerate;

		int i;
		for (i = 0; i < buffersize*2; i++)
			bufferdata[i] = 0;
		for (i = 0; i < NUM_BUFFERS; i++)
		{
			dll_al_BufferData(buffers[i], format, bufferdata, buffersize, frequency);
			dll_al_SourceQueueBuffers(source, 1, &buffers[i]);
		}

		dll_al_SourcePlay(source);

		Thread::createThread(openal_thread, (void*)aSoloud);

        aSoloud->mBackendString = "OpenAL";
		return 0;
	}	
};
#endif

//-----soloud_openal_dll.c-------------------------------------------------------------------------------
#if defined(WITH_OPENAL)
extern "C" 
{
#ifdef __APPLE__
#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#else
#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"
#endif

#if defined(_MSC_VER)
#define WINDOWS_VERSION
#endif

#ifdef SOLOUD_STATIC_OPENAL

// statically linked OpenAL 
int dll_al_found() { return 1; }
ALCdevice* dll_alc_OpenDevice(const ALCchar *devicename) { return alcOpenDevice(devicename); }
void dll_alc_CloseDevice(ALCdevice *device) { alcCloseDevice(device); }
ALCcontext* dll_alc_CreateContext(ALCdevice *device, const ALCint* attrlist) { return alcCreateContext(device, attrlist); }
void dll_alc_DestroyContext(ALCcontext *context) { return alcDestroyContext(context); }
ALCboolean dll_alc_MakeContextCurrent(ALCcontext *context) { return alcMakeContextCurrent(context); }
void dll_al_GetSourcei(ALuint source, ALenum param, ALint *value) { alGetSourcei(source, param, value); }
void dll_al_SourceQueueBuffers(ALuint source, ALsizei nb, const ALuint *buffers) { alSourceQueueBuffers(source, nb, buffers); }
void dll_al_SourceUnqueueBuffers(ALuint source, ALsizei nb, ALuint *buffers) { alSourceUnqueueBuffers(source, nb, buffers); }
void dll_al_BufferData(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq) { alBufferData(buffer, format, data, size, freq); }
void dll_al_SourcePlay(ALuint source) { alSourcePlay(source); }
void dll_al_SourceStop(ALuint source) { alSourceStop(source); }
void dll_al_GenBuffers(ALsizei n, ALuint *buffers) { alGenBuffers(n, buffers); }
void dll_al_DeleteBuffers(ALsizei n, ALuint *buffers) { alDeleteBuffers(n, buffers); }
void dll_al_GenSources(ALsizei n, ALuint *sources) { alGenSources(n, sources); }
void dll_al_DeleteSources(ALsizei n, ALuint *sources) { alDeleteSources(n, sources); }

#else

typedef ALCdevice* (*alc_OpenDevice)(const ALCchar *devicename);
typedef void (*alc_CloseDevice)(ALCdevice *device);
typedef ALCcontext* (*alc_CreateContext)(ALCdevice *device, const ALCint* attrlist);
typedef void (*alc_DestroyContext)(ALCcontext *context);
typedef ALCboolean (*alc_MakeContextCurrent)(ALCcontext *context);
typedef void (*al_GetSourcei)(ALuint source, ALenum param, ALint *value);
typedef void (*al_SourceQueueBuffers)(ALuint source, ALsizei nb, const ALuint *buffers);
typedef void (*al_SourceUnqueueBuffers)(ALuint source, ALsizei nb, ALuint *buffers);
typedef void (*al_BufferData)(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq);
typedef void (*al_SourcePlay)(ALuint source);
typedef void (*al_SourceStop)(ALuint source);
typedef void (*al_GenBuffers)(ALsizei n, ALuint *buffers);
typedef void (*al_DeleteBuffers)(ALsizei n, ALuint *buffers);
typedef void (*al_GenSources)(ALsizei n, ALuint *sources);
typedef void (*al_DeleteSources)(ALsizei n, ALuint *sources);

static alc_OpenDevice dAlcOpenDevice;
static alc_CloseDevice dAlcCloseDevice;
static alc_CreateContext dAlcCreateContext;
static alc_DestroyContext dAlcDestroyContext;
static alc_MakeContextCurrent dAlcMakeContextCurrent;
static al_GetSourcei dAlGetSourcei;
static al_SourceQueueBuffers dAlSourceQueueBuffers;
static al_SourceUnqueueBuffers dAlSourceUnqueueBuffers;
static al_BufferData dAlBufferData;
static al_SourcePlay dAlSourcePlay;
static al_SourceStop dAlSourceStop;
static al_GenBuffers dAlGenBuffers;
static al_DeleteBuffers dAlDeleteBuffers;
static al_GenSources dAlGenSources;
static al_DeleteSources dAlDeleteSources;

#ifdef WINDOWS_VERSION
#include <windows.h>

static HMODULE openDll()
{
	HMODULE x = LoadLibraryA("soft_oal.dll");
	if (x == 0) x = LoadLibraryA("OpenAL32.dll");
	return x;
}

static void* getDllProc(HMODULE aDllHandle, const char *aProcName)
{
    return GetProcAddress(aDllHandle, aProcName);
}

#else
#include <dlfcn.h> // dll functions

typedef void* HMODULE;

static HMODULE openDll()
{
    return dlopen("libopenal.so", RTLD_LAZY);
}

static void* getDllProc(HMODULE aLibrary, const char *aProcName)
{
    return dlsym(aLibrary, aProcName);
}

#endif

static int load_dll()
{
#ifdef WINDOWS_VERSION
	HMODULE dll = NULL;
#else
	void * dll = NULL;
#endif

	if (dAlcOpenDevice != NULL)
	{
		return 1;
	}

    dll = openDll();

    if (dll)
    {
        dAlcOpenDevice = (alc_OpenDevice)getDllProc(dll, "alcOpenDevice");
        dAlcCloseDevice = (alc_CloseDevice)getDllProc(dll, "alcCloseDevice");
        dAlcCreateContext = (alc_CreateContext)getDllProc(dll, "alcCreateContext");
        dAlcDestroyContext = (alc_DestroyContext)getDllProc(dll, "alcDestroyContext");
        dAlcMakeContextCurrent = (alc_MakeContextCurrent)getDllProc(dll, "alcMakeContextCurrent");
        dAlGetSourcei = (al_GetSourcei)getDllProc(dll, "alGetSourcei");
        dAlSourceQueueBuffers = (al_SourceQueueBuffers)getDllProc(dll, "alSourceQueueBuffers");
        dAlSourceUnqueueBuffers = (al_SourceUnqueueBuffers)getDllProc(dll, "alSourceUnqueueBuffers");
        dAlBufferData = (al_BufferData)getDllProc(dll, "alBufferData");
        dAlSourcePlay = (al_SourcePlay)getDllProc(dll, "alSourcePlay");
        dAlSourceStop = (al_SourceStop)getDllProc(dll, "alSourceStop");
        dAlGenBuffers = (al_GenBuffers)getDllProc(dll, "alGenBuffers");
        dAlDeleteBuffers = (al_GenBuffers)getDllProc(dll, "alDeleteBuffers");
        dAlGenSources = (al_GenSources)getDllProc(dll, "alGenSources");
        dAlDeleteSources = (al_GenSources)getDllProc(dll, "alDeleteSources");

        if (dAlcOpenDevice &&
        	dAlcCloseDevice &&
			dAlcCreateContext &&
			dAlcDestroyContext &&
			dAlcMakeContextCurrent &&
            dAlGetSourcei &&
			dAlSourceQueueBuffers &&
            dAlSourceUnqueueBuffers &&
			dAlBufferData && 
			dAlSourcePlay &&
			dAlSourceStop &&
            dAlGenBuffers &&
            dAlDeleteBuffers &&
			dAlGenSources &&
			dAlDeleteSources)
        {
            return 1;
        }
	}
	dAlcOpenDevice = 0;
	return 0;
}

int dll_al_found()
{
	return load_dll();
}

ALCdevice* dll_alc_OpenDevice(const ALCchar *devicename)
{
	if (load_dll())
		return dAlcOpenDevice(devicename);
	return NULL;
}

void dll_alc_CloseDevice(ALCdevice *device)
{
	if (load_dll())
		dAlcCloseDevice(device);
}

ALCcontext* dll_alc_CreateContext(ALCdevice *device, const ALCint* attrlist)
{
	if (load_dll())
		return dAlcCreateContext(device, attrlist);
	return NULL;
}

void dll_alc_DestroyContext(ALCcontext *context)
{
	if (load_dll())
		dAlcDestroyContext(context);
}

ALCboolean dll_alc_MakeContextCurrent(ALCcontext *context)
{
	if (load_dll())
		return dAlcMakeContextCurrent(context);
	return 0;
}

void dll_al_GetSourcei(ALuint source, ALenum param, ALint *value)
{
	if (load_dll())
		dAlGetSourcei(source, param, value);
}

void dll_al_SourceQueueBuffers(ALuint source, ALsizei nb, const ALuint *buffers)
{
	if (load_dll())
		dAlSourceQueueBuffers(source, nb, buffers);
}

void dll_al_SourceUnqueueBuffers(ALuint source, ALsizei nb, ALuint *buffers)
{
	if (load_dll())
		dAlSourceUnqueueBuffers(source, nb, buffers);
}

void dll_al_BufferData(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq)
{
	if (load_dll())
		dAlBufferData(buffer, format, data, size, freq);
}

void dll_al_SourcePlay(ALuint source)
{
	if (load_dll())
		dAlSourcePlay(source);
}

void dll_al_SourceStop(ALuint source)
{
	if (load_dll())
		dAlSourceStop(source);
}

void dll_al_GenBuffers(ALsizei n, ALuint *buffers)
{
	if (load_dll())
		dAlGenBuffers(n, buffers);
}

void dll_al_DeleteBuffers(ALsizei n, ALuint *buffers)
{
	if (load_dll())
		dAlDeleteBuffers(n, buffers);
}

void dll_al_GenSources(ALsizei n, ALuint *sources)
{
	if (load_dll())
		dAlGenSources(n, sources);
}

void dll_al_DeleteSources(ALsizei n, ALuint *sources)
{
	if (load_dll())
		dAlDeleteSources(n, sources);
}
#endif
} // extern "C"
#endif //defined(WITH_OPENAL)

//----soloud_opensles.cpp-----------------------------------------------------------------------
#if !defined(WITH_OPENSLES)
namespace SoLoud
{
	result opensles_init(SoLoud::Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer)
	{
		return NOT_IMPLEMENTED;
	}
}
#else

#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Platform.h"

#if defined( __ANDROID__ )
#include "SLES/OpenSLES_Android.h"
#endif

// Error logging.
#if defined( __ANDROID__ ) 
#  include <android/log.h>
#  define LOG_ERROR( _msg ) \
   __android_log_print( ANDROID_LOG_ERROR, "SoLoud", _msg )
#  define LOG_INFO( _msg ) \
   __android_log_print( ANDROID_LOG_INFO, "SoLoud", _msg )

#else
   printf( _msg )
#endif

#define NUM_BUFFERS 2

namespace SoLoud
{
	struct BackendData
	{
		BackendData()
		{
			memset(this, 0, sizeof(BackendData));
		}

		~BackendData()
		{
			// Wait until thread is done.
			threadrun++;
			while (threadrun == 1)
			{
				Thread::sleep(10);
			}

			if(playerObj)
			{
				(*playerObj)->Destroy(playerObj);
			}

			if(outputMixObj)
			{
				(*outputMixObj)->Destroy(outputMixObj);
			}

			if(engineObj)
			{
				(*engineObj)->Destroy(engineObj);
			}

			for(int idx = 0; idx < NUM_BUFFERS; ++idx)
			{
				delete [] outputBuffers[idx];
			}
		}

		// Engine.
		SLObjectItf engineObj;
		SLEngineItf engine;

		// Output mix.
		SLObjectItf outputMixObj;
		SLVolumeItf outputMixVol;

		// Data.
		SLDataLocator_OutputMix outLocator;
		SLDataSink dstDataSink;

		// Player.
		SLObjectItf playerObj;
		SLPlayItf player;
		SLVolumeItf playerVol;
		SLAndroidSimpleBufferQueueItf playerBufferQueue;

		unsigned int bufferSize;
		unsigned int channels;
		short *outputBuffers[ NUM_BUFFERS ];
		int buffersQueued;
		int activeBuffer;
		volatile int threadrun;

		SLDataLocator_AndroidSimpleBufferQueue inLocator;
	};

 	void soloud_opensles_deinit(SoLoud::Soloud *aSoloud)
	{
		BackendData *data = static_cast<BackendData*>(aSoloud->mBackendData);
		delete data;
		aSoloud->mBackendData = NULL;
	}

	static void opensles_iterate(SoLoud::Soloud *aSoloud)
	{
		BackendData *data = static_cast<BackendData*>(aSoloud->mBackendData);

		// If we have no buffered queued, queue one up for playback.
		if(data->buffersQueued == 0)
		{
			// Get next output buffer, advance, next buffer.
			short * outputBuffer = data->outputBuffers[data->activeBuffer];
			data->activeBuffer = (data->activeBuffer + 1) % NUM_BUFFERS;
			short * nextBuffer = data->outputBuffers[data->activeBuffer];

			// Mix this buffer. 
			const int bufferSizeBytes = data->bufferSize * data->channels * sizeof(short);
			(*data->playerBufferQueue)->Enqueue(data->playerBufferQueue, outputBuffer, bufferSizeBytes);
			++data->buffersQueued;

			aSoloud->mixSigned16(nextBuffer,data->bufferSize);
		}
	}

	static void opensles_thread(void *aParam)
	{
		Soloud *soloud = static_cast<Soloud*>(aParam);
		BackendData *data = static_cast<BackendData*>(soloud->mBackendData);
		while (data->threadrun == 0)
		{
			opensles_iterate(soloud);

			// TODO: Condition var?
			Thread::sleep(1);
		}
		data->threadrun++;
	}

	static void SLAPIENTRY soloud_opensles_play_callback(SLPlayItf player, void *context, SLuint32 event)
	{
		Soloud *soloud = static_cast<Soloud*>(context);
		BackendData *data = static_cast<BackendData*>(soloud->mBackendData);
		if( event & SL_PLAYEVENT_HEADATEND )
		{
			data->buffersQueued--;
		}
	}

	result opensles_init(SoLoud::Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer, unsigned int aChannels)
	{
		BackendData *data = new BackendData();

		// Allocate output buffer to mix into.
		data->bufferSize = aBuffer;
		data->channels = aChannels;
		const int bufferSizeBytes = data->bufferSize * data->channels * sizeof(short);
		for(int idx = 0; idx < NUM_BUFFERS; ++idx)
		{
			data->outputBuffers[idx] = new short[ data->bufferSize * data->channels ];
			memset(data->outputBuffers[idx], 0, bufferSizeBytes);
		}

		// Create engine.
		const SLEngineOption opts[] = { SL_ENGINEOPTION_THREADSAFE, SL_BOOLEAN_TRUE };
		if(slCreateEngine( &data->engineObj, 1, opts, 0, NULL, NULL ) != SL_RESULT_SUCCESS)
		{
			LOG_ERROR( "Failed to create engine." );
			return UNKNOWN_ERROR;
		}

		// Realize and get engine interfaxce.	
		(*data->engineObj)->Realize(data->engineObj, SL_BOOLEAN_FALSE);
		if((*data->engineObj)->GetInterface(data->engineObj, SL_IID_ENGINE, &data->engine) != SL_RESULT_SUCCESS)
		{
			LOG_ERROR( "Failed to get engine interface." );
			return UNKNOWN_ERROR;
		}

		// Create output mix.
		const SLInterfaceID ids[] = { SL_IID_VOLUME };
		const SLboolean req[] = { SL_BOOLEAN_FALSE };

		if((*data->engine)->CreateOutputMix(data->engine, &data->outputMixObj, 1, ids, req) != SL_RESULT_SUCCESS)
		{
			LOG_ERROR( "Failed to create output mix object." );
			return UNKNOWN_ERROR;
		}
		(*data->outputMixObj)->Realize(data->outputMixObj, SL_BOOLEAN_FALSE);

		if((*data->outputMixObj)->GetInterface(data->outputMixObj, SL_IID_VOLUME, &data->outputMixVol) != SL_RESULT_SUCCESS)
		{
			LOG_INFO( "Failed to get output mix volume interface." );
		}

		// Create android buffer queue.
		data->inLocator.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
		data->inLocator.numBuffers = NUM_BUFFERS;

		// Setup data format.
		SLDataFormat_PCM format;
		format.formatType = SL_DATAFORMAT_PCM;
		format.numChannels = data->channels;
		format.samplesPerSec = aSamplerate * 1000; //mHz
		format.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
		format.containerSize = 16;
		format.endianness = SL_BYTEORDER_LITTLEENDIAN;

		// Determine channel mask.
		if(data->channels == 2)
		{
			format.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
		}
		else
		{
			format.channelMask = SL_SPEAKER_FRONT_CENTER;
		}
		 
		SLDataSource src;
		src.pLocator = &data->inLocator;
		src.pFormat = &format;

		// Output mix.
		data->outLocator.locatorType = SL_DATALOCATOR_OUTPUTMIX;
		data->outLocator.outputMix = data->outputMixObj;
		 
		data->dstDataSink.pLocator = &data->outLocator;
		data->dstDataSink.pFormat = NULL;

		// Setup player.
		{
			const SLInterfaceID ids[] = { SL_IID_VOLUME, SL_IID_ANDROIDSIMPLEBUFFERQUEUE };
			const SLboolean req[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
 
			(*data->engine)->CreateAudioPlayer(data->engine, &data->playerObj, &src, &data->dstDataSink, sizeof(ids) / sizeof(ids[0]), ids, req);
		 
			(*data->playerObj)->Realize(data->playerObj, SL_BOOLEAN_FALSE);
	 
			(*data->playerObj)->GetInterface(data->playerObj, SL_IID_PLAY, &data->player);
			(*data->playerObj)->GetInterface(data->playerObj, SL_IID_VOLUME, &data->playerVol);
 
			(*data->playerObj)->GetInterface(data->playerObj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &data->playerBufferQueue);
		}

		// Begin playing.
		{
			const int bufferSizeBytes = data->bufferSize * data->channels * sizeof(short);
			(*data->playerBufferQueue)->Enqueue(data->playerBufferQueue, data->outputBuffers[0], bufferSizeBytes);
			data->activeBuffer = (data->activeBuffer + 1) % NUM_BUFFERS;

			(*data->player)->RegisterCallback(data->player, soloud_opensles_play_callback, aSoloud);
			(*data->player)->SetCallbackEventsMask(data->player, SL_PLAYEVENT_HEADATEND);
			(*data->player)->SetPlayState(data->player, SL_PLAYSTATE_PLAYING);

		}
		
		//
		aSoloud->postinit(aSamplerate,data->bufferSize,aFlags,2);
		aSoloud->mBackendData = data;
		aSoloud->mBackendCleanupFunc = soloud_opensles_deinit;

		LOG_INFO( "Creating audio thread." );
		Thread::createThread(opensles_thread, (void*)aSoloud);

		aSoloud->mBackendString = "OpenSL ES";
		return SO_NO_ERROR;
	}	
};
#endif

//----soloud_oss.cpp-----------------------------------------------------------------------
#if !defined(WITH_OSS)

namespace SoLoud
{
    result oss_init(Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer)
	{
		return NOT_IMPLEMENTED;
	}
};

#else
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <unistd.h>
#include <string.h>

namespace SoLoud
{
    static const int OSS_DEVICE_COUNT = 4;
    static const char *OSS_DEVICES[OSS_DEVICE_COUNT] = 
    { 
        "/dev/dsp", 
        "/dev/dsp0.0", 
        "/dev/dsp1.0", 
        "/dev/dsp2.0" 
    };

    struct OSSData
    {
        float *buffer;
        short *sampleBuffer;
        int ossDeviceHandle;
        Soloud *soloud;
        int samples;
        int channels;
        bool audioProcessingDone;
        Thread::ThreadHandle threadHandle;
    };

    static void ossThread(void *aParam)
    {
        OSSData *data = static_cast<OSSData*>(aParam);
        while (!data->audioProcessingDone) 
        {
            data->soloud->mix(data->buffer, data->samples);
            for (int i=0;i<data->samples*data->channels;++i)
            {
                data->sampleBuffer[i] = static_cast<short>(floor(data->buffer[i] 
                                                                 * static_cast<float>(0x7fff)));
            }
            write(data->ossDeviceHandle, data->sampleBuffer, 
                  sizeof(short)*data->samples*data->channels);
        }
    }

    static void ossCleanup(Soloud *aSoloud)
    {
        if (0 == aSoloud->mBackendData)
        {
            return;
        }
        OSSData *data = static_cast<OSSData*>(aSoloud->mBackendData);
        data->audioProcessingDone = true;
        if (data->threadHandle)
        {
            Thread::wait(data->threadHandle);
            Thread::release(data->threadHandle);
        }
        ioctl(data->ossDeviceHandle, SNDCTL_DSP_RESET, 0);       
        if (0 != data->sampleBuffer)
        {
            delete[] data->sampleBuffer;
        }
        if (0 != data->buffer)
        {
            delete[] data->buffer;
        }
        close(data->ossDeviceHandle);
        delete data;
        aSoloud->mBackendData = 0;
    }

    result oss_init(Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer, unsigned int aChannels)
    {
        OSSData *data = new OSSData;
        memset(data, 0, sizeof(OSSData));
        aSoloud->mBackendData = data;
        aSoloud->mBackendCleanupFunc = ossCleanup;
        data->samples = aBuffer;
        data->soloud = aSoloud;
        bool deviceIsOpen = false;
        for (int i=0;i<OSS_DEVICE_COUNT;++i)
        {
            data->ossDeviceHandle = open(OSS_DEVICES[i], O_WRONLY, 0);
            if (-1 != data->ossDeviceHandle)
            {
                deviceIsOpen = true;
                break;
            }
        }
        if (!deviceIsOpen)
        {
            return UNKNOWN_ERROR;
        }
        int flags = fcntl(data->ossDeviceHandle, F_GETFL);
        flags &= ~O_NONBLOCK;
        if (-1 == fcntl(data->ossDeviceHandle, F_SETFL, flags))
        {
            return UNKNOWN_ERROR;
        }        
        int format = AFMT_S16_NE;
        if (-1 == ioctl(data->ossDeviceHandle, SNDCTL_DSP_SETFMT, &format))
        {
            return UNKNOWN_ERROR;
        }
        if (format != AFMT_S16_NE)
        {
            return UNKNOWN_ERROR;
        }
        int channels = 2;
        data->channels = channels;
        if (-1 == ioctl(data->ossDeviceHandle, SNDCTL_DSP_CHANNELS, &data->channels))
        {
            return UNKNOWN_ERROR;
        }
        if (channels != data->channels)
        {
            return UNKNOWN_ERROR;
        }
        int speed = aSamplerate;
        if (-1 == ioctl(data->ossDeviceHandle, SNDCTL_DSP_SPEED, &speed))
        {
            return UNKNOWN_ERROR;
        }
        if (speed != aSamplerate)
        {
            return UNKNOWN_ERROR;
        }
        data->buffer = new float[data->samples*data->channels];
        data->sampleBuffer = new short[data->samples*data->channels];
        aSoloud->postinit(aSamplerate, data->samples * data->channels, aFlags, 2);
        data->threadHandle = Thread::createThread(ossThread, data);
        if (0 == data->threadHandle)
        {
            return UNKNOWN_ERROR;
        }
        aSoloud->mBackendString = "OSS";
        return 0;
    }
};
#endif
//----soloud_portaudio.cpp-----------------------------------------------------------------------
#if !defined(WITH_PORTAUDIO)

namespace SoLoud
{
	result portaudio_init(SoLoud::Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer)
	{
		return NOT_IMPLEMENTED;
	}
}
#else

#include "portaudio.h"


extern "C"
{
	int dll_Pa_found();
	PaError dll_Pa_Initialize( void );
	PaError dll_Pa_Terminate( void );
	PaError dll_Pa_CloseStream( PaStream *stream );
	PaError dll_Pa_StartStream( PaStream *stream );
	PaError dll_Pa_OpenDefaultStream( PaStream** stream,
                              int numInputChannels,
                              int numOutputChannels,
                              PaSampleFormat sampleFormat,
                              double sampleRate,
                              unsigned long framesPerBuffer,
                              PaStreamCallback *streamCallback,
                              void *userData );
};



namespace SoLoud
{
	static PaStream *gStream;

	static int portaudio_callback( 
				const void *input,
				void *output,
				unsigned long frameCount,
				const PaStreamCallbackTimeInfo* timeInfo,
				PaStreamCallbackFlags statusFlags,
				void *userData ) 
	{
		SoLoud::Soloud *soloud = (SoLoud::Soloud *)userData;
		float *mixdata = (float*)(soloud->mBackendData);
		soloud->mix((float*)output, frameCount);

		return 0;
	}

	static void portaudio_mutex_lock(void * mutex)
	{
		Thread::lockMutex(mutex);
	}

	static void portaudio_mutex_unlock(void * mutex)
	{
		Thread::unlockMutex(mutex);
	}

	void soloud_portaudio_deinit(SoLoud::Soloud *aSoloud)
	{
		dll_Pa_CloseStream(gStream);
		dll_Pa_Terminate();
	}

	result portaudio_init(SoLoud::Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer, unsigned int aChannels)
	{
		if (!dll_Pa_found())
			return DLL_NOT_FOUND;

		aSoloud->postinit(aSamplerate, aBuffer * 2, aFlags, 2);
		aSoloud->mBackendCleanupFunc = soloud_portaudio_deinit;
		dll_Pa_Initialize();
		dll_Pa_OpenDefaultStream(&gStream, 0, 2, paFloat32, aSamplerate, paFramesPerBufferUnspecified, portaudio_callback, (void*)aSoloud);
		dll_Pa_StartStream(gStream);
        aSoloud->mBackendString = "PortAudio";
		return 0;
	}
	
};
#endif
//----soloud_portaudio_dll.c-----------------------------------------------------------------------
#if defined(WITH_PORTAUDIO)
extern "C"
{

#if defined(_MSC_VER)
#define WINDOWS_VERSION
#endif

#include "portaudio.h"

typedef PaError (*Pa_InitializeProc)( void );
typedef PaError (*Pa_TerminateProc)( void );
typedef PaError (*Pa_CloseStreamProc)( PaStream *stream );
typedef PaError (*Pa_StartStreamProc)( PaStream *stream );
typedef PaError (*Pa_OpenDefaultStreamProc)( PaStream** stream,
                              int numInputChannels,
                              int numOutputChannels,
                              PaSampleFormat sampleFormat,
                              double sampleRate,
                              unsigned long framesPerBuffer,
                              PaStreamCallback *streamCallback,
                              void *userData );

static Pa_InitializeProc dPa_Initialize = NULL;
static Pa_TerminateProc dPa_Terminate = NULL;
static Pa_CloseStreamProc dPa_CloseStream = NULL;
static Pa_StartStreamProc dPa_StartStream = NULL;
static Pa_OpenDefaultStreamProc dPa_OpenDefaultStream = NULL;



#ifdef WINDOWS_VERSION
#include <Windows.h>

static HMODULE openDll()
{
    HMODULE dllh = LoadLibrary("portaudio_x86.dll");
    return dllh;
}

static void *getdllproc(HMODULE dllhandle, const char *procname)
{
    HMODULE dllh = (HMODULE)dllhandle;
    return GetProcAddress(dllh, procname);
}

#else
#include <dlfcn.h> // dll functions

static void* openDll()
{
    void* res = dlopen("libportaudio_x86.so", RTLD_LAZY);

//	if (!res) res = dlopen("/Library/Frameworks/PortAudio.framework", RTLD_LAZY);

	return res;
}

static void *getdllproc(void* dllhandle, const char *procname)
{
    void* library = dllhandle;
    return dlsym(library,procname);
}

#endif



static int load_dll()
{
#ifdef WINDOWS_VERSION
	HMODULE dll = NULL;
#else
	void * dll = NULL;
#endif

	if (dPa_OpenDefaultStream != NULL)
	{
		return 1;
	}

    dll = openDll();

    if (dll)
    {
		dPa_Initialize = (Pa_InitializeProc)getdllproc(dll,"Pa_Initialize");
		dPa_Terminate = (Pa_TerminateProc)getdllproc(dll,"Pa_Terminate");
		dPa_CloseStream = (Pa_CloseStreamProc)getdllproc(dll,"Pa_CloseStream");
		dPa_StartStream = (Pa_StartStreamProc)getdllproc(dll,"Pa_StartStream");
		dPa_OpenDefaultStream = (Pa_OpenDefaultStreamProc)getdllproc(dll,"Pa_OpenDefaultStream");

		if (dPa_Initialize == NULL ||
			dPa_Terminate == NULL ||
			dPa_CloseStream == NULL ||
			dPa_StartStream == NULL ||
			dPa_OpenDefaultStream == NULL)
		{
			dPa_OpenDefaultStream = NULL;
			return 0;
		}
		return 1;
	}
	return 0;	
}

int dll_Pa_found()
{
	return load_dll();
}

PaError dll_Pa_Initialize( void )
{
	if (load_dll())
		return dPa_Initialize();
	return paNotInitialized;
}

PaError dll_Pa_Terminate( void )
{
	if (load_dll())
		return dPa_Terminate();
	return paNotInitialized;
}

PaError dll_Pa_CloseStream( PaStream *stream )
{
	if (load_dll())
		return dPa_CloseStream(stream);
	return paNotInitialized;
}

PaError dll_Pa_StartStream( PaStream *stream )
{
	if (load_dll())
		return dPa_StartStream(stream);
	return paNotInitialized;
}

PaError dll_Pa_OpenDefaultStream( PaStream** stream,
                            int numInputChannels,
                            int numOutputChannels,
                            PaSampleFormat sampleFormat,
                            double sampleRate,
                            unsigned long framesPerBuffer,
                            PaStreamCallback *streamCallback,
                            void *userData )
{
	if (load_dll())
		return dPa_OpenDefaultStream(stream,numInputChannels,numOutputChannels,sampleFormat,sampleRate,framesPerBuffer,streamCallback,userData);
	return paNotInitialized;
}

}
#endif //defined(WITH_PORTAUDIO)

//----soloud_sdl.cpp-----------------------------------------------------------------------
#if !(defined(WITH_SDL) || defined(WITH_SDL2))

namespace SoLoud
{
	result sdl_init(SoLoud::Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer)
	{
		return NOT_IMPLEMENTED;
	}
}

#else

#if defined(_MSC_VER)
#include "SDL.h"
#else
#include "SDL/SDL.h"
#endif
#include <math.h>


extern "C"
{
	int dll_SDL_found();
	int dll_SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
	void dll_SDL_CloseAudio();
	void dll_SDL_PauseAudio(int pause_on);
	SDL_mutex * dll_SDL_CreateMutex();
	void dll_SDL_DestroyMutex(SDL_mutex * mutex);
	int dll_SDL_mutexP(SDL_mutex * mutex);
	int dll_SDL_mutexV(SDL_mutex * mutex);
};


namespace SoLoud
{
	static SDL_AudioSpec gActiveAudioSpec;

	void soloud_sdl_audiomixer(void *userdata, Uint8 *stream, int len)
	{
		short *buf = (short*)stream;
		SoLoud::Soloud *soloud = (SoLoud::Soloud *)userdata;
		if (gActiveAudioSpec.format == AUDIO_F32)
		{
			int samples = len / (gActiveAudioSpec.channels * sizeof(float));
			soloud->mix((float *)buf, samples);
		}
		else // assume s16 if not float
		{
			int samples = len / (gActiveAudioSpec.channels * sizeof(short));
			soloud->mixSigned16(buf, samples);
		}
	}

	static void soloud_sdl_deinit(SoLoud::Soloud *aSoloud)
	{
		dll_SDL_CloseAudio();
	}

	result sdl_init(SoLoud::Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer, unsigned int aChannels)
	{
		if (!dll_SDL_found())
			return DLL_NOT_FOUND;

		SDL_AudioSpec as;
		as.freq = aSamplerate;
		as.format = AUDIO_F32;
		as.channels = aChannels;
		as.samples = aBuffer;
		as.callback = soloud_sdl_audiomixer;
		as.userdata = (void*)aSoloud;

		if (dll_SDL_OpenAudio(&as, &gActiveAudioSpec) < 0)
		{
			as.format = AUDIO_S16;
			if (dll_SDL_OpenAudio(&as, &gActiveAudioSpec) < 0 || gActiveAudioSpec.format != AUDIO_S16)
			{
				return UNKNOWN_ERROR;
			}
		}

		aSoloud->postinit(gActiveAudioSpec.freq, gActiveAudioSpec.samples, aFlags, gActiveAudioSpec.channels);

		aSoloud->mBackendCleanupFunc = soloud_sdl_deinit;

		dll_SDL_PauseAudio(0);
        aSoloud->mBackendString = "SDL (dynamic)";
		return 0;
	}
	
};
#endif
//----soloud_sdl_dll.c-----------------------------------------------------------------------
#if (defined(WITH_SDL) || defined(WITH_SDL2))
#include <stdlib.h>
#if defined(_MSC_VER)
#define WINDOWS_VERSION
#include "SDL.h"
#else
#include "SDL/SDL.h"
#endif
#include <math.h>

typedef int (*SDLOpenAudio)(SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
typedef void (*SDLCloseAudio)();
typedef void (*SDLPauseAudio)(int pause_on);

static SDLOpenAudio dSDL_OpenAudio = NULL;
static SDLCloseAudio dSDL_CloseAudio = NULL;
static SDLPauseAudio dSDL_PauseAudio = NULL;

#ifdef WINDOWS_VERSION
#include <windows.h>

static HMODULE openDll()
{
	HMODULE res = LoadLibraryA("SDL2.dll");
	if (!res) res = LoadLibraryA("SDL.dll");
    return res;
}

static void* getDllProc(HMODULE aDllHandle, const char *aProcName)
{
    return GetProcAddress(aDllHandle, aProcName);
}

#else
#include <dlfcn.h> // dll functions

static void * openDll()
{
	void * res;
	res = dlopen("/Library/Frameworks/SDL2.framework/SDL2", RTLD_LAZY);
	if (!res) res = dlopen("/Library/Frameworks/SDL.framework/SDL", RTLD_LAZY);
	if (!res) res = dlopen("SDL2.so", RTLD_LAZY);
	if (!res) res = dlopen("SDL.so", RTLD_LAZY);
    return res;
}

static void* getDllProc(void * aLibrary, const char *aProcName)
{
    return dlsym(aLibrary, aProcName);
}

#endif

static int load_dll()
{
#ifdef WINDOWS_VERSION
	HMODULE dll = NULL;
#else
	void * dll = NULL;
#endif

	if (dSDL_OpenAudio != NULL)
	{
		return 1;
	}

    dll = openDll();

    if (dll)
    {
	    dSDL_OpenAudio = (SDLOpenAudio)getDllProc(dll, "SDL_OpenAudio");
	    dSDL_CloseAudio = (SDLCloseAudio)getDllProc(dll, "SDL_CloseAudio");
	    dSDL_PauseAudio = (SDLPauseAudio)getDllProc(dll, "SDL_PauseAudio");


        if (dSDL_OpenAudio && 
        	dSDL_CloseAudio &&
        	dSDL_PauseAudio)
        {
        	return 1;
        }
	}
	dSDL_OpenAudio = NULL;
    return 0;
}

int dll_SDL_found()
{
	return load_dll();
}

int dll_SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained)
{
	if (load_dll())
		return dSDL_OpenAudio(desired, obtained);
	return 0;
}

void dll_SDL_CloseAudio()
{
	if (load_dll())
		dSDL_CloseAudio();
}

void dll_SDL_PauseAudio(int pause_on)
{
	if (load_dll())
		dSDL_PauseAudio(pause_on);
}
#endif //(defined(WITH_SDL) || defined(WITH_SDL2))

//----soloud_sdl2_static.cpp-----------------------------------------------------------------------
#include <stdlib.h>
#if !defined(WITH_SDL2_STATIC)

namespace SoLoud
{
	result sdl2static_init(SoLoud::Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer)
	{
		return NOT_IMPLEMENTED;
	}
}

#else	//  !defined(WITH_SDL2_STATIC)

#if defined(_MSC_VER)
#include "SDL.h"
#else
#include "SDL2/SDL.h"
#endif
#include <math.h>


namespace SoLoud
{
	static SDL_AudioSpec gActiveAudioSpec;

	void soloud_sdl2static_audiomixer(void *userdata, Uint8 *stream, int len)
	{
		short *buf = (short*)stream;
		SoLoud::Soloud *soloud = (SoLoud::Soloud *)userdata;
		if (gActiveAudioSpec.format == AUDIO_F32)
		{
			int samples = len / (gActiveAudioSpec.channels * sizeof(float));
			soloud->mix((float *)buf, samples);
		}
		else // assume s16 if not float
		{
			int samples = len / (gActiveAudioSpec.channels * sizeof(short));
			soloud->mixSigned16(buf, samples);
		}
	}

	static void soloud_sdl2static_deinit(SoLoud::Soloud *aSoloud)
	{
		SDL_CloseAudio();
	}

	result sdl2static_init(SoLoud::Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer, unsigned int aChannels)
	{
		SDL_AudioSpec as;
		as.freq = aSamplerate;
		as.format = AUDIO_F32;
		as.channels = aChannels;
		as.samples = aBuffer;
		as.callback = soloud_sdl2static_audiomixer;
		as.userdata = (void*)aSoloud;

		if (SDL_OpenAudio(&as, &gActiveAudioSpec) < 0)
		{
			as.format = AUDIO_S16;
			if (SDL_OpenAudio(&as, &gActiveAudioSpec) < 0 || gActiveAudioSpec.format != AUDIO_S16)
			{
				return UNKNOWN_ERROR;
			}
		}

		aSoloud->postinit(gActiveAudioSpec.freq, gActiveAudioSpec.samples, aFlags, gActiveAudioSpec.channels);

		aSoloud->mBackendCleanupFunc = soloud_sdl2static_deinit;

		SDL_PauseAudio(0);
        aSoloud->mBackendString = "SDL2 (static)";
		return 0;
	}
	
};
#endif //  !defined(WITH_SDL2_STATIC)

//----soloud_sdl_static.cpp-----------------------------------------------------------------------
#if !defined(WITH_SDL_STATIC)

namespace SoLoud
{
	result sdlstatic_init(SoLoud::Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer)
	{
		return NOT_IMPLEMENTED;
	}
}

#else

#if defined(_MSC_VER)
#include "SDL.h"
#else
#include "SDL/SDL.h"
#endif
#include <math.h>


namespace SoLoud
{
	static SDL_AudioSpec gActiveAudioSpec;
	void soloud_sdlstatic_audiomixer(void *userdata, Uint8 *stream, int len)
	{
		short *buf = (short*)stream;
		SoLoud::Soloud *soloud = (SoLoud::Soloud *)userdata;
		if (gActiveAudioSpec.format == AUDIO_F32)
		{
			int samples = len / (gActiveAudioSpec.channels * sizeof(float));
			soloud->mix((float *)buf, samples);
		}
		else // assume s16 if not float
		{
			int samples = len / (gActiveAudioSpec.channels * sizeof(short));
			soloud->mixSigned16(buf, samples);
		}
	}

	static void soloud_sdlstatic_deinit(SoLoud::Soloud *aSoloud)
	{
		SDL_CloseAudio();
	}

	result sdlstatic_init(SoLoud::Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer, unsigned int aChannels)
	{
		SDL_AudioSpec as;
		as.freq = aSamplerate;
		as.format = AUDIO_F32;
		as.channels = aChannels;
		as.samples = aBuffer;
		as.callback = soloud_sdlstatic_audiomixer;
		as.userdata = (void*)aSoloud;

		if (SDL_OpenAudio(&as, &gActiveAudioSpec) < 0)
		{
			as.format = AUDIO_S16;
			if (SDL_OpenAudio(&as, &gActiveAudioSpec) < 0 || gActiveAudioSpec.format != AUDIO_S16)
			{
				return UNKNOWN_ERROR;
			}
		}

		aSoloud->postinit(gActiveAudioSpec.freq, gActiveAudioSpec.samples, aFlags, gActiveAudioSpec.channels);

		aSoloud->mBackendCleanupFunc = soloud_sdlstatic_deinit;

		SDL_PauseAudio(0);
        aSoloud->mBackendString = "SDL (static)";
		return 0;
	}
	
};
#endif

//----soloud_wasapi.cpp-----------------------------------------------------------------------
#if !defined(WITH_WASAPI)

namespace SoLoud
{
	result wasapi_init(Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer)
	{
		return NOT_IMPLEMENTED;
	}
};

#else

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

#define SAFE_RELEASE(x) \
    if (0 != (x)) \
    { \
        (x)->Release(); \
        (x) = 0; \
    }

namespace SoLoud
{
    struct WASAPIData
    {
        IMMDeviceEnumerator *deviceEnumerator;
        IMMDevice *device;
        IAudioClient *audioClient;
        IAudioRenderClient *renderClient;
        HANDLE bufferEndEvent;
        HANDLE audioProcessingDoneEvent;
        Thread::ThreadHandle thread;
        Soloud *soloud;
        UINT32 bufferFrames;
        int channels;
    };

    static void wasapiSubmitBuffer(WASAPIData *aData, UINT32 aFrames)
    {
        BYTE *buffer = 0;
        if (FAILED(aData->renderClient->GetBuffer(aFrames, &buffer)))
        {
            return;
        }
		aData->soloud->mixSigned16((short *)buffer, aFrames);
        aData->renderClient->ReleaseBuffer(aFrames, 0);
    }

    static void wasapiThread(LPVOID aParam)
    {
        WASAPIData *data = static_cast<WASAPIData*>(aParam);
        wasapiSubmitBuffer(data, data->bufferFrames);
        data->audioClient->Start();
        while (WAIT_OBJECT_0 != WaitForSingleObject(data->audioProcessingDoneEvent, 0)) 
        {
            WaitForSingleObject(data->bufferEndEvent, INFINITE);
            UINT32 padding = 0;
            if (FAILED(data->audioClient->GetCurrentPadding(&padding)))
            {
                continue;
            }
            UINT32 frames = data->bufferFrames - padding;
            wasapiSubmitBuffer(data, frames);
        }
    }

    static void wasapiCleanup(Soloud *aSoloud)
    {
        if (0 == aSoloud->mBackendData)
        {
            return;
        }
        WASAPIData *data = static_cast<WASAPIData*>(aSoloud->mBackendData);
        SetEvent(data->audioProcessingDoneEvent);
        SetEvent(data->bufferEndEvent);
		if (data->thread)
		{
			Thread::wait(data->thread);
			Thread::release(data->thread);
		}
        CloseHandle(data->bufferEndEvent);
        CloseHandle(data->audioProcessingDoneEvent);
        if (0 != data->audioClient)
        {
            data->audioClient->Stop();
        }
        SAFE_RELEASE(data->renderClient);
        SAFE_RELEASE(data->audioClient);
        SAFE_RELEASE(data->device);
        SAFE_RELEASE(data->deviceEnumerator);
        delete data;
        aSoloud->mBackendData = 0;
        CoUninitialize();
    }

	result wasapi_init(Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer, unsigned int aChannels)
    {
        if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED)))
        {
            return UNKNOWN_ERROR;
        }
        WASAPIData *data = new WASAPIData;
        ZeroMemory(data, sizeof(WASAPIData));
        aSoloud->mBackendData = data;
        aSoloud->mBackendCleanupFunc = wasapiCleanup;
		
        data->bufferEndEvent = CreateEvent(0, FALSE, FALSE, 0);
        if (0 == data->bufferEndEvent)
        {
            return UNKNOWN_ERROR;
        }
        data->audioProcessingDoneEvent = CreateEvent(0, FALSE, FALSE, 0);
        if (0 == data->audioProcessingDoneEvent)
        {
            return UNKNOWN_ERROR;
        }
        if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), 0, CLSCTX_ALL, 
                   __uuidof(IMMDeviceEnumerator), 
                   reinterpret_cast<void**>(&data->deviceEnumerator)))) 
        {
            return UNKNOWN_ERROR;
        }
        if (FAILED(data->deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, 
                                                                   &data->device))) 
        {
            return UNKNOWN_ERROR;
        }
        if (FAILED(data->device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, 0, 
                                          reinterpret_cast<void**>(&data->audioClient)))) 
        {
            return UNKNOWN_ERROR;
        }
        WAVEFORMATEX format;
        ZeroMemory(&format, sizeof(WAVEFORMATEX));
        format.nChannels = 2;
        format.nSamplesPerSec = aSamplerate;
        format.wFormatTag = WAVE_FORMAT_PCM;
        format.wBitsPerSample = sizeof(short)*8;
        format.nBlockAlign = (format.nChannels*format.wBitsPerSample)/8;
        format.nAvgBytesPerSec = format.nSamplesPerSec*format.nBlockAlign;
        REFERENCE_TIME dur = static_cast<REFERENCE_TIME>(static_cast<double>(aBuffer)
                                           / (static_cast<double>(aSamplerate)*(1.0/10000000.0)));
		HRESULT res = data->audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
			AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
			dur, 0, &format, 0);
		if (FAILED(res)) 
        {
            return UNKNOWN_ERROR;
        }
        data->bufferFrames = 0;
        if (FAILED(data->audioClient->GetBufferSize(&data->bufferFrames)))
        {
            return UNKNOWN_ERROR;
        }
        if (FAILED(data->audioClient->GetService(__uuidof(IAudioRenderClient), 
                                                 reinterpret_cast<void**>(&data->renderClient)))) 
        {
            return UNKNOWN_ERROR;
        }
        if (FAILED(data->audioClient->SetEventHandle(data->bufferEndEvent)))
        {
            return UNKNOWN_ERROR;
        }
        data->channels = format.nChannels;
        data->soloud = aSoloud;
        aSoloud->postinit(aSamplerate, data->bufferFrames * format.nChannels, aFlags, 2);
        data->thread = Thread::createThread(wasapiThread, data);
        if (0 == data->thread)
        {
            return UNKNOWN_ERROR;
        }
        aSoloud->mBackendString = "WASAPI";
        return 0;
    }
};
#endif
//----soloud_winmm.cpp-----------------------------------------------------------------------
#if !defined(WITH_WINMM)

namespace SoLoud
{
	result winmm_init(Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer)
	{
		return NOT_IMPLEMENTED;
	}
};

#else

#include <windows.h>
#include <mmsystem.h>

#ifdef _MSC_VER
#pragma comment(lib, "winmm.lib")
#endif

namespace SoLoud
{
    static const int BUFFER_COUNT = 2;

    struct SoLoudWinMMData
    {
        AlignedFloatBuffer buffer;
        short *sampleBuffer[BUFFER_COUNT];
        WAVEHDR header[BUFFER_COUNT];
        HWAVEOUT waveOut;
        HANDLE bufferEndEvent;
        HANDLE audioProcessingDoneEvent;
        Soloud *soloud;
        int samples;
        Thread::ThreadHandle threadHandle;
    };

    static void winMMThread(LPVOID aParam)
    {
        SoLoudWinMMData *data = static_cast<SoLoudWinMMData*>(aParam);
        while (WAIT_OBJECT_0 != WaitForSingleObject(data->audioProcessingDoneEvent, 0)) 
        {
            for (int i=0;i<BUFFER_COUNT;++i) 
            {
                if (0 != (data->header[i].dwFlags & WHDR_INQUEUE)) 
                {
                    continue;
                }
                short *tgtBuf = data->sampleBuffer[i];
				
				data->soloud->mixSigned16(tgtBuf, data->samples);

				if (MMSYSERR_NOERROR != waveOutWrite(data->waveOut, &data->header[i], 
                                                     sizeof(WAVEHDR))) 
                {
                    return;
                }
            }
            WaitForSingleObject(data->bufferEndEvent, INFINITE);
        }
    }

    static void winMMCleanup(Soloud *aSoloud)
    {
        if (0 == aSoloud->mBackendData)
        {
            return;
        }
        SoLoudWinMMData *data = static_cast<SoLoudWinMMData*>(aSoloud->mBackendData);
        SetEvent(data->audioProcessingDoneEvent);
        SetEvent(data->bufferEndEvent);
        Thread::wait(data->threadHandle);
        Thread::release(data->threadHandle);
        CloseHandle(data->audioProcessingDoneEvent);
        CloseHandle(data->bufferEndEvent);
        waveOutReset(data->waveOut);
        for (int i=0;i<BUFFER_COUNT;++i) 
        {
            waveOutUnprepareHeader(data->waveOut, &data->header[i], sizeof(WAVEHDR));
            if (0 != data->sampleBuffer[i])
            {
                delete[] data->sampleBuffer[i];
            }
        }
        waveOutClose(data->waveOut);
        delete data;
        aSoloud->mBackendData = 0;
    }

	result winmm_init(Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer, unsigned int aChannels)
    {
        SoLoudWinMMData *data = new SoLoudWinMMData;
        ZeroMemory(data, sizeof(SoLoudWinMMData));
        aSoloud->mBackendData = data;
        aSoloud->mBackendCleanupFunc = winMMCleanup;
        data->samples = aBuffer;
        data->soloud = aSoloud;
        data->bufferEndEvent = CreateEvent(0, FALSE, FALSE, 0);
        if (0 == data->bufferEndEvent)
        {
            return UNKNOWN_ERROR;
        }
        data->audioProcessingDoneEvent = CreateEvent(0, FALSE, FALSE, 0);
        if (0 == data->audioProcessingDoneEvent)
        {
            return UNKNOWN_ERROR;
        }
        WAVEFORMATEX format;
        ZeroMemory(&format, sizeof(WAVEFORMATEX));
        format.nChannels = 2;
        format.nSamplesPerSec = aSamplerate;
        format.wFormatTag = WAVE_FORMAT_PCM;
        format.wBitsPerSample = sizeof(short)*8;
        format.nBlockAlign = (format.nChannels*format.wBitsPerSample)/8;
        format.nAvgBytesPerSec = format.nSamplesPerSec*format.nBlockAlign;
        if (MMSYSERR_NOERROR != waveOutOpen(&data->waveOut, WAVE_MAPPER, &format, 
                            reinterpret_cast<DWORD_PTR>(data->bufferEndEvent), 0, CALLBACK_EVENT)) 
        {
            return UNKNOWN_ERROR;
        }
        data->buffer.init(data->samples*format.nChannels);
        for (int i=0;i<BUFFER_COUNT;++i) 
        {
            data->sampleBuffer[i] = new short[data->samples*format.nChannels];
            ZeroMemory(&data->header[i], sizeof(WAVEHDR));
            data->header[i].dwBufferLength = data->samples*sizeof(short)*format.nChannels;
            data->header[i].lpData = reinterpret_cast<LPSTR>(data->sampleBuffer[i]);
            if (MMSYSERR_NOERROR != waveOutPrepareHeader(data->waveOut, &data->header[i], 
                                                         sizeof(WAVEHDR))) 
            {
                return UNKNOWN_ERROR;
            }
        }
        aSoloud->postinit(aSamplerate, data->samples * format.nChannels, aFlags, 2);
        data->threadHandle = Thread::createThread(winMMThread, data);
        if (0 == data->threadHandle)
        {
            return UNKNOWN_ERROR;
        }
        aSoloud->mBackendString = "WinMM";
        return 0;
    }
};

#endif
//----soloud_xaudio2.cpp-----------------------------------------------------------------------
#if !defined(WITH_XAUDIO2)

namespace SoLoud
{
	result xaudio2_init(Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer)
	{
		return NOT_IMPLEMENTED;
	}
};

#else	//  !defined(WITH_XAUDIO2)

#include <windows.h>

#ifdef _MSC_VER
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#else
#include "backend/xaudio2/xaudio2.h"
#endif

namespace SoLoud
{
    static const int BUFFER_COUNT = 2;

    struct XAudio2Data
    {
        float *buffer[BUFFER_COUNT];
        IXAudio2 *xaudio2;
        IXAudio2MasteringVoice *masteringVoice;
        IXAudio2SourceVoice *sourceVoice;
        HANDLE bufferEndEvent;
        HANDLE audioProcessingDoneEvent;
        class VoiceCallback *voiceCb;
        Thread::ThreadHandle thread;
        Soloud *soloud;
        int samples;
        UINT32 bufferLengthBytes;
    };

    class VoiceCallback : public IXAudio2VoiceCallback
    {
    public:
        VoiceCallback(HANDLE aBufferEndEvent) 
            : IXAudio2VoiceCallback(), mBufferEndEvent(aBufferEndEvent) {}
        virtual ~VoiceCallback() {}

    private:
        // Called just before this voice's processing pass begins.
        void __stdcall OnVoiceProcessingPassStart(UINT32 aBytesRequired) {}

        // Called just after this voice's processing pass ends.
        void __stdcall OnVoiceProcessingPassEnd() {}

        // Called when this voice has just finished playing a buffer stream
        // (as marked with the XAUDIO2_END_OF_STREAM flag on the last buffer).
        void __stdcall OnStreamEnd() {}

        // Called when this voice is about to start processing a new buffer.
        void __stdcall OnBufferStart(void *aBufferContext) {}

        // Called when this voice has just finished processing a buffer.
        // The buffer can now be reused or destroyed.
        void __stdcall OnBufferEnd(void *aBufferContext) 
        {
            SetEvent(mBufferEndEvent);
        }

        // Called when this voice has just reached the end position of a loop.
        void __stdcall OnLoopEnd(void *aBufferContext) {}

        // Called in the event of a critical error during voice processing,
        // such as a failing xAPO or an error from the hardware XMA decoder.
        // The voice may have to be destroyed and re-created to recover from
        // the error.  The callback arguments report which buffer was being
        // processed when the error occurred, and its HRESULT code.
        void __stdcall OnVoiceError(void *aBufferContext, HRESULT aError) {}

        HANDLE mBufferEndEvent;
    };

    static void xaudio2Thread(LPVOID aParam)
    {
        XAudio2Data *data = static_cast<XAudio2Data*>(aParam);
        int bufferIndex = 0;
        while (WAIT_OBJECT_0 != WaitForSingleObject(data->audioProcessingDoneEvent, 0)) 
        {
            XAUDIO2_VOICE_STATE state;
            data->sourceVoice->GetState(&state);
            while (state.BuffersQueued < BUFFER_COUNT) 
            {
                data->soloud->mix(data->buffer[bufferIndex], data->samples);
                XAUDIO2_BUFFER info = {0};
                info.AudioBytes = data->bufferLengthBytes;
                info.pAudioData = reinterpret_cast<const BYTE*>(data->buffer[bufferIndex]);
                data->sourceVoice->SubmitSourceBuffer(&info);
                ++bufferIndex;
                if (bufferIndex >= BUFFER_COUNT)
                {
                    bufferIndex = 0;
                }
                data->sourceVoice->GetState(&state);
            }
            WaitForSingleObject(data->bufferEndEvent, INFINITE);
        }
    }

    static void xaudio2Cleanup(Soloud *aSoloud)
    {
        if (0 == aSoloud->mBackendData)
        {
            return;
        }
        XAudio2Data *data = static_cast<XAudio2Data*>(aSoloud->mBackendData);
        SetEvent(data->audioProcessingDoneEvent);
        SetEvent(data->bufferEndEvent);
        Thread::wait(data->thread);
        Thread::release(data->thread);
        CloseHandle(data->bufferEndEvent);
        CloseHandle(data->audioProcessingDoneEvent);
        if (0 != data->sourceVoice) 
        {
            data->sourceVoice->Stop();
            data->sourceVoice->FlushSourceBuffers();
        }
        if (0 != data->xaudio2)
        {
            data->xaudio2->StopEngine();
        }
        if (0 != data->sourceVoice)
        {
            data->sourceVoice->DestroyVoice();
        }
        if (0 != data->voiceCb)
        {
            delete data->voiceCb;
        }
        if (0 != data->masteringVoice)
        {
            data->masteringVoice->DestroyVoice();
        }
        if (0 != data->xaudio2)
        {
            data->xaudio2->Release();
        }
        for (int i=0;i<BUFFER_COUNT;++i) 
        {
            if (0 != data->buffer[i])
            {
                delete[] data->buffer[i];
            }
        }
        delete data;
        aSoloud->mBackendData = 0;
        CoUninitialize();
    }

    result xaudio2_init(Soloud *aSoloud, unsigned int aFlags, unsigned int aSamplerate, unsigned int aBuffer, unsigned int aChannels)
    {
        if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED)))
        {
            return UNKNOWN_ERROR;
        }
        XAudio2Data *data = new XAudio2Data;
        ZeroMemory(data, sizeof(XAudio2Data));
        aSoloud->mBackendData = data;
        aSoloud->mBackendCleanupFunc = xaudio2Cleanup;
        data->bufferEndEvent = CreateEvent(0, FALSE, FALSE, 0);
        if (0 == data->bufferEndEvent)
        {
            return UNKNOWN_ERROR;
        }
        data->audioProcessingDoneEvent = CreateEvent(0, FALSE, FALSE, 0);
        if (0 == data->audioProcessingDoneEvent)
        {
            return UNKNOWN_ERROR;
        }
        WAVEFORMATEX format;
        ZeroMemory(&format, sizeof(WAVEFORMATEX));
        format.nChannels = 2;
        format.nSamplesPerSec = aSamplerate;
        format.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
        format.nAvgBytesPerSec = aSamplerate*sizeof(float)*format.nChannels;
        format.nBlockAlign = sizeof(float)*format.nChannels;
        format.wBitsPerSample = sizeof(float)*8;
        if (FAILED(XAudio2Create(&data->xaudio2)))
        {
            return UNKNOWN_ERROR;
        }
        if (FAILED(data->xaudio2->CreateMasteringVoice(&data->masteringVoice, 
                                                       format.nChannels, aSamplerate))) 
        {
            return UNKNOWN_ERROR;
        }
        data->voiceCb = new VoiceCallback(data->bufferEndEvent);
        if (FAILED(data->xaudio2->CreateSourceVoice(&data->sourceVoice, 
                   &format, XAUDIO2_VOICE_NOSRC|XAUDIO2_VOICE_NOPITCH, 2.f, data->voiceCb))) 
        {
            return UNKNOWN_ERROR;
        }
        data->bufferLengthBytes = aBuffer * format.nChannels * sizeof(float);
        for (int i=0;i<BUFFER_COUNT;++i)
        {
            data->buffer[i] = new float[aBuffer * format.nChannels];
        }
        data->samples = aBuffer;
        data->soloud = aSoloud;
        aSoloud->postinit(aSamplerate, aBuffer * format.nChannels, aFlags, 2);
        data->thread = Thread::createThread(xaudio2Thread, data);
        if (0 == data->thread)
        {
            return UNKNOWN_ERROR;
        }
        data->sourceVoice->Start();
        aSoloud->mBackendString = "XAudio2";
        return 0;
    }
};

#endif //  !defined(WITH_XAUDIO2)

//--- end file here ----------------------------------------------------------------
//--warning handling:----------------------------------------------------------------
#ifdef _MSC_VER
#   pragma warning(default:4100)
#else // _MSC_VER
#   pragma GCC diagnostic pop
#endif //_MSC_VER
//-----------------------------------------------------------------------------------

#ifndef NO_IMGUISOLOUD_METHODS
namespace ImGuiSoloud {

#ifndef NO_IMGUISOLOUD_BASICWAVE
// TODO: create a struct that bundles the Soloud::Piano example
BasicPiano::BasicPiano() {
    pSoloud = NULL;
    bushandle = 0;
    memset(&gPlonked[0],'\0',128*sizeof(Plonked));
    gAttack = 0.02f;
    gRelease = 0.5f;
    filter_param0[0]=0;filter_param0[1]=0;filter_param0[2]=1;filter_param0[3]=1;
    filter_param1[0]=8000;filter_param1[1]=0;filter_param1[2]=1000;filter_param1[3]=0;
    filter_param2[0]=3;filter_param2[1]=0;filter_param2[2]=2;filter_param2[3]=0;
    gFilterSelect = 0;
    inited = false;

    gWaveSelect = 2; // SoLoud::Basicwave::SQUARE
    gWave.setWaveform(gWaveSelect);
}

void BasicPiano::init(SoLoud::Soloud &gSoloud, const int *pOptional18KeysOverrideFromFDiesisToB) {
    inited = true;

    pSoloud = &gSoloud;
    //gSoloud.init(SoLoud::Soloud::CLIP_ROUNDOFF | SoLoud::Soloud::ENABLE_VISUALIZATION);

    pSoloud->setGlobalVolume(0.75);
    pSoloud->setPostClipScaler(0.75);
    bushandle = pSoloud->play(gBus);

#   ifdef SOLOUD_LOFIFILTER_H
    gBus.setFilter(0, &gLofiFilter);
#   endif
#   ifdef SOLOUD_ECHOFILTER_H
    gEchoFilter.setParams(0.5f, 0.5f);
    gBus.setFilter(1, &gEchoFilter);
#   endif
#   ifdef SOLOUD_DCREMOVAL_H
    gBus.setFilter(3, &gDCRemovalFilter);
#   endif

    if (pOptional18KeysOverrideFromFDiesisToB) {
        for (int i=0;i<18;i++) DefaultKeys[i] = pOptional18KeysOverrideFromFDiesisToB[i];
    }
}

int BasicPiano::DefaultKeys[18] = {(int)'1',(int)'q',(int)'2',(int)'w',(int)'3',(int)'e',(int)'r',(int)'5',(int)'t',(int)'6',
                                   (int)'y',(int)'u',(int)'8',(int)'i',(int)'9',(int)'o',(int)'0',(int)'p'};

void BasicPiano::play() {
    if (!inited) return;

    pSoloud->setFilterParameter(bushandle, 0, 0, filter_param0[0]);
    pSoloud->setFilterParameter(bushandle, 1, 0, filter_param0[1]);
    pSoloud->setFilterParameter(bushandle, 2, 0, filter_param0[2]);
    pSoloud->setFilterParameter(bushandle, 3, 0, filter_param0[3]);

    pSoloud->setFilterParameter(bushandle, 0, 1, filter_param1[0]);
    pSoloud->setFilterParameter(bushandle, 0, 2, filter_param2[0]);
    ImGuiIO& io = ImGui::GetIO();

    static bool KeysDown[128]={0};

#   define NOTEKEY(x, p)\
    if (io.KeysDown[x] && !KeysDown[x]) { plonk((float)pow(0.943875f, p));KeysDown[x]=true;} \
    if (!io.KeysDown[x] && KeysDown[x]) { unplonk((float)pow(0.943875f, p));KeysDown[x]=false;}

    for (int i=0;i<18;i++) {
        NOTEKEY(DefaultKeys[i],18-i);
    }

    /*
    NOTEKEY((int)'1', 18); // F#
    NOTEKEY((int)'q', 17); // G
    NOTEKEY((int)'2', 16); // G#
    NOTEKEY((int)'w', 15); // A
    NOTEKEY((int)'3', 14); // A#
    NOTEKEY((int)'e', 13); // B
    NOTEKEY((int)'r', 12); // C
    NOTEKEY((int)'5', 11); // C#
    NOTEKEY((int)'t', 10); // D
    NOTEKEY((int)'6', 9); // D#
    NOTEKEY((int)'y', 8); // E
    NOTEKEY((int)'u', 7); // F
    NOTEKEY((int)'8', 6); // F#
    NOTEKEY((int)'i', 5); // G
    NOTEKEY((int)'9', 4); // G#
    NOTEKEY((int)'o', 3); // A
    NOTEKEY((int)'0', 2); // A#
    NOTEKEY((int)'p', 1); // B
    */

#   undef NOTEKEY
}

void BasicPiano::renderGUI() {
    if (!inited) return;
    ImGui::PushID(this);
    static const char* forms[] = {"Sine","Triangle","Square","Saw","InverseSaw"};
    ImGui::PushItemWidth(ImGui::GetWindowWidth()/3);
    if (ImGui::Combo("Waveform##SoLoudPianoWaveForm",&gWaveSelect,&forms[0],sizeof forms/sizeof forms[0],sizeof forms/sizeof forms[0])) {
        gWave.setWaveform(gWaveSelect);
    }
    ImGui::PopItemWidth();
#   ifdef SOLOUD_BQRFILTER_H
    if (ImGui::TreeNode("BQRFilter"))
    {
        if (ImGui::RadioButton("None", gFilterSelect == 0))
        {
            gFilterSelect = 0;
            gBus.setFilter(2, 0);
        }
        if (ImGui::RadioButton("Lowpass", gFilterSelect == 1))
        {
            gFilterSelect = 1;
            gBQRFilter.setParams(SoLoud::BiquadResonantFilter::LOWPASS, 44100, 1000, 2);
            gBus.setFilter(2, &gBQRFilter);
        }
        if (ImGui::RadioButton("Highpass", gFilterSelect == 2))
        {
            gFilterSelect = 2;
            gBQRFilter.setParams(SoLoud::BiquadResonantFilter::HIGHPASS, 44100, 1000, 2);
            gBus.setFilter(2, &gBQRFilter);
        }
        if (ImGui::RadioButton("Bandpass", gFilterSelect == 3))
        {
            gFilterSelect = 3;
            gBQRFilter.setParams(SoLoud::BiquadResonantFilter::BANDPASS, 44100, 1000, 2);
            gBus.setFilter(2, &gBQRFilter);
        }
        ImGui::SliderFloat("Wet##4", &filter_param0[2], 0, 1);
        filter_param1[2] = pSoloud->getFilterParameter(bushandle, 2, 2);
        if (ImGui::SliderFloat("Frequency##4", &filter_param1[2], 0, 8000))
        {
            pSoloud->setFilterParameter(bushandle, 2, 2, filter_param1[2]);
        }
        if (ImGui::SliderFloat("Resonance##4", &filter_param2[2], 1, 20))
        {
            pSoloud->setFilterParameter(bushandle, 2, 3, filter_param2[2]);
        }
        if (ImGui::Button("Oscillate +/- 1kHz"))
        {
            float from = filter_param1[2] - 500;
            if (from < 0) from = 0;
            pSoloud->oscillateFilterParameter(bushandle, 2, 2, from, from + 1000, 1);
        }
        ImGui::TreePop();
    }
#   endif
#   ifdef SOLOUD_LOFIFILTER_H
    if (ImGui::TreeNode("Lofi filter"))
    {
        ImGui::SliderFloat("Wet##2", &filter_param0[0], 0, 1);
        ImGui::SliderFloat("Rate##2", &filter_param1[0], 1000, 8000);
        ImGui::SliderFloat("Bit depth##2", &filter_param2[0], 0, 8);
        ImGui::TreePop();
    }
#   endif
#   ifdef SOLOUD_ECHOFILTER_H
    if (ImGui::TreeNode("Echo filter"))
    {
        ImGui::SliderFloat("Wet##3", &filter_param0[1], 0, 1);
        ImGui::TreePop();
    }
#   endif
#   ifdef SOLOUD_DCREMOVAL_H
    if (ImGui::TreeNode("DC Removal filter"))
    {
        ImGui::SliderFloat("Wet##1", &filter_param0[3], 0, 1);
        ImGui::TreePop();
    }
#   endif
    ImGui::PopID();
}

void BasicPiano::plonk(float rel, float vol) {
    int i = 0;
    while (gPlonked[i].mHandle != 0 && i < 128) i++;
    if (i == 128) return;
    vol = (vol + 10) / (float)(0x7f + 10);
    vol *= vol;
    int handle = gBus.play(gWave, 0);
    pSoloud->fadeVolume(handle, vol, gAttack);
    pSoloud->setRelativePlaySpeed(handle, 2 * rel);
    gPlonked[i].mHandle = handle;
    gPlonked[i].mRel = rel;
}
void BasicPiano::unplonk(float rel) {
    int i = 0;
    while (gPlonked[i].mRel != rel &&i < 128) i++;
    if (i == 128) return;
    pSoloud->fadeVolume(gPlonked[i].mHandle, 0, gRelease);
    pSoloud->scheduleStop(gPlonked[i].mHandle, gRelease);
    gPlonked[i].mHandle = 0;
}
void BasicPiano::replonk(float vol)  {
    int i = 0;
    while (gPlonked[i].mHandle != 0 && i < 128) i++;
    if (i == 128) return;
    vol = (vol + 10) / (float)(0x7f + 10);
    vol *= vol;
    for (i = 0; i < 128; i++)   {
        if (gPlonked[i].mHandle != 0)   {
            pSoloud->fadeVolume(gPlonked[i].mHandle, vol, 0.1);
        }
    }
}


#endif //NO_IMGUISOLOUD_BASICWAVE

} // namespace ImGuiSoloud
#endif //NO_IMGUISOLOUD_METHODS


