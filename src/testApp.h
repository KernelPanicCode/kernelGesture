#pragma once

#include "ofMain.h"
#include "ofxFaceTracker.h"
#include "ofxOscSender.h"
#include "ofxOscMessage.h"

#define numGesture 10

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
	
	ofVideoGrabber cam;
	ofxFaceTracker tracker;
	ExpressionClassifier classifier;
	ofxOscSender sender;
	ofPixels gray;
	ofImage edge;
	bool sended;
	std::vector<int> concurrency;
};
