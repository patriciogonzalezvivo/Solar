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

#define TIME_ANIMATION 4.0
//#define TIME_ANIMATION .015
//#define TIME_MANUAL

#define BODIES_TRAIL
#define HUD_LINES
#define HELIO_ARROW
#define MOON_PHASES

#define BODIES_ECLIP_HELIO
//#define BODIES_ECLIP_GEO
#define BODIES_EQUAT
//#define BODIES_HORIZ
#define SUN_HORIZ
#define MOON_HORIZ

#define EQUAT_DIR
#define EQUAT_DISK

//#define TOPO_SHADER
//#define TOPO_ARROW
//#define TOPO_DISK

//#define DEBUG_AXIS

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
    
    // Observer
    Observer        obs;
    // Place
    double          lng, lat;
    ofPoint         loc;
    // Time
    std::string     date;
    std::string     time;
    double          sec;
    int             min;
    int             hour;
    double          day, prevDay;
    int             month, prevMonth;
    int             year, prevYear;
    std::string     oneYearIn;
    
    // Scene
    ofEasyCam       cam;
    double          scale;
    bool            bWriten;
    
    // BODIES
    // -----------------------
    Body            sun;
    vector<ofxBody> planets;
    
    // MOON
    // -----------------------
    ofxBody         moon;
#ifdef MOON_PHASES
    ofShader        moon_shader;
    int             moon_prevPhase;
    vector<ofxMoon> moons;
    Luna            luna;
#endif
    
    // EART
    // -----------------------
#ifdef TOPO_SHADER
    ofTexture       earth_texture;
    ofShader        earth_shader;
#endif
    
    // Ecliptical
    ofPoint         toEarth;
    
    // Equatorial
    ofPoint         v_equi;
    
    // Horizontal
    float           X,Y,Z, T;

    // HUD
    // -----------------------
    vector<Line>    lines;
    ofVboMesh       billboard;
};
