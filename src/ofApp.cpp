#include "ofApp.h"

#include "GeoLoc/src/GeoLoc.h"
#include "Astro/src/AstroOps.h"

#include "TimeOps.h"
double initial_jd;

const std::string month_names[] = { "ENE", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

ofPoint toOf(const Vector &_ve) {
    return ofPoint(_ve.x, _ve.y, _ve.z);
}

void drawString(const std::string &str, int x , int y) {
    ofSetColor(255);
    ofSetDrawBitmapMode(OF_BITMAPMODE_SIMPLE);
    ofDrawBitmapStringHighlight(str, x - str.length() * 4, y);
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    ofSetBackgroundColor(0);
    ofSetCircleResolution(36);
    
    syphon.setName("Solar");
    cam.setPosition(-71.8425, 80.3674, -4.14539);
    bWriten = false;
    scale = 100.;
    
    ofLoadImage(earth_texture, "diffuse.png");
    earth_shader.load("shaders/earth");
    
    // Location
    geoLoc(lng, lat, ofToDataPath(GEOIP_DB), ofToDataPath(GEOLOC_FILE));
    obs = Observer(lng, lat);
    loc = ofQuaternion(-lat, ofPoint(1., 0., 0.)) * ofQuaternion(lng-180, ofPoint(0., 1., 0.)) * ofPoint(0.,0.,2.);
    
    // Time
    initial_jd = obs.getJulianDate();
    
    // Bodies
    BodyId planets_names[] = { MERCURY, VENUS, EARTH, MARS, JUPITER, SATURN, URANUS, NEPTUNE, PLUTO, LUNA };
    float planets_sizes[] = { 0.0561, 0.1377, 0.17, 0.255, 1.87*.5, 1.615*.5, 0.68, 0.68, 0.0306, 0.0459 };
    for (int i = 0; i < 9; i++) {
        planets.push_back(ofxBody(planets_names[i], planets_sizes[i] * 10.));
    }
    
    sun = Body(SUN);
    
    // Moon
    moon = ofxBody(LUNA, 0.5);
#ifdef MOON_PHASES
    moon_shader.load("shaders/moon.vert","shaders/moon.frag");
    
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
    luna = Luna();
#endif
   
}

//--------------------------------------------------------------
void ofApp::update(){

    // TIME CALCULATIONS
    // --------------------------------
#ifdef TIME_ANIMATION
    obs.setJuliaDay(initial_jd + ofGetElapsedTimef() * TIME_ANIMATION);
#else
    obs.setTime();
#endif
    
    TimeOps::JDtoMDY(obs.getJulianDate(), month, day, year);
    date = ofToString(year) + "/" + ofToString(month,2,'0') + "/" + ofToString(int(day),2,'0');
    TimeOps::toHMS(day, hour, min, sec);
    time = " " + ofToString(hour,2,'0') + ":" + ofToString(min,2,'0') + ":" + ofToString(int(sec),2,'0');
    
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
    moon.compute(obs);
    moon.m_helioC = ( moon.getGeoPosition() * 20*scale ) + ( planets[2].getHelioPosition() * scale);
    
    // HUDS ELEMENTS
    // --------------------------------
    
    // Equatorial North, Vernal Equinox and Summer Solstice
    Vector z = AstroOps::eclipticToEquatorial(obs, Vector(0.0, -90.0, 1)).getEquatorialVector();
    Vector y = AstroOps::eclipticToEquatorial(obs, Vector(0.0, 0.0 , 1)).getEquatorialVector();
    Vector x = AstroOps::eclipticToEquatorial(obs, Vector(90.0, 0.0, 1)).getEquatorialVector();
    
    n_pole = toOf(z).normalize();
    v_equi = toOf(y).normalize();
    s_sols = toOf(x).normalize();
    
    toEarth = planets[2].m_helioC;
    toEarth.normalize();
    
    // HUD EVENTS
    // --------------------------------
    
#ifdef MOON_PHASES
    // Moon phases
    luna.compute(obs);
    float moon_phase = luna.getAge()/Luna::SYNODIC_MONTH;
    int moon_curPhase = moon_phase * 8;
    if (moon_curPhase != moon_prevPhase) {
        moons.push_back(ofxMoon(planets[2].m_helioC.getNormalized() * 110., moon_phase));
        moon_prevPhase = moon_curPhase;
    }
#endif
    
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
#ifdef MOON_PHASES
        moons.clear();
#endif
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
#ifdef BODIES_TRAIL
        planets[i].drawTrail(ofFloatColor(.5));
#endif
        if (planets[i].getBodyId() != EARTH) {
            planets[i].drawSphere(ofFloatColor(.9));
#ifdef BODIES_ECLIP_HELIO
            ofSetColor(120, 100);
            ofDrawLine(ofPoint(0.), planets[i].m_helioC);
#endif
        }
    }

    ofPushMatrix();

    // ECLIPTIC GEOCENTRIC COORD SYSTEM
    // --------------------------------------- begin Ec Geo
    ofTranslate(planets[2].m_helioC);

    ofPushMatrix();
    ofSetColor(255.,0.,0.);
    ofDrawLine(n_pole * 4.,n_pole * -4.);
    ofDrawLine(v_equi * 4., v_equi * -4);
    ofDrawLine(s_sols * 4.,s_sols * -4);

    ofSetColor(255);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD );
    ofDrawBitmapString("N", n_pole * 5.5);
    ofDrawBitmapString("S", -n_pole * 5.5);
    ofPopMatrix();

