/*
 *  ofxUIScrollbarCanvas.cpp
 *
 *  an ofxUIRectangle that can be dragged around with the mouse
 *
 *  Created on: Sep 13, 2013
 *      Author: noura
 */

#include "ofxUIScrollbarCanvas.h"

ofxUIScrollbarCanvas::ofxUIScrollbarCanvas(float _x, float _y, float _w, float _h, float _contentHeight)
: ofxUIScrollableCanvas(_x, _y, _w, _h)
, init_x(_x), init_y(_y), init_w(_w), init_h(_h) {
    scrollbar_w = OFX_UI_MIN_SCROLLBAR_W;
    scrollbar_h = OFX_UI_MIN_SCROLLBAR_H;
    kind = OFX_UI_WIDGET_SCROLLBARCANVAS;

    scrollbarTrack = new ofRectangle(_x + _w - scrollbar_w, _y, scrollbar_w, _h);
    scrollbar = new ofxUIDraggableRect(_x + _w - scrollbar_w, _y, scrollbar_w, scrollbar_h, *scrollbarTrack);
    
    setContentHeight(_contentHeight);
}

ofxUIScrollbarCanvas::~ofxUIScrollbarCanvas() {
    delete scrollbar;
    //TODO delete other things (in separate commit)
}

ofRectangle ofxUIScrollbarCanvas::getAvailableSpace() {
    return ofRectangle(rect->x, rect->y, rect->width - scrollbar_w, rect->height);
}

void ofxUIScrollbarCanvas::update() {
    /*
    ofxUIScrollableCanvas::update();
    float scrollPercent = (scrollbar->getCenterY() - scrollTop) / (scrollBottom - scrollTop);
    sRect->y = rect->y + (scrollPercent * (rect->getMaxY() - fabs(rect->height)));
     */
}

void ofxUIScrollbarCanvas::draw() {
    ofxUIScrollableCanvas::draw();
    scrollbar->draw();
}

void ofxUIScrollbarCanvas::mousePressed(int x, int y, int button) {
    if (!sRect->inside(x, y)) {
        return;
    }
    if (x < scrollbarTrack->x) {
        ofxUIScrollableCanvas::mousePressed(x, y, button);
    }
}

void ofxUIScrollbarCanvas::mouseReleased(int x, int y, int button) {
    if (!sRect->inside(x, y)) {
        return;
    }
    if (x < scrollbarTrack->x) {
        ofxUIScrollableCanvas::mouseReleased(x, y, button);
    }
}

void ofxUIScrollbarCanvas::mouseDragged(int x, int y, int button) {
    if (scrollbar->isHit() && scrollBottom != scrollTop) {
        float scrollPercent = (scrollbar->getCenterY() - scrollTop) / (scrollBottom - scrollTop);
        rect->y = init_y - scrollPercent * (contentHeight - init_h);
    } else {
        ofxUIScrollableCanvas::mouseDragged(x, y, button);
        float scrollPercent = (init_y - rect->y) / (contentHeight - init_h);
        scrollPercent = MAX(0.0, scrollPercent);
        scrollPercent = MIN(1.0, scrollPercent);
        float scrollbarY = init_y + scrollPercent * (init_h - scrollbar->height);
        scrollbar->y = scrollbarY;
    }
}

void ofxUIScrollbarCanvas::setContentHeight(float _contentHeight) {
    contentHeight = _contentHeight;
    scrollbar_h = MAX(OFX_UI_MIN_SCROLLBAR_H, MIN(init_h * init_h / contentHeight, init_h));
    
    scrollbar->setHeight(scrollbar_h);
    
    scrollTop = init_y + scrollbar->height/2;
    scrollBottom = init_y + init_h - scrollbar->height/2;
}

void ofxUIScrollbarCanvas::setScrollbarImage(string imagePath) {
    scrollbar->setImage(imagePath);
}