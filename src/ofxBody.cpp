//
//  ofxBody.cpp
//  Solar
//
//  Created by Patricio González Vivo on 1/18/18.
//

#include "ofxBody.h"

const std::string body_names[] = { "Sun", "Mer", "Ven", "Earth", "Mar", "Jup", "Sat", "Ur", "Nep", "Pl", "Moon" };

ofxBody::ofxBody() {
    m_bodyId = NAB;
}

ofxBody::ofxBody(BodyId _planet, float _size) {
    m_bodyId = _planet;
    m_size = _size;
}

ofPoint ofxBody::getGeoPosition() {
    Vector gPos = getGeocentricVector();
    return ofPoint(gPos.x, gPos.y, gPos.z);
}

ofPoint ofxBody::getHelioPosition() {
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
    
    if (m_bodyId != EARTH &&
        m_bodyId != LUNA &&
        m_bodyId != SUN) {
        ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD );
        ofDrawBitmapString(body_names[m_bodyId], m_helioC + ofPoint(m_size*2. + 1.5));
    }
}
