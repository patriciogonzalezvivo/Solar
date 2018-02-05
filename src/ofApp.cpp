#include "ofApp.h"

#include "GeoLoc/src/GeoLoc.h"
#include "Astro/src/AstroOps.h"
#include "Astro/src/EcPoint.h"

#include "TimeOps.h"
double initial_jd;

const std::string month_names[] = { "ENE", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

const ofFloatColor palette[] = {
    ofFloatColor(0.020, 0.051, 0.090),
    ofFloatColor(0.376, 0.099, 0.082),
    ofFloatColor(0.918, 0.275, 0.247),
    ofFloatColor(0.337, 0.780, 0.847),
    ofFloatColor(0.621, 0.964, 0.988),
    ofFloatColor(0.996, 1.000, 1.000)
};

ofPoint toOf(const Vector &_ve) {
    return ofPoint(_ve.x, _ve.y, _ve.z);
}

void drawString(const std::string &str, int x , int y) {
    ofSetColor(255);
    ofSetDrawBitmapMode(OF_BITMAPMODE_SIMPLE);
    ofDrawBitmapStringHighlight(str, x - str.length() * 4, y);
}

void drawDisk(float in_radio, float out_radio, ofFloatColor c) {
    for (int i = 0; i < 90; i ++) {
        if ((i*4)%90 == 0) {
            ofSetColor(c, 255);
        }
        else {
            ofSetColor(c, 200);
        }
        ofPoint p;
        float a = ofDegToRad(i*4-90);
        p.x = cos(a);
        p.y = sin(a);
        ofDrawLine(p*in_radio,p*out_radio);
    }
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    ofSetBackgroundColor(0);
    ofSetCircleResolution(36);
    
    syphon.setName("Solar");
    cam.setPosition(-71.8425, 80.3674, 4.14539);
    bWriten = false;
    scale = 100.;
    
    // Location
    geoLoc(lng, lat, ofToDataPath(GEOIP_DB), ofToDataPath(GEOLOC_FILE));
    obs = Observer(lng, lat);
    loc = ofQuaternion(-lat, ofPoint(1., 0., 0.)) * ofQuaternion(lng-180, ofPoint(0., 1., 0.)) * ofPoint(0.,0.,2.);
    
    // Time
    initial_jd = obs.getJD();
//    initial_jd = TimeOps::now(true)-.5;
    
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
    
#ifdef TOPO_SHADER
    ofLoadImage(earth_texture, "diffuse.png");
    earth_shader.load("shaders/earth");
#endif
    
    T = X = Y = Z = 0;
}

//--------------------------------------------------------------
void ofApp::update(){

    // TIME CALCULATIONS
    // --------------------------------
#ifdef TIME_ANIMATION
    obs.setJD(initial_jd + ofGetElapsedTimef() * TIME_ANIMATION);
#else
#ifdef TIME_MANUAL
    obs.setJD(initial_jd + T);
#else
    obs.setJD(TimeOps::now(true));
#endif
#endif
    
    TimeOps::toDMY(obs.getJD(), day, month, year);
//    date = ofToString(year) + "/" + ofToString(month,2,'0') + "/" + ofToString(int(day),2,'0');
    date = TimeOps::formatDateTime(obs.getJD(), Y_MON_D);
    time = std::string(TimeOps::formatTime(obs.getJD(), true));
    
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
    moon.m_helioC = ( moon.getGeoPosition() * 20 * scale ) + ( planets[2].getHelioPosition() * scale);
    
    // HUDS ELEMENTS
    // --------------------------------
    
    // Calculate Equinox vector
    v_equi = toOf(AstroOps::eclipticToEquatorial(obs, EcPoint(0.0, 0.0 , 1)).getEquatorialVector() ).normalize();
    
    // Equatorial North, Vernal Equinox and Summer Solstice

    toEarth = planets[2].m_helioC;
    toEarth.normalize();
    
    // HUD EVENTS
    // --------------------------------
    
#ifdef MOON_PHASES
    // Moon phases
    luna.compute(obs);
    cout << "Moon ecliptic radius: " << moon.getGeocentricEcliptic().getRadius() << endl;
    cout << "Moon vector magnitud: " << moon.getGeocentricVector().getMagnitud() << endl;
    cout << "Luna: " << luna.getDistance()* AstroOps::KM_TO_AU << endl;
    float moon_phase = luna.getAge()/Luna::SYNODIC_MONTH;
    int moon_curPhase = moon_phase * 8;
    if (moon_curPhase != moon_prevPhase) {
        moons.push_back(ofxMoon(planets[2].m_helioC.getNormalized() * 110., moon_phase));
        moon_prevPhase = moon_curPhase;
    }
#endif
    
    // Equinoxes & Solstices
    if (abs(toEarth.dot(v_equi)) > .9999995 && !bWriten) {
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
        int dow = TimeOps::toDOW( obs.getJD() );
        
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

    ofEnableDepthTest();
    ofEnableAlphaBlending();

    // Set Scene
    cam.begin();
    ofPushMatrix();

    // ECLIPTIC HELIOCENTRIC COORD SYSTEM
    // --------------------------------------- begin Heliocentric Ecliptic

    // Draw Sun
    ofSetColor(255);
#ifdef DEBUG_AXIS
    ofDrawAxis(15);
#endif
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
    // --------------------------------------- begin Geocentric Ecliptic
    ofTranslate(planets[2].m_helioC);
#ifdef DEBUG_AXIS
    ofDrawAxis(10);
#endif

#ifdef BODIES_ECLIP_GEO
    // Check that Geocentric Vector to planets match
    ofSetColor(100,100);
    for ( int i = 0; i < planets.size(); i++) {
        if (planets[i].getBodyId() != EARTH ) {
            ofPoint toPlanet = toOf(planets[i].getGeocentricVector()) * scale;
            ofDrawLine(ofPoint(0.), toPlanet);
        }
    }
#endif

    ofPushMatrix();
    // EQUATORIAL COORD SYSTEM
    // --------------------------------------- begin Equatorial
    ofRotateX(ofRadToDeg(-obs.getObliquity()));
    
#ifdef DEBUG_AXIS
    ofDrawAxis(7);
#endif
    
#ifdef EQUAT_DIR
    // Poles, Equinoxes and Solsices
    ofSetColor(palette[2], 100);
    ofPoint n_pole = ofPoint(0., 0., 1.);
    ofPoint s_sols = ofPoint(0., 1., 0.);
    ofDrawLine(s_sols * 4.,s_sols * -4);
    
    ofSetColor(palette[2]);
    ofDrawLine(n_pole * 4.,n_pole * -4.);
    ofDrawLine(v_equi * 4., v_equi * -4);
    
    ofSetColor(255);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD );
    ofDrawBitmapString("N", n_pole * 5.5);
    ofDrawBitmapString("S", -n_pole * 5.5);
#endif
    
#ifdef BODIES_EQUAT
    // Check that Equatorial Vector to planets match
    ofSetColor(palette[1]);
    for ( int i = 0; i < planets.size(); i++) {
        if (planets[i].getBodyId() != EARTH ) {
            ofPoint toPlanet = toOf(planets[i].getEquatorialVector()) * scale;
            ofDrawLine(ofPoint(0.), toPlanet);
        }
    }
#endif

#ifdef EQUAT_DISK
    ofNoFill();
    ofSetColor(255,0,0);
    ofDrawCircle(ofPoint(0.,0.,0.), 4.);
    
    // Disk
    drawDisk(3,4, palette[1]);
#endif

    ofPushMatrix();
    // -------------------------------------- begin of Sphere
    ofRotateX(90);
    ofRotateY(-90);
    
    ofPushMatrix();
    // -------------------------------------- begin Hour Angle (Topo)
    // Rotate earth
    
    ofRotateY((TimeOps::toGreenwichSiderealHour(obs.getJD())/24.)*360.);
    
#ifdef DEBUG_AXIS
    ofDrawAxis(5);
#endif
    
    // Earth
    ofFill();
#ifdef TOPO_SHADER
    ofSetColor(255);
    earth_shader.begin();
    earth_shader.setUniformTexture("u_diffuse", earth_texture, 0);
    ofDrawSphere(1.7);
    earth_shader.end();
#else
    ofSetColor(ofFloatColor(.9));
    ofDrawSphere(1.7);
#endif

#ifdef TOPO_ARROW
    // Location arrow
    ofSetColor(255);
    ofDrawArrow(ofPoint(0.), loc);
#endif

    ofPushMatrix();
    // -------------------------------------- begin location (topo)
    ofRotateY(lng);
    ofRotateX(lat);
    ofTranslate(0., 0., -1.71);
    
#ifdef TOPO_DISK
    // Check that Horizontal Vector to planets match
    drawDisk(.5, .6, palette[3]);
#endif
    
    ofPushMatrix();
    // -------------------------------------- begin Horizontal (topo)
    ofRotateY(-90);
    
    ofRotateX(X);
    ofRotateY(Y);
    ofRotateZ(Z);
    
#ifdef DEBUG_AXIS
    ofDrawAxis(2);
#endif
    
#ifdef SUN_HORIZ
    ofSetColor(palette[3], 250);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD );
    if (sun.getAltitud() > 0) {
        ofPoint toSun = toOf(sun.getHorizontalVector()) * scale;
        ofDrawLine(ofPoint(0.), toSun);
        ofDrawBitmapString(sun.getBodyName(), toSun);
    }
#endif
    
#ifdef MOON_HORIZ
    ofSetColor(palette[3], 250);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD );
    if (moon.getAltitud() > 0) {
        ofPoint toMoon = toOf(moon.getHorizontalVector()) * 20 * scale;
        ofDrawLine(ofPoint(0.), toMoon);
        ofDrawBitmapString(moon.getBodyName(), toMoon);
    }
