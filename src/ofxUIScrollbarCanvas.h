/*
 *  ofxUIScrollbarCanvas.h
 *
 *  an ofxUIRectangle that can be dragged around with the mouse
 *
 *  Created on: Sep 13, 2013
 *      Author: noura
 */

#ifndef example_all_ofxUIScrollbarCanvas_h
#define example_all_ofxUIScrollbarCanvas_h

#define OFX_UI_MIN_SCROLLBAR_H 15
#define OFX_UI_MIN_SCROLLBAR_W 10

#include "ofMain.h"
#include "ofxUI.h"

class ofxUIScrollbarCanvas : public ofxUIScrollableCanvas {

public:
    
    ofxUIScrollbarCanvas(float _x, float _y, float _w, float _h, float _contentHeight = 1000);
    ~ofxUIScrollbarCanvas();
    
    // returns the bounds of the area that is not obscured behind the scroll bar
    ofRectangle getAvailableSpace();
    void update();
    void draw();
    
    // You have to tell it how long the content will be if you want it to properly scroll to the bottom of the content.
    void setContentHeight(float _contentHeight);
    
    void setScrollbarImage(string imagePath);
    
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    
protected:
    //TODO I think I can use sRect dimensions instead of init_*
    float init_x, init_y, init_w, init_h;
    float scrollbar_w, scrollbar_h;
    float contentHeight;
    float scrollTop, scrollBottom;
    ofxUIDraggableRect * scrollbar;
    ofRectangle * scrollbarTrack;
    
};


#endif
