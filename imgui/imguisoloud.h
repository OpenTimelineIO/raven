/*
SoLoud audio engine [http://sol.gfxile.net/soloud/index.html]
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
#ifndef SOLOUD_H
#define SOLOUD_H

// SoLoud audio engine [http://sol.gfxile.net/soloud/index.html]
// AMALGAMATION FILES for SoLoud: {imguisoloud.h, imguisoloud.cpp, stb_vorbis.c, imguimodplug.cpp.inl}.
// CHANGES:
// ->	now stb_vorbis.c is no more customized (SoLoud used a customized "wrapped" version).
// ->	SoLoud::Wav and SoLoud::WavStream are enabled by default.
// ->	Other AudioSources need to be defined like this: YES_IMGUISOLOUD_SPEECH, YES_IMGUISOLOUD_MONOTONE, etc (at the top of this file or at the project level).
//	YES_IMGUISOLOUD_MODPLUG is the same as WITH_MODPLUG: when defined, imguisoloud.cpp should automatically include imguimodplug.cpp.inl (= libmodplug amalgamated).
//	
//  How to play .mid files with Modplug (from the original SoLoud docs):
// 	The midi formats (.mid and .abc) require a library of instruments (patches) to be available.
// 	One free set can be downloaded from the SoLoud downloads page. By default, the patches are
// 	loaded from pat/ directory.
//
// DEMOS:
// ->	the code of the demos has not been integrated. Without it this is a generic SoLoud amalgamated distribution that does not depend on ImGui.
//	main problem with adding the demo code is that we should embed audio files inside imguisoloud.cpp, and there might be some size/copyright issues.
// POSSIBLE ENHANCEMENTS (that probably won't be done):
// -> Remove STL.
// -> Move new/delete calls to ImGui::Allocate/Free => I think this is a bit painful (mainly because of stb_vorbis.c and the huge imguimodplug.cpp.inl, but I'm not sure about it).
// -> Fix some memory leak or Valgrind issue.
// TESTED: on Ubuntu 64 only with WITH_SDL2_STATIC (and not all the audio source types).

// TIP: main.cpp has some code that uses imguisoloud, if you define YES_IMGUISOLOUD at the project level.

// BUGS: there's a memory leak on Linux when using pthreads (so that WITH_SDL2 is good, but WITH_OPENAL leaks).

/*
// If no backend is defined, use WITH_SDL2_STATIC (however when used inside imguiaddons this is overridden. See addons/imgui_user.h).
#if (!defined(WITH_SDL) && !defined(WITH_SDL_STATIC) && !defined(WITH_SDL2) && !defined(WITH_SDL2_STATIC) && !defined(WITH_PORTAUDIO)  && !defined(WITH_OPENAL) && !defined(WITH_XAUDIO2) && !defined(WITH_WINMM)  && !defined(WITH_WASAPI) && !defined(WITH_OSS) && !defined(WITH_ALSA) && !defined(WITH_NULLDRIVER))
#	define WITH_SDL2_STATIC
#endif //WITH_SDL2_STATIC
// Removed: don't know what happens if nothing is defined. Anyway I already define stuff in addons/imgui_user.h, so it's not my interest to do it here...
*/


#ifdef YES_IMGUISOLOUD_ALL
#   ifndef NO_IMGUISOLOUD_SPEECH
#       define YES_IMGUISOLOUD_SPEECH
#   endif //NO_IMGUISOLOUD_SPEECH
#   ifndef NO_IMGUISOLOUD_MODPLUG
#       define YES_IMGUISOLOUD_MODPLUG
#   endif //NO_IMGUISOLOUD_MODPLUG
#   ifndef NO_IMGUISOLOUD_MONOTONE
#       define YES_IMGUISOLOUD_MONOTONE
#   endif //NO_IMGUISOLOUD_MONOTONE
#   ifndef NO_IMGUISOLOUD_SFXR
#       define YES_IMGUISOLOUD_SFXR
#   endif //NO_IMGUISOLOUD_SFXR
#   ifndef NO_IMGUISOLOUD_TEDSID
#       define YES_IMGUISOLOUD_TEDSID
#   endif //NO_IMGUISOLOUD_TEDSID
#   ifndef NO_IMGUISOLOUD_VIC
#   define YES_IMGUISOLOUD_VIC
#   endif //NO_IMGUISOLOUD_VIC
#endif //YES_IMGUISOLOUD_ALL

#ifdef WITH_MODPLUG
#   undef YES_IMGUISOLOUD_MODPLUG
#   define YES_IMGUISOLOUD_MODPLUG
#endif //WITH_MODPLUG
#ifdef YES_IMGUISOLOUD_MODPLUG
#   undef WITH_MODPLUG
#   define WITH_MODPLUG		    
#endif //YES_IMGUISOLOUD_MODPLUG 

//----soloud.h-----------------------------------------------------------------------------------------------
//#include <stdlib.h> // rand
//#include <math.h> // sin

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#ifdef _MSC_VER
#define WINDOWS_VERSION
#endif

#if !defined(DISABLE_SIMD)
#if defined(__x86_64__) || defined( _M_X64 ) || defined( __i386 ) || defined( _M_IX86 )
#define SOLOUD_SSE_INTRINSICS
#endif
#endif

#define SOLOUD_VERSION 111

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Configuration defines

// Maximum number of filters per stream
#define FILTERS_PER_STREAM 8

// Number of samples to process on one go
#define SAMPLE_GRANULARITY 512

// Maximum number of concurrent voices (hard limit is 4095)
#define VOICE_COUNT 1024

// Use linear resampler
#define RESAMPLER_LINEAR

// 1)mono, 2)stereo 4)quad 6)5.1
#define MAX_CHANNELS 6

//
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

// Typedefs have to be made before the includes, as the 
// includes depend on them.
namespace SoLoud
{
	class Soloud;
	typedef void (*mutexCallFunction)(void *aMutexPtr);
	typedef void (*soloudCallFunction)(Soloud *aSoloud);
	typedef unsigned int result;
	typedef unsigned int handle;
	typedef double time;
};

namespace SoLoud
{
	// Class that handles aligned allocations to support vectorized operations
	class AlignedFloatBuffer
	{
	public:
		float *mData; // aligned pointer
		unsigned char *mBasePtr; // raw allocated pointer (for delete)

		// ctor
		AlignedFloatBuffer();
		// Allocate and align buffer
		result init(unsigned int aFloats);
		// dtor
		~AlignedFloatBuffer();
	};
};

//----soloud_filter.h-----------------------------------------------------------------------------------------
#define SOLOUD_FILTER_H
namespace SoLoud
{
	class Fader;

	class FilterInstance
	{
	public:
		unsigned int mNumParams;
		unsigned int mParamChanged;
		float *mParam;
		Fader *mParamFader;
		

		FilterInstance();
		virtual result initParams(int aNumParams);
		virtual void updateParams(time aTime);
		virtual void filter(float *aBuffer, unsigned int aSamples, unsigned int aChannels, float aSamplerate, time aTime);
		virtual void filterChannel(float *aBuffer, unsigned int aSamples, float aSamplerate, time aTime, unsigned int aChannel, unsigned int aChannels);
		virtual float getFilterParameter(unsigned int aAttributeId);
		virtual void setFilterParameter(unsigned int aAttributeId, float aValue);
		virtual void fadeFilterParameter(unsigned int aAttributeId, float aTo, time aTime, time aStartTime);
		virtual void oscillateFilterParameter(unsigned int aAttributeId, float aFrom, float aTo, time aTime, time aStartTime);
		virtual ~FilterInstance();
	};

	class Filter
	{
	public:
		Filter();
		virtual FilterInstance *createInstance() = 0;
		virtual ~Filter();
	};
};
//----soloud_fader.h------------------------------------------------------------------------------------------
#define SOLOUD_FADER_H
namespace SoLoud
{
	// Helper class to process faders
	class Fader
	{
	public:
		// Value to fade from
		float mFrom;
		// Value to fade to
		float mTo;
		// Delta between from and to
		float mDelta;
		// Total time to fade
		time mTime;
		// Time fading started
		time mStartTime;
		// Time fading will end
		time mEndTime;
		// Current value. Used in case time rolls over.
		float mCurrent;
		// Active flag; 0 means disabled, 1 is active, 2 is LFO, -1 means was active, but stopped
		int mActive;
		// Ctor
		Fader();
		// Set up LFO
		void setLFO(float aFrom, float aTo, time aTime, time aStartTime);
		// Set up fader
		void set(float aFrom, float aTo, time aTime, time aStartTime);
		// Get the current fading value
		float get(time aCurrentTime);
	}; 
};
//----soloud_audiosource.h------------------------------------------------------------------------------------
#define SOLOUD_AUDIOSOURCE_H
namespace SoLoud
{
	class AudioSource;	
	class AudioSourceInstance;
	class AudioSourceInstance3dData;

	struct AudioSourceResampleData
	{
		AudioSourceResampleData();
		~AudioSourceResampleData();
		float *mBuffer;
	};

	class AudioCollider
	{
	public:
		// Calculate volume multiplier. Assumed to return value between 0 and 1.
		virtual float collide(Soloud *aSoloud, AudioSourceInstance3dData *aAudioInstance3dData,	int aUserData) = 0;
	};

	class AudioAttenuator
	{
	public:
		virtual float attenuate(float aDistance, float aMinDistance, float aMaxDistance, float aRolloffFactor) = 0;
	};