#endif

#ifdef BODIES_HORIZ
    ofSetColor(palette[3], 100);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD );
    for ( int i = 0; i < planets.size(); i++) {
        if (planets[i].getBodyId() != EARTH &&
            planets[i].getAltitud() > 0) {
            ofPoint toPlanet = toOf(planets[i].getHorizontalVector()) * scale;
            ofDrawLine(ofPoint(0.), toPlanet);
            ofDrawBitmapString(planets[i].getBodyName(), toPlanet);
        }
    }
#endif
    
    // --------------------------------------- end Horizontal (topo)
    ofPopMatrix();
    
    // --------------------------------------- end Location (Topo)
    ofPopMatrix();
    
    // --------------------------------------- end Hour Angle (Topo)
    ofPopMatrix();
    
    // --------------------------------------- end of Sphere
    ofPopMatrix();

    // --------------------------------------- end Equatorial
    ofPopMatrix();

    // --------------------------------------- end Geocentric Ecliptic
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

#ifdef HUD_LINES
    // Draw Hud elements
    ofSetColor(255);
    for ( int i = 0; i < lines.size(); i++ ) {
        ofDrawLine(lines[i].A, lines[i].B);

        if (lines[i].text != "") {
            ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD );
            ofDrawBitmapString (lines[i].text, lines[i].T);
        }
    }
