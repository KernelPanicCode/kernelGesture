#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	//OF_FULLSCREEN
	//OF_WINDOW
	ofSetupOpenGL(&window, 800, 600, OF_WINDOW);
	ofRunApp(new testApp());
}
