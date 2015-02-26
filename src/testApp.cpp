#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void testApp::setup() {
	ofSetVerticalSync(true);
	cam.initGrabber(640, 480);
	
	tracker.setup();
	tracker.setRescale(.5);

	sender.setup("127.0.0.1",9090);
	classifier.load("expressions");

	sended =  false;
	
	cleanList();
}
void testApp::cleanList(){
	concurrency.assign(classifier.size(),0);
	cout<<"Limpiando"<<endl;
}
void testApp::addConcurrency(int index){
	int &con = concurrency.at(index);
	con = con ++;
	cout<<"Agregado: "<<index<<endl;
}
void testApp::print(){
	for(std::vector<int>::iterator it = concurrency.begin(); it != concurrency.end();++it)
	{
		cout<<*it<<endl;
	}
}
int testApp::mean(){
	int bigIndex = -1;
	int big = 0;
	int i = 0;
	for(std::vector<int>::iterator it = concurrency.begin(); it != concurrency.end();++it)
	{
		if(*it > big){
			big = *it;
			bigIndex = i;
		}
		i++;
	}

}
void testApp::sendMessage(){
	ofxOscMessage msg;
	
	int meanIndex = -1;

	meanIndex = mean();

	if(meanIndex == -1)
		return;
	msg.setAddress("/gesto");
	//ex = classifier.getPrimaryExpression();

	msg.addIntArg(meanIndex);
	msg.addFloatArg((double)classifier.getProbability(meanIndex));


	cout<<"mensaje: "<<meanIndex<<endl;
	sender.sendMessage(msg);
}
void testApp::update() {
	cam.update();
	
	if(cam.isFrameNew()) {
		if(tracker.update(toCv(cam))) {
			classifier.classify(tracker);
			addConcurrency( classifier.getPrimaryExpression() );
		}
		convertColor(cam,gray,CV_RGB2GRAY);		
		Canny(gray,edge,3,184,3);
		edge.update();
	}
	unsigned long long timeEl = ofGetElapsedTimeMillis()%10000;

	if(timeEl > 9500 && timeEl < 9999 && !sended){
		sendMessage();
		sended = true;
		cleanList();
	}
	if(timeEl >= 9500 && timeEl < 9999 && sended){

	}
	if(timeEl < 9500 )
		sended = false;
}

void testApp::draw() {
	ofSetColor(255);
	//cam.draw(0, 0);
	//tracker.draw();
	edge.draw(0,0);
	
	int w = 100, h = 12;
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(5, 10);
	int n = classifier.size();
	int primary = classifier.getPrimaryExpression();
  for(int i = 0; i < n; i++){
		ofSetColor(i == primary ? ofColor::red : ofColor::black);
		ofRect(0, 0, w * classifier.getProbability(i) + .5, h);
		ofSetColor(255);
		ofDrawBitmapString(classifier.getDescription(i), 5, 9);
		ofTranslate(0, h + 5);
  }
	ofPopMatrix();
	ofPopStyle();
	
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), ofGetWidth() - 20, ofGetHeight() - 10);
	drawHighlightString(
		string() +
		"r - reset\n" +
		"e - add expression\n" +
		"a - add sample\n" +
		"s - save expressions\n"
		"l - load expressions",
		14, ofGetHeight() - 7 * 12);
}

void testApp::keyPressed(int key) {
	if(key == 'r') {
		tracker.reset();
		classifier.reset();
	}
	if(key == 'e') {
		classifier.addExpression();
	}
	if(key == 'a') {
		classifier.addSample(tracker);
	}
	if(key == 's') {
		classifier.save("expressions");
	}
	if(key == 'l') {
		classifier.load("expressions");
	}
}