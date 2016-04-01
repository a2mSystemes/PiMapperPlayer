#include "Application.h"
#include "PresentationState.h"
#include "ofConstants.h"


namespace ofx {
namespace piMapper {

Application::Application(){
	playerName = "defaultPlayer";
	ofSetBackgroundColor(ofColor::black);
	_surfaceManager.setMediaServer(&_mediaServer);
	_gui.setMediaServer(&_mediaServer);
	_gui.setCmdManager(&_cmdManager);
	_gui.setSurfaceManager(&_surfaceManager);
	
	setState(PresentationState::instance());
	ofAddListener(ofEvents().keyPressed, this, &Application::onKeyPressed);
	ofAddListener(ofEvents().keyReleased, this, &Application::onKeyReleased);
	ofAddListener(ofEvents().mousePressed, this, &Application::onMousePressed);
	
	ofAddListener(Gui::instance()->jointPressedEvent, this, &Application::onJointPressed);
	ofAddListener(Gui::instance()->surfacePressedEvent, this, &Application::onSurfacePressed);
	ofAddListener(Gui::instance()->backgroundPressedEvent, this, &Application::onBackgroundPressed);
}

void Application::setup(){
	if(!loadXmlSettings(PIMAPPER_USER_SURFACES_XML_FILE)){
		ofLogWarning("Application::setup()") << "Failed to load user settings. Populated default one" << endl;
	}
	ofSetBackgroundColor(ofColor::black);
	ofEnableAntiAliasing();
	ofSetFrameRate(25);
	ofHideCursor();
	consoleListener.setup(this);
	_osc.setup(5250);
	ofAddListener(ofxOscEvent::packetIn, this, &Application::onPacketIn);
}

ApplicationBaseState * Application::getState(){
	return _state;
}

void Application::draw(){
	_state->draw(this);
	_info.draw();
}

// Here we handle application state changes only
void Application::onKeyPressed(ofKeyEventArgs & args){

	// For now we set the state of the new system and also the old
	// before it is completely ported to the state system.

	switch(args.key){
	 case OF_KEY_SHIFT:
		 _shiftKeyDown = true;
		 break;
		 
	 case '/':
		 _shiftKeyDown = !_shiftKeyDown;
		 break;
		 
	 case OF_KEY_HOME:
		 _cmdManager.exec(
			 new ofx::piMapper::SetApplicationStateCmd(
				 this, PresentationState::instance(),
				 &_gui, GuiMode::NONE));
		 break;

	 case '2':
		 _cmdManager.exec(
			 new ofx::piMapper::SetApplicationStateCmd(
				 this, TextureMappingState::instance(),
				 &_gui, GuiMode::TEXTURE_MAPPING));
		 break;

	 case '3':
		 _cmdManager.exec(
			 new ofx::piMapper::SetApplicationStateCmd(
				 this, ProjectionMappingState::instance(),
				 &_gui, GuiMode::PROJECTION_MAPPING));
		 break;

	 case '4':
		 _cmdManager.exec(
			 new ofx::piMapper::SetApplicationStateCmd(
				 this, SourceSelectionState::instance(),
				 &_gui, GuiMode::SOURCE_SELECTION));
		 break;
	 case OF_OSMC_KEY_CONTEXT_MENU:
		 ofLogVerbose(__func__) << args.key << " was pressed";
		 _cmdManager.exec(
			 new ofx::piMapper::SetApplicationStateCmd(
				 this, PlaylistSelectionState::instance(),
				 &_gui, GuiMode::PLAYLIST_SELECTION_MODE));
		 break;

	 case 'f':
		 ofToggleFullscreen();
		 break;

	 case 'M':
		 ofShowCursor();
		 break;

	 case 'm':
		 ofHideCursor();
		 break;

	 case 'i':
		 _info.toggle();
		 break;

	 case 's':
		 _surfaceManager.saveXmlSettings(SettingsLoader::instance()->getLastLoadedFilename());
		 break;

	 case 'z':
		 _cmdManager.undo();
		 break;

	 default:
		 // All the other keypresses are handled by the application state onKeyPressed
		 _state->onKeyPressed(this, args);
		 break;
	}
}

void Application::onKeyReleased(ofKeyEventArgs & args){
	switch(args.key){
	 case OF_KEY_SHIFT:
		_shiftKeyDown = false;
		break;
	}
}

void Application::onMousePressed(ofMouseEventArgs & args){}

void Application::onJointPressed(GuiJointEvent & e){
	_state->onJointPressed(this, e);
}

void Application::onSurfacePressed(GuiSurfaceEvent & e){
	_state->onSurfacePressed(this, e);
}

void Application::onBackgroundPressed(GuiBackgroundEvent & e){
	_state->onBackgroundPressed(this, e);
}

void Application::addFboSource(FboSource & fboSource){
	_mediaServer.addFboSource(fboSource);
}

void Application::setState(ApplicationBaseState * st){
	_state = st;
}

bool Application::isShiftKeyDown(){
	return _shiftKeyDown;
}

void Application::onCharacterReceived(KeyListenerEventData & e){
	ofKeyEventArgs args;
	args.key = (int)e.character;
	
	// These if's have been added because we get
	// capital letters A, B, C and D when pressing
	// arrows on the keyboard via terminal.
	
	switch(args.key){
	 case 'A':
		args.key = OF_KEY_UP;
		break;
	 case 'B':
		args.key = OF_KEY_DOWN;
		break;
	 case 'C':
		args.key = OF_KEY_RIGHT;
		break;
	 case 'D':
		args.key = OF_KEY_LEFT;
		break;
	}
	
	cout << "TermListener: " << e.character << endl;
	onKeyPressed(args);
};

bool Application::loadXmlSettings(string fileName){
	if(!ofFile::doesFileExist(fileName)){
		ofLogError("Application::loadXmlSettings()") << fileName << " does not exist";
		ofFile newFile(PIMAPPER_DEF_SURFACES_XML_FILE);
		newFile.copyTo(PIMAPPER_USER_SURFACES_XML_FILE);
	}
	if(!_surfaceManager.loadXmlSettings(fileName)){
		ofLogError("Application::loadXmlSettings()") << "Failed to load " << fileName << endl;
		return false;
	}
	return true;
}

void Application::onPacketIn(ofxOscMessage& m) {
	ofLogVerbose(__func__) << "receive oscMessage with adress : " << m.getAddress();
	if(m.getAddress() == "/all/play"){ // play the loaded file
		ofKeyEventArgs args;
		args.key = OF_OSMC_KEY_PLAYPAUSE;
		ofNotifyEvent( ofEvents().keyPressed, args );
		return;
	}else if(m.getAddress() == "/all/loadFile"){ // load a file need to send a OK response

	}else if(m.getAddress() == "/all/restart"){ // load a file need to send a OK response
		ofKeyEventArgs args;
		args.key = OF_OSMC_KEY_STOP;
		ofNotifyEvent( ofEvents().keyPressed, args );
		return;
	}else if(m.getAddress() == "/" + playerName + "/playlistSelectionMode/enter"){ //player "playerName" need to go in selection mode
		 _cmdManager.exec(
			 new ofx::piMapper::SetApplicationStateCmd(
				 this, PlaylistSelectionState::instance(),
				 &_gui, GuiMode::PLAYLIST_SELECTION_MODE));
	}else if(m.getAddress() == "/" + playerName + "/playlistSelectionMode/quit"){ //player "playerName" need to leave in selection mode
		 _cmdManager.exec(
			 new ofx::piMapper::SetApplicationStateCmd(
				 this, PresentationState::instance(),
				 &_gui, GuiMode::NONE));
	}else if(m.getAddress() == "/key/up"){ // send key event up
		ofKeyEventArgs args;
		args.key = OF_KEY_UP;
		ofNotifyEvent( ofEvents().keyPressed, args );
		return;
	}else if (m.getAddress() == "/key/down"){ // send key event down
		ofKeyEventArgs args;
		args.key = OF_KEY_DOWN;
		ofNotifyEvent( ofEvents().keyPressed, args );
		return;
	}else if (m.getAddress() == "/key/return"){ // send key event return
		ofKeyEventArgs args;
		args.key = OF_OSMC_KEY_RETURN;
		ofNotifyEvent( ofEvents().keyPressed, args );
		return;
	}else if(m.getAddress() == "/key/backspace"){
		ofKeyEventArgs args;
		args.key = OF_OSMC_KEY_BACK;
		ofNotifyEvent( ofEvents().keyPressed, args );
		return;
	}else if (m.getAddress() == "/key/space"){
		ofKeyEventArgs args;
		args.key = ' ';
		ofNotifyEvent( ofEvents().keyPressed, args );
		return;
	}
}
const std::string& Application::getPlayerName() const {
	return playerName;
}

void Application::setPlayerName(const std::string& playerName) {
	this->playerName = playerName;
}

} // namespace piMapper
} // namespace ofx
