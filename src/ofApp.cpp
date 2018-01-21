#include "ofApp.h"

#include "GeoLoc/src/GeoLoc.h"
#include "Astro/src/AstroOps.h"

#include "TimeOps.h"
double initial_jd;

const std::string month_names[] = { "ENE", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

//--------------------------------------------------------------
void ofApp::setup(){
    double lng = 0.;
    double lat = 0.;
    geoLoc(lng, lat, ofToDataPath(GEOIP_DB), ofToDataPath(GEOLOC_FILE));
    obs = Observer(lng, lat);
    
    initial_jd = obs.getJulianDate();
    
    scale = 100.;

    BodyId planets_names[] = { MERCURY, VENUS, EARTH, MARS, JUPITER, SATURN, URANUS, NEPTUNE, PLUTO, LUNA };
    float planets_sizes[] = { 0.0561, 0.1377, 0.17, 0.255, 1.87*.5, 1.615*.5, 0.68, 0.68, 0.0306, 0.0459 };
    
    sun = Body(SUN);
    luna = Luna();
    moon = ofxBody(LUNA, 0.5);
    for (int i = 0; i < 9; i++) {
        planets.push_back(ofxBody(planets_names[i], planets_sizes[i] * 10.));
    }
    
    ofSetBackgroundColor(0);
    cam.setDistance(20);
    cam.setPosition(0, 0, 100);
    
    syphon.setName("Solar");
}

//--------------------------------------------------------------
void ofApp::update(){

    // TIME CALCULATIONS
    // --------------------------------
#ifndef TIME_ANIMATION
    obs.setTime();
#else
    obs.setJuliaDay(initial_jd + ofGetElapsedTimef() * 5.);
#endif
    TimeOps::JDtoMDY(obs.getJulianDate(), month, day, year);
    
    // BODIES
    // --------------------------------
    
    // Update sun position
    sun.compute(obs);
    
    // Update planets positions
    for ( int i = 0; i < planets.size(); i++) {
        planets[i].compute(obs);
        planets[i].m_helioC = planets[i].getHelioPosition() * scale;
    }
    
    // Update moon position (the distance from the earth is not in scale)
    luna.compute(obs);
    moon.compute(obs);
    moon.m_helioC = ( moon.getGeoPosition() * 20*scale ) + ( planets[2].getHelioPosition() * scale);
    
    // HUD
    // --------------------------------
    if (month != prevMonth && int(day) == 1) {
        ofVec3f toEarth = planets[2].m_helioC;
        Line newLine;
        newLine.A = toEarth.normalize() * 80.;
        newLine.B = toEarth.normalize() * 90.;
        
        newLine.text = month_names[month-1];
        newLine.T = toEarth.normalize() * 70.;
        
        lines.push_back(newLine);
    }
    else if (int(day) != int(prevDay)) {
        int dow = TimeOps::MJDtoDOW(TimeOps::JDtoMJD(obs.getJulianDate()));
        
        ofVec3f toEarth = planets[2].m_helioC;
        Line newLine;

        if (dow == 0) {
            newLine.A = toEarth.normalize() * 82.5;
        }
        else {
            newLine.A = toEarth.normalize() * 85.;
        }
        newLine.B = toEarth.normalize() * 90.;
        lines.push_back(newLine);
    }
    prevMonth = month;
    prevDay = day;
}

void drawString(std::string &str, int x , int y) {
    ofSetColor(255);
    ofSetDrawBitmapMode(OF_BITMAPMODE_SIMPLE);
    ofDrawBitmapStringHighlight(str, x - str.length() * 4, y);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofEnableDepthTest();
    ofEnableAlphaBlending();
    
    cam.setTarget(planets[2].m_helioC);
    cam.roll(90);
    
    cam.begin();
    ofPushMatrix();
    
    // Sun
    ofSetColor(255);
    ofDrawSphere(10);
    
    // Planets
    for ( int i = 0; i < planets.size(); i++) {
        planets[i].drawTrail(ofFloatColor(.5));
        planets[i].drawSphere(ofFloatColor(.9));
    }
    
    // Moon
    moon.drawTrail(ofFloatColor(.8, 0., 0.));
    moon.drawSphere(ofFloatColor(1., 0., 0.));
    
    // Hud
    ofSetColor(255);
    for ( int i = 0; i < lines.size(); i++ ) {
        ofDrawLine(lines[i].A, lines[i].B);
        
        if (lines[i].text != "") {
            ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD );
            ofDrawBitmapString(lines[i].text, lines[i].T);
        }
    }
    
    ofPopMatrix();
    cam.end();
    ofDisableDepthTest();
    
    std::string date = ofToString(year) + "/" + ofToString(month,2,'0') + "/" + ofToString(int(day),2,'0');
    drawString(date, ofGetWidth()*.5, ofGetHeight()-30);
    
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
