#include "SelPrevSurfaceCmd.h"

namespace ofx {
namespace piMapper {

SelPrevSurfaceCmd::SelPrevSurfaceCmd(SurfaceManager * surfaceManager) {
	_surfaceManager = surfaceManager;
	if (_surfaceManager->size() == 0)
		_noSurfaceYet = true;
	else
		_noSurfaceYet = false;
}

void SelPrevSurfaceCmd::exec() {
	if (_noSurfaceYet)
		return;
	if ((_surfaceManager->getSelectedSurface() == 0)) {
		_prevSelectedSurface = 0;
		_surfaceManager->selectSurface(0);
	} else {
		_prevSelectedSurface = _surfaceManager->getSelectedSurface();
		_surfaceManager->selectPrevSurface();
	}
}

void SelPrevSurfaceCmd::undo() {
	ofLogNotice("SelPrevSurfaceCmd", "undo");
	_surfaceManager->selectSurface(_prevSelectedSurface);
	_prevSelectedSurface = 0;
}

} // namespace piMapper
} // namespace ofx

