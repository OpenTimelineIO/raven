
#ifdef __EMSCRIPTEN__
#define WITH_SDL2_STATIC
#endif

#ifdef __APPLE__
#define WITH_COREAUDIO
#endif

#ifdef __linux__
#define WITH_ALSA
#endif

#ifdef _WIN32
#define WITH_WINMM
#endif

#define YES_IMGUISOLOUD_MODPLUG
#include "imguisoloud.cpp"

#include "audio.h"

#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

namespace SoLoud
{
	Mp3Instance::Mp3Instance(Mp3 *aParent)
	{
		m_parent = aParent;
		m_playhead = 0;
	}

	Mp3Instance::~Mp3Instance()
	{
	}

	void Mp3Instance::getAudio(float *aBuffer, unsigned int aSamples)
	{
		unsigned int samples = aSamples;
		if (m_playhead + samples > m_parent->mSampleCount) {
			samples = m_parent->mSampleCount - m_playhead;
		}
		if (samples > 0) {
			for (int ch = 0; ch < mChannels; ch++)
			{
				for (int s = 0; s < samples; s++) {
					// Mp3 buffer is interleaved per-sample
					// but SoLoud buffer has channels separate
					aBuffer[s + aSamples*ch] = m_parent->mSampleData[(m_playhead + s) * mChannels + ch];
				}
			}
		}
		m_playhead += aSamples;
	}

	result Mp3Instance::rewind()
	{
		m_playhead = 0;
		return 0;
	}

	bool Mp3Instance::hasEnded()
	{
		return m_playhead >= m_parent->mSampleCount;
	}

	Mp3::Mp3()
	{
		mSampleCount = 0;
		mSampleData = NULL;
	}

	Mp3::~Mp3()
	{
		if (mSampleData) {
			drmp3_free(mSampleData, NULL);
			mSampleData = NULL;
			mSampleCount = 0;
		}
	}

	result Mp3::load(const char *path)
	{
		drmp3_config config;
		drmp3_uint64 frame_count;

		if (mSampleData) {
			drmp3_free(mSampleData, NULL);
			mSampleData = NULL;
			mSampleCount = 0;
		}

		mSampleData = drmp3_open_file_and_read_pcm_frames_f32(
			path,
			&config,
			&frame_count,
			NULL);

		if (mSampleData) {
			mSampleCount = frame_count;
			mChannels = config.channels;
			mBaseSamplerate = config.sampleRate;
			return SO_NO_ERROR;
		}else{
			return FILE_LOAD_FAILED;
		}
	}

	time Mp3::getLength()
	{
		return (float)mSampleCount / (float)mBaseSamplerate;
	}

	AudioSourceInstance * Mp3::createInstance()
	{
		return new Mp3Instance(this);
	}
};
