#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	//OF_FULLSCREEN
	//OF_WINDOW
	ofSetupOpenGL(&window, 640, 480, OF_FULLSCREEN);
	ofRunApp(new testApp());
}