	class AudioSourceInstance3dData
	{
	public:
		// ctor
		AudioSourceInstance3dData();
		// Set settings from audiosource
		void init(AudioSource &aSource);
		// 3d position
		float m3dPosition[3];
		// 3d velocity
		float m3dVelocity[3];
		// 3d cone direction
		/*
		float m3dConeDirection[3];
		// 3d cone inner angle
		float m3dConeInnerAngle;
		// 3d cone outer angle
		float m3dConeOuterAngle;
		// 3d cone outer volume multiplier
		float m3dConeOuterVolume;
		*/
		// 3d min distance
		float m3dMinDistance;
		// 3d max distance
		float m3dMaxDistance;
		// 3d attenuation rolloff factor
		float m3dAttenuationRolloff;
		// 3d attenuation model
		unsigned int m3dAttenuationModel;
		// 3d doppler factor
		float m3dDopplerFactor;
		// Pointer to a custom audio collider object
		AudioCollider *mCollider;
		// Pointer to a custom audio attenuator object
		AudioAttenuator *mAttenuator;
		// User data related to audio collider
		int mColliderData;

		// Doppler sample rate multiplier
		float mDopplerValue;		
		// Overall 3d volume
		float m3dVolume;
		// Channel volume
		float mChannelVolume[MAX_CHANNELS];
		// Copy of flags
		unsigned int mFlags;
		// Latest handle for this voice
		handle mHandle;
	};

	// Base class for audio instances
	class AudioSourceInstance
	{
	public:
		enum FLAGS
		{			
			// This audio instance loops (if supported)
			LOOPING = 1,
			// This audio instance is protected - won't get stopped if we run out of voices
			PROTECTED = 2,
			// This audio instance is paused
			PAUSED = 4,
			// This audio instance is affected by 3d processing
			PROCESS_3D = 8,
			// This audio instance has listener-relative 3d coordinates
			LISTENER_RELATIVE = 16,
			// Currently inaudible
			INAUDIBLE = 32,
			// If inaudible, should be killed (default = don't kill kill)
			INAUDIBLE_KILL = 64,
			// If inaudible, should still be ticked (default = pause)
			INAUDIBLE_TICK = 128
		};
		// Ctor
		AudioSourceInstance();
		// Dtor
		virtual ~AudioSourceInstance();
		// Play index; used to identify instances from handles
		unsigned int mPlayIndex;
		// Loop count
		unsigned int mLoopCount;
		// Flags; see AudioSourceInstance::FLAGS
		unsigned int mFlags;
		// Pan value, for getPan()
		float mPan;
		// Volume for each channel (panning)
		float mChannelVolume[MAX_CHANNELS];
		// Set volume
		float mSetVolume;
		// Overall volume overall = set * 3d
		float mOverallVolume;
		// Base samplerate; samplerate = base samplerate * relative play speed
		float mBaseSamplerate;
		// Samplerate; samplerate = base samplerate * relative play speed
		float mSamplerate;
		// Number of channels this audio source produces
		unsigned int mChannels;
		// Relative play speed; samplerate = base samplerate * relative play speed
		float mSetRelativePlaySpeed;
		// Overall relative plays peed; overall = set * 3d
		float mOverallRelativePlaySpeed;
		// How long this stream has played, in seconds.
		time mStreamTime;
		// Fader for the audio panning
		Fader mPanFader;
		// Fader for the audio volume
		Fader mVolumeFader;
		// Fader for the relative play speed
		Fader mRelativePlaySpeedFader;
		// Fader used to schedule pausing of the stream
		Fader mPauseScheduler;
		// Fader used to schedule stopping of the stream
		Fader mStopScheduler;
		// Affected by some fader
		int mActiveFader;
		// Current channel volumes, used to ramp the volume changes to avoid clicks
		float mCurrentChannelVolume[MAX_CHANNELS];
		// ID of the sound source that generated this instance
		unsigned int mAudioSourceID;
		// Handle of the bus this audio instance is playing on. 0 for root.
		unsigned int mBusHandle;
		// Filter pointer
		FilterInstance *mFilter[FILTERS_PER_STREAM];
		// Initialize instance. Mostly internal use.
		void init(AudioSource &aSource, int aPlayIndex);
		// Buffers for the resampler
		AudioSourceResampleData *mResampleData[2];
		// Sub-sample playhead; 16.16 fixed point
		unsigned int mSrcOffset;
		// Samples left over from earlier pass
		unsigned int mLeftoverSamples;
		// Number of samples to delay streaming
		unsigned int mDelaySamples;

		// Get N samples from the stream to the buffer
		virtual void getAudio(float *aBuffer, unsigned int aSamples) = 0;
		// Has the stream ended?
		virtual bool hasEnded() = 0;
		// Seek to certain place in the stream. Base implementation is generic "tape" seek (and slow).
		virtual void seek(time aSeconds, float *mScratch, unsigned int mScratchSize);
		// Rewind stream. Base implementation returns NOT_IMPLEMENTED, meaning it can't rewind.
		virtual result rewind();
		// Get information. Returns 0 by default.
		virtual float getInfo(unsigned int aInfoKey);
	};

	class Soloud;

	// Base class for audio sources
	class AudioSource
	{
	public:
		enum FLAGS
		{
			// The instances from this audio source should loop
			SHOULD_LOOP = 1,
			// Only one instance of this audio source should play at the same time
			SINGLE_INSTANCE = 2,
			// Visualization data gathering enabled. Only for busses.
			VISUALIZATION_DATA = 4,
			// Audio instances created from this source are affected by 3d processing
			PROCESS_3D = 8,
			// Audio instances created from this source have listener-relative 3d coordinates
			LISTENER_RELATIVE = 16,
			// Delay start of sound by the distance from listener
			DISTANCE_DELAY = 32,
			// If inaudible, should be killed (default)
			INAUDIBLE_KILL = 64,
			// If inaudible, should still be ticked (default = pause)
			INAUDIBLE_TICK = 128
		};
		enum ATTENUATION_MODELS
		{
			// No attenuation
			NO_ATTENUATION = 0,
			// Inverse distance attenuation model
			INVERSE_DISTANCE = 1,
			// Linear distance attenuation model
			LINEAR_DISTANCE = 2,
			// Exponential distance attenuation model
			EXPONENTIAL_DISTANCE = 3
		};

		// Flags. See AudioSource::FLAGS
		unsigned int mFlags;
		// Base sample rate, used to initialize instances
		float mBaseSamplerate;
		// Default volume for created instances
		float mVolume;
		// Number of channels this audio source produces
		unsigned int mChannels;
		// Sound source ID. Assigned by SoLoud the first time it's played.
		unsigned int mAudioSourceID;
		// 3d min distance
		float m3dMinDistance;
		// 3d max distance
		float m3dMaxDistance;
		// 3d attenuation rolloff factor
		float m3dAttenuationRolloff;
		// 3d attenuation model
		unsigned int m3dAttenuationModel;
		// 3d doppler factor
		float m3dDopplerFactor;
		// Filter pointer
		Filter *mFilter[FILTERS_PER_STREAM];
		// Pointer to the Soloud object. Needed to stop all instances in dtor.
		Soloud *mSoloud;
		// Pointer to a custom audio collider object
		AudioCollider *mCollider;
		// Pointer to custom attenuator object
		AudioAttenuator *mAttenuator;
		// User data related to audio collider
		int mColliderData;

		// CTor
		AudioSource();
		// Set default volume for instances
		void setVolume(float aVolume);
		// Set the looping of the instances created from this audio source
		void setLooping(bool aLoop);
		// Set whether only one instance of this sound should ever be playing at the same time
		void setSingleInstance(bool aSingleInstance);
		
		// Set the minimum and maximum distances for 3d audio source (closer to min distance = max vol)
		void set3dMinMaxDistance(float aMinDistance, float aMaxDistance);
		// Set attenuation model and rolloff factor for 3d audio source
		void set3dAttenuation(unsigned int aAttenuationModel, float aAttenuationRolloffFactor);
		// Set doppler factor to reduce or enhance doppler effect, default = 1.0
		void set3dDopplerFactor(float aDopplerFactor);
		// Enable 3d processing. Implicitly set by play3d calls.
		void set3dProcessing(bool aDo3dProcessing);
		// Set the coordinates for this audio source to be relative to listener's coordinates.
		void set3dListenerRelative(bool aListenerRelative);
		// Enable delaying the start of the sound based on the distance.
		void set3dDistanceDelay(bool aDistanceDelay);

		// Set a custom 3d audio collider. Set to NULL to disable.
		void set3dCollider(AudioCollider *aCollider, int aUserData = 0);
		// Set a custom attenuator. Set to NULL to disable.
		void set3dAttenuator(AudioAttenuator *aAttenuator);

		// Set behavior for inaudible sounds
		void setInaudibleBehavior(bool aMustTick, bool aKill);

		// Set filter. Set to NULL to clear the filter.
		virtual void setFilter(unsigned int aFilterId, Filter *aFilter);
		// DTor
		virtual ~AudioSource();
		// Create instance from the audio source. Called from within Soloud class.
		virtual AudioSourceInstance *createInstance() = 0;
		// Stop all instances of this audio source
		void stop();
	};
};
//----soloud_bus.h--------------------------------------------------------------------------------------------
#define SOLOUD_BUS_H
namespace SoLoud
{
	class Bus;

