#include "ofApp.h"

#include "GeoLoc/src/GeoLoc.h"
#include "Astro/src/AstroOps.h"
#include "Astro/src/models/TLE.h"

#include "TimeOps.h"

const std::string month_names[] = { "ENE", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

const ofFloatColor palette[] = {
    ofFloatColor(0.020, 0.051, 0.090),
    ofFloatColor(0.376, 0.099, 0.082),
    ofFloatColor(0.918, 0.275, 0.247),
    ofFloatColor(0.337, 0.780, 0.847),
    ofFloatColor(0.621, 0.964, 0.988),
    ofFloatColor(0.996, 1.000, 1.000)
};

glm::vec3 toOf(const Vector &_ve) {
    return glm::vec3(_ve.x, _ve.y, _ve.z);
}

void drawString(const std::string &str, int x , int y) {
    ofSetColor(255);
    ofSetDrawBitmapMode(OF_BITMAPMODE_SIMPLE);
    ofDrawBitmapStringHighlight(str, x - str.length() * 4, y);
}

void drawDisk(float in_radio, float out_radio, int step, ofFloatColor c) {
    int total = 360/step;
    for (int i = 0; i < total; i++) {
        ofPoint p;
        float a = ofDegToRad(i*step-180);
        p.x = cos(a);
        p.y = sin(a);
        
        ofSetColor(c, 200);
        if ((i*4)%90 == 0) {
            ofSetColor(c, 255);
        }
        ofDrawLine(p*in_radio, p*out_radio);
    }
}

void drawDial(float radio, float width, int step, ofFloatColor c) {
    ofSetColor(c, 200);
    int total = 360/step;
    for (int i = 0; i < total; i++) {
        ofPoint p1, p2;
        float a = ofDegToRad(i*step);
        float b = ofDegToRad(i*step);
        p1.x = cos(a);
        p1.y = sin(a);
        p1.z = -width*.5;
        
        p2.x = cos(b);
        p2.y = sin(b);
        p2.z = +width*.5;
        
        ofDrawLine(p1*radio, p2*radio);
    }
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    ofSetBackgroundColor(0);
    ofSetCircleResolution(36);
    
#ifdef TARGET_OSX
    syphon.setName("Solar");
#endif

    cam.setPosition(-71.8425, 80.3674, 4.14539);
    bWriten = false;
    scale = 500.;
    
    // Location
    geoLoc(lng, lat, ofToDataPath(GEOLOC_FILE));
    obs = Observer(lng, lat);
    loc = ofQuaternion(-lat, ofPoint(1., 0., 0.)) * ofQuaternion(lng-180, ofPoint(0., 1., 0.)) * ofPoint(0.,0.,2.);
    
    // Time
    time_offset = 0.;
    time_step = 0.0005;
    time_play = false;
    
    // Earth
    earthSize = 1.7;
    earthScaleFactor = ((earthSize * AstroOps::AU_TO_KM)/AstroOps::EARTH_EQUATORIAL_RADIUS_KM);
    
    // Sun
    sun = Body(SUN);
    
    // Moon
    moonScaleFactor = .5;
    moon = ofxBody(LUNA, (earthSize/AstroOps::EARTH_EQUATORIAL_RADIUS_KM) * Luna::DIAMETER_KM);
#ifdef MOON_PHASES
    moon_shader.load("shaders/moon");
    
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
    
    // Planets
    BodyId planets_names[] = { MERCURY, VENUS, EARTH, MARS, JUPITER, SATURN, URANUS, NEPTUNE, PLUTO, LUNA };
    float planets_sizes[] = { 0.561, 1.377, earthSize, 2.55, 18.7*.5, 16.15*.5, 6.8, 6.8, 0.306, 0.459 };
    for (int i = 0; i < 9; i++) {
        planets.push_back(ofxBody(planets_names[i], planets_sizes[i]));
    }
    
#ifdef SATELLITES
    // Satellites
    TLE sats[] = {
        // TLE("HOBBLE",
        //     "1 20580U 90037B   18154.57093887 +.00000421 +00000-0 +14812-4 0  9997",
        //     "2 20580 028.4684 205.1197 0002723 359.7851 153.4291 15.09046689343324"),
        // TLE("TERRA",
        //     "1 25994U 99068A   18154.24441102 -.00000021  00000-0  53030-5 0  9998",
        //     "2 25994  98.2062 229.3170 0001386  97.9233 262.2105 14.57104269981794"),
        TLE("GOES 16",
            "1 41866U 16071A   18154.50918000 -.00000249  00000-0  00000-0 0  9998",
            "2 41866   0.0024 323.4828 0001042 138.0782 258.4507  1.00269829  5675"),
        TLE("GOES 17",
            "1 43226U 18022A   18153.55154698 -.00000184 +00000-0 +00000-0 0  9995",
            "2 43226 000.0506 131.5682 0001494 306.5382 281.9134 01.00275070000988"),
//        TLE("SUOMI",
//            "1 37849U 11061A   18154.59022466  .00000019  00000-0  29961-4 0  9994",
//            "2 37849  98.7369  93.2509 0000790 115.8241 296.7478 14.19549859341951"),
        // TLE("NOAA 19",
        //     "1 33591U 09005A   18154.53769778  .00000063  00000-0  59621-4 0  9992",
        //     "2 33591  99.1410 132.2940 0014182   9.6985 350.4457 14.12282740480248"),
        // TLE("NOAA 20",
        //     "1 43013U 17073A   18154.54421336  .00000003  00000-0  22344-4 0  9998",
        //     "2 43013  98.7249  93.0462 0000870  77.9803 282.1471 14.19559862 27975"),
        TLE("ISS",
            "1 25544U 98067A   18151.37845806  .00001264  00000-0  26359-4 0  9999",
            "2 25544  51.6399 102.5027 0003948 138.3660   3.9342 15.54113216115909")
    };
    
    int N = sizeof(sats)/sizeof(sats[0]);
    for (int i = 0; i < N; i++) {
        satellites.push_back(ofxSatellite(sats[i], 0.05));
    }
    
#endif
    
#ifdef TOPO_SHADER
    ofLoadImage(earth_texture, "diffuse.png");
    earth_shader.load("shaders/earth");
#endif
    
#ifdef TOPO_HUD
    vector<std::string> direction = { "N", "E", "S", "W" };
    int step = 5;
    int total = 360/step;
    int labelstep = total/direction.size();
    for (int i = 0; i < total; i++) {
        HorLine h1, v1;
        float a = i*step;
        float b = (i+1)*step;
        h1.A = Horizontal(0., a, DEGS);
        h1.B = Horizontal(0., b, DEGS);
        
        v1.A = Horizontal(0., a, DEGS);
        
        if (i%labelstep == 0) {
            h1.T = Horizontal(10., a+10., DEGS);
            h1.text = direction[int(i/labelstep)];
            v1.B = Horizontal(10., a, DEGS);
        }
        else {
            v1.B = Horizontal(5., a, DEGS);
        }
        
        topoLines.push_back(h1);
        topoLines.push_back(v1);
    }
#endif
}

//--------------------------------------------------------------
void ofApp::update(){

    // TIME CALCULATIONS
    // --------------------------------
    if (time_play) {
        time_offset += time_step;
    }

    obs.setJD(TimeOps::now(UTC) + time_offset);
    
    TimeOps::toDMY(obs.getJD(), day, month, year);
    date = TimeOps::formatDateTime(obs.getJD(), Y_MON_D);
    time = std::string(TimeOps::formatTime(obs.getJD() + 0.1666666667, true));
    
    // Updating BODIES positions
    // --------------------------------
    
    // Update sun position
    sun.compute(obs);
    
    // Update planets positions
    for ( unsigned int i = 0; i < planets.size(); i++) {
        planets[i].compute(obs);
        planets[i].m_helioC = planets[i].getHelioPosition(AU) * scale;
    }
    
    // Update moon position (the distance from the earth is not in scale)
    moon.compute(obs);
    moon.m_helioC = ( moon.getGeoPosition(AU) * (earthScaleFactor * moonScaleFactor) ) + (planets[2].getHelioPosition(AU) * scale);

    #ifdef SATELLITES
    for ( unsigned int i = 0; i < satellites.size(); i++) {
        satellites[i].compute(obs);
        satellites[i].m_geoC = toOf(satellites[i].getECI().getPosition(AU) * earthScaleFactor);
        satellites[i].m_helioC = satellites[i].m_geoC + planets[2].getHelioPosition(AU) * scale;
    }
    #endif
    
    // HUDS ELEMENTS
    // --------------------------------
    
    // Calculate Equinox vector
    v_equi = glm::normalize(toOf( AstroOps::toEquatorial(obs, Ecliptic(0.0, 0.0 , 1, RADS, AU)).getVector() ));
    
    // Equatorial North, Vernal Equinox and Summer Solstice

    toEarth = planets[2].m_helioC;
    toEarth.normalize();
    
    // HUD EVENTS
    // --------------------------------
    
#ifdef MOON_PHASES
    // Moon phases
    luna.compute(obs);
//    cout << "Luna: " << luna << endl;
    float moon_phase = luna.getAge()/Luna::SYNODIC_MONTH;
    int moon_curPhase = moon_phase * 8;
    if (moon_curPhase != moon_prevPhase) {
        moons.push_back(ofxMoon(glm::normalize(planets[2].m_helioC) * 110., moon_phase));
        moon_prevPhase = moon_curPhase;
    }
#endif
    
    // Equinoxes & Solstices
    if (abs(toEarth.dot(v_equi)) > .9999995 && !bWriten) {
        SrcLine newLine;
        newLine.A = planets[2].m_helioC;
        newLine.B = toEarth * 90.;

        newLine.text = "Eq. " + ofToString(int(day),2,'0');
        newLine.T = toEarth * 104. + ofPoint(0.,0.,2);

        lines.push_back(newLine);
        bWriten = true;
    }
    else if (abs(toEarth.dot(v_equi)) < .001 && !bWriten) {
        SrcLine newLine;
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
        
        SrcLine newLine;
        newLine.A = toEarth * 80.;
        newLine.B = toEarth * 90.;
        
        newLine.text = month_names[month-1];
        newLine.T = toEarth * 70.;
        
        lines.push_back(newLine);
    }
    else if (int(day) != int(prevDay)) {
        int dow = TimeOps::toDOW( obs.getJD() );
        
        SrcLine newLine;

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
//    cam.setTarget(planets[2].m_helioC);

    ofEnableDepthTest();
    ofEnableAlphaBlending();

    // Set Scene
    cam.begin();
    ofPushMatrix();
    
    ofTranslate(-planets[2].m_helioC);
    
    // ECLIPTIC HELIOCENTRIC COORD SYSTEM
    // --------------------------------------- begin Heliocentric Ecliptic

    // Draw Sun
    ofSetColor(255);
#ifdef DEBUG_AXIS
    ofDrawAxis(15);
#endif
    ofDrawSphere(10);

    // Draw Planets and their orbits (HelioCentric)
    for ( unsigned int i = 0; i < planets.size(); i++) {
#ifdef BODIES_TRAIL
        planets[i].drawTrail(ofFloatColor(.5));
#endif
        if (planets[i].getId() != EARTH) {
            planets[i].draw(ofFloatColor(.9));
#ifdef BODIES_ECLIP_HELIO
            ofSetColor(120, 100);
            ofDrawLine(ofPoint(0.), planets[i].m_helioC);
#endif
        }
    }
    
#ifdef SATELLITES
    //  SATELLITES
    //  ---------------------------------------
    for (unsigned int i = 0; i < satellites.size(); i++) {
#ifdef BODIES_TRAIL
        satellites[i].drawHeliocentricTrail(palette[4]);
#endif
        satellites[i].draw(ofFloatColor(1.));
    }
#endif

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
            ofPoint toPlanet = toOf(planets[i].getEclipticGeocentric().getVector()) * scale;
            ofDrawLine(ofPoint(0.), toPlanet);
        }
    }
#endif
    
    ofPushMatrix();
    
    // EQUATORIAL COORD SYSTEM
    // --------------------------------------- begin Equatorial
    ofRotateXRad(-obs.getObliquity());
    
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
            glm::vec3 toPlanet = toOf(planets[i].getEquatorialVector(AU)) * scale;
            ofDrawLine(glm::vec3(0.), toPlanet);
        }
    }
