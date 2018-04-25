#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp{
    vector<ofPoint> vertices0;
    ofxPanel gui;
    ofxIntSlider countX;
    ofxFloatSlider stepX;
    ofxFloatSlider twistX;
    ofxGuiGroup globalGroup;
    ofxFloatSlider Scale;
    ofxFloatSlider Rotate;
    ofxFloatSlider Background;
    ofxGuiGroup primGroup;
    ofxFloatSlider shiftY, rotate;
    ofxVec2Slider size;
    ofxColorSlider color;
    ofxToggle filled, type;ofxIntSlider countY;
    ofxFloatSlider stepY, twistY, pinchY;
    ofTexture image;
    ofVideoPlayer video;
    ofVideoGrabber camera;
    ofxGuiGroup mixerGroup;
    ofxFloatSlider imageAlpha, videoAlpha, cameraAlpha;
    ofxFloatSlider show2d, show3d;
    ofxFloatSlider rad, deform, deformFreq, extrude;
    ofSoundPlayer sound;
    ofFile file;
    ofBuffer buffer;
    ofxToggle automate;
    ofxOscReceiver oscReceiver;
   
    //Shader
    ofShader shader;
    ofxToggle kenabled;
    ofxIntSlider ksectors;
    
    //3D
    ofxFloatSlider kangle, kx, ky;
    ofSpherePrimitive sphere;
    ofEasyCam cam;
    
    //FBO
    ofFbo fbo;
    ofFbo fbo2;
    ofFbo fbo3d;
    
    //Lights
    ofLight light;
    ofLight light2;
    
    //materials
    ofMaterial material;
    
    
   
    
    
	public:
		void setup();
        void update();
		void draw();
        void draw2d();
        void draw3d();
        void stripePattern();
        void exit();
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void audioIn(float *input, int bufferSize, int nChannels);
        float soundLevel;
        
    
        void matrixPattern();
        bool showGui;

		
};
