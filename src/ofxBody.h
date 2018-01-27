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
    ofxBody(BodyId _planet, float _size);
    
    void drawTrail(ofFloatColor _color);
    void drawSphere(ofFloatColor _color);
    
    ofPoint getGeoPosition();
    ofPoint getHelioPosition();
    
    ofPoint     m_helioC;
    
protected:
    ofPolyline  m_trail;
    float       m_size;
};
