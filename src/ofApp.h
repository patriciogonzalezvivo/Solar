#pragma once

#include "ofMain.h"

#include "ofxSyphon.h"

#define GEOIP_DB "GeoLiteCity.dat"
#define GEOLOC_FILE "geoLoc.csv"

#include "Astro/src/Observer.h"
#include "Astro/src/Luna.h"
#include "Astro/src/Star.h"
#include "Astro/src/Constellation.h"

#include "ofxBody.h"
#include "ofxMoon.h"

#define TIME_ANIMATION

struct Line {
    ofPoint A;
    ofPoint B;
    ofPoint T;
    std::string text;
};

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofxSyphonServer syphon;
    
    // Observers
    Observer    obs;
    ofEasyCam   cam;
    
    // Bodies
    Body        sun;
    Luna        luna;
    ofxBody     moon;
    int         moon_prevPhase;
    vector<ofxMoon> moons;
    vector<ofxBody> planets;
    
    // Ecliptical
    ofPoint toEarth;
    
    // Equatorial
    ofPoint     n_pole;
    ofPoint     v_equi;
    ofPoint     s_sols;

    // HUD
    vector<Line> lines;
    ofVboMesh   billboard;
    ofShader    shader_moon;
    
    double      scale;
    
    // Time
    std::string date, oneYearIn;
    double      day, prevDay;
    int         month, prevMonth;
    int         year, prevYear;
    bool        bWriten;
};
