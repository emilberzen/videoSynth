#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    sphere.set(250, 80);
    oscReceiver.setup( 12345 );
    vertices0 = sphere.getMesh().getVertices();
    
    file.open("a.txt");
    buffer.set(file);
    
    //sound
    sound.loadSound( "bensound-scifi.mp3" );
    sound.setVolume( 0.8 );
    sound.setLoop( true );
    ofSoundStreamSetup( 0, 1, 44100, 128, 4 );
    soundLevel = 0;
    
    ofSetWindowTitle( "Video synth" );
    ofToggleFullscreen();
    ofSetFrameRate( 60 );
    ofLoadImage( image, "IMG_1845.JPG" );
    
    video.loadMovie( "Spring Break.mov" );
    video.play();
    
    fbo.allocate( ofGetWidth(), ofGetHeight(), GL_RGB );
    fbo2.allocate( ofGetWidth(), ofGetHeight(), GL_RGB );
    float w = fbo2.getWidth();
    float h = fbo2.getHeight(); sphere.mapTexCoords(0, h, w, 0); sphere.rotate(180, 0, 1, 0);
    
    gui.setup( "Parameters", "settings.xml" );
    gui.add( countX.setup( "countX", 50, 0, 200 ) );
    gui.add( stepX.setup( "stepX", 20, 0, 255 ) );
    gui.add( twistX.setup( "twistX", 5, -45, 180 ) );
    gui.add( countY.setup( "countY", 0, 0, 50) );
    gui.add( stepY.setup( "stepY", 20, 0, 200 ) );
    gui.add( twistY.setup( "twistY", 0, -30, 180 ) );
    gui.add( pinchY.setup( "pinchY", 0, 0, 1 ) );
    
    primGroup.setup( "Primitive" );
    primGroup.add( shiftY.setup("shiftY",0.0,-1000.0,1000.0 ) );
    primGroup.add( rotate.setup("rotate",0.0,-360.0,360.0 ) );
    primGroup.add( size.setup( "size",ofVec2f(6,6),ofVec2f(0,0),ofVec2f(20,20) ) );
    primGroup.add( color.setup( "color", ofColor::black,ofColor(0,0,0,0),ofColor::white ) );
    primGroup.add( filled.setup( "filled", false ) );
    gui.add( &primGroup );
    
    mixerGroup.setup( "Mixer" );
    mixerGroup.setHeaderBackgroundColor( ofColor::darkRed );
    mixerGroup.setBorderColor( ofColor::darkRed );
    mixerGroup.add( imageAlpha.setup( "image", 100,0,255 ) );
    mixerGroup.add( videoAlpha.setup( "video", 200,0,255 ) );
    mixerGroup.add( cameraAlpha.setup( "camera", 100,0,255 ) );
    
    shader.load( "kaleido" );
    mixerGroup.add( kenabled.setup( "kenabled", true ) );
    mixerGroup.add( ksectors.setup( "ksectors", 10, 1, 100 ) );
    mixerGroup.add( kangle.setup( "kangle", 0, -180, 180 ) );
    mixerGroup.add( kx.setup( "kx", 0.5, 0, 1 ) );
    mixerGroup.add( ky.setup( "ky", 0.5, 0, 1 ) );
    mixerGroup.add( show2d.setup("show2d", 255, 0, 255) );
    mixerGroup.add( show3d.setup("show3d", 255, 0, 255) );
    mixerGroup.add( rad.setup("rad", 250, 0, 500) );
    mixerGroup.add( deform.setup("deform", 0.3, 0, 1.5) );
    mixerGroup.add( deformFreq.setup("deformFreq", 3, 0, 10) );
    mixerGroup.add( extrude.setup("extrude", 0, 0, 1 ) );
    mixerGroup.add( automate.setup( "automate", true ) );
    
    fbo3d.allocate( ofGetWidth(), ofGetHeight(), GL_RGBA );
    gui.minimizeAll();
    gui.add( &mixerGroup );
    
    gui.loadFromFile( "settings.xml" );
    globalGroup.setup( "Global" );
    globalGroup.add( Background.setup("Background",255,0, 255));
    gui.add( &globalGroup );
    
    
    showGui = true;

    

}

