
#ifndef AUDIO_H
#define AUDIO_H
#include <iosfwd>
#include <iostream>

#include <map>

#include "camera.h"
#include <AudioFile.h>
#include "../utils.hpp"
#include "apk_helpers.h"

#include "../ani.h"
struct AudioSound {
	void setPositionSeconds(float seconds);
	void setPositionMillis(float milliseconds);
	float getPositionSeconds();
	long getPositionMillis();
	std::shared_ptr<ani::MediaPlayer> mStream;
	static inline std::map<std::string, std::shared_ptr<AudioSound>> audiofiles = {};
	const char
		*WAV_FILE_MISSING = "wav_file_missing",
		*WAV_FILE_NULL = "wav_file_null",
		*WAV_SUCCESS = "wav_ok",
		*WAV_UNINIT = "wav_uninitialized",
		*WAV_UNABLE_TO_PLAY = "wav_unable_to_play"
		;
	AudioSound(std::string filename, std::string audioName = "\0", bool isFromAPK = true);
	void Loop ();
	void unLoop ();
	void Stop();
	void Play();
	void Pause();
	bool isPlaying();
	void SetVolume(int v);
	void SetVolume(int l, int r);
	int GetVolume();
};
#endif