	class BusInstance : public AudioSourceInstance
	{
		Bus *mParent;
		unsigned int mScratchSize;
		AlignedFloatBuffer mScratch;
	public:
		// Mono-mixed wave data for visualization and for visualization FFT input
		float mVisualizationWaveData[256];

		BusInstance(Bus *aParent);
		virtual void getAudio(float *aBuffer, unsigned int aSamples);
		virtual bool hasEnded();
		virtual ~BusInstance();
	};

	class Bus : public AudioSource
	{
	public:
		Bus();
		virtual BusInstance *createInstance();
		// Set filter. Set to NULL to clear the filter.
		virtual void setFilter(unsigned int aFilterId, Filter *aFilter);
		// Play sound through the bus
		handle play(AudioSource &aSound, float aVolume = 1.0f, float aPan = 0.0f, bool aPaused = 0);
		// Play sound through the bus, delayed in relation to other sounds called via this function.
		handle playClocked(time aSoundTime, AudioSource &aSound, float aVolume = 1.0f, float aPan = 0.0f);
		// Start playing a 3d audio source through the bus
		handle play3d(AudioSource &aSound, float aPosX, float aPosY, float aPosZ, float aVelX = 0.0f, float aVelY = 0.0f, float aVelZ = 0.0f, float aVolume = 1.0f, bool aPaused = 0);
		// Start playing a 3d audio source through the bus, delayed in relation to other sounds called via this function.
		handle play3dClocked(time aSoundTime, AudioSource &aSound, float aPosX, float aPosY, float aPosZ, float aVelX = 0.0f, float aVelY = 0.0f, float aVelZ = 0.0f, float aVolume = 1.0f);
		// Set number of channels for the bus (default 2)
		result setChannels(unsigned int aChannels);
		// Enable or disable visualization data gathering
		void setVisualizationEnable(bool aEnable);
		
		// Calculate and get 256 floats of FFT data for visualization. Visualization has to be enabled before use.
		float *calcFFT();

		// Get 256 floats of wave data for visualization. Visualization has to be enabled before use.
		float *getWave();
	public:
		BusInstance *mInstance;
		unsigned int mChannelHandle;
		// FFT output data
		float mFFTData[256];
		// Snapshot of wave data for visualization
		float mWaveData[256];
		// Internal: find the bus' channel
		void findBusHandle();
	};
};
//----soloud_error.h------------------------------------------------------------------------------------------
#define SOLOUD_ERROR_H
namespace SoLoud
{
	enum SOLOUD_ERRORS
	{
		SO_NO_ERROR       = 0, // No error
		INVALID_PARAMETER = 1, // Some parameter is invalid
		FILE_NOT_FOUND    = 2, // File not found
		FILE_LOAD_FAILED  = 3, // File found, but could not be loaded
		DLL_NOT_FOUND     = 4, // DLL not found, or wrong DLL
		OUT_OF_MEMORY     = 5, // Out of memory
		NOT_IMPLEMENTED   = 6, // Feature not implemented
		UNKNOWN_ERROR     = 7  // Other error
	};
};

//----soloud.h-[2]--------------------------------------------------------------------------------------------
namespace SoLoud
{

	// Soloud core class.
	class Soloud
	{
	public:
		// Back-end data; content is up to the back-end implementation.
		void * mBackendData;
		// Pointer for the audio thread mutex.
		void * mAudioThreadMutex;
		// Called by SoLoud to shut down the back-end. If NULL, not called. Should be set by back-end.
		soloudCallFunction mBackendCleanupFunc;

		// CTor
		Soloud();
		// DTor
		~Soloud();

		enum BACKENDS
		{
			AUTO = 0,
			SDL,
			SDL2,
			PORTAUDIO,
			WINMM,
			XAUDIO2,
			WASAPI,
			ALSA,
			OSS,
			OPENAL,
			COREAUDIO,
			OPENSLES,
			NULLDRIVER,
			BACKEND_MAX,
		};

		enum FLAGS
		{
			// Use round-off clipper
			CLIP_ROUNDOFF = 1,
			ENABLE_VISUALIZATION = 2,
			LEFT_HANDED_3D = 4
		};

		// Initialize SoLoud. Must be called before SoLoud can be used.
		result init(unsigned int aFlags = Soloud::CLIP_ROUNDOFF, unsigned int aBackend = Soloud::AUTO, unsigned int aSamplerate = Soloud::AUTO, unsigned int aBufferSize = Soloud::AUTO, unsigned int aChannels = 2);

		// Deinitialize SoLoud. Must be called before shutting down.
		void deinit();

		// Query SoLoud version number (should equal to SOLOUD_VERSION macro)
		unsigned int getVersion() const;

		// Translate error number to an asciiz string
		const char * getErrorString(result aErrorCode) const;

		// Returns current backend ID (BACKENDS enum)
		unsigned int getBackendId();
		// Returns current backend string. May be NULL.
		const char * getBackendString();
		// Returns current backend channel count (1 mono, 2 stereo, etc)
		unsigned int getBackendChannels();
		// Returns current backend sample rate
		unsigned int getBackendSamplerate();
		// Returns current backend buffer size
		unsigned int getBackendBufferSize();

		// Set speaker position in 3d space
		result setSpeakerPosition(unsigned int aChannel, float aX, float aY, float aZ);
		
		// Start playing a sound. Returns voice handle, which can be ignored or used to alter the playing sound's parameters. Negative volume means to use default.
		handle play(AudioSource &aSound, float aVolume = -1.0f, float aPan = 0.0f, bool aPaused = 0, unsigned int aBus = 0);
		// Start playing a sound delayed in relation to other sounds called via this function. Negative volume means to use default.
		handle playClocked(time aSoundTime, AudioSource &aSound, float aVolume = -1.0f, float aPan = 0.0f, unsigned int aBus = 0);
		// Start playing a 3d audio source
		handle play3d(AudioSource &aSound, float aPosX, float aPosY, float aPosZ, float aVelX = 0.0f, float aVelY = 0.0f, float aVelZ = 0.0f, float aVolume = 1.0f, bool aPaused = 0, unsigned int aBus = 0);
		// Start playing a 3d audio source, delayed in relation to other sounds called via this function.
		handle play3dClocked(time aSoundTime, AudioSource &aSound, float aPosX, float aPosY, float aPosZ, float aVelX = 0.0f, float aVelY = 0.0f, float aVelZ = 0.0f, float aVolume = 1.0f, unsigned int aBus = 0);
        // Start playing a sound without any panning. It will be played at full volume.
        handle playBackground(AudioSource &aSound, float aVolume = -1.0f, bool aPaused = 0, unsigned int aBus = 0);


		// Seek the audio stream to certain point in time. Some streams can't seek backwards. Relative play speed affects time.
		void seek(handle aVoiceHandle, time aSeconds);
		// Stop the sound.
		void stop(handle aVoiceHandle);
		// Stop all voices.
		void stopAll();
		// Stop all voices that play this sound source
		void stopAudioSource(AudioSource &aSound);

		// Set a live filter parameter. Use 0 for the global filters.
		void setFilterParameter(handle aVoiceHandle, unsigned int aFilterId, unsigned int aAttributeId, float aValue);
		// Get a live filter parameter. Use 0 for the global filters.
		float getFilterParameter(handle aVoiceHandle, unsigned int aFilterId, unsigned int aAttributeId);
		// Fade a live filter parameter. Use 0 for the global filters.
		void fadeFilterParameter(handle aVoiceHandle, unsigned int aFilterId, unsigned int aAttributeId, float aTo, time aTime);
		// Oscillate a live filter parameter. Use 0 for the global filters.
		void oscillateFilterParameter(handle aVoiceHandle, unsigned int aFilterId, unsigned int aAttributeId, float aFrom, float aTo, time aTime);

		// Get current play time, in seconds.
		time getStreamTime(handle aVoiceHandle);
		// Get current pause state.
		bool getPause(handle aVoiceHandle);
		// Get current volume.
		float getVolume(handle aVoiceHandle);
		// Get current overall volume (set volume * 3d volume)
		float getOverallVolume(handle aVoiceHandle);
		// Get current pan.
		float getPan(handle aVoiceHandle);
		// Get current sample rate.
		float getSamplerate(handle aVoiceHandle);
		// Get current voice protection state.
		bool getProtectVoice(handle aVoiceHandle);
		// Get the current number of busy voices.
		unsigned int getActiveVoiceCount(); 
		// Get the current number of voices in SoLoud
		unsigned int getVoiceCount();
		// Check if the handle is still valid, or if the sound has stopped.
		bool isValidVoiceHandle(handle aVoiceHandle);
		// Get current relative play speed.
		float getRelativePlaySpeed(handle aVoiceHandle);
		// Get current post-clip scaler value.
		float getPostClipScaler() const;
		// Get current global volume
		float getGlobalVolume() const;
		// Get current maximum active voice setting
		unsigned int getMaxActiveVoiceCount() const;
		// Query whether a voice is set to loop.
		bool getLooping(handle aVoiceHandle);
		
