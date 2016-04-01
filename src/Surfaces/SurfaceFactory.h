#pragma once

#include "ofMain.h"
#include "BaseSurface.h"
#include "SurfaceType.h"
#include "TriangleSurface.h"
#include "QuadSurface.h"
#include "GridWarpSurface.h"

namespace ofx {
namespace piMapper {

// The surface factory singleton
class SurfaceFactory {
	public:
		static SurfaceFactory * instance();
	
		// Create new surface based on type
		BaseSurface * createSurface(int type);
	
	private:
		static SurfaceFactory * _instance;
	
		TriangleSurface * createTriangleSurface();
		QuadSurface * createQuadSurface();
		GridWarpSurface * createGridWarpSurface();
	};

} // namespace piMapper
} // namespace ofx