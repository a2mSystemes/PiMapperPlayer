/*
 * PlaylistSelectionState.h
 *
 *  Created on: 29 mars 2016
 *      Author: ange
 */

#pragma once

#include "Application.h"
#include "ofEvents.h"
#include "ofLog.h"
#include "ofGraphics.h"
#include "ofConstants.h"

namespace ofx {
namespace piMapper {

class PlaylistSelectionState : public ApplicationBaseState {

	public:
		static PlaylistSelectionState * instance();
		void draw(Application * app);
		void onKeyPressed(Application * app, ofKeyEventArgs & args);
	private:
		static PlaylistSelectionState * _instance;

};

} // namespace piMapper
} // namespace ofx