		// Set voice's loop state
		void setLooping(handle aVoiceHandle, bool aLooping);
		// Set current maximum active voice setting
		result setMaxActiveVoiceCount(unsigned int aVoiceCount);
		// Set behavior for inaudible sounds
		void setInaudibleBehavior(handle aVoiceHandle, bool aMustTick, bool aKill);
		// Set the global volume
		void setGlobalVolume(float aVolume);
		// Set the post clip scaler value
		void setPostClipScaler(float aScaler);
		// Set the pause state
		void setPause(handle aVoiceHandle, bool aPause);
		// Pause all voices
		void setPauseAll(bool aPause);
		// Set the relative play speed
		result setRelativePlaySpeed(handle aVoiceHandle, float aSpeed);
		// Set the voice protection state
		void setProtectVoice(handle aVoiceHandle, bool aProtect);
		// Set the sample rate
		void setSamplerate(handle aVoiceHandle, float aSamplerate);
		// Set panning value; -1 is left, 0 is center, 1 is right
		void setPan(handle aVoiceHandle, float aPan);
		// Set absolute left/right volumes
		void setPanAbsolute(handle aVoiceHandle, float aLVolume, float aRVolume, float aLBVolume = 0, float aRBVolume = 0, float aCVolume = 0, float aSVolume = 0);
		// Set overall volume
		void setVolume(handle aVoiceHandle, float aVolume);
		// Set delay, in samples, before starting to play samples. Calling this on a live sound will cause glitches.
		void setDelaySamples(handle aVoiceHandle, unsigned int aSamples);

		// Set up volume fader
		void fadeVolume(handle aVoiceHandle, float aTo, time aTime);
		// Set up panning fader
		void fadePan(handle aVoiceHandle, float aTo, time aTime);
		// Set up relative play speed fader
		void fadeRelativePlaySpeed(handle aVoiceHandle, float aTo, time aTime);
		// Set up global volume fader
		void fadeGlobalVolume(float aTo, time aTime);
		// Schedule a stream to pause
		void schedulePause(handle aVoiceHandle, time aTime);
		// Schedule a stream to stop
		void scheduleStop(handle aVoiceHandle, time aTime);

		// Set up volume oscillator
		void oscillateVolume(handle aVoiceHandle, float aFrom, float aTo, time aTime);
		// Set up panning oscillator
		void oscillatePan(handle aVoiceHandle, float aFrom, float aTo, time aTime);
		// Set up relative play speed oscillator
		void oscillateRelativePlaySpeed(handle aVoiceHandle, float aFrom, float aTo, time aTime);
		// Set up global volume oscillator
		void oscillateGlobalVolume(float aFrom, float aTo, time aTime);

		// Set global filters. Set to NULL to clear the filter.
		void setGlobalFilter(unsigned int aFilterId, Filter *aFilter);

		// Enable or disable visualization data gathering
		void setVisualizationEnable(bool aEnable);
		
		// Calculate and get 256 floats of FFT data for visualization. Visualization has to be enabled before use.
		float *calcFFT();

		// Get 256 floats of wave data for visualization. Visualization has to be enabled before use.
		float *getWave();

		// Get current loop count. Returns 0 if handle is not valid. (All audio sources may not update loop count)
		unsigned int getLoopCount(handle aVoiceHandle);
		
		// Get audiosource-specific information from a voice. 
		float getInfo(handle aVoiceHandle, unsigned int aInfoKey);

		// Create a voice group. Returns 0 if unable (out of voice groups / out of memory)
		handle createVoiceGroup();
		// Destroy a voice group. 
		result destroyVoiceGroup(handle aVoiceGroupHandle);
		// Add a voice handle to a voice group
		result addVoiceToGroup(handle aVoiceGroupHandle, handle aVoiceHandle);
		// Is this handle a valid voice group?
		bool isVoiceGroup(handle aVoiceGroupHandle);
		// Is this voice group empty?
		bool isVoiceGroupEmpty(handle aVoiceGroupHandle);

		// Perform 3d audio parameter update
		void update3dAudio();
		
		// Set the speed of sound constant for doppler
		result set3dSoundSpeed(float aSpeed);
		// Get the current speed of sound constant for doppler
		float get3dSoundSpeed();
		// Set 3d listener parameters
		void set3dListenerParameters(float aPosX, float aPosY, float aPosZ, float aAtX, float aAtY, float aAtZ, float aUpX, float aUpY, float aUpZ, float aVelocityX = 0.0f, float aVelocityY = 0.0f, float aVelocityZ = 0.0f);
		// Set 3d listener position
		void set3dListenerPosition(float aPosX, float aPosY, float aPosZ);
		// Set 3d listener "at" vector
		void set3dListenerAt(float aAtX, float aAtY, float aAtZ);
		// set 3d listener "up" vector
		void set3dListenerUp(float aUpX, float aUpY, float aUpZ);
		// Set 3d listener velocity
		void set3dListenerVelocity(float aVelocityX, float aVelocityY, float aVelocityZ);

		// Set 3d audio source parameters
		void set3dSourceParameters(handle aVoiceHandle, float aPosX, float aPosY, float aPosZ, float aVelocityX = 0.0f, float aVelocityY = 0.0f, float aVelocityZ = 0.0f);
		// Set 3d audio source position
		void set3dSourcePosition(handle aVoiceHandle, float aPosX, float aPosY, float aPosZ);
		// Set 3d audio source velocity
		void set3dSourceVelocity(handle aVoiceHandle, float aVelocityX, float aVelocityY, float aVelocityZ);
		// Set 3d audio source min/max distance (distance < min means max volume)
		void set3dSourceMinMaxDistance(handle aVoiceHandle, float aMinDistance, float aMaxDistance);
		// Set 3d audio source attenuation parameters
		void set3dSourceAttenuation(handle aVoiceHandle, unsigned int aAttenuationModel, float aAttenuationRolloffFactor);
		// Set 3d audio source doppler factor to reduce or enhance doppler effect. Default = 1.0
		void set3dSourceDopplerFactor(handle aVoiceHandle, float aDopplerFactor);

		// Rest of the stuff is used internally.

		// Returns mixed float samples in buffer. Called by the back-end, or user with null driver.
		void mix(float *aBuffer, unsigned int aSamples);
		// Returns mixed 16-bit signed integer samples in buffer. Called by the back-end, or user with null driver.
		void mixSigned16(short *aBuffer, unsigned int aSamples);
	public:
		// Mix N samples * M channels. Called by other mix_ functions. 
		void mix_internal(unsigned int aSamples);

		// Handle rest of initialization (called from backend)
		void postinit(unsigned int aSamplerate, unsigned int aBufferSize, unsigned int aFlags, unsigned int aChannels);

		// Update list of active voices
		void calcActiveVoices();
		// Perform mixing for a specific bus
		void mixBus(float *aBuffer, unsigned int aSamples, float *aScratch, unsigned int aBus, float aSamplerate, unsigned int aChannels);
		// Max. number of active voices. Busses and tickable inaudibles also count against this.
		unsigned int mMaxActiveVoices;
		// Highest voice in use so far
		unsigned int mHighestVoice;
		// Scratch buffer, used for resampling.
		AlignedFloatBuffer mScratch;
		// Current size of the scratch, in samples.
		unsigned int mScratchSize;
		// Amount of scratch needed.
		unsigned int mScratchNeeded;
		// Output scratch buffer, used in mix_().
		AlignedFloatBuffer mOutputScratch;
		// Audio voices.
		AudioSourceInstance *mVoice[VOICE_COUNT];
		// Output sample rate (not float)
		unsigned int mSamplerate;
		// Output channel count
		unsigned int mChannels;
		// Current backend ID
		unsigned int mBackendID;
		// Current backend string
		const char * mBackendString;
		// Maximum size of output buffer; used to calculate needed scratch.
		unsigned int mBufferSize;
		// Flags; see Soloud::FLAGS
		unsigned int mFlags;
		// Global volume. Applied before clipping.
		float mGlobalVolume;
		// Post-clip scaler. Applied after clipping.
		float mPostClipScaler;
		// Current play index. Used to create audio handles.
		unsigned int mPlayIndex;
		// Current sound source index. Used to create sound source IDs.
		unsigned int mAudioSourceID;
		// Fader for the global volume.
		Fader mGlobalVolumeFader;
		// Global stream time, for the global volume fader. 
		time mStreamTime;
		// Last time seen by the playClocked call
		time mLastClockedTime;
		// Global filter
		Filter *mFilter[FILTERS_PER_STREAM];
		// Global filter instance
		FilterInstance *mFilterInstance[FILTERS_PER_STREAM];
		// Find a free voice, stopping the oldest if no free voice is found.
		int findFreeVoice();
		// Converts handle to voice, if the handle is valid. Returns -1 if not.
		int getVoiceFromHandle(handle aVoiceHandle) const;
		// Converts voice + playindex into handle
		handle getHandleFromVoice(unsigned int aVoice) const;
		// Stop voice (not handle).
		void stopVoice(unsigned int aVoice);
		// Set voice (not handle) pan.
		void setVoicePan(unsigned int aVoice, float aPan);
		// Set voice (not handle) relative play speed.
		result setVoiceRelativePlaySpeed(unsigned int aVoice, float aSpeed);
		// Set voice (not handle) volume.
		void setVoiceVolume(unsigned int aVoice, float aVolume);
		// Set voice (not handle) pause state.
		void setVoicePause(unsigned int aVoice, int aPause);
		// Update overall volume from set and 3d volumes
		void updateVoiceVolume(unsigned int aVoice);
		// Update overall relative play speed from set and 3d speeds
		void updateVoiceRelativePlaySpeed(unsigned int aVoice);
		// Perform 3d audio calculation for array of voices
		void update3dVoices(unsigned int *aVoiceList, unsigned int aVoiceCount);
		// Clip the samples in the buffer
		void clip(AlignedFloatBuffer &aBuffer, AlignedFloatBuffer &aDestBuffer, unsigned int aSamples, float aVolume0, float aVolume1);
		// Mono-mixed wave data for visualization and for visualization FFT input
		float mVisualizationWaveData[256];
		// FFT output data
		float mFFTData[256];
		// Snapshot of wave data for visualization
		float mWaveData[256];