#endif

    // --------------------------------------- end Heliocentric Ecliptic
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
    
    if ( key == 'q' ) {
        X--;
        cout << "X: " << X << endl;
    }
    else if ( key == 'a' ) {
        X++;
        cout << "X: " << X << endl;
    }
    else if ( key == 'z' ) {
        X = 0;
        cout << "X: " << X << endl;
    }
    else if ( key == 'w' ) {
        Y--;
        cout << "Y: " << Y << endl;
    }
    else if ( key == 's' ) {
        Y++;
        cout << "Y: " << Y << endl;
    }
    else if ( key == 'x' ) {
        Y = 0;
        cout << "Y: " << Y << endl;
    }
    else if ( key == 'e' ) {
        Z--;
        cout << "Z: " << Z << endl;
    }
    else if ( key == 'd' ) {
        Z++;
        cout << "Z: " << Z << endl;
    }
    else if ( key == 'c' ) {
        Z = 0;
        cout << "Z: " << Z << endl;
    }
    else if ( key == 'r' ) {
        T -= 0.001;
        cout << "T: " << T << endl;
    }
    else if ( key == 'f' ) {
        T += 0.001;
        cout << "T: " << T << endl;
    }
    else if ( key == 'v' ) {
        T = 0;
        cout << "T: " << T << endl;
    }
    else {
    cam.setDistance(10);
    }
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
