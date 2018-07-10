//
//  ofxSatellite.cpp
//  Solar
//
//  Created by Patricio González Vivo on 6/3/18.
//

#include "ofxSatellite.h"

ofxSatellite::ofxSatellite() {
    m_bodyId = NAB;
}

ofxSatellite::ofxSatellite(const TLE& _tle) {
    setTLE(_tle);
}

glm::vec3 ofxSatellite::getGeoPosition(DISTANCE_UNIT _type) {
    Vector gPos = getEclipticGeocentric().getVector(_type);
    return glm::vec3(gPos.x, gPos.y, gPos.z);
}

glm::vec3 ofxSatellite::getHelioPosition(DISTANCE_UNIT _type) {
    Vector hPos = getEclipticHeliocentric().getVector(_type);
    return glm::vec3(hPos.x, hPos.y, hPos.z);
}

void ofxSatellite::drawGeocentricTrail(ofFloatColor _color) {
    ofSetColor(_color);
    
    if ( m_geoTrail.size() == 0) {
        m_geoTrail.addVertex(m_geoC);
    } else if ( m_geoTrail[m_geoTrail.size()-1].x != m_geoC.x ||
               m_geoTrail[m_geoTrail.size()-1].y != m_geoC.y ||
               m_geoTrail[m_geoTrail.size()-1].z != m_geoC.z ) {
        m_geoTrail.addVertex(m_geoC);
    }
    m_geoTrail.draw();
}

void ofxSatellite::drawHeliocentricTrail(ofFloatColor _color) {
    ofSetColor(_color);
    
    if ( m_helioTrail.size() == 0) {
        m_helioTrail.addVertex(m_helioC);
    } else if ( m_helioTrail[m_helioTrail.size()-1].x != m_helioC.x ||
               m_helioTrail[m_helioTrail.size()-1].y != m_helioC.y ||
               m_helioTrail[m_helioTrail.size()-1].z != m_helioC.z ) {
        m_helioTrail.addVertex(m_helioC);
    }
    m_helioTrail.draw();
}

void ofxSatellite::clearTale() {
    m_helioTrail.clear();
    m_geoTrail.clear();
}

void ofxSatellite::draw(ofFloatColor _color, float _size) {
    ofPushMatrix();
    ofTranslate(m_helioC);
    ofSetColor(_color);
    ofDrawBox(_size);
    
    glm::vec3 fromEarth = m_geoC * 0.25;
    ofSetColor(170);
    ofDrawLine(ofPoint(0.0), fromEarth);
    ofSetColor(250);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD );
    ofDrawBitmapString(getName(), fromEarth + _size);
    ofPopMatrix();
}
