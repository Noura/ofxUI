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

#define OFX_UI_MIN_SCROLLBAR_H 25
#define OFX_UI_MIN_SCROLLBAR_W 15

#include "ofMain.h"
#include "ofxUI.h"

class ofxUIScrollbarCanvas : public ofxUIScrollableCanvas {

public:
    
    ofxUIScrollbarCanvas(float _x, float _y, float _w, float _h, float _contentHeight = 1000)
    : ofxUIScrollableCanvas(_x, _y, _w, _h)
    , init_x(_x) ,init_y(_y), init_w(_w), init_h(_h) {
        scrollbar_w = OFX_UI_MIN_SCROLLBAR_W;
        scrollbar_h = OFX_UI_MIN_SCROLLBAR_H;
        kind = OFX_UI_WIDGET_SCROLLBARCANVAS;
        
        scrollbarTrack = new ofRectangle(_x + _w - scrollbar_w, _y, scrollbar_w, _h);
        scrollbar = new ofxUIDraggableRect(_x + _w - scrollbar_w, _y, scrollbar_w, scrollbar_h, *scrollbarTrack);
        
        setContentHeight(_contentHeight);
    }
        
    ~ofxUIScrollbarCanvas() {
        delete scrollbar;
        //TODO delete other things in separate commit
    }
        
    // returns the bounds of the area that is not obscured behind the scroll bar
    ofRectangle getAvailableSpace()  {
        return ofRectangle(rect->x, rect->y, rect->width - scrollbar_w, rect->height);
    }
    
    void draw() {
        ofxUIScrollableCanvas::draw();
        scrollbar->draw();
    }
    
    // You have to tell it how long the content will be if you want it to properly scroll to the bottom of the content.
    void setContentHeight(float _contentHeight) {
        contentHeight = _contentHeight;
        scrollbar_h = MAX(OFX_UI_MIN_SCROLLBAR_H, MIN(init_h * init_h / contentHeight, init_h));
        
        scrollbar->setHeight(scrollbar_h);
        
        scrollTop = init_y + scrollbar->height/2;
        scrollBottom = init_y + init_h - scrollbar->height/2;
    }
    
    // This will re-flow the content to fill gaps left by removed widgets
    void reflowWidgets(vector<ofxUIWidget*> ws, float h, float y0) {
        float y = y0;
        for (vector<ofxUIWidget*>::iterator it = ws.begin(); it < ws.end(); it++) {
            (*it)->getRect()->setY(y);
            y += h;
        }
    }
    
    // optionally use an image as the scrollbar
    void setScrollbarImage(string imagePath) {
        scrollbar->setImage(imagePath);
    }
    
    void mousePressed(int x, int y, int button) {
        if (!sRect->inside(x, y)) {
            return;
        }
        if (!scrollbarTrack->inside(x, y)) {
            ofxUIScrollableCanvas::mousePressed(x, y, button);
        }
    }

    void mouseReleased(int x, int y, int button) {
        if (!sRect->inside(x, y)) {
            return;
        }
        if (!scrollbarTrack->inside(x,y)) {
            ofxUIScrollableCanvas::mouseReleased(x, y, button);
        }
    }

    void mouseDragged(int x, int y, int button) {
        if (scrollbar->isHit() && scrollBottom != scrollTop) {
            float scrollPercent = (scrollbar->getCenterY() - scrollTop) / (scrollBottom - scrollTop);
            scrollPercent = CLAMP(scrollPercent, 0.0, 1.0);
            rect->y = init_y - scrollPercent * (contentHeight - init_h);
        } else {
            ofxUIScrollableCanvas::mouseDragged(x, y, button);
            float scrollPercent = (init_y - rect->y) / (contentHeight - init_h);
            scrollPercent = CLAMP(scrollPercent, 0.0, 1.0);
            float scrollbarY = init_y + scrollPercent * (init_h - scrollbar->height);
            scrollbar->y = scrollbarY;
        }
    }
    
    void scrollToBottom() {
        rect->y = init_y - (contentHeight - init_h);
        scrollbar->y = init_y + (init_h - scrollbar->height);
    }
    
    
    
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
