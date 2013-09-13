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

class ofxUIDraggableRect : public ofxUIRectangle {

public:
    
    ofxUIDraggableRect(float _x, float _y, float _w, float _h);
    ~ofxUIDraggableRect();
    
    bool isHit();
    
    virtual void onMousePressed(ofMouseEventArgs &e);
    virtual void onMouseReleased(ofMouseEventArgs &e);
    virtual void onMouseDragged(ofMouseEventArgs &e);
    
protected:
    
    bool hit;
    float mouseDx, mouseDy;
    
};


#endif
