/*
 *  ofxUIDraggableRect.cpp
 *
 *  an ofxUIRectangle that can be dragged around with the mouse
 *
 *  Created on: Sep 13, 2013
 *      Author: noura
 */

#include "ofxUIDraggableRect.h"

ofxUIDraggableRect::ofxUIDraggableRect(float _x, float _y, float _w, float _h, ofRectangle _bounds)
: ofxUIRectangle(_x, _y, _w, _h)
, hit(false)
, mouseDx(0)
, mouseDy(0)
, imagePath("") {
    bounds = new ofxUIRectangle(_bounds.x, _bounds.y, _bounds.width, _bounds.height);
    ofAddListener(ofEvents().mousePressed, this, &ofxUIDraggableRect::onMousePressed);
    ofAddListener(ofEvents().mouseReleased, this, &ofxUIDraggableRect::onMouseReleased);
    ofAddListener(ofEvents().mouseDragged, this, &ofxUIDraggableRect::onMouseDragged);
}

ofxUIDraggableRect::~ofxUIDraggableRect() {
    delete bounds;
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
    //TODO this assumes that this->x and this->y are the top left corner and that bounds->x and bounds->y are the top left corner (i.e. that width and height are both positive for both this and bounds)
    if (hit) {
        x = bounded(e.x - mouseDx, bounds->x, bounds->x + bounds->width - width);
        y = bounded(e.y - mouseDy, bounds->y, bounds->y + bounds->height - height);
    }
}

float ofxUIDraggableRect::bounded(float x, float min, float max) {
    if ( x >= min and x <= max) {
        return x;
    } else if (x < min) {
        return min;
    } else if (x > max) {
        return max;
    }
    return x;
}

float ofxUIDraggableRect::getCenterX() {
    return getMinX() + width/2;
}

float ofxUIDraggableRect::getCenterY() {
    return getMinY() + height/2;
}

void ofxUIDraggableRect::setImage(string _imagePath) {
    // TODO it would probably be more efficient to take in a pointer to an ofImage in case multiple things wanted to share the same image. also ofxUI has some concept of shared resources which is probably for stuff like this.
    imagePath = _imagePath;
    image.loadImage(imagePath);
    image.resize(width, height);
}

void ofxUIDraggableRect::draw() {
    ofxUIRectangle::draw();
    if (imagePath != "") {
        ofPushStyle();
        ofSetColor(255);
        image.draw(x, y);
        ofPopStyle();
    }
}

void ofxUIDraggableRect::setHeight(float _h) {
    ofxUIRectangle::setHeight(_h);
    if (imagePath != "") {
        image.resize(width, height);
    }
}






