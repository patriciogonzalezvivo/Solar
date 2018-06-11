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

ofxBody::ofxBody(BodyId _planet) {
    m_bodyId = _planet;
}

glm::vec3 ofxBody::getGeoPosition(UNIT_TYPE _type) {
    Vector gPos = getEclipticGeocentric().getVector(_type);
    return glm::vec3(gPos.x, gPos.y, gPos.z);
}

glm::vec3 ofxBody::getHelioPosition(UNIT_TYPE _type) {
    Vector hPos = getEclipticHeliocentric().getVector(_type);
    return glm::vec3(hPos.x, hPos.y, hPos.z);
}

void ofxBody::drawTrail(ofFloatColor _color) {
    ofSetColor(_color);
    
    if ( m_trail.size() == 0) {
        m_trail.addVertex(m_helioC);
    } else if ( m_trail[m_trail.size()-1].x != m_helioC.x ||
                m_trail[m_trail.size()-1].y != m_helioC.y ||
                m_trail[m_trail.size()-1].z != m_helioC.z ) {
        m_trail.addVertex(m_helioC);
    }
    m_trail.draw();
}

void ofxBody::clearTale() {
    m_trail.clear();
}

void ofxBody::draw(ofFloatColor _color, float _size) {
    ofSetColor(_color);
    ofDrawSphere(m_helioC, _size);
    
    if (m_bodyId != EARTH &&
        m_bodyId != LUNA &&
        m_bodyId != SUN) {
        ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD );
        ofDrawBitmapString(getName(), m_helioC + ofPoint(_size*2. + 1.5));
    }
}
