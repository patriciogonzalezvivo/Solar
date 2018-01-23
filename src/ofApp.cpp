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
    cam.setPosition(0, 0, 1000);
    ofSetCircleResolution(36);
    
    syphon.setName("Solar");
    
    bWriten = false;
}

//--------------------------------------------------------------
void ofApp::update(){

    // TIME CALCULATIONS
    // --------------------------------
#ifndef TIME_ANIMATION
    obs.setTime();
#else
    obs.setJuliaDay(initial_jd + ofGetElapsedTimef() * 7.);
#endif
    TimeOps::JDtoMDY(obs.getJulianDate(), month, day, year);
    date = ofToString(month,2,'0') + "/" + ofToString(int(day),2,'0');
    
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
    
    Vector z = AstroOps::eclipticToEquatorial(obs, Vector(0,HALF_PI,1));
    Vector y = AstroOps::eclipticToEquatorial(obs, Vector(0,0,1));
    Vector x = AstroOps::eclipticToEquatorial(obs, Vector(HALF_PI,0,1));
    
    n_pole = ofPoint(z.x, z.y, z.z).normalize();
    v_equi = ofPoint(y.x, y.y, y.z).normalize();
    s_sols = ofPoint(x.x, x.y, x.z).normalize();
    
    toEarth = planets[2].m_helioC;
    toEarth.normalize();
    
    if (abs(toEarth.dot(v_equi)) > .999999 && !bWriten) {
        Line newLine;
        newLine.A = planets[2].m_helioC;
        newLine.B = toEarth * 90.;

        newLine.text = "Equinox " + date;
        newLine.T = toEarth * 104. + ofPoint(0.,0.,2);

        lines.push_back(newLine);
        bWriten = true;
    }
    else if (abs(toEarth.dot(v_equi)) < .001 && !bWriten) {
        Line newLine;
        newLine.A = planets[2].m_helioC;
        newLine.B = toEarth * 90.;
        
        newLine.text = "Solstice " + date;
        newLine.T = toEarth * 104. + ofPoint(0.,0.,2);
        
        lines.push_back(newLine);
        bWriten = true;
    }
    else {
        bWriten = false;
    }
    
    // HUD
    // --------------------------------
    if (month != prevMonth && int(day) == 1) {
        
        Line newLine;
        newLine.A = toEarth * 80.;
        newLine.B = toEarth * 90.;
        
        newLine.text = month_names[month-1];
        newLine.T = toEarth * 70.;
        
        lines.push_back(newLine);
    }
    else if (int(day) != int(prevDay)) {
        int dow = TimeOps::MJDtoDOW(TimeOps::JDtoMJD(obs.getJulianDate()));
        
        Line newLine;

        if (dow == 0) {
            newLine.A = toEarth * 82.5;
        }
        else {
            newLine.A = toEarth * 85.;
        }
        newLine.B = toEarth.normalize() * 90.;
        lines.push_back(newLine);
    }
    prevMonth = month;
    prevDay = day;
}

void drawString(std::string str, int x , int y) {
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
    
    ofPushMatrix();
    ofSetColor(255.,0.,0.);
    ofTranslate(planets[2].m_helioC);
    ofDrawLine(n_pole * 4.,n_pole * -4.);
    ofDrawLine(v_equi * -4.,v_equi * 4.);
    ofDrawLine(s_sols * 4.,s_sols * -4.);
    
    ofSetColor(255);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD );
    ofDrawBitmapString("N", n_pole * 5.5);
    ofDrawBitmapString("Eq", v_equi * 5.5);
    
    ofPoint axis = s_sols.cross( n_pole ).normalize();
    float angle = acos(s_sols.dot( n_pole ));
    
    ofNoFill();
    ofSetColor(255,0,0,100);
    // https://forum.openframeworks.cc/t/rotate-3d-object-to-align-to-vector/5085/2
    // http://www.euclideanspace.com/maths/algebra/vectors/angleBetween/index.htm
//    ofRotate(ofRadToDeg(angle), axis.x, axis.y, axis.y);
//    ofRotateX(ofRadToDeg(obs.getObliquity()+HALF_PI));
    ofRotateX(ofRadToDeg(obs.getObliquity()));
    
    ofDrawCircle(ofPoint(0.,0.,0.), 4.);
    for (int i = 0; i < 90; i ++) {
        ofPoint p;
        float a = ofDegToRad(i*4);
        p.x = cos(a);
        p.y = sin(a);
        ofDrawLine(p*4.,p*3);
    }
    ofPopMatrix();
    
    ofSetColor(255);
    
    ofDrawLine(toEarth*100., toEarth*90.);
    
    // Moon
    ofFill();
    moon.drawTrail(ofFloatColor(.4));
    moon.drawSphere(ofFloatColor(0.6));
    
    // Hud
    ofSetColor(255);
    for ( int i = 0; i < lines.size(); i++ ) {
        ofDrawLine(lines[i].A, lines[i].B);
        
        if (lines[i].text != "") {
            ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD );
            ofDrawBitmapString (lines[i].text, lines[i].T);
        }
    }
    
    ofPopMatrix();
    cam.end();
    ofDisableDepthTest();
    
    drawString(ofToString(year) + "/" + date, ofGetWidth()*.5, ofGetHeight()-30);
    
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