#endif

#ifdef EQUAT_DISK
    ofNoFill();
    ofSetColor(255,0,0);
    ofDrawCircle(ofPoint(0.,0.,0.), 4.);
    
    // Disk
    drawDisk(3, 4, 4, palette[1]);
#endif

    ofPushMatrix();
    // -------------------------------------- begin of Sphere
    ofRotateXDeg(90);
    ofRotateYDeg(-90);
    
    ofPushMatrix();
    // -------------------------------------- begin Hour Angle (Topo)
    // Rotate earth
    ofRotateYRad( TimeOps::toGreenwichSiderealTime(obs.getJD()) );
#ifdef DEBUG_AXIS
    ofDrawAxis(5);
#endif
    
    // Earth
    ofFill();
#ifdef TOPO_SHADER
    ofSetColor(255);
    earth_shader.begin();
    earth_shader.setUniformTexture("u_diffuse", earth_texture, 0);
    ofDrawSphere(earthSize);
    earth_shader.end();
#else
    ofSetColor(ofFloatColor(.9));
    ofDrawSphere(earthSize);
#endif

#ifdef TOPO_ARROW
    // Location arrow
    ofSetColor(255);
    ofDrawLine(ofPoint(0.), loc);
#endif

    ofPushMatrix();
    // -------------------------------------- begin location (topo)
    ofRotateYDeg(lng);
    ofRotateXDeg(lat);
    ofTranslate(0., 0., -earthSize);
    
