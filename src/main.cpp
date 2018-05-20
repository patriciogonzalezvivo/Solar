#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

#ifdef TARGET_OPENGLES
    ofGLESWindowSettings settings;
    settings.setGLESVersion(2);
#else
    ofGLWindowSettings settings;
    settings.setGLVersion(3, 2);  // Programmable pipeline
#endif
    ofCreateWindow(settings);
    ofRunApp(new ofApp());
}

