#include "VideoSource.h"

namespace ofx {
namespace piMapper {

bool VideoSource::enableAudio = true;

VideoSource::VideoSource() {
	loadable = true;
	loaded = false;
	type = SourceType::SOURCE_TYPE_VIDEO;
#ifdef TARGET_RASPBERRY_PI
	omxPlayer = 0;
#else
	videoPlayer = 0;
	_initialVolumeSet = false;
#endif
}

VideoSource::~VideoSource() {
}

void VideoSource::loadVideo(string & filePath) {
	path = filePath;
	setNameFromPath(filePath);
#ifdef TARGET_RASPBERRY_PI
	omxPlayer = OMXPlayerCache::instance()->load(filePath);
	texture = &(omxPlayer->getTextureReference());
#else
	videoPlayer = new ofVideoPlayer();
	videoPlayer->load(filePath);
	videoPlayer->setLoopState(OF_LOOP_NONE);
	videoPlayer->play();
	videoPlayer->setVolume(VideoSource::enableAudio ? 1.0f : 0.0f);
	texture = &(videoPlayer->getTexture());
	ofAddListener(ofEvents().update, this, &VideoSource::update);
#endif
	ofRegisterKeyEvents(this, OF_EVENT_ORDER_BEFORE_APP);
	loaded = true;
}

void VideoSource::clear() {
	texture = 0;
#ifdef TARGET_RASPBERRY_PI
	OMXPlayerCache::instance()->unload(path);
#else
	ofRemoveListener(ofEvents().update, this, &VideoSource::update);
	videoPlayer->stop();
	videoPlayer->close();
	delete videoPlayer;
	videoPlayer = 0;
#endif
	ofUnregisterKeyEvents(this, OF_EVENT_ORDER_BEFORE_APP);

	loaded = false;
}

#ifndef TARGET_RASPBERRY_PI
void VideoSource::update(ofEventArgs & args) {
	if (videoPlayer != 0) {
		if (!_initialVolumeSet) {
			if (videoPlayer->isInitialized()) {
				videoPlayer->setVolume(VideoSource::enableAudio ? 1.0f : 0.0f);
				_initialVolumeSet = true;
			}
		}
		videoPlayer->update();
	}
}
#endif

void VideoSource::keyPressed(ofKeyEventArgs& args) {
	ofLogVerbose(__func__) << "key pressed";

	switch (args.key) {
#ifdef TARGET_RASPBERRY_PI
	case OF_OSMC_KEY_PLAYPAUSE:
		omxPlayer->togglePause();
		break;
	case OF_OSMC_KEY_STOP:
		omxPlayer->seekToTimeInSeconds(0);
		break;
	case OF_OSMC_KEY_CONTEXT_MENU:
		break;
	case OF_OSMC_KEY_INFOS:
		break;
	case OF_OSMC_KEY_REWIND:
		break;
	case OF_OSMC_KEY_FASTFORWARD:
		break;
#endif
	case OF_KEY_UP:
		ofLogVerbose(__func__) << "key up";
		break;
	case OF_KEY_DOWN:
		ofLogVerbose(__func__) << "key down";
		break;

	}

}

void VideoSource::keyReleased(ofKeyEventArgs& args) {
}

} // namespace piMapper
} // namespace ofx