#ifdef TOPO_DISK
    // Check that Horizontal Vector to planets match
    drawDisk(.5, .8, 5, palette[3]);
#endif
    
    ofPushMatrix();
    // -------------------------------------- begin Horizontal (topo)xw
    ofRotateXDeg(90);
    
#ifdef TOPO_HUD
    drawDial(.8, .05, 4, palette[3]);
#endif
    
    ofRotateYDeg(90);
#ifdef TOPO_HUD
    drawDial(.8, .05, 4, palette[3]);
#endif
    
#ifdef DEBUG_AXIS
    ofDrawAxis(2);
#endif
    
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD );
    
#ifdef SUN_HORIZ
    ofSetColor(palette[3], 250);
    if (sun.getHorizontal().getAltitud(RADS) > 0) {
        ofPoint toSun = toOf(sun.getHorizontalVector()) * scale;
        ofDrawLine(ofPoint(0.), toSun);
#ifdef TOPO_LABELS
        ofDrawBitmapString(sun.getBodyName(), toSun);
#endif
    }
#endif
    
#ifdef MOON_HORIZ
    ofSetColor(palette[3], 250);
    if (moon.getHorizontal().getAltitud(RADS) > 0) {
        ofPoint toMoon = toOf(moon.getHorizontalVector()) * 20 * scale;
        ofDrawLine(ofPoint(0.), toMoon);
#ifdef TOPO_LABELS
        ofDrawBitmapString(moon.getBodyName(), toMoon);
#endif
    }
