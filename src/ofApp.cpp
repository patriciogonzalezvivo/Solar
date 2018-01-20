#include "ofApp.h"

#include "GeoLoc/src/GeoLoc.h"
#include "Astro/src/AstroOps.h"

#include "TimeOps.h"
double initial_jd;

//--------------------------------------------------------------
void ofApp::setup(){
    double lng = 0.;
    double lat = 0.;
    geoLoc(lng, lat, ofToDataPath(GEOIP_DB), ofToDataPath(GEOLOC_FILE));
    obs = Observer(lng, lat);
    
    initial_jd = obs.getJulianDate();

    BodyId planets_names[] = { MERCURY, VENUS, EARTH, MARS, JUPITER, SATURN, URANUS, NEPTUNE, PLUTO, LUNA };
    float planets_sizes[] = { 0.0561, 0.1377, 0.17, 0.255, 1.87, 1.615, 0.68, 0.68, 0.0306, 0.0459 };
    
    sun = Body(SUN);
    moon = ofxBody(LUNA, 0.5);
    for (int i = 0; i < 9; i++) {
        planets.push_back(ofxBody(planets_names[i], planets_sizes[i] * 10.));
    }
    
    ofSetBackgroundColor(0);
    cam.setDistance(20);
    
    syphon.setName("Solar");
}

//--------------------------------------------------------------
void ofApp::update(){

    // TIME CALCULATIONS
#ifndef TIME_ANIMATION
    obs.setTime();
#else
    obs.setJuliaDay(initial_jd + ofGetElapsedTimef() * 5.);
#endif
    TimeOps::JDtoMDY(obs.getJulianDate(), month, day, year);
    
    // BODIES
    
    // Update sun position
    sun.compute(obs);
    
    // Update planets positions
    for ( int i = 0; i < planets.size(); i++) {
        planets[i].compute(obs);
        planets[i].m_helioC = planets[i].getHelioPosition() * 100.;
    }
    
    // Update moon position
    moon.compute(obs);
    moon.m_helioC = ( moon.getGeoPosition() * 2000. ) + ( planets[2].getHelioPosition() * 100. );
}

std::string getDateString(Observer &_obs) {
    double day = 0.0;
    int month, year = 0;
    
    return ofToString(year) + "/" + ofToString(month,2,'0') + "/" + ofToString(int(day),2,'0');
}

void drawBillboard(std::string &str, int x , int y) {
    ofFill();
    ofSetColor(0);
    ofDrawRectangle(x - str.length() * 5, y-15, str.length() * 10, 20);
    ofSetColor(255);
    ofDrawBitmapString(str, x - str.length() * 4, y);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofEnableDepthTest();
    
    cam.setTarget(planets[2].m_helioC);
    cam.roll(90);
    
    cam.begin();
    ofPushMatrix();
    
    // Sun
    ofSetColor(255);
    ofDrawSphere(6);
    
    for ( int i = 0; i < planets.size(); i++) {
        planets[i].drawTrail(ofFloatColor(.5));
        planets[i].drawSphere(ofFloatColor(.9));
    }
    
    moon.drawTrail(ofFloatColor(.8, 0., 0.));
    moon.drawSphere(ofFloatColor(1., 0., 0.));
    
    ofPopMatrix();
    cam.end();
    ofDisableDepthTest();
    
//    cout << ofToString( planets[2].m_helioC.length() ) << endl;
    
    std::string date = getDateString(obs);
    drawBillboard(date, ofGetWidth()*.5, ofGetHeight()-30);
    
    syphon.publishScreen();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