		// 3d listener position
		float m3dPosition[3];
		// 3d listener look-at
		float m3dAt[3];
		// 3d listener up
		float m3dUp[3];
		// 3d listener velocity
		float m3dVelocity[3];
		// 3d speed of sound (for doppler)
		float m3dSoundSpeed;

		// 3d position of speakers
		float m3dSpeakerPosition[3 * MAX_CHANNELS];

		// Data related to 3d processing, separate from AudioSource so we can do 3d calculations without audio mutex.
		AudioSourceInstance3dData m3dData[VOICE_COUNT];
		
		// For each voice group, first int is number of ints alocated.
		unsigned int **mVoiceGroup;
		unsigned int mVoiceGroupCount;

		// List of currently active voices
		unsigned int mActiveVoice[VOICE_COUNT];
		// Number of currently active voices
		unsigned int mActiveVoiceCount;
		// Active voices list needs to be recalculated
		bool mActiveVoiceDirty;

		// Remove all non-active voices from group
		void trimVoiceGroup(handle aVoiceGroupHandle);
		// Get pointer to the zero-terminated array of voice handles in a voice group
		handle * voiceGroupHandleToArray(handle aVoiceGroupHandle) const;

		// Lock audio thread mutex.
		void lockAudioMutex();
		// Unlock audio thread mutex.
		void unlockAudioMutex();
	};
};

//----soloud_fftfilter.h-----------------------------------------------------------------------------------------------
#define SOLOUD_FFTFILTER_H
namespace SoLoud
{
	class FFTFilter;

	class FFTFilterInstance : public FilterInstance
	{
		float *mTemp;
		float *mInputBuffer;
		float *mMixBuffer;
		unsigned int mOffset[MAX_CHANNELS];
		FFTFilter *mParent;
	public:
		virtual void fftFilterChannel(float *aFFTBuffer, unsigned int aSamples, float aSamplerate, time aTime, unsigned int aChannel, unsigned int aChannels);
		virtual void filterChannel(float *aBuffer, unsigned int aSamples, float aSamplerate, time aTime, unsigned int aChannel, unsigned int aChannels);
		virtual ~FFTFilterInstance();
		FFTFilterInstance(FFTFilter *aParent);
		FFTFilterInstance();
	};

	class FFTFilter : public Filter
	{
	public:
		virtual FilterInstance *createInstance();
		FFTFilter();
	};
}
//----soloud_bassboostfilter.h-----------------------------------------------------------------------------------------------
#define SOLOUD_BASSBOOSTFILTER_H
namespace SoLoud
{
	class BassboostFilter;

	class BassboostFilterInstance : public FFTFilterInstance
	{
		enum FILTERATTRIBUTE
		{
			WET = 0,
			BOOST = 1
		};
		BassboostFilter *mParent;
	public:
		virtual void fftFilterChannel(float *aFFTBuffer, unsigned int aSamples, float aSamplerate, time aTime, unsigned int aChannel, unsigned int aChannels);
		BassboostFilterInstance(BassboostFilter *aParent);
	};

	class BassboostFilter : public FFTFilter
	{
	public:
		enum FILTERATTRIBUTE
		{
			WET = 0,
			BOOST = 1
		};
		float mBoost;
		result setParams(float aBoost);
		virtual FilterInstance *createInstance();
		BassboostFilter();
	};
}
//----soloud_biquadresonantfilter.h-----------------------------------------------------------------------------------------------
#define SOLOUD_BQRFILTER_H
namespace SoLoud
{
	class BiquadResonantFilter;

	struct BQRStateData
	{
		float mY1, mY2, mX1, mX2;
	};

	class BiquadResonantFilterInstance : public FilterInstance
	{
		enum FILTERATTRIBUTE
		{
			WET = 0,
			SAMPLERATE = 1,
			FREQUENCY = 2,
			RESONANCE = 3
		};

		int mActive;
		BQRStateData mState[2];
		float mA0, mA1, mA2, mB1, mB2;
		int mDirty;
		int mFilterType;

		BiquadResonantFilter *mParent;
		void calcBQRParams();
	public:
		virtual void filterChannel(float *aBuffer, unsigned int aSamples, float aSamplerate, time aTime, unsigned int aChannel, unsigned int aChannels);
		virtual ~BiquadResonantFilterInstance();
		BiquadResonantFilterInstance(BiquadResonantFilter *aParent);
	};

	class BiquadResonantFilter : public Filter
	{
	public:
		enum FILTERTYPE
		{
			NONE = 0,
			LOWPASS = 1,
			HIGHPASS = 2,
			BANDPASS = 3
		};
		enum FILTERATTRIBUTE
		{
			WET = 0,
			SAMPLERATE = 1,
			FREQUENCY = 2,
			RESONANCE = 3
		};
		int mFilterType;
		float mSampleRate;
		float mFrequency;
		float mResonance;
		virtual BiquadResonantFilterInstance *createInstance();
		BiquadResonantFilter();
		result setParams(int aType, float aSampleRate, float aFrequency, float aResonance);
		virtual ~BiquadResonantFilter();
	};
}
//----soloud_dcremovalfilter.h-----------------------------------------------------------------------------------------------
#define SOLOUD_DCREMOVAL_H
namespace SoLoud
{
	class DCRemovalFilter;

	class DCRemovalFilterInstance : public FilterInstance
	{
		float *mBuffer;
		float *mTotals;
		int mBufferLength;
		DCRemovalFilter *mParent;
		int mOffset;

	public:
		virtual void filter(float *aBuffer, unsigned int aSamples, unsigned int aChannels, float aSamplerate, time aTime);
		virtual ~DCRemovalFilterInstance();
		DCRemovalFilterInstance(DCRemovalFilter *aParent);
	};

	class DCRemovalFilter : public Filter
	{
	public:
		float mLength;
		virtual FilterInstance *createInstance();
		DCRemovalFilter();
		result setParams(float aLength = 0.1f);
	};
}
//----soloud_echofilter.h-----------------------------------------------------------------------------------------------
#define SOLOUD_ECHOFILTER_H

namespace SoLoud
{
	class EchoFilter;

	class EchoFilterInstance : public FilterInstance
	{
		float *mBuffer;
		int mBufferLength;
		EchoFilter *mParent;
		int mOffset;

	public:
		virtual void filter(float *aBuffer, unsigned int aSamples, unsigned int aChannels, float aSamplerate, time aTime);
		virtual ~EchoFilterInstance();
		EchoFilterInstance(EchoFilter *aParent);
	};

	class EchoFilter : public Filter
	{
	public:
		float mDelay;
		float mDecay;
		float mFilter;
		virtual FilterInstance *createInstance();
		EchoFilter();
		result setParams(float aDelay, float aDecay = 0.7f, float aFilter = 0.0f);
	};
}
//----soloud_flangerfilter.h-----------------------------------------------------------------------------------------------
#define SOLOUD_FLANGERFILTER_H
namespace SoLoud
{
	class FlangerFilter;

	class FlangerFilterInstance : public FilterInstance
	{
		float *mBuffer;
		int mBufferLength;
		FlangerFilter *mParent;
		int mOffset;
		double mIndex;

	public:
		virtual void filter(float *aBuffer, unsigned int aSamples, unsigned int aChannels, float aSamplerate, time aTime);
		virtual ~FlangerFilterInstance();
		FlangerFilterInstance(FlangerFilter *aParent);
	};

	class FlangerFilter : public Filter
	{
	public:
		enum FILTERPARAMS
		{
			WET,
			DELAY,
			FREQ
		};
		float mDelay;
		float mFreq;
		virtual FilterInstance *createInstance();
		FlangerFilter();
		result setParams(float aDelay, float aFreq);
	};
}
//----soloud_lofifilter.h-----------------------------------------------------------------------------------------------
#define SOLOUD_LOFIFILTER_H
namespace SoLoud
{
	class LofiFilter;

	struct LofiChannelData
	{
		float mSample;
		float mSamplesToSkip;
	};

	class LofiFilterInstance : public FilterInstance
	{
		enum FILTERPARAMS
		{
			WET,
			SAMPLERATE,
			BITDEPTH
		};
		LofiChannelData mChannelData[2];
		
		LofiFilter *mParent;
	public:
		virtual void filterChannel(float *aBuffer, unsigned int aSamples, float aSamplerate, time aTime, unsigned int aChannel, unsigned int aChannels);
		virtual ~LofiFilterInstance();
		LofiFilterInstance(LofiFilter *aParent);
	};

