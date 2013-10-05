/*
 *  ofxUIDraggableRect.h
 *
 *  an ofxUIRectangle that can be dragged around with the mouse
 *
 *  Created on: Sep 13, 2013
 *      Author: noura
 */

#ifndef OFXUI_DRAGGABLE_RECT
#define OFXUI_DRAGGABLE_RECT

#include "ofxUIRectangle.h"
#include "ofMain.h"

class ofxUIDraggableRect : public ofxUIRectangle {

public:
    
    ofxUIDraggableRect(float _x, float _y, float _w, float _h, ofRectangle _bounds)
    : ofxUIRectangle(_x, _y, _w, _h)
    , hit(false)
    , mouseDx(0)
    , mouseDy(0)
    , image(NULL) {
        bounds = new ofxUIRectangle(_bounds.x, _bounds.y, _bounds.width, _bounds.height);
        ofAddListener(ofEvents().mousePressed, this, &ofxUIDraggableRect::onMousePressed);
        ofAddListener(ofEvents().mouseReleased, this, &ofxUIDraggableRect::onMouseReleased);
        ofAddListener(ofEvents().mouseDragged, this, &ofxUIDraggableRect::onMouseDragged);
    }
    
    ~ofxUIDraggableRect() {
        delete bounds;
        delete image;
        ofRemoveListener(ofEvents().mousePressed, this, &ofxUIDraggableRect::onMousePressed);
        ofRemoveListener(ofEvents().mouseReleased, this, &ofxUIDraggableRect::onMouseReleased);
        ofRemoveListener(ofEvents().mouseDragged, this, &ofxUIDraggableRect::onMouseDragged);
    }
    
    bool isHit() {
        return hit;
    }
    
    float getCenterX() {
        return getMinX() + width/2;
    }
    
    float getCenterY() {
        return getMinY() + height/2;
    }
    
    void setHeight(float _h) {
        ofxUIRectangle::setHeight(_h);
        if (image) {
            image->resize(width, height);
        }
    }
    
    void setImage(string _imagePath) {
        delete image;
        image = new ofImage();
        image->loadImage(_imagePath);
        image->resize(width, height);
    }
    
    void draw() {
        ofxUIRectangle::draw();
        if (image) {
            ofPushStyle();
            ofSetColor(255);
            image->draw(x, y);
            ofPopStyle();
        }
    }
    
    virtual void onMousePressed(ofMouseEventArgs &e) {
        if (inside(e.x, e.y)) {
            hit = true;
            mouseDx = e.x - x;
            mouseDy = e.y - y;
        }
    }
    
    virtual void onMouseReleased(ofMouseEventArgs &e) {
        hit = false;
        mouseDx = 0;
        mouseDy = 0;
    }
    
    virtual void onMouseDragged(ofMouseEventArgs &e) {
        //TODO this assumes that this->x and this->y are the top left corner and that bounds->x and bounds->y are the top left corner (i.e. that width and height are both positive for both this and bounds)
        if (hit) {
            x = bounded(e.x - mouseDx, bounds->x, bounds->x + bounds->width - width);
            y = bounded(e.y - mouseDy, bounds->y, bounds->y + bounds->height - height);
        }
    }
    
protected:
    
    float bounded(float x, float min, float max) {
        if ( x >= min and x <= max) {
            return x;
        } else if (x < min) {
            return min;
        } else if (x > max) {
            return max;
        }
        return x;
    }
    
    bool hit;
    float mouseDx, mouseDy;
    ofxUIRectangle * bounds;
    ofImage * image;

};


#endif
