#pragma once

#include "ofMain.h"
#include "ofxSmartShader.h"

#define GEOLOC_FILE "geoLoc.csv"

#include "Astro/src/Observer.h"
#include "Astro/src/Star.h"
#include "Astro/src/Constellation.h"
#include "Astro/src/Luna.h"

#include "ofxBody.h"
#include "ofxMoon.h"
#include "ofxSatellite.h"

#ifdef TARGET_OSX
#include "ofxSyphon.h"
#endif

#define TIME_STEP .001

#define SATELLITES

#define BODIES_TRAIL
#define SATELITES_TRAIL
#define HUD_LINES
#define HELIO_ARROW
//#define MOON_PHASES

#define BODIES_ECLIP_HELIO
//#define BODIES_ECLIP_GEO
//#define BODIES_EQUAT
//#define BODIES_HORIZ
//#define SUN_HORIZ
//#define MOON_HORIZ

#define EQUAT_DIR
#define EQUAT_DISK

#define TOPO_SHADER
#define TOPO_ARROW
#define TOPO_DISK
#define TOPO_HUD
//#define TOPO_HUD_LABLES
//#define TOPO_LABELS

//#define DEBUG_AXIS
//#define DEBUG_FPS

struct SrcLine {
    ofPoint A;
    ofPoint B;
    ofPoint T;
    std::string text;
};

struct HorLine {
    Horizontal A;
    Horizontal B;
    Horizontal T;
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
    
#ifdef TARGET_OSX
    ofxSyphonServer syphon;
#endif
    
    // Observer
    Observer        obs;
    // Place
    double          lng, lat;
    ofPoint         loc;
    
    // Time
    std::string     date;
    std::string     time;
    int             day, prevDay;
    int             month, prevMonth;
    int             year, prevYear;
    std::string     oneYearIn;
    
    // Scene
    ofEasyCam       cam;
    double          scale;
    bool            bWriten;
    
    // SUN
    // -----------------------
    Body            sun;
    
    // PLANETS
    // -----------------------
    vector<ofxBody> planets;
    
    // MOON
    // -----------------------
    ofxBody         moon;
    float           moonScaleFactor; // for the distance
#ifdef MOON_PHASES
    ofxSmartShader  moon_shader;
    int             moon_prevPhase;
    vector<ofxMoon> moons;
    Luna            luna;
#endif
    
    // EART
    // -----------------------
    float           earthSize;
    float           earthScaleFactor;
#ifdef TOPO_SHADER
    ofTexture       earth_texture;
    ofxSmartShader  earth_shader;
#endif
    
#ifdef SATELLITES
    // SATELLITES
    // -----------------------
    vector<ofxSatellite> satellites;
#endif
    
    // Ecliptical
    ofPoint         toEarth;
    
    // Equatorial
    ofPoint         v_equi;
    
    // Animation
    float           time_offset;
    bool            time_play;

    // HUD
    // -----------------------
    vector<SrcLine> lines;
#ifdef TOPO_HUD
    vector<HorLine> topoLines;
#endif
    ofVboMesh       billboard;
};