	class LofiFilter : public Filter
	{
	public:
		enum FILTERPARAMS
		{
			WET,
			SAMPLERATE,
			BITDEPTH
		};
		float mSampleRate;
		float mBitdepth;
		virtual LofiFilterInstance *createInstance();
		LofiFilter();
		result setParams(float aSampleRate, float aBitdepth);
		virtual ~LofiFilter();
	};
}
//----soloud_file.h-----------------------------------------------------------------------------------------------
#define SOLOUD_FILE_H
typedef void* Soloud_Filehack;
namespace SoLoud
{
	class File
	{
	public:
		virtual ~File() {}
		unsigned int read8();
		unsigned int read16();
		unsigned int read32();
		virtual int eof() = 0;
		virtual unsigned int read(unsigned char *aDst, unsigned int aBytes) = 0;
		virtual unsigned int length() = 0;
		virtual void seek(int aOffset) = 0;
		virtual unsigned int pos() = 0;
		virtual FILE * getFilePtr() { return 0; }
		virtual unsigned char * getMemPtr() { return 0; }
	};

	class DiskFile : public File
	{
	public:
		FILE *mFileHandle;

		virtual int eof();
		virtual unsigned int read(unsigned char *aDst, unsigned int aBytes);
		virtual unsigned int length();
		virtual void seek(int aOffset);
		virtual unsigned int pos();
		virtual ~DiskFile();
		DiskFile();
		DiskFile(FILE *fp);
		result open(const char *aFilename);
		virtual FILE * getFilePtr();
	};

	class MemoryFile : public File
	{
	public:
		unsigned char *mDataPtr;
		unsigned int mDataLength;
		unsigned int mOffset;
		bool mDataOwned;

		virtual int eof();
		virtual unsigned int read(unsigned char *aDst, unsigned int aBytes);
		virtual unsigned int length();
		virtual void seek(int aOffset);
		virtual unsigned int pos();
		virtual unsigned char * getMemPtr();
		virtual ~MemoryFile();
		MemoryFile();
		result openMem(unsigned char *aData, unsigned int aDataLength, bool aCopy=false, bool aTakeOwnership=true);
		result openToMem(const char *aFilename);
		result openFileToMem(File *aFile);
	};
};
#ifndef NO_IMGUISOLOUD_WAV
//----soloud_wav.h-----------------------------------------------------------------------------------------------
#define SOLOUD_WAV_H
struct stb_vorbis;
namespace SoLoud
{
	class Wav;
	class File;

	class WavInstance : public AudioSourceInstance
	{
		Wav *mParent;
		unsigned int mOffset;
	public:
		WavInstance(Wav *aParent);
		virtual void getAudio(float *aBuffer, unsigned int aSamples);
		virtual result rewind();
		virtual bool hasEnded();
	};

	class Wav : public AudioSource
	{
		result loadwav(File *aReader);
		result loadogg(stb_vorbis *aVorbis);
		result testAndLoadFile(File *aReader);
	public:
		float *mData;
		unsigned int mSampleCount;

		Wav();
		virtual ~Wav();
		result load(const char *aFilename);
		result loadMem(unsigned char *aMem, unsigned int aLength, bool aCopy = false, bool aTakeOwnership = true);
		result loadFile(File *aFile);
		
		virtual AudioSourceInstance *createInstance();
		time getLength();
	};
};
//----soloud_wavstream.h-----------------------------------------------------------------------------------------------
#define SOLOUD_WAVSTREAM_H
//#include <stdio.h>
struct stb_vorbis;
namespace SoLoud
{
	class WavStream;
	class File;

	class WavStreamInstance : public AudioSourceInstance
	{
		WavStream *mParent;
		unsigned int mOffset;
		File *mFile;
		stb_vorbis *mOgg;
		unsigned int mOggFrameSize;
		unsigned int mOggFrameOffset;
		float **mOggOutputs;
	public:
		WavStreamInstance(WavStream *aParent);
		virtual void getAudio(float *aBuffer, unsigned int aSamples);
		virtual result rewind();
		virtual bool hasEnded();
		virtual ~WavStreamInstance();
	};

	class WavStream : public AudioSource
	{
		result loadwav(File * fp);
		result loadogg(File * fp);
	public:
		int mOgg;
		char *mFilename;
		File *mMemFile;
		File *mStreamFile;
		unsigned int mDataOffset;
		unsigned int mBits;
		unsigned int mSampleCount;

		WavStream();
		virtual ~WavStream();
		result load(const char *aFilename);
		result loadMem(unsigned char *aData, unsigned int aDataLen, bool aCopy = false, bool aTakeOwnership = true);
		result loadToMem(const char *aFilename);
		result loadFile(File *aFile);
		result loadFileToMem(File *aFile);		
		virtual AudioSourceInstance *createInstance();
		time getLength();
	public:
		result parse(File *aFile);
	};
};
#endif //NO_IMGUISOLOUD_WAV
//----demos/piano/soloud_basicwave.h--------------------------------------------------------------------------------
#ifndef NO_IMGUISOLOUD_BASICWAVE    // I would like to "embed" the whole piano demo someway too
#define BASICWAVE_H
namespace SoLoud
{
    class Basicwave;

    class BasicwaveInstance : public AudioSourceInstance
    {
        Basicwave *mParent;
        int mOffset;
    public:
        BasicwaveInstance(Basicwave *aParent);
        virtual void getAudio(float *aBuffer, unsigned int aSamples);
        virtual bool hasEnded();
    };

    class Basicwave : public AudioSource
    {
    public:
        enum WAVEFORMS
        {
            SINE = 0,
            TRIANGLE,
            SQUARE,
            SAW,
            INVERSESAW
        };
        float mFreq;
        int mWaveform;
        Basicwave();
        virtual ~Basicwave();
        void setSamplerate(float aSamplerate);
        void setWaveform(int aWaveform);
        virtual AudioSourceInstance *createInstance();
    };
};
#endif //NO_IMGUISOLOUD_BASICWAVE

//----soloud_speech.h-----------------------------------------------------------------------------------------------
#ifdef YES_IMGUISOLOUD_SPEECH
/*
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
#define SOLOUD_SPEECH_H
//----../src/audiosource/speech/darray.h----------------------------------------------------------------------------
#define DARRAY_H
class darray
{
protected:
	char *mData;
	int mUsed;
	int mAllocated;
	int mAllocChunk;
public:
	darray();
	~darray();
	void clear();
	char *getDataInPos(int aPosition);
	void put(int aData);
	int getSize() const { return mUsed; }
	char *getData() { return mData; } 
};
//----../src/audiosource/speech/resonator.h--------------------------------------------------------------------------
#define RESONATOR_H
class resonator
{
	float mA, mB, mC, mP1, mP2;
public:

	/* Convert formant freqencies and bandwidth into resonator difference equation coefficents
	 */
	void initResonator(
		int aFrequency,                       /* Frequency of resonator in Hz  */
		int aBandwidth,                      /* Bandwidth of resonator in Hz  */
		int aSamplerate);

	/* Convert formant freqencies and bandwidth into anti-resonator difference equation constants
	 */
	void initAntiresonator(
		int aFrequency,                       /* Frequency of resonator in Hz  */
		int aBandwidth,                      /* Bandwidth of resonator in Hz  */
		int aSamplerate);

	/* Set gain */
	void setGain(float aG);

	/* Generic resonator function */
	float resonate(float input);

	/* Generic anti-resonator function
	   Same as resonator except that a,b,c need to be set with initAntiresonator()
	   and we save inputs in p1/p2 rather than outputs.
	   There is currently only one of these - "mNasalZero"
	
	   Output = (mNasalZero.a * input) + (mNasalZero.b * oldin1) + (mNasalZero.c * oldin2) 
	 */

	float antiresonate(float input);

	resonator();

	~resonator();
};
//----../src/audiosource/speech/klatt.h------------------------------------------------------------------------------
#define KLATT_H
#define CASCADE_PARALLEL      1
#define ALL_PARALLEL          2
#define NPAR                 40
class klatt_frame
{
public:
	int mF0FundamentalFreq;          // Voicing fund freq in Hz                       
	int mVoicingAmpdb;               // Amp of voicing in dB,            0 to   70    
	int mFormant1Freq;               // First formant freq in Hz,        200 to 1300  
	int mFormant1Bandwidth;          // First formant bw in Hz,          40 to 1000   
	int mFormant2Freq;               // Second formant freq in Hz,       550 to 3000  
	int mFormant2Bandwidth;          // Second formant bw in Hz,         40 to 1000   
	int mFormant3Freq;               // Third formant freq in Hz,        1200 to 4999 
	int mFormant3Bandwidth;          // Third formant bw in Hz,          40 to 1000   
	int mFormant4Freq;               // Fourth formant freq in Hz,       1200 to 4999 
	int mFormant4Bandwidth;          // Fourth formant bw in Hz,         40 to 1000   
	int mFormant5Freq;               // Fifth formant freq in Hz,        1200 to 4999 
	int mFormant5Bandwidth;          // Fifth formant bw in Hz,          40 to 1000   
	int mFormant6Freq;               // Sixth formant freq in Hz,        1200 to 4999 
	int mFormant6Bandwidth;          // Sixth formant bw in Hz,          40 to 2000   
	int mNasalZeroFreq;              // Nasal zero freq in Hz,           248 to  528  
	int mNasalZeroBandwidth;         // Nasal zero bw in Hz,             40 to 1000   
	int mNasalPoleFreq;              // Nasal pole freq in Hz,           248 to  528  
	int mNasalPoleBandwidth;         // Nasal pole bw in Hz,             40 to 1000   
	int mAspirationAmpdb;            // Amp of aspiration in dB,         0 to   70    
	int mNoSamplesInOpenPeriod;      // # of samples in open period,     10 to   65   
	int mVoicingBreathiness;         // Breathiness in voicing,          0 to   80    
	int mVoicingSpectralTiltdb;      // Voicing spectral tilt in dB,     0 to   24    
	int mFricationAmpdb;             // Amp of frication in dB,          0 to   80    
	int mSkewnessOfAlternatePeriods; // Skewness of alternate periods,   0 to   40 in sample#/2
	int mFormant1Ampdb;              // Amp of par 1st formant in dB,    0 to   80  
	int mFormant1ParallelBandwidth;  // Par. 1st formant bw in Hz,       40 to 1000 
	int mFormant2Ampdb;              // Amp of F2 frication in dB,       0 to   80  
	int mFormant2ParallelBandwidth;  // Par. 2nd formant bw in Hz,       40 to 1000 
	int mFormant3Ampdb;              // Amp of F3 frication in dB,       0 to   80  
	int mFormant3ParallelBandwidth;  // Par. 3rd formant bw in Hz,       40 to 1000 
	int mFormant4Ampdb;              // Amp of F4 frication in dB,       0 to   80  
	int mFormant4ParallelBandwidth;  // Par. 4th formant bw in Hz,       40 to 1000 
	int mFormant5Ampdb;              // Amp of F5 frication in dB,       0 to   80  
	int mFormant5ParallelBandwidth;  // Par. 5th formant bw in Hz,       40 to 1000 
	int mFormant6Ampdb;              // Amp of F6 (same as r6pa),        0 to   80  
	int mFormant6ParallelBandwidth;  // Par. 6th formant bw in Hz,       40 to 2000 
	int mParallelNasalPoleAmpdb;     // Amp of par nasal pole in dB,     0 to   80  
	int mBypassFricationAmpdb;       // Amp of bypass fric. in dB,       0 to   80  
	int mPalallelVoicingAmpdb;       // Amp of voicing,  par in dB,      0 to   70  
	int mOverallGaindb;              // Overall gain, 60 dB is unity,    0 to   60  
	klatt_frame();
};