//--------------------------------------------------------------
void ofApp::update(){
    video.update();
    ofSoundUpdate();
    if ( automate ) {
    if ( camera.isInitialized() ) camera.update();
    
        
        //OSC
        while ( oscReceiver.hasWaitingMessages() ) { ofxOscMessage m; oscReceiver.getNextMessage( &m );
            if ( m.getAddress() == "/pinchY" ) {
                pinchY = m.getArgAsFloat( 0 );
            }
        }
        
    //deforms sphere
    vector<ofPoint> &vertices = sphere.getMesh().getVertices();
    for (int i=0; i<vertices.size(); i++) {
        ofPoint v = vertices0[i]; v.normalize();
        float sx = cos( v.x * deformFreq );
        float sy = sin( v.y * deformFreq );
        float sz = cos( v.z * deformFreq );
        v.x += sy * sz * deform;
        v.y += sx * sz * deform;
        v.z += sx * sy * deform;
        v *= rad;
        vertices[i] = v;
    }
    
    //deforms sphere based on brightness of pixels
    ofPixels pixels;
    fbo2.readToPixels(pixels);
    for (int i=0; i<vertices.size(); i++) {
        ofVec2f t = sphere.getMesh().getTexCoords()[i];
        t.x = ofClamp( t.x, 0, pixels.getWidth()-1 );
        t.y = ofClamp( t.y, 0, pixels.getHeight()-1 );
        float br = pixels.getColor(t.x, t.y).getBrightness();
        vertices[i] *= 1 + br / 255.0 * extrude;
    }
    
       
    //LFO
    float phase = 0.01 * ofGetElapsedTimef() * M_TWO_PI;
    float value = sin( phase * 5);
    rotate = ofMap(value, -1, 1, 0, 360);
    
    float phase1 = 0.2 * ofGetElapsedTimef();
   // deformFreq = ofNoise( ofMap(phase1,-1,1,5,10) );
    
    float *spectrum = ofSoundGetSpectrum(128);
    double level = 0;
    for (int i=0; i<128; i++) {
        level += spectrum[i] * spectrum[i];
    }
    level = sqrt( level / 128 );
    
        
    //Deform with soundlevel 
    level += soundLevel;
    float newRad = ofMap( level, 0, 1, 0, 10, true );
    
    //deformFreq = deformFreq + 0.2 * (newRad-deformFreq);
   
    if ( buffer.isLastLine() ) buffer.resetLineReader();
    string line = buffer.getNextLine();
    vector<string> values = ofSplitString( line, "\t" );
    if ( values.size() >= 128 ) {
        float value1 = ofToFloat(values[1]);
        float value2 = ofToFloat(values[5]);
        float value3 = ofToFloat(values[100]);
        twistX = ofMap( value2-value1, -9400, -9100, -10, 10 );
        twistY = ofMap( value3-value1, -1700, -1650, -10, 10 );
    }
    
    
    
    
    }

}


void ofApp::stripePattern(){
    float time = ofGetElapsedTimef();
    ofSetColor( color );
    ofSetLineWidth( 1 );
    
    //Create a pattern of triangles
    if ( filled ) ofFill(); else ofNoFill(); for (int i=-countX; i<=countX; i++) {
        ofPushMatrix();
        ofTranslate( i * stepX, 0 );
        ofRotate( i * twistX );
        ofTranslate( 0, shiftY );
        ofRotate( rotate );
        ofScale( size->x, size->y );
        ofDrawTriangle( 50, 0, -50, 100, 50, 100 );
       
        

        ofPopMatrix();
    }
   
    }

void ofApp::matrixPattern() {
    for (int y=-countY; y<=countY; y++) {
        ofPushMatrix();
        ofSetLineWidth(3.0);
        //---------------------
        if ( countY > 0 ) {
            float scl = ofMap( y, -countY, countY, 1-pinchY, 1 );
            ofScale( scl, scl );
        }
        ofTranslate( 0, y * stepY );
        ofRotate( y * twistY );
        stripePattern();
        //---------------------
        ofPopMatrix();
    } }
    

