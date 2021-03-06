// SelNextSurfaceCmd
// Selects next surface in the projection mapping mode
// Created by Krisjanis Rijnieks 2016-02-03

#pragma once

#include "BaseCmd.h"
#include "BaseSurface.h"
#include "SurfaceManager.h"

namespace ofx {
namespace piMapper {

class SelNextSurfaceCmd : public BaseUndoCmd {

	public:
		SelNextSurfaceCmd(SurfaceManager * surfaceManager);
		void exec();
		void undo();

	private:
		SurfaceManager * _surfaceManager;
		BaseSurface * _prevSelectedSurface;
		bool _noSurfaceYet;

};

} // namespace piMapper
} // namespace ofx

