//
//  ofxBody.cpp
//  Solar
//
//  Created by Patricio González Vivo on 1/18/18.
//

#include "ofxBody.h"

ofxBody::ofxBody() {
    m_bodyId = NAB;
}

ofxBody::ofxBody(BodyId _planet, float _size) {
    m_bodyId = _planet;
    m_size = _size;
}

ofVec3f ofxBody::getGeoPosition() {
    Vector gPos = getGeocentricVector();
    return ofPoint(gPos.x, gPos.y, gPos.z);
}

ofVec3f ofxBody::getHelioPosition() {
    Vector hPos = getHeliocentricVector();
    return ofPoint(hPos.x, hPos.y, hPos.z);
}

void ofxBody::drawTrail(ofFloatColor _color) {
    ofSetColor(_color);
    
    if ( m_trail.size() == 0) {
        m_trail.addVertex(m_helioC);
    } else if ( m_trail[m_trail.size()-1] != m_helioC ) {
        m_trail.addVertex(m_helioC);
    }
    m_trail.draw();
}

void ofxBody::drawSphere(ofFloatColor _color) {
    ofSetColor(_color);
    ofDrawSphere(m_helioC, m_size);
}
