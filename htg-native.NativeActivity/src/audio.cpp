
#include "../include/Audio.h"
#include "../ndk-helper/JNIHelper.h"


void AudioSound::setPositionSeconds(float seconds) {
	setPositionMillis(seconds * 1000);
}
float AudioSound::getPositionSeconds() {
	return mStream->timestamp();
}
long AudioSound::getPositionMillis() {
	return mStream->timestampMsec();
}

void AudioSound::setPositionMillis(float milliseconds) {
	mStream->seek(milliseconds);
}

AudioSound::AudioSound(std::string filename, std::string audioName, bool isAPK) {
	if (audioName == "\0") {
		audioName = filename;
	}
	if (audiofiles.find(audioName) != audiofiles.end()) {
		audiofiles.erase(audioName);
	}
	std::shared_ptr<AudioSound> ptr{};
	ptr.reset(this);
	audiofiles.insert({ audioName,  ptr});
	mStream = std::make_shared<ani::MediaPlayer>();
	mStream->setDataSource(filename, isAPK);
	mStream->prepare();
}


void AudioSound::Loop() {
	mStream->setLooping(true);
}

void AudioSound::unLoop() {
	mStream->setLooping(false);
}

void AudioSound::Stop() {
	mStream->stop();
}
void AudioSound::Play() {
	mStream->play();
}

void AudioSound::Pause() {
	mStream->pause();
}

bool AudioSound::isPlaying() {
	return mStream->isPlaying();
}

void AudioSound::SetVolume(int l, int r) {
	float _l = (l != 0) ? l / 100. : 0;
	float _r = (r != 0) ? r / 100. : 0;
	mStream->pan(_l, _r);
}
void AudioSound::SetVolume(int v) {
	if (v == 0) {
		mStream->setVolume(0);
	}
	mStream->setVolume(v / 100.);
}

int AudioSound::GetVolume() {
	return round(mStream->get_volume() * 100.);
}