class darray;
class Element;

class Slope
{
public:
	float mValue;                   /* boundary value */
	int mTime;                      /* transition time */
	Slope() 
	{
		mValue = 0;
		mTime = 0;
	}
};


class klatt
{
	// resonators
	resonator mParallelFormant1, mParallelFormant2, mParallelFormant3, 
		      mParallelFormant4, mParallelFormant5, mParallelFormant6,
		      mParallelResoNasalPole, mNasalPole, mNasalZero, 
			  mCritDampedGlotLowPassFilter, mDownSampLowPassFilter, mOutputLowPassFilter;
public:
	int mF0Flutter;
	int mSampleRate;
	int mNspFr;
	int mF0FundamentalFreq;        // Voicing fund freq in Hz  
	int mVoicingAmpdb;          // Amp of voicing in dB,    0 to   70  
	int mSkewnessOfAlternatePeriods;         // Skewness of alternate periods,0 to   40  
	int mTimeCount;     // used for f0 flutter
	int mNPer;          // Current loc in voicing period   40000 samp/s
	int mT0;            // Fundamental period in output samples times 4 
	int mNOpen;         // Number of samples in open phase of period  
	int mNMod;          // Position in period to begin noise amp. modul 

	// Various amplitude variables used in main loop

	float mAmpVoice;     // mVoicingAmpdb converted to linear gain  
	float mAmpBypas;     // mBypassFricationAmpdb converted to linear gain  
	float mAmpAspir;     // AP converted to linear gain  
	float mAmpFrica;     // mFricationAmpdb converted to linear gain  
	float mAmpBreth;     // ATURB converted to linear gain  

	// State variables of sound sources

	int mSkew;                  // Alternating jitter, in half-period units  
	float mNatglotA;           // Makes waveshape of glottal pulse when open  
	float mNatglotB;           // Makes waveshape of glottal pulse when open  
	float mVWave;               // Ditto, but before multiplication by mVoicingAmpdb  
	float mVLast;               // Previous output of voice  
	float mNLast;               // Previous output of random number generator  
	float mGlotLast;            // Previous value of glotout  
	float mDecay;               // mVoicingSpectralTiltdb converted to exponential time const  
	float mOneMd;               // in voicing one-pole ELM_FEATURE_LOW-pass filter  


	float natural_source(int aNper);

	void frame_init(klatt_frame *frame);
	void flutter(klatt_frame *pars);
	void pitch_synch_par_reset(klatt_frame *frame, int ns);
	void parwave(klatt_frame *frame, short int *jwave);
	void init();
	static int phone_to_elm(char *aPhoneme, int aCount, darray *aElement);

	int mElementCount;
	unsigned char *mElement;
	int mElementIndex;
	klatt_frame mKlattFramePars;
	Element * mLastElement;
	int mTStress;
	int mNTStress;
	Slope mStressS;
	Slope mStressE;
	float mTop;
	void initsynth(int aElementCount,unsigned char *aElement);
	int synth(int aSampleCount, short *aSamplePointer);
	klatt();
};
//----../src/audiosource/speech/tts.h--------------------------------------------------------------------------------
extern int xlate_string (const char *string,darray *phone);
//----soloud_speech.h-[2]--------------------------------------------------------------------------------------------
namespace SoLoud
{
	class Speech;

	class Speech : public AudioSource
	{
	public:
		int mFrames;
		darray mElement;
		Speech();
		result setText(const char *aText);
		virtual ~Speech();
		virtual AudioSourceInstance *createInstance();
	};

	class SpeechInstance : public AudioSourceInstance
	{
	public:
		klatt mSynth;
		Speech *mParent;
		short *mSample;
		int mSampleCount;
		int mOffset;
	public:
		SpeechInstance(Speech *aParent);
        virtual ~SpeechInstance();
		virtual void getAudio(float *aBuffer, unsigned int aSamples);
		virtual result rewind();
		virtual bool hasEnded();
	};
};
#endif // YES_IMGUISOLOUD_SPEECH

//----soloud_modplug.h-----------------------------------------------------------------------------------------------
#ifdef YES_IMGUISOLOUD_MODPLUG
namespace SoLoud
{
	class Modplug;
	class File;

	class ModplugInstance : public AudioSourceInstance
	{
		Modplug *mParent;
		void *mModplugfile;
		int mPlaying;

	public:
		ModplugInstance(Modplug *aParent);
		virtual ~ModplugInstance();
		virtual void getAudio(float *aBuffer, unsigned int aSamples);
		virtual bool hasEnded();
	};

	class Modplug : public AudioSource
	{
	public:
		char *mData;
		unsigned int mDataLen;
		float mEstimatedDuration;
		Modplug();
		virtual ~Modplug();
		result load(const char* aFilename);
		result loadMem(unsigned char *aMem, unsigned int aLength, bool aCopy = false, bool aTakeOwnership = true);
		result loadFile(File *aFile);
		float getLength();
		virtual AudioSourceInstance *createInstance();
        static const char* SupportedExtensions;
	};
};
#endif //YES_IMGUISOLOUD_MODPLUG
//----soloud_monotone.h-----------------------------------------------------------------------------------------------
#ifdef  YES_IMGUISOLOUD_MONOTONE
namespace SoLoud
{
	class Monotone;
	class File;

	struct MonotoneSong
	{
		char *mTitle;
		char *mComment;
		unsigned char mVersion; // must be 1
		unsigned char mTotalPatterns;
		unsigned char mTotalTracks;
		unsigned char mCellSize; // must be 2 for version 1
		unsigned char mOrder[256];
		unsigned int *mPatternData; // 64 rows * mTotalPatterns * mTotalTracks
	};

	struct MonotoneChannel
	{
		int mEnabled; 
		int mActive;
		int mFreq[3];
		int mPortamento;
		int mArpCounter;
		int mArp;
		int mLastNote;
		int mPortamentoToNote;
		int mVibrato;
		int mVibratoIndex;
		int mVibratoDepth;
		int mVibratoSpeed;
	};

	struct MonotoneHardwareChannel
	{
		int mEnabled;
		float mSamplePos;
		float mSamplePosInc;
	};

	class MonotoneInstance : public AudioSourceInstance
	{
		Monotone *mParent;		
	public:
		MonotoneChannel mChannel[12];
		MonotoneHardwareChannel mOutput[12];
		int mNextChannel;
		int mTempo; // ticks / row. Tick = 60hz. Default 4.
		int mOrder;
		int mRow;
		int mSampleCount;
		int mRowTick;

		MonotoneInstance(Monotone *aParent);
		virtual void getAudio(float *aBuffer, unsigned int aSamples);
		virtual bool hasEnded();
	};

	class Monotone : public AudioSource
	{
	public:
		enum MONOTONE_WAVEFORMS
		{
			SQUARE = 0,
			SAW    = 1,
			SIN    = 2,
			SAWSIN = 3
		};
		
