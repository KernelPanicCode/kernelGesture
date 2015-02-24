#pragma once

#include "ofMain.h"
#include "ofxFaceTracker.h"
#include "ofxOscSender.h"
#include "ofxOscMessage.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void sendMessage();
	
	ofVideoGrabber cam;
	ofxFaceTracker tracker;
	ExpressionClassifier classifier;
	ofxOscSender sender;
};
