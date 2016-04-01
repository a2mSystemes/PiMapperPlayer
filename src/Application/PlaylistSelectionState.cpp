/*
 * PlaylistSelectionState.cpp
 *
 *  Created on: 29 mars 2016
 *      Author: ange
 */

#include "PlaylistSelectionState.h"

namespace ofx {
namespace piMapper {
PlaylistSelectionState * PlaylistSelectionState::_instance = 0;

PlaylistSelectionState* ofx::piMapper::PlaylistSelectionState::instance() {
	if(_instance == 0){
			_instance = new ofx::piMapper::PlaylistSelectionState();
		}
		return _instance;
}

void ofx::piMapper::PlaylistSelectionState::draw(Application* app) {
	app->getGui()->draw();
}

void PlaylistSelectionState::onKeyPressed(Application* app,
		ofKeyEventArgs& args) {
	switch(args.key){
	case OF_KEY_DOWN:
		break;
	case OF_KEY_UP:
		break;
	case OF_KEY_RIGHT:
		break;
	case OF_KEY_LEFT:
		break;
	case OF_KEY_RETURN:
		break;
	default:
		break;
	}
}

} // namespace piMapper
} // namespace ofx
