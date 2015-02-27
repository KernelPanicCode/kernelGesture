#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void testApp::setup() {

	ofSetVerticalSync(true);

	/*Setttings ->>>>> */
	ofxXmlSettings settings;
	settings.loadFile("settings.xml");

	int device = settings.getValue("settings:cam", 0);
	string ipSend = settings.getValue("settings:ipSend","127.0.0.1");
	int portSend = settings.getValue("settings:portSend",9000);
	int portControl = settings.getValue("settings:portControl",9091);


	/*<<<<<< --------  Setttings */
	cout<<"ipSend"<<ipSend<<endl;
	cout<<"iport"<<portSend<<endl;
	cam.listDevices();
	cam.setDeviceID(device);
	cam.initGrabber(camx, camy);
	
	tracker.setup();
	tracker.setRescale(.5);

	sender.setup(ipSend,portSend);
	receiver.setup(portControl);
	classifier.load("expressions");

	state = 0;
	fade = 0;
	noiseX = camx;
	noiseY = camy;
	segIni = 0.0;
	cleanList();
}
void testApp::cleanList(){
	concurrency.assign(classifier.size(),0);
	probability.assign(classifier.size(),0.0);
	//cout<<"Limpiando"<<endl;
}
void testApp::addConcurrency(int index,double prob){
	int &con = concurrency.at(index);
	con = con + 1;
	double &dob = probability.at(index);
	dob = dob + prob;
	//cout<<"Agregado: "<<index<<endl;
}
void testApp::print(){
	for(std::vector<double>::iterator it = probability.begin(); it != probability.end();++it)
	{
		cout<<"print: "<<*it<<endl;
	}
}
void testApp::mean(int &index, int &prob){
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
	if(bigIndex <0 || bigIndex >= classifier.size()){
		prob = 0;
		index = -1;
	}
	else
	{
		double &d = probability.at(bigIndex);
		if(big == 0 || d == 0){
			prob = 0;
			index= 0;
		}
		else
		{
			//cout<<"Index:"<<bigIndex<<"Prob:"<<d<<"conc"<<big<<endl;
			prob = (int)round((d / big)*100);
			index = bigIndex;
		}
	}
}
void testApp::sendMessage(){
	ofxOscMessage msg;
	
	int meanIndex = -1;
	int prob= 0.0;

	mean(meanIndex,prob);

	if(meanIndex < 0 ||meanIndex >= classifier.size()||prob <=0 )
	{	cout<<"Sin msj"<<endl;
		return;
	}
	msg.setAddress("/gesto");
	
	msg.addIntArg(meanIndex);
	msg.addIntArg(prob);
	msg.addStringArg(classifier.getDescription(meanIndex));	

	cout<<"mensaje: "<<meanIndex<<"Prob"<<prob<<"Msg"<<classifier.getDescription(meanIndex)<<endl;
	sender.sendMessage(msg);
}
void testApp::update() {
	cam.update();
	
	if(cam.isFrameNew()) {
		if(tracker.update(toCv(cam))) {
			classifier.classify(tracker);
			int ind = classifier.getPrimaryExpression();
			if(ind >= 0 && ind < classifier.size())
				addConcurrency( ind, classifier.getProbability(ind) );
		}
		convertColor(cam,gray,CV_RGB2GRAY);		
		Canny(gray,edge,noiseX,noiseY,3);
		edge.update();
	}
	unsigned long long timeEl = ofGetElapsedTimeMillis()%20000;

	if(timeEl > 19500 && timeEl < 19999 && !sended){
		//print();
		sendMessage();
		sended = true;
		cleanList();
	}
	if(timeEl >= 19500 && timeEl < 19999 && sended){

	}
	if(timeEl < 19500 )
		sended = false;

	if(ofGetElapsedTimef() - segIni < 60 * 8){
		noiseX = ofMap(ofGetElapsedTimef()- segIni,0,60*8,640,0);
		noiseY = ofMap(ofGetElapsedTimef() - segIni,0,60*8,480,0);
	}else
	{
		noiseX = 0;
		noiseY = 0;
	}

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
			    if(state == 1){
			    	fade = fadeInicial;
			    	segIni = ofGetElapsedTimef();
			    }
			}
		}
	}
}
void testApp::draw() {
	ofSetColor(255);
	//cam.draw(camx,camy);
	//tracker.draw();
	edge.draw( 0,0,ofGetWidth(),ofGetHeight());
	
	int w = 250, h = 30;
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(5, 10);
	int n = classifier.size();
	int primary = classifier.getPrimaryExpression();
  for(int i = 0; i < n; i++){
		ofSetColor(i == primary ? ofColor::red : ofColor::white,115);
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
		ofSetColor(ofColor::black,ofMap(fade,0,fadeInicial,0,255));
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
	if(key==267){
        ofSetFullscreen(ofGetWindowMode()==OF_WINDOW);
    }
    
    if(key == 8 && state > 0){
    	state--;
    	if(state == 2)
	    	fade = 0;
	    if(state == 1)
			fade = fadeInicial;
    }
    
    if(key == 13 && state <2){	
		state++;
		if(state == 2)
	    	fade = 0;
	    if(state == 1){
	    	fade = fadeInicial;
	    	segIni = ofGetElapsedTimef();
	    }

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