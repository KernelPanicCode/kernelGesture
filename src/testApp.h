#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxFaceTracker.h"
#include "ofxOscSender.h"
#include "ofxOscReceiver.h"
#include "ofxOscMessage.h"

#define fadeTotal 500
#define fadeInicial 200
#define camx 640
#define camy 480

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int);
	void sendMessage();
	void cleanList();
	void addConcurrency(int,double);
	void mean(int&,int&);
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
	std::vector<double> probability;
	float fade;
	int state;
	int noiseX;
	int noiseY;
	float segIni;
};
