#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void testApp::setup() {
	ofSetVerticalSync(true);
	cam.listDevices();
	cam.setDeviceID(0);
	cam.initGrabber(640, 480);
	
	tracker.setup();
	tracker.setRescale(.5);

	sender.setup("127.0.0.1",9090);
	receiver.setup(9091);
	classifier.load("expressions");

	state = 0;
	fade = 0;
	cleanList();
}
void testApp::cleanList(){
	concurrency.assign(classifier.size(),0);
	cout<<"Limpiando"<<endl;
}
void testApp::addConcurrency(int index){
	int &con = concurrency.at(index);
	con = con + 1;
	//cout<<"Agregado: "<<index<<endl;
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
	return bigIndex;
}
void testApp::sendMessage(){
	ofxOscMessage msg;
	
	int meanIndex = -1;

	meanIndex = mean();

	if(meanIndex == -1)
		return;
	msg.setAddress("/gesto");
	
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
		//print();
		sendMessage();
		sended = true;
		cleanList();
	}
	if(timeEl >= 9500 && timeEl < 9999 && sended){

	}
	if(timeEl < 9500 )
		sended = false;

	oscReceiverUpdate();
}
void testApp::oscReceiverUpdate(){
	ofxOscMessage ms;
	while(receiver.hasWaitingMessages()){
		receiver.getNextMessage( &ms );
		if ( ms.getAddress().compare("/control") == 0 )
		{
			int st;
			st = ms.getArgAsInt32(0);
			if(st >=0 && st <=2){
				state = st;
				if(state == 2)
			    	fade = 0;
			    if(state == 1)
			    	fade = fadeTotal;
			}
		}
	}
}
void testApp::draw() {
	ofSetColor(255);
	//cam.draw(0,0);
	//tracker.draw();
	edge.draw( 0,0);
	
	int w = 250, h = 30;
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(5, 10);
	int n = classifier.size();
	int primary = classifier.getPrimaryExpression();
  for(int i = 0; i < n; i++){
		ofSetColor(i == primary ? ofColor::red : ofColor::black,125);
		ofRect(0, 0, w * classifier.getProbability(i) + .5, h);
		ofSetColor(255);
		ofDrawBitmapString(classifier.getDescription(i), 5, 9);
		ofTranslate(0, h + 5);
  }
	ofPopMatrix();
	ofPopStyle();
	
	//ofDrawBitmapString(ofToString((int) ofGetFrameRate()), ofGetWidth() - 20, ofGetHeight() - 10);
	/*drawHighlightString(
		string() +
		"r - reset\n" +
		"e - add expression\n" +
		"a - add sample\n" +
		"s - save expressions\n"
		"l - load expressions",
		14, ofGetHeight() - 7 * 12);
	*/
	drawHighlightString(
		string() +
		"Nom: - Fer\n" +
		"year: 198x\n" +
		"gen - fem\n" +
		"status - ......",
		14, ofGetHeight() - 7 * 12);


	if(state == 0 ){
		ofSetColor(ofColor::black,255);
		ofRect(0,0,ofGetWidth(),ofGetHeight());
	}
	if(state==1&& fade >  0){
		ofSetColor(ofColor::black,ofMap(fade,0,fadeTotal,0,255));
		ofRect(0,0,ofGetWidth(),ofGetHeight());
		if(fade > 250)
			fade -= 0.5;
		else
			fade--;
	}
	if(state == 2){
		ofSetColor(ofColor::black,ofMap(fade,0,fadeTotal,0,255));
		ofRect(0,0,ofGetWidth(),ofGetHeight());
		if(fade <fadeTotal)
			fade++;
	}
	
}

void testApp::keyPressed(int key) {
	cout<<key<<endl;
	if(key==267){
        ofSetFullscreen(ofGetWindowMode()==OF_WINDOW);
    }
    
    if(key == 8 && state > 0){
    	state--;
    	if(state == 2)
	    	fade = 0;
	    if(state == 1)
			fade = fadeTotal;
    }
    
    if(key == 13 && state <2){	
		state++;
		if(state == 2)
	    	fade = 0;
	    if(state == 1)
	    	fade = fadeTotal;

	}
    
	/*if(key == 'r') {
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
	}*/
}