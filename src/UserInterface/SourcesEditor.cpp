#include "SourcesEditor.h"

namespace ofx {
namespace piMapper {

SourcesEditor::SourcesEditor() {
	init();
	// Create new MediaServer instance,
	// we will need to clear this in the deconstr
	mediaServer = new MediaServer();
	isMediaServerExternal = false;
	addMediaServerListeners();
}

void SourcesEditor::init() {
	mediaServer = 0; // Pointers to 0 pointer so we can check later
	isMediaServerExternal = false;
	registerAppEvents();
}

SourcesEditor::SourcesEditor(MediaServer * externalMediaServer) {
	init();
	// Assign external MediaServer instance pointer
	mediaServer = externalMediaServer;
	isMediaServerExternal = true;
	addMediaServerListeners();
}

SourcesEditor::~SourcesEditor() {
	//unregisterAppEvents();
	delete selectors[0];
	delete selectors[1];
	delete selectors[2];
	//removeMediaServerListeners();
	clearMediaServer();
}

void SourcesEditor::registerAppEvents() {
	ofAddListener(ofEvents().setup, this, &SourcesEditor::setup);
}

void SourcesEditor::unregisterAppEvents() {
	ofRemoveListener(ofEvents().setup, this, &SourcesEditor::setup);
}

void SourcesEditor::setup(ofEventArgs & args) {
	selectors.push_back(new RadioList());
	selectors.push_back(new RadioList());
	selectors.push_back(new RadioList());

	// Get media count
	int numImages = mediaServer->getNumImages();
	int numVideos = mediaServer->getNumVideos();
	int numFbos = mediaServer->getNumFboSources();

	if (numVideos) {
		vector<string> videoNames = mediaServer->getVideoNames();
		selectors[0]->setup("Videos", videoNames, mediaServer->getVideoPaths());
		ofAddListener(selectors[0]->onRadioSelected, this,
				&SourcesEditor::handleVideoSelected);
	}
	// Depending on media count, decide what to load and initialize
	if (numImages) {
		// Get image names from media server
		vector<string> imageNames = mediaServer->getImageNames();
		selectors[1]->setup("Images", imageNames, mediaServer->getImagePaths());
		ofAddListener(selectors[1]->onRadioSelected, this,
				&SourcesEditor::handleImageSelected);
	}
	if (numFbos) {
		vector<string> fboNames = mediaServer->getFboSourceNames();
		selectors[2]->setup("FBOs", fboNames, fboNames);
		ofAddListener(selectors[2]->onRadioSelected, this,
				&SourcesEditor::handleFboSelected);
	}

	// Align menus
	int menuPosY = ofGetHeight() / 3;
	int menuPosX = 100;
	int distX = 800;

	if (numVideos) {
		selectors[0]->setPosition(menuPosX, menuPosY);
		menuPosX += distX;
	}
	if (numImages) {
		selectors[1]->setPosition(menuPosX, menuPosY);
		menuPosX += distX;
	}
	if (numFbos) {
		selectors[2]->setPosition(menuPosX, menuPosY);
	}

}

void SourcesEditor::draw() {
	// Don't draw if there is no source selected
	if (surfaceManager->getSelectedSurface() == 0) {
		//ofLogNotice("SourcesEditor") << "No surface selected";
		return;
	}
	if (selectors[0]->size()) {
		selectors[0]->draw();
	}
	if (selectors[1]->size()) {
		selectors[1]->draw();
	}
	if (selectors[2]->size()) {
		selectors[2]->draw();
	}

}

void SourcesEditor::disable() {
	ofUnregisterKeyEvents(this, OF_EVENT_ORDER_BEFORE_APP);
	if (selectors[0]->size()) {
		selectors[0]->disable();
	}
	if (selectors[1]->size()) {
		selectors[1]->disable();
	}
	if (selectors[2]->size()) {
		selectors[2]->disable();
	}
}

void SourcesEditor::enable() {
	selectedToggle = 0;
	selectedGui = 0;
	ofRegisterKeyEvents(this, OF_EVENT_ORDER_BEFORE_APP);
	// Don't enable if there is no surface selected
	if (surfaceManager->getSelectedSurface() == 0) {
		ofLogNotice("SourcesEditor")
				<< "No surface selected. Not enabling and not showing source list.";
		return;
	}
	if (selectors[0]->size()) {
		selectors[0]->enable();
	}
	if (selectors[1]->size()) {
		selectors[1]->enable();
	}
	if (selectors[2]->size()) {
		selectors[2]->enable();
	}
	BaseSource * source = surfaceManager->getSelectedSurface()->getSource();

	// TODO: getPath should be replaced with something like getId() as now we
	//       use paths for loadable sources and names for FBOs
	if (source->getType() == SourceType::SOURCE_TYPE_FBO) {
		selectSourceRadioButton(source->getName());
	} else {
		selectSourceRadioButton(source->getPath());
	}
}

void SourcesEditor::setSurfaceManager(SurfaceManager * newSurfaceManager) {
	surfaceManager = newSurfaceManager;
}

void SourcesEditor::setCmdManager(CmdManager * cmdManager) {
	_cmdManager = cmdManager;
}

void SourcesEditor::setMediaServer(MediaServer * newMediaServer) {
	// If the new media server is not valid
	if (newMediaServer == 0) {
		// Log an error and return from the routine
		ofLogFatalError("SourcesEditor") << "New media server is 0";
		exit(EXIT_FAILURE);
	}
	// Attempt to clear existing media server and assign new one
	clearMediaServer();
	//cout << "old ms addr: " << mediaServer << endl;
	//cout << "new ms addr: " << newMediaServer << endl;
	mediaServer = newMediaServer;
	isMediaServerExternal = true;
}

MediaServer * SourcesEditor::getMediaServer() {
	return mediaServer;
}

void SourcesEditor::selectSourceRadioButton(string & sourcePath) {
	if (sourcePath == "") {
		ofLogNotice("SourcesEditor") << "Path is empty";
		if (selectors[0]->size()) {
			selectors[0]->unselectAll();
		}
		if (selectors[1]->size()) {
			selectors[1]->unselectAll();
		}
		if (selectors[2]->size()) {
			selectors[2]->unselectAll();
		}
		return;
	} else {
		// Check image selector first
		bool imageRadioSelected = false;
		bool videoRadioSelected = false;
		bool fboRadioSelected = false;

		if (selectors[0]->size()) {
			videoRadioSelected = selectors[0]->selectItemByValue(sourcePath);
		}
		if (selectors[1]->size()) {
			imageRadioSelected = selectors[1]->selectItemByValue(sourcePath);
		}
		if (selectors[2]->size()) {
			fboRadioSelected = selectors[2]->selectItemByValue(sourcePath);
		}
		if (imageRadioSelected || videoRadioSelected || fboRadioSelected) {
			return;
		}
		// Log warning if we are still here
		ofLogWarning("SourcesEditor")
				<< "Could not find option in any of the source lists";
	}
}

void SourcesEditor::addMediaServerListeners() {
	// Check if the media server is valid
	if (mediaServer == 0) {
		ofLogError("SourcesEditor::addMediaServerListeners",
				"Media server not set");
		return;
	}
	// Add listeners to custom events of the media server
	ofAddListener(mediaServer->onImageAdded, this,
			&SourcesEditor::handleImageAdded);
	ofAddListener(mediaServer->onImageRemoved, this,
			&SourcesEditor::handleImageRemoved);
	ofAddListener(mediaServer->onVideoAdded, this,
			&SourcesEditor::handleVideoAdded);
	ofAddListener(mediaServer->onVideoRemoved, this,
			&SourcesEditor::handleVideoRemoved);
	ofAddListener(mediaServer->onImageLoaded, this,
			&SourcesEditor::handleImageLoaded);
	ofAddListener(mediaServer->onImageUnloaded, this,
			&SourcesEditor::handleImageUnloaded);

	ofAddListener(mediaServer->onFboSourceAdded, this,
			&SourcesEditor::handleFboSourceAdded);
	ofAddListener(mediaServer->onFboSourceRemoved, this,
			&SourcesEditor::handleFboSourceRemoved);
	ofAddListener(mediaServer->onFboSourceLoaded, this,
			&SourcesEditor::handleFboSourceLoaded);
	ofAddListener(mediaServer->onFboSourceUnloaded, this,
			&SourcesEditor::handleFboSourceUnloaded);

}

void SourcesEditor::removeMediaServerListeners() {
	// Check if the media server is valid
	if (mediaServer == 0) {
		ofLogError("SourcesEditor::addMediaServerListeners",
				"Media server not set");
		return;
	}
	// Remove listeners to custom events of the media server
	ofRemoveListener(mediaServer->onImageAdded, this,
			&SourcesEditor::handleImageAdded);
	ofRemoveListener(mediaServer->onImageRemoved, this,
			&SourcesEditor::handleImageRemoved);
	ofRemoveListener(mediaServer->onVideoAdded, this,
			&SourcesEditor::handleVideoAdded);
	ofRemoveListener(mediaServer->onVideoRemoved, this,
			&SourcesEditor::handleVideoRemoved);
	ofRemoveListener(mediaServer->onImageLoaded, this,
			&SourcesEditor::handleImageLoaded);
	ofRemoveListener(mediaServer->onImageUnloaded, this,
			&SourcesEditor::handleImageUnloaded);
	ofRemoveListener(mediaServer->onFboSourceAdded, this,
			&SourcesEditor::handleFboSourceAdded);
	ofRemoveListener(mediaServer->onFboSourceRemoved, this,
			&SourcesEditor::handleFboSourceRemoved);
	ofRemoveListener(mediaServer->onFboSourceLoaded, this,
			&SourcesEditor::handleFboSourceLoaded);
	ofRemoveListener(mediaServer->onFboSourceUnloaded, this,
			&SourcesEditor::handleFboSourceUnloaded);
}

void SourcesEditor::handleImageSelected(string & imagePath) {
	_cmdManager->exec(
			new SetSourceCmd(SourceType::SOURCE_TYPE_IMAGE, imagePath,
					surfaceManager->getSelectedSurface(),
					(SourcesEditor *) this));
}

void SourcesEditor::setImageSource(string & imagePath) {
	// Unselect selected items
	selectors[0]->unselectAll();
	selectors[2]->unselectAll();

	BaseSurface * surface = surfaceManager->getSelectedSurface();
	if (surface == 0) {
		ofLogWarning("SourcesEditor") << "No surface selected";
		return;
	}

	// Unload old media
	BaseSource * source = surface->getSource();
	if (source->isLoadable()) {
		mediaServer->unloadMedia(source->getPath());
	} else {
		mediaServer->unloadMedia(source->getName());
	}

	// Load new image
	surface->setSource(mediaServer->loadImage(imagePath));
}

void SourcesEditor::handleVideoSelected(string & videoPath) {
	_cmdManager->exec(
			new SetSourceCmd(SourceType::SOURCE_TYPE_VIDEO, videoPath,
					surfaceManager->getSelectedSurface(),
					(SourcesEditor *) this));
}

void SourcesEditor::setVideoSource(string & videoPath) {
	// Unselect any selected items
	selectors[2]->unselectAll();
	selectors[1]->unselectAll();

	BaseSurface * surface = surfaceManager->getSelectedSurface();
	if (surface == 0) {
		ofLogWarning("SourcesEditor") << "No surface selected";
		return;
	}

	// Unload old media
	BaseSource * source = surface->getSource();
	if (source->isLoadable()) {
		mediaServer->unloadMedia(source->getPath());
	} else {
		mediaServer->unloadMedia(source->getName());
	}

	// Load new video
	surface->setSource(mediaServer->loadVideo(videoPath));
}

void SourcesEditor::handleFboSelected(string & fboName) {
	_cmdManager->exec(
			new SetSourceCmd(SourceType::SOURCE_TYPE_FBO, fboName,
					surfaceManager->getSelectedSurface(),
					(SourcesEditor *) this));
}

void SourcesEditor::setFboSource(string & fboName) {
	selectors[0]->unselectAll();
	selectors[1]->unselectAll();

	// Get selected surface
	BaseSurface * surface = surfaceManager->getSelectedSurface();
	if (surface == 0) {
		ofLogWarning("SourcesEditor") << "No surface selected";
		return;
	}

	// Unload old media
	BaseSource * source = surface->getSource();
	if (source->isLoadable()) {
		mediaServer->unloadMedia(source->getPath());
	} else {
		mediaServer->unloadMedia(source->getName());
	}

	// Load new FBO
	surface->setSource(mediaServer->loadFboSource(fboName));
}

void SourcesEditor::clearSource() {
	BaseSurface * surface = surfaceManager->getSelectedSurface();

	// Unload old media
	BaseSource * source = surface->getSource();
	if (source->isLoadable()) {
		mediaServer->unloadMedia(source->getPath());
	} else {
		mediaServer->unloadMedia(source->getName());
	}

	// Reset default source
	surface->setSource(surface->getDefaultSource());
}

void SourcesEditor::clearMediaServer() {
	// If mediaServer is local, clear it
	if (!isMediaServerExternal) {
		// Clear all loaded sources
		mediaServer->clear();
		// Destroy the pointer and set it to 0 pointer
		delete mediaServer;
		mediaServer = 0;
	}
}

void SourcesEditor::handleImageAdded(string & path) {
}
void SourcesEditor::handleImageRemoved(string & path) {
}
void SourcesEditor::handleVideoAdded(string & path) {
}
void SourcesEditor::handleVideoRemoved(string & path) {
}
void SourcesEditor::handleImageLoaded(string & path) {
}
void SourcesEditor::handleImageUnloaded(string & path) {
}
void SourcesEditor::handleFboSourceAdded(string & name) {
}
void SourcesEditor::handleFboSourceRemoved(string & name) {
}
void SourcesEditor::handleFboSourceLoaded(string & name) {
}

void SourcesEditor::keyPressed(ofKeyEventArgs& args) {
	selectors[0]->clearColors();
	selectors[1]->clearColors();
	selectors[2]->clearColors();
	ofLogWarning(__func__) << "selectedToggle is : " << selectedToggle
					<< " in Frame " << selectedGui;
	switch (args.key) {
	case OF_KEY_UP:

		if (selectedToggle <= 0) {
			getNextValidGui();
			selectedToggle = selectors[selectedGui]->size() - 1;
				ofLogWarning(__func__) << "selectedToggle is : "
						<< selectedToggle << " in Frame " << selectedGui;
		}else{
			selectedToggle--;
		}
		ofLogWarning(__func__) << "selectedToggle is : " << selectedToggle
				<< " in Frame " << selectedGui;
		break;
	case OF_KEY_DOWN:
		if (selectedToggle == selectors[selectedGui]->size() - 1) {
			//we change GUI
			getNextValidGui();
			selectedToggle = 0;
		} else
			selectedToggle++;
			ofLogWarning(__func__) << "selectedToggle is : " << selectedToggle
					<< " in Frame " << selectedGui;
		;
		break;
	case OF_OSMC_KEY_RETURN:
		selectors[selectedGui]->selectItem(selectedToggle);
		break;
	}
	/**
	 * TODO change colors
	 */
	ofxToggle* toggle =
			static_cast<ofxToggle*>(selectors[selectedGui]->getGuiGroup().getControl(
					selectedToggle));
	toggle->setBackgroundColor(ofColor(0, 170, 250, 220));
}

void SourcesEditor::keyReleased(ofKeyEventArgs& args) {
}

void SourcesEditor::handleFboSourceUnloaded(string & name) {
}

bool SourcesEditor::getNextValidGui() {
	bool found = false;
	int gui = selectedGui;
	for (; gui < (selectors.size() ); gui++){
		ofLogVerbose(__func__) << "selector size : " << selectors[gui]->size();
			if(selectors[gui]->size() == 0){
				ofLogVerbose(__func__) << "selector " << selectors[gui]->getTitle() << " is empty";
			}else{
				ofLogVerbose(__func__) << "selector " << selectors[gui]->getTitle() << " not empty";
				if (gui == selectedGui) continue;
				found = true;
				selectedGui = gui;
				break;

			}
	}
	if (!found)
		selectedGui = 0;
	return found;
}

bool SourcesEditor::getPrevValidGui() {
	bool found = false;
	int gui = selectedGui;
	for (; gui > 0 ; gui--){
		ofLogVerbose(__func__) << "selector size : " << selectors[gui]->size();
			if(selectors[gui]->size() == 0){
				ofLogVerbose(__func__) << "selector " << selectors[gui]->getTitle() << " is empty";
			}else{
				ofLogVerbose(__func__) << "selector " << selectors[gui]->getTitle() << " not empty";
				if (gui == selectedGui) continue;
				found = true;
				selectedGui = gui;
				break;

			}
	}
	if (!found)
		selectedGui = 0;
	return found;
}

} // namespace piMapper
} // namespace ofx
