//
//  ofxBody.h
//  Solar
//
//  Created by Patricio González Vivo on 1/18/18.
//

#pragma once

#include "ofMain.h"
#include "Astro/src/Body.h"

class ofxBody : public Body {
public:
    ofxBody();
    ofxBody(BodyId _planet);
    
    void drawTrail(ofFloatColor _color);
    void draw(ofFloatColor _color, float _size);
    
    void clearTale();
    
    glm::vec3   getGeoPosition(DISTANCE_UNIT _type);
    glm::vec3   getHelioPosition(DISTANCE_UNIT _type);
    
    glm::vec3   m_helioC;
    
protected:
    ofPolyline  m_trail;
};