//--------------------------------------------------------------
void ofApp::draw(){
    fbo.begin();
    draw2d();
    fbo.end();
    
    //Shader
    ofSetColor( 255 );
    fbo2.begin();
    
    if ( kenabled ) {
        shader.begin();
        shader.setUniform1i( "ksectors", ksectors );
        shader.setUniform1f( "kangleRad", ofDegToRad(kangle) );
        shader.setUniform2f( "kcenter", kx*ofGetWidth(),
                            ky*ofGetHeight() );
        shader.setUniform2f( "screenCenter", 0.5*ofGetWidth(),
                            0.5*ofGetHeight() );
    }
    fbo.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    if ( kenabled ) shader.end();
    fbo2.end();
    
    //drawing 3d & 2d
    fbo3d.begin();
    ofBackground( 255);
    draw3d();
    fbo3d.end();
    ofBackground( 0 );
    ofSetColor( 255, show2d );
    fbo2.draw( 0, 0 );
    ofSetColor( 255, show3d );
    fbo3d.draw( 0, 0 );
    //draws GUI
    if ( showGui ) gui.draw();


}

void ofApp::draw2d(){
    ofBackground( Background );
    
    //2D 3D mixer
    ofEnableBlendMode( OF_BLENDMODE_ADD );
    ofDisableSmoothing();
    ofSetColor( 255, imageAlpha );
    image.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    ofSetColor( 255,videoAlpha );
    video.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    

    if ( camera.isInitialized() ) {
        ofSetColor( 255,cameraAlpha );
        camera.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    }
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    
    matrixPattern();
    
    ofPopMatrix();
    
}

void ofApp::draw3d(){
    cam.begin();
    fbo2.getTexture().bind();
    float time = ofGetElapsedTimef();
    float longitude = 20*time;
    float latitude = 10*cos(time*0.8);
    float radius = 600 + 50*cos(time*0.4);
    cam.orbit( longitude, latitude, radius, ofPoint(0,0,0) );
    
    
    //Light and material
    light.setPosition(ofGetWidth()/2, ofGetHeight()/2, 6000);
    light.enable();
    light2.setPosition(ofGetWidth(), ofGetHeight(), -2000);
    light2.enable();

    material.begin();
    ofEnableDepthTest();
    
    
   
    ofSetColor( ofColor::white );
    //sphere.drawWireframe();
    sphere.draw();
    cam.end();
    
    ofDisableDepthTest();
    material.end();
    light.disable();
    light2.disable();
    ofDisableLighting();
    fbo2.getTexture().unbind();
    
    
}

void ofApp::audioIn(float *input, int bufferSize, int nChannels){
    double v = 0;
    for (int i=0; i<bufferSize; i++) {
        v += input[i] * input[i];
    }
    v = sqrt( v / bufferSize );
    soundLevel = v;
}

void ofApp::exit() {
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key=='z') showGui = !showGui;
    if ( key == OF_KEY_RETURN ) ofSaveScreen( "screenshot" + ofToString(ofRandom(0, 1000), 0) + ".png");
    
    if ( key == 's' ) {
        ofFileDialogResult res;
        res = ofSystemSaveDialog( "preset.xml", "Saving Preset" ); if ( res.bSuccess ) gui.saveToFile( res.filePath );
    }
    if ( key == 'l' ) {
        ofFileDialogResult res;
        res = ofSystemLoadDialog( "Loading Preset" );
        if ( res.bSuccess ) gui.loadFromFile( res.filePath );
    }
    
    if( key == 'q'){
        video.close();
    }
    
    if ( key == 'c' ) {
        camera.setDeviceID( 0 );
        camera.setDesiredFrameRate( 30 );
        camera.initGrabber( 1280, 720 );
    }
    if ( key == 'p' ) {
        if ( !sound.getIsPlaying() ) sound.play();
        else sound.stop();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
    if ( showGui && x < 250 ) cam.disableMouseInput();
    else cam.enableMouseInput();
    

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
