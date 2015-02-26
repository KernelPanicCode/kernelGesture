#pragma once

#include "ofMain.h"
#include "ofxFaceTracker.h"
#include "ofxOscSender.h"
#include "ofxOscReceiver.h"
#include "ofxOscMessage.h"

#define numGesture 10
#define fadeTotal 2500

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void sendMessage();
	void cleanList();
	void addConcurrency(int index);
	int mean();
	void print();
	void oscReceiverUpdate();
	ofVideoGrabber cam;
	ofxFaceTracker tracker;
	ExpressionClassifier classifier;
	ofxOscSender sender;
	ofxOscReceiver receiver;
	ofPixels gray;
	ofImage edge;
	bool sended;
	std::vector<int> concurrency;
	float fade;
	int state;
};
