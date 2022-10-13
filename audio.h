#pragma once

#define YES_IMGUISOLOUD_MODPLUG
#include "imguisoloud.h"


namespace SoLoud
{
	class Mp3;
	struct drmp3;

	class Mp3Instance : public AudioSourceInstance
	{
	public:
		Mp3Instance(Mp3 *aParent);
		~Mp3Instance();

		virtual void getAudio(float *aBuffer, unsigned int aSamples);
		virtual result rewind();
		virtual bool hasEnded();

	private:
		Mp3* m_parent;
		size_t m_playhead;
	};

	class Mp3 : public AudioSource
	{
	public:
		Mp3();
		virtual ~Mp3();
		result load(const char *aFilename);
		virtual AudioSourceInstance *createInstance();
		time getLength();

		float* mSampleData;
		size_t mSampleCount;

	private:
		friend class Mp3Instance;
		// struct drmp3* _drmp3;
	};
};
