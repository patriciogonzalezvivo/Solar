#include "ofApp.h"

#include "GeoLoc/src/GeoLoc.h"
#include "Astro/src/AstroOps.h"

#include "TimeOps.h"
double initial_jd;

const std::string month_names[] = { "ENE", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

ofPoint coord2EquatorialSphere(Star &_star, float _distance) {
    Vector eclip = _star.getEquatorialVector() * _distance;
    return ofPoint(eclip.x, eclip.y, eclip.z);
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    ofSetBackgroundColor(0);
    ofSetCircleResolution(36);
    
    cam.setPosition(0, 0, 1000);
    
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
    
    billboard.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    billboard.addVertex(ofPoint(-1.,-1));
    billboard.addTexCoord(ofVec2f(0.,1.));
    billboard.addColor(ofFloatColor(1.));
    billboard.addVertex(ofPoint(-1.,1));
    billboard.addTexCoord(ofVec2f(0.,0.));
    billboard.addColor(ofFloatColor(1.));
    billboard.addVertex(ofPoint(1.,1));
    billboard.addTexCoord(ofVec2f(1.,0.));
    billboard.addColor(ofFloatColor(1.));
    billboard.addVertex(ofPoint(1.,-1));
    billboard.addTexCoord(ofVec2f(1.,1.));
    billboard.addColor(ofFloatColor(1.));
    
    shader_moon.load("shaders/moon.vert","shaders/moon.frag");
    
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
    obs.setJuliaDay(initial_jd + ofGetElapsedTimef() * 4.);
#endif
    TimeOps::JDtoMDY(obs.getJulianDate(), month, day, year);
    date = ofToString(year) + "/" + ofToString(month,2,'0') + "/" + ofToString(int(day),2,'0');
    
    // Updating BODIES positions
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
    
    
    // HUDS ELEMENTS
    // --------------------------------
    
    // Equatorial North, Vernal Equinox and Summer Solstice
    Vector z = AstroOps::eclipticToEquatorial(obs, Vector(0, HALF_PI, 1, true)).getEquatorialVector();
    Vector y = AstroOps::eclipticToEquatorial(obs, Vector(0, 0 ,1, true)).getEquatorialVector();
    Vector x = AstroOps::eclipticToEquatorial(obs, Vector(HALF_PI, 0, 1, true)).getEquatorialVector();
    
    n_pole = ofPoint(z.x, z.y, z.z).normalize();
    v_equi = ofPoint(y.x, y.y, y.z).normalize();
    s_sols = ofPoint(x.x, x.y, x.z).normalize();
    
    toEarth = planets[2].m_helioC;
    toEarth.normalize();
    
    // HUD EVENTS
    // --------------------------------
    
    // Moon phases
    float moon_phase = luna.getAge()/Luna::SYNODIC_MONTH;
    int moon_curPhase = moon_phase * 8;
    if (moon_curPhase != moon_prevPhase) {
        moons.push_back(ofxMoon(planets[2].m_helioC.getNormalized() * 110., moon_phase));
        moon_prevPhase = moon_curPhase;
    }
    
    // Equinoxes & Solstices
    if (abs(toEarth.dot(v_equi)) > .999999 && !bWriten) {
        Line newLine;
        newLine.A = planets[2].m_helioC;
        newLine.B = toEarth * 90.;

        newLine.text = "Eq. " + ofToString(int(day),2,'0');
        newLine.T = toEarth * 104. + ofPoint(0.,0.,2);

        lines.push_back(newLine);
        bWriten = true;
    }
    else if (abs(toEarth.dot(v_equi)) < .001 && !bWriten) {
        Line newLine;
        newLine.A = planets[2].m_helioC;
        newLine.B = toEarth * 90.;
        
        newLine.text = "So. " + ofToString(int(day),2,'0');
        newLine.T = toEarth * 104. + ofPoint(0.,0.,2);
        
        lines.push_back(newLine);
        bWriten = true;
    }
    else {
        bWriten = false;
    }
    
    // Year's cycles, Months & Days
    if (oneYearIn == "" ) {
        oneYearIn = ofToString(year+1) + "/" + ofToString(month,2,'0') + "/" + ofToString(int(day),2,'0');
        cout << "One year in day " << oneYearIn << endl;
    }
    else if (oneYearIn == date) {
        oneYearIn = "";
        moons.clear();
        lines.clear();
    }
    else if (month != prevMonth && int(day) == 1) {
        
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
    
    prevYear = year;
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
    
    // Set Camera
    cam.setTarget(planets[2].m_helioC);
    cam.roll(90);
    
    ofEnableDepthTest();
    ofEnableAlphaBlending();
    
    // Set Scene
    cam.begin();
    ofPushMatrix();
    
    // ECLIPTIC HELIOCENTRIC COORD SYSTEM
    // --------------------------------------- begin Ec Helio

    // Draw Sun
    ofSetColor(255);
    ofDrawSphere(10);
    
    // Draw Planets and their orbits (HelioCentric)
    for ( int i = 0; i < planets.size(); i++) {
        planets[i].drawTrail(ofFloatColor(.5));
        planets[i].drawSphere(ofFloatColor(.9));
        
        if (planets[i].getBodyId() != EARTH ) {
            ofSetColor(120, 100);
            ofDrawLine(ofPoint(0.), planets[i].m_helioC);
        }
    }
    
    //    // Check that Geocentric Vector to planets match
    //    ofSetColor(100,100);
    //    for ( int i = 0; i < planets.size(); i++) {
    //        if (planets[i].getBodyId() != EARTH ) {
    //            Vector geo = planets[i].getGeocentricVector() * scale;
    //            ofPoint toPlanet = ofPoint(geo.x, geo.y, geo.z);
    //            ofDrawLine(ofPoint(0.), toPlanet);
    //        }
    //    }
    
    ofPushMatrix();
    
    // ECLIPTIC GEOCENTRIC COORD SYSTEM
    // --------------------------------------- begin Ec Geo
    ofTranslate(planets[2].m_helioC);
    
    ofSetColor(255.,0.,0.);
    ofDrawLine(n_pole * 4.,n_pole * -4.);
    ofDrawArrow(v_equi, v_equi * 4.2, .1);
    ofDrawArrow(v_equi, v_equi * -4.2, .1);
    ofDrawLine(s_sols * -4.,s_sols * 4);
    
    ofSetColor(255);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD );
    ofDrawBitmapString("N", n_pole * 5.5);
    
//    // Check that Geocentric Vector to planets match
//    ofSetColor(100,100);
//    for ( int i = 0; i < planets.size(); i++) {
//        if (planets[i].getBodyId() != EARTH ) {
//            Vector geo = planets[i].getGeocentricVector() * scale;
//            ofPoint toPlanet = ofPoint(geo.x, geo.y, geo.z);
//            ofDrawLine(ofPoint(0.), toPlanet);
//        }
//    }
    
    ofPushMatrix();
    
    // EQUATORIAL COORD SYSTEM
    // --------------------------------------- begin Eq
    ofRotateX(ofRadToDeg(-obs.getObliquity()));
    
    ofNoFill();
    ofSetColor(255,0,0,120);
    ofDrawCircle(ofPoint(0.,0.,0.), 4.);
    
    // Disk
    for (int i = 0; i < 90; i ++) {
        ofPoint p;
        float a = ofDegToRad(i*4);
        p.x = cos(a);
        p.y = sin(a);
        ofDrawLine(p*4.,p*3);
    }
    
    // Check that Equatorial Vector to planets match
    ofSetColor(255,0,0,80);
    for ( int i = 0; i < planets.size(); i++) {
        if (planets[i].getBodyId() != EARTH ) {
            Vector eq = planets[i].EqPoint::getEquatorialVector();
            eq *= planets[i].getRadius() * scale;
            ofPoint toPlanet = ofPoint(eq.x, eq.y, eq.z);
            ofDrawLine(ofPoint(0.), toPlanet);
        }
    }
    
    // --------------------------------------- end Eq
    ofPopMatrix();
    
    // --------------------------------------- end Ec Geo
    ofPopMatrix();
    
    // Draw Earth-Sun Vector
    ofFill();
    ofSetColor(255);
//    ofDrawLine(toEarth*100., toEarth*90.);
    ofDrawArrow(toEarth*90., toEarth*95., .2);
    
    // Moon
    ofFill();
    moon.drawTrail(ofFloatColor(.4));
    moon.drawSphere(ofFloatColor(0.6));
    
    // Moon Phases
    shader_moon.begin();
    for ( int i = 0; i < moons.size(); i++ ) {
        moons[i].draw(billboard, shader_moon, 2.);
    }
    shader_moon.end();
    
    // Draw Hud elements
    ofSetColor(255);
    for ( int i = 0; i < lines.size(); i++ ) {
        ofDrawLine(lines[i].A, lines[i].B);
        
        if (lines[i].text != "") {
            ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD );
            ofDrawBitmapString (lines[i].text, lines[i].T);
        }
    }
    
    // --------------------------------------- end Ec Helio
    ofPopMatrix();
    
    cam.end();
    ofDisableDepthTest();
    ofDisableAlphaBlending();
    
    // Draw Date
    drawString(date, ofGetWidth()*.5, ofGetHeight()-30);
    
    // Share screen through Syphon
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
