// TODO: Move this to the Application State system.

#pragma once

#include "ofMain.h"
//#include "ofEvents.h"
//#include "ofGraphics.h"
//#include "ofUtils.h"

#include "SurfaceManager.h"
#include "TextureEditor.h"
#include "ProjectionEditor.h"
#include "SourcesEditor.h"
#include "GuiMode.h"
#include "CmdManager.h"
#include "SelSurfaceCmd.h"
#include "MvSurfaceVertCmd.h"
#include "MvAllTexCoordsCmd.h"
#include "MvTexCoordCmd.h"
#include "SelVertexCmd.h"
#include "DeselectSurfaceCmd.h"
#include "Gui.h"

namespace ofx {
namespace piMapper {

class SurfaceManagerGui {
	
    public:
		SurfaceManagerGui();
		~SurfaceManagerGui();

		void registerMouseEvents();
		void unregisterMouseEvents();

		void draw();
		void mousePressed(ofMouseEventArgs & args);
		void mouseReleased(ofMouseEventArgs & args);
		void mouseDragged(ofMouseEventArgs & args);

		void setSurfaceManager(SurfaceManager * newSurfaceManager);
		void setMediaServer(MediaServer * newMediaServer);
		void setCmdManager(CmdManager * cmdManager);

		void setMode(int newGuiMode);
		int getMode();
		void drawSelectedSurfaceHighlight();
		void drawSelectedSurfaceTextureHighlight();
		void startDrag();
		void stopDrag();
	
		ProjectionEditor * getProjectionEditor();
		TextureEditor * getTextureEditor();
		SourcesEditor * getSourcesEditor();
	
		void onVertexChanged(int & i);
		void onVerticesChanged(vector<ofVec3f> & vertices);
		void onSurfaceSelected(int & surfaceIndex);
		void onVertexSelected(int & vertexIndex);
		void onVertexUnselected(int & vertexIndex);

	private:
		SurfaceManager * surfaceManager;
		MediaServer * mediaServer;
		TextureEditor textureEditor;
		ProjectionEditor projectionEditor;
		SourcesEditor sourcesEditor;
		int guiMode;
		bool bDrag;
		ofVec2f clickPosition;
		CmdManager * _cmdManager;

};

} // namespace piMapper
} // namespace ofx
