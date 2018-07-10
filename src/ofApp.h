#pragma once

#include "ofMain.h"
#include "ofxShader.h"

#define GEOLOC_FILE "geoLoc.csv"

#include "Astro/src/Observer.h"
#include "Astro/src/Star.h"
#include "Astro/src/Constellation.h"
#include "Astro/src/Luna.h"

#include "ofxBody.h"
#include "ofxMoon.h"
#include "ofxSatellite.h"

#define SATELLITES

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
    float           planetsSizes[10];
    vector<ofxBody> planets;
    
    // MOON
    // -----------------------
    ofxBody         moon;
    float           moonSize;
    float           moonScaleDistance; // for the distance
    ofxShader  moon_shader;
    int             moon_prevPhase;
    vector<ofxMoon> moons;
    Luna            luna;
    
    // EART
    // -----------------------
    float           earthSize;
    float           earthScaleFactor;
    ofTexture       earth_texture;
    ofxShader       earth_shader;
    
#ifdef SATELLITES
    // SATELLITES
    // -----------------------
    float           satellitesSize;
    vector<ofxSatellite> satellites;
#endif
    
    // HUD
    // -----------------------
    vector<SrcLine> lines;
    vector<HorLine> topoLines;
    ofVboMesh       billboard;
    
    // Ecliptical
    ofPoint         toEarth;
    
    // Equatorial
    ofPoint         v_equi;
    
    // Animation
    float           time_offset;
    float           time_step;
    bool            time_play;
    
    bool            bHelioCoords;
    bool            bEclipCoords;
    bool            bEquatCoords;
    bool            bHorizCoords;
    
    bool            bEquatDir;
    bool            bEquatDisk;
    
    bool            bBodiesTrail;
    
    bool            bHudLines;
    bool            bMoonPhases;
    
    bool            bTopoArrow;
    bool            bTopoDisk;
    bool            bTopoHud;
    bool            bTopoHudLables;
    bool            bTopoLables;
    
    bool            bDebugFps;
};
