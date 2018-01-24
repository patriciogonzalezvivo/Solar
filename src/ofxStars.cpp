#include "ofxStars.h"
#include "Astro/src/AstroOps.h"

ofxStars::ofxStars() {
    for (int i = 0; i < Star::TOTAL; i++) {
        m_stars.push_back(Star(i));
    }
    m_starsPos.reserve(Star::TOTAL);
    m_starsCol.reserve(Star::TOTAL);
    m_starsMesh.setMode(OF_PRIMITIVE_POINTS);
    
    for (int i = 0; i < Constellation::TOTAL; i++) {
        m_constellations.push_back(Constellation(i));
    }
    m_constellationsMesh.setMode(OF_PRIMITIVE_LINES);
    
    m_starsShader.load("shaders/stars");
}
//
//ofPoint coord2TopoSphere(Observer &_obs, Star &_star, float _distance) {
//    _star.compute(_obs);
//
//    ofVec3f displacementFromCenter = ofVec3f(0.0f,0.0f,_distance);
//    ofQuaternion longRot;
//    longRot.makeRotate(-_star.getAzimuth(), 1.0f, 0.0f, 0.0f);
//    ofQuaternion latRot;
//    latRot.makeRotate(_star.getAltitud(), 0.0f, 1.0f, 0.0f);
//    ofQuaternion level;
//    level.makeRotate(90, 0.0f, 0.0f, 1.0f);
//    return level * (longRot * (latRot * displacementFromCenter));
//}
//
//void ofxStars::updateTopo(Observer &_obs, float _radius) {
//    for (int i = 0; i < m_stars.size(); i++) {
//        m_starsPos[i] = coord2TopoSphere(_obs, m_stars[i], _radius);
//    }
//    m_starsMesh.clear();
//    m_starsMesh.addVertices(m_starsPos);
//
//    m_constellationsMesh.clear();
//    for (auto& constellation : m_constellations) {
//        ofPoint prev = ofPoint(0.0);
//        vector<int> indices = constellation.getStarIndices();
//        for (int i = 0; i < indices.size(); i+=2) {
//            m_constellationsMesh.addVertex(m_starsPos[indices[i]]);
//            m_constellationsMesh.addVertex(m_starsPos[indices[i+1]]);
//        }
//    }
//}

ofPoint coord2EquatorialSphere(Star &_star, float _distance) {
    Vector eclip = _star.getEquatorialVector() * _distance;
    return ofPoint(eclip.x, eclip.y, eclip.z);
}

void ofxStars::updateEqua(float _radius) {
    for (int i = 0; i < m_stars.size(); i++) {
        m_starsPos[i] = coord2EquatorialSphere( m_stars[i], _radius);
        m_starsCol[i] = ofFloatColor(m_stars[i].getMagnitud(), 1., 1.);
    }
    m_starsVbo.setVertexData(&m_starsPos[0], Star::TOTAL, GL_STATIC_DRAW);
//    m_starsVbo.setColorData(&m_starsCol[0], Star::TOTAL, GL_STATIC_DRAW);
    m_starsMesh.clear();
    m_starsMesh.addVertices(m_starsPos);
//    m_starsMesh.addColors(m_starsCol);
    
    m_constellationsMesh.clear();
    for (auto& constellation : m_constellations) {
        ofPoint prev = ofPoint(0.0);
        vector<int> indices = constellation.getStarIndices();
        for (int i = 0; i < indices.size(); i+=2) {
            m_constellationsMesh.addVertex(m_starsPos[indices[i]]);
            m_constellationsMesh.addVertex(m_starsPos[indices[i+1]]);
        }
    }
}

void ofxStars::drawStars (ofCamera &_cam) {
    glDepthMask(GL_FALSE);
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofEnablePointSprites();
    
    m_starsShader.begin();
    _cam.begin();
    
    m_starsVbo.draw(GL_POINTS, 0, m_starsPos.size()-1);
    m_starsMesh.draw();

    _cam.end();
    m_starsShader.end();
    
    ofDisablePointSprites();
    ofDisableBlendMode();
    glDepthMask(GL_TRUE);
}

void ofxStars::drawConstellations () {
    m_constellationsMesh.draw();
}