#endif

#ifdef BODIES_HORIZ
    ofSetColor(palette[3], 100);
    for ( int i = 0; i < planets.size(); i++) {
        if (planets[i].getBodyId() != EARTH &&
            planets[i].getHorizontal().getAltitud(RADS) > 0) {
            ofPoint toPlanet = toOf(planets[i].getHorizontalVector()) * scale;
            ofDrawLine(ofPoint(0.), toPlanet);
#ifdef TOPO_LABELS
            ofDrawBitmapString(planets[i].getBodyName(), toPlanet);
#endif
        }
    }
#endif
    
#ifdef TOPO_HUD
    for (int i = 0; i < topoLines.size(); i++) {
        ofSetColor(palette[3]);
        ofPoint a = toOf(topoLines[i].A.getVector());
        ofPoint b = toOf(topoLines[i].B.getVector());
        
#ifdef TOPO_HUD_LABLES
        if (topoLines[i].text != "") {
            ofSetColor(palette[4]);
            ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD );
            ofDrawBitmapString (topoLines[i].text, toOf(topoLines[i].T.getVector()));
        }
#endif
        
        ofDrawLine(a, b);
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
    ofDrawLine(toEarth*90., toEarth*95.);

    // Moon
    ofFill();
#ifdef BODIES_TRAIL
    moon.drawTrail(ofFloatColor(.4));
#endif
    moon.draw(ofFloatColor(0.6));

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

#ifdef TARGET_OSX
    // Share screen through Syphon
    syphon.publishScreen();
#endif
    
#ifdef DEBUG_FPS
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 5, 15);
#endif
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if ( key == 'r' ) {
        time_offset -= time_step;
    }
    else if ( key == 'f' ) {
        time_offset += time_step;
    }
    if ( key == '-' ) {
        time_step -= 0.0001;
    }
    else if ( key == '=' ) {
        time_step += 0.0001;
    }
    else if ( key == 'v' ) {
        time_offset = 0;
        moon.clearTale();
        for (unsigned int i = 0; i < planets.size(); i++){
            planets[i].clearTale();
        }
        for (unsigned int i = 0; i < satellites.size(); i++){
            satellites[i].clearTale();
        }
    }
    else {
        time_play = !time_play;
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
