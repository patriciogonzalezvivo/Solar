//
//  ofxMoon.hpp
//  Solar
//
//  Created by Patricio González Vivo on 1/21/18.
//

#pragma once

#include "ofMain.h"

class ofxMoon {
public:
    ofxMoon(ofPoint _pos, float _phase) {
        m_position = _pos;
        m_phase = _phase;
    }
    
    void draw(ofVboMesh &_bill, ofShader &_shader, float _size) {
        ofSetColor(255);
        ofPushMatrix();
        ofTranslate(m_position);
        ofScale(_size, _size);
        _shader.setUniform1f("u_synodic_day", m_phase);
        _bill.draw();
        ofPopMatrix();
    }
    
protected:
    ofPoint     m_position;
    float       m_phase;
};
