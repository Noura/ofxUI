/*
 *  ofxUIDraggableRect.h
 *
 *  an ofxUIRectangle that can be dragged around with the mouse
 *
 *  Created on: Sep 13, 2013
 *      Author: noura
 */

#include "ofxUIDraggableRect.h"

ofxUIDraggableRect::ofxUIDraggableRect(float _x, float _y, float _w, float _h)
: ofxUIRectangle(_x, _y, _w, _h)
, hit(false)
, mouseDx(0)
, mouseDy(0) {
    ofAddListener(ofEvents().mousePressed, this, &ofxUIDraggableRect::onMousePressed);
    ofAddListener(ofEvents().mouseReleased, this, &ofxUIDraggableRect::onMouseReleased);
    ofAddListener(ofEvents().mouseDragged, this, &ofxUIDraggableRect::onMouseDragged);
}

ofxUIDraggableRect::~ofxUIDraggableRect() {
    ofRemoveListener(ofEvents().mousePressed, this, &ofxUIDraggableRect::onMousePressed);
    ofRemoveListener(ofEvents().mouseReleased, this, &ofxUIDraggableRect::onMouseReleased);
    ofRemoveListener(ofEvents().mouseDragged, this, &ofxUIDraggableRect::onMouseDragged);
}

bool ofxUIDraggableRect::isHit() {
    return hit;
}

void ofxUIDraggableRect::onMousePressed(ofMouseEventArgs &e) {
    if (inside(e.x, e.y)) {
        hit = true;
        mouseDx = e.x - x;
        mouseDy = e.y - y;
    }
}

void ofxUIDraggableRect::onMouseReleased(ofMouseEventArgs &e) {
    hit = false;
    mouseDx = 0;
    mouseDy = 0;
}

void ofxUIDraggableRect::onMouseDragged(ofMouseEventArgs &e) {
    if (hit) {
        x = e.x - mouseDx;
        y = e.y - mouseDy;
    }
}