#ifdef BODIES_ECLIP_GEO
    // Check that Geocentric Vector to planets match
    ofSetColor(100,100);
    for ( int i = 0; i < planets.size(); i++) {
        if (planets[i].getBodyId() != EARTH ) {
            Vector geo = planets[i].getGeocentricVector() * scale;
            ofPoint toPlanet = ofPoint(geo.x, geo.y, geo.z);
            ofDrawLine(ofPoint(0.), toPlanet);
        }
    }
#endif

    ofPushMatrix();

    // EQUATORIAL COORD SYSTEM
    // --------------------------------------- begin Eq
    ofRotateX(ofRadToDeg(-obs.getObliquity()));

#ifdef BODIES_EQUAT
    // Check that Equatorial Vector to planets match
    ofSetColor(255,0,0,100);
    for ( int i = 0; i < planets.size(); i++) {
        if (planets[i].getBodyId() != EARTH ) {
            ofPoint toPlanet = toOf(planets[i].EqPoint::getEquatorialVector());
            toPlanet *= planets[i].getRadius() * scale;
            ofDrawLine(ofPoint(0.), toPlanet);
        }
    }
#endif
    
    ofPushMatrix();
    ofRotateX(-45);
    // Rotate earth
    ofRotateY((TimeOps::greenwichSiderealHour(obs.getJulianDate())/24.)*360. + 90);
    
    // Location
    ofSetColor(255);
    ofDrawArrow(ofPoint(0.), loc);
    
    // Earth
    ofFill();
    ofSetColor(255);
    earth_shader.begin();
    earth_shader.setUniformTexture("u_diffuse", earth_texture, 0);
    ofDrawSphere(1.7);
    earth_shader.end();
    
    // Equator
    ofPushMatrix();
    ofRotateX(90);
    ofNoFill();
    ofSetColor(255,0,0,120);
    ofDrawCircle(ofPoint(0.,0.,0.), 4.);
    
    // Disk
    ofSetColor(255,0,0,120);
    for (int i = 0; i < 90; i ++) {
        ofPoint p;
        float a = ofDegToRad(i*4);
        p.x = cos(a);
        p.y = sin(a);
        ofDrawLine(p*4.,p*3);
    }
    ofPopMatrix();
    ofPopMatrix();

    // --------------------------------------- end Eq
    ofPopMatrix();

    // --------------------------------------- end Ec Geo
    ofPopMatrix();

    // Draw Earth-Sun Vector
    ofFill();
    ofSetColor(255);
    ofDrawArrow(toEarth*90., toEarth*95., .2);

    // Moon
    ofFill();
#ifdef BODIES_TRAIL
    moon.drawTrail(ofFloatColor(.4));
#endif
    moon.drawSphere(ofFloatColor(0.6));

#ifdef MOON_PHASES
    // Moon Phases
    moon_shader.begin();
    for ( int i = 0; i < moons.size(); i++ ) {
        moons[i].draw(billboard, moon_shader, 2.);
    }
    moon_shader.end();
#endif

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
    drawString(date + " " + time, ofGetWidth()*.5, ofGetHeight()-30);
    drawString("lng: " + ofToString(lng,2,'0') + "  lat: " + ofToString(lat,2,'0'), ofGetWidth()*.5, ofGetHeight()-10);

    // Share screen through Syphon
    syphon.publishScreen();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    cam.setDistance(20);
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