		int mNotesHz[800];
		int mVibTable[32];
		int mHardwareChannels;
		int mWaveform;
		MonotoneSong mSong;
		Monotone();
		~Monotone();
		result setParams(int aHardwareChannels, int aWaveform = SQUARE);
		result load(const char *aFilename);
		result loadMem(unsigned char *aMem, unsigned int aLength, bool aCopy = false, bool aTakeOwnership = true);
		result loadFile(File *aFile);
		virtual AudioSourceInstance *createInstance();
	public:
		void clear();
	};
};
#endif //YES_IMGUISOLOUD_MONOTONE
//----soloud_sfxr.h-----------------------------------------------------------------------------------------------
#ifdef YES_IMGUISOLOUD_SFXR
namespace SoLoud
{
	class File;

	class Prg
	{
	public:
		// random generator
		Prg();
		unsigned int state[16];
		unsigned int index;
		unsigned int rand();
		void srand(int aSeed);
	};

	struct SfxrParams
	{
		int wave_type;

		float p_base_freq;
		float p_freq_limit;
		float p_freq_ramp;
		float p_freq_dramp;
		float p_duty;
		float p_duty_ramp;

		float p_vib_strength;
		float p_vib_speed;
		float p_vib_delay;

		float p_env_attack;
		float p_env_sustain;
		float p_env_decay;
		float p_env_punch;

		bool filter_on;
		float p_lpf_resonance;
		float p_lpf_freq;
		float p_lpf_ramp;
		float p_hpf_freq;
		float p_hpf_ramp;

		float p_pha_offset;
		float p_pha_ramp;

		float p_repeat_speed;

		float p_arp_speed;
		float p_arp_mod;

		float master_vol;

		float sound_vol;
	};

	class Sfxr;

	class SfxrInstance : public AudioSourceInstance
	{
		Sfxr *mParent;

		Prg mRand;
		SfxrParams mParams;

		bool playing_sample;
		int phase;
		double fperiod;
		double fmaxperiod;
		double fslide;
		double fdslide;
		int period;
		float square_duty;
		float square_slide;
		int env_stage;
		int env_time;
		int env_length[3];
		float env_vol;
		float fphase;
		float fdphase;
		int iphase;
		float phaser_buffer[1024];
		int ipp;
		float noise_buffer[32];
		float fltp;
		float fltdp;
		float fltw;
		float fltw_d;
		float fltdmp;
		float fltphp;
		float flthp;
		float flthp_d;
		float vib_phase;
		float vib_speed;
		float vib_amp;
		int rep_time;
		int rep_limit;
		int arp_time;
		int arp_limit;
		double arp_mod;

		void resetSample(bool aRestart);

	public:
		SfxrInstance(Sfxr *aParent);
		virtual void getAudio(float *aBuffer, unsigned int aSamples);
		virtual bool hasEnded();
	};

	class Sfxr : public AudioSource
	{
	public:
		SfxrParams mParams;

		enum SFXR_PRESETS 
		{
			COIN,
			LASER,
			EXPLOSION,
			POWERUP,
			HURT,
			JUMP,
			BLIP
		};

		Prg mRand;
		
		Sfxr();
		virtual ~Sfxr();
		void resetParams();
		result loadParams(const char* aFilename);
		result loadParamsMem(unsigned char *aMem, unsigned int aLength, bool aCopy = false, bool aTakeOwnership = true);
		result loadParamsFile(File *aFile);

		result loadPreset(int aPresetNo, int aRandSeed);
		virtual AudioSourceInstance *createInstance();
	};
};
#endif //YES_IMGUISOLOUD_SFXR
//----soloud_tedsid.h-----------------------------------------------------------------------------------------------
#ifdef YES_IMGUISOLOUD_TEDSID
/*
The TED / SID support is based on tedplay (c) 2012 Attila Grosz, used under Unlicense http://unlicense.org/
*/
#define TEDSID_H
class SIDsound;
class TED;

namespace SoLoud
{
	class TedSid;
	class File;

	class TedSidInstance : public AudioSourceInstance
	{
		TedSid *mParent;		
		SIDsound *mSID;
		TED *mTED;
		unsigned int mSampleCount;
		int mNextReg;
		int mNextVal;
		int mRegValues[128];
	public:

		TedSidInstance(TedSid *aParent);
		~TedSidInstance();
		virtual void getAudio(float *aBuffer, unsigned int aSamples);
		virtual void tick();
		virtual bool hasEnded();
		virtual float getInfo(unsigned int aInfoKey);
	};

	class TedSid : public AudioSource
	{
	public:
		File *mFile;
		int mModel;
		bool mFileOwned;
		TedSid();
		~TedSid();
		result load(const char *aFilename);
		result loadToMem(const char *aFilename);
		result loadMem(unsigned char *aMem, unsigned int aLength, bool aCopy = false, bool aTakeOwnership = true);
		result loadFileToMem(File *aFile);
		result loadFile(File *aFile);
		virtual AudioSourceInstance *createInstance();
	};
};
#endif //YES_IMGUISOLOUD_TEDSID
//----soloud_vic.h-----------------------------------------------------------------------------------------------
#ifdef YES_IMGUISOLOUD_VIC
/*
A very bare bones emulator for Commodore VIC-20 sound chip. Supports both PAL and NTSC models.
Bass, alto and soprano should be quite close to original vic, noise probably not so.

The first three channels (bass, alto and soprano) are square waveform generators with 7-bit frequency.
The highest bit of each oscillator register switches the oscillator on/off.
The fourth oscillator generates a noise waveform.

VIC-20 does not have per channel volume control, only global volume,
which you can change by setting audio source's volume.

To get that authentic moldy VIC-20 sound, the audio source should be coupled with a biquad resonant filter
with the following params: type = LOWPASS, sample rate = 44100, frequency = 1500, resonance = 2.0.
*/
namespace SoLoud
{
	class Vic;

	class VicInstance : public AudioSourceInstance
	{
	public:
		VicInstance(Vic *aParent);
		~VicInstance();

		virtual void getAudio(float *aBuffer, unsigned int aSamples);
		virtual bool hasEnded();

	private:
		Vic*			m_parent;
		unsigned int	m_phase[4];
		unsigned int	m_noisePos;
	};

	class Vic : public AudioSource
	{
	public:
		// VIC model
		enum
		{
			PAL	= 0,
			NTSC,
		};

		// VIC sound registers
		enum
		{
			BASS = 0,
			ALTO,
			SOPRANO,
			NOISE,
			MAX_REGS
		};

		Vic();
		virtual ~Vic();
		
		void setModel(int model);
		int getModel() const;

		void setRegister(int reg, unsigned char value) 		{ m_regs[reg] = value; }
		unsigned char getRegister(int reg) const			{ return m_regs[reg]; }

		virtual AudioSourceInstance *createInstance();

	private:
		friend class VicInstance;

		int				m_model;
		float			m_clocks[4];		// base clock frequencies for oscillators, dependent on VIC model
		unsigned char	m_regs[MAX_REGS];		
		unsigned char 	m_noise[8192];
	};
};
#endif //YES_IMGUISOLOUD_VIC

#ifndef NO_IMGUISOLOUD_METHODS
namespace ImGuiSoloud {

#ifndef NO_IMGUISOLOUD_BASICWAVE
// a class that bundles the Soloud::Piano example (in the SoLoud repository)
// TODO: should be a static class or a singleton: multiple instances simply don't work!
// In addition it relies on the ImGui::GetIO().KeyDown[] values (its first 128 values must match the ASCII values and the keyboard must be a "QWERTY"):
// however we can pass "pOptional18KeysOverrideFromFDiesisToB" to override them (maybe we could make a GUI for that).
#define IMGUISOLOUD_HAS_BASICPIANO
class BasicPiano {
protected:
    struct Plonked  {
        int mHandle;
        float mRel;
    };
public:
    BasicPiano();
    ~BasicPiano() {}
    inline bool isInited() const {return inited;}
    void init(SoLoud::Soloud& gSoloud,const int* pOptional18KeysOverrideFromFDiesisToB=NULL);
    void play();
    void renderGUI();
protected:
    SoLoud::Soloud* pSoloud;			// SoLoud engine core
    SoLoud::Bus gBus;
    int bushandle;
    static int DefaultKeys[18];     // From F# to B [by default, with each char casted to int: "1q2w3er5t6yu8i9o0p"]

    SoLoud::Basicwave gWave;		// Simple wave audio source

    // Filters
#   ifdef SOLOUD_ECHOFILTER_H
    SoLoud::EchoFilter gEchoFilter;
#   endif
#   ifdef SOLOUD_BQRFILTER_H
    SoLoud::BiquadResonantFilter gBQRFilter;
#   endif
#   ifdef SOLOUD_LOFIFILTER_H
    SoLoud::LofiFilter gLofiFilter;
#   endif
#   ifdef SOLOUD_DCREMOVAL_H
    SoLoud::DCRemovalFilter gDCRemovalFilter;
#   endif

    int gWaveSelect;
    int gFilterSelect;

    Plonked gPlonked[128];
    float gAttack;
    float gRelease;
    float filter_param0[4];
    float filter_param1[4];
    float filter_param2[4];
    bool inited;
    void plonk(float rel, float vol = 0x50);
    void unplonk(float rel);
    void replonk(float vol = 0x50);
    void operator=(const BasicPiano&) {}
    BasicPiano(const BasicPiano&) {}
};
#endif //NO_IMGUISOLOUD_BASICWAVE

} // namespace ImGuiSoloud
#endif //NO_IMGUISOLOUD_METHODS


//----end file-----------------------------------------------------------------------------------------
#endif //SOLOUD_H


