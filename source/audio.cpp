#include <stdlib.h>

#include <map>

#include "audio.h"
#include "fmod_errors.h"
#include "dbg.h"

static FMOD_SYSTEM* fmodSystem = NULL;
static std::map<std::string, FMOD_SOUND*> soundMap;

#define fmodErrcheck(_result) _fmodErrcheck(_result, __FILE__, __LINE__)

void _fmodErrcheck(FMOD_RESULT result, const char* file, int line)
{
    if (result != FMOD_OK)
    {
        log_err("%s(%d): FMOD error %d - %s", file, line, result, FMOD_ErrorString(result));
    }
}

void audioInit()
{
	fmodErrcheck(FMOD_System_Create(&fmodSystem));
	fmodErrcheck(FMOD_System_Init(fmodSystem, 32, FMOD_INIT_NORMAL, NULL));
}

void audioExit()
{
	for(auto it= soundMap.begin(); it != soundMap.end(); ++it)
	{
		fmodErrcheck(FMOD_Sound_Release(it->second));
	}

    fmodErrcheck(FMOD_System_Close(fmodSystem));
    fmodErrcheck(FMOD_System_Release(fmodSystem));
}

FMOD_SOUND* Sound::loadSound(const char* filename)
{
	auto it(soundMap.find(filename));
	if (it != soundMap.end()) return (it->second);

	FMOD_SOUND* ret = NULL;

	log_info("loading sound : %s", filename);

	fmodErrcheck(FMOD_System_CreateSound(fmodSystem, filename, FMOD_DEFAULT, 0, &ret));

	soundMap[filename] = ret;

	return ret;
}

Sound::Sound(const char* filename, bool loop):
	sound(NULL),
	channel(NULL)
{
	if(!fmodSystem) audioInit();

	sound = loadSound(filename);

	setLoop(loop);
}

void Sound::setLoop(bool mode)
{
	fmodErrcheck(FMOD_Sound_SetMode(sound, mode ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF));
}

void Sound::play()
{
	if(channel) stop();
	fmodErrcheck(FMOD_System_PlaySound(fmodSystem, sound, 0, false, &channel));
}

void Sound::stop()
{
	fmodErrcheck(FMOD_Channel_Stop(channel));
	channel = NULL;
}
