#pragma once

#include "ofMain.h"
#include "Astro/src/Star.h"
#include "Astro/src/Constellation.h"

class ofxStars {
public:
    ofxStars();
    
    void updateEqua(float _radius);
//    void updateTopo(Observer &_obs, float _radius);
    
    void drawStars(ofCamera &_cam);
    void drawConstellations();
    
private:
    std::vector<int>            virtualsky_i;
    std::vector<Star>           m_stars;
    std::vector<ofPoint>        m_starsPos;
    std::vector<ofFloatColor>   m_starsCol;
    ofVbo                       m_starsVbo;
    ofVboMesh                   m_starsMesh;
    ofShader                    m_starsShader;
    
    std::vector<Constellation>  m_constellations;
    ofMesh                      m_constellationsMesh;
};
