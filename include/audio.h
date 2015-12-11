#ifndef AUDIO_H
#define AUDIO_H

#include <fmod.h>

class Sound
{
	public:
		Sound(const char* filename, bool loop = false);

		void play();
		void stop();
		
		void setLoop(bool mode);

		static FMOD_SOUND* loadSound(const char* filename);
		static void deleteSound(FMOD_SOUND* s);

	private:
		FMOD_SOUND* sound;
		FMOD_CHANNEL* channel;
};

void audioInit();
void audioExit();

#endif
