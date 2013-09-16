/*
 *  ofxUIDraggableRect.h
 *
 *  an ofxUIRectangle that can be dragged around with the mouse
 *
 *  Created on: Sep 13, 2013
 *      Author: noura
 */

#ifndef example_all_ofxUIDraggableRect_h
#define example_all_ofxUIDraggableRect_h

#include "ofxUIRectangle.h"
#include "ofMain.h"

class ofxUIDraggableRect : public ofxUIRectangle {

public:
    
    ofxUIDraggableRect(float _x, float _y, float _w, float _h, ofRectangle _bounds);
    ~ofxUIDraggableRect();
    
    bool isHit();
    float getCenterX();
    float getCenterY();
    
    void setHeight(float _h);
    void setImage(string _imagePath);
    
    void draw();
    
    virtual void onMousePressed(ofMouseEventArgs &e);
    virtual void onMouseReleased(ofMouseEventArgs &e);
    virtual void onMouseDragged(ofMouseEventArgs &e);
    
protected:
    
    float bounded(float x, float min, float max);
    
    bool hit;
    float mouseDx, mouseDy;
    ofxUIRectangle * bounds;
    ofImage image;
    string imagePath;

};


#endif
