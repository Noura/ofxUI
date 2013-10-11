/*
 *  ofxUIScrollbarCanvas.h
 *
 *  This is intended to be used as a vertically scrolling list of items that can
 *  be dynamically added to and removed from. To enable the list to be dynamic,
 *  some of the features of ofxUICanvas or ofxUIScrollableCanvas have been
 *  removed.
 *  
 *  To add widgets you should only use
 *  ofxUIScrollbarCanvas::addWidgetToList
 *
 *  To remove widgets you should only use
 *  ofxUIScrollbarCanvas::removeWidgetFromList
 *
 *  Created on: Sep 13, 2013
 *      Author: noura
 */

#ifndef OFXUI_SCROLLBAR_CANVAS
#define OFXUI_SCROLLBAR_CANVAS

#define OFX_UI_MIN_SCROLLBAR_H 25
#define OFX_UI_MIN_SCROLLBAR_W 15

#include "ofMain.h"
#include "ofxUI.h"

// TODO fix the bug where if the content is so short it does not need to scroll, sometimes the scrollbar does not make itself the right size and so scrolling appears "backwards" because as you drag the scrollbar the content "sticks" to the scrollbar

class ofxUIScrollbarCanvas : public ofxUIScrollableCanvas {

public:
    
    //TODO when the content starts out really short and is growing taller, the scrollbar grows taller with the content. this seems weird.
    
    ofxUIScrollbarCanvas(float _x, float _y, float _w, float _h)
    : ofxUIScrollableCanvas(_x, _y, _w, _h)
    , init_x(_x)
    , init_y(_y)
    , init_w(_w)
    , init_h(_h)
    , listPadding(5.0) {
        scrollbar_w = OFX_UI_MIN_SCROLLBAR_W;
        scrollbar_h = OFX_UI_MIN_SCROLLBAR_H;
        kind = OFX_UI_WIDGET_SCROLLBARCANVAS;
        
        scrollbarTrack = new ofRectangle(_x + _w - scrollbar_w, _y, scrollbar_w, _h);
        scrollbar = new ofxUIDraggableRect(_x + _w - scrollbar_w, _y, scrollbar_w, scrollbar_h, *scrollbarTrack);
        
        setContentHeight(_h);
    }

    ~ofxUIScrollbarCanvas() {
        delete scrollbar;
        delete scrollbarTrack;
        for (list<ofxUIWidget*>::iterator it = listItems.begin(); it != listItems.end(); it++) {
            delete (*it);
        }
    }
    
    ofxUIWidget* addWidgetToList(ofxUIWidget * widget, bool reflow = true) {
        // If you know you are going to be calling addWidgetToList many times in a row, you might opt to pass in reflow = false and then call reflowWidgets() at the end for efficiency
        listItems.push_back(widget);
        if (widget->hasLabel() &&
            widget->getKind() != OFX_UI_WIDGET_TEXTAREA) {
            setLabelFont(((ofxUIWidgetWithLabel*)widget)->getLabelWidget());
        }
        widget->setParent(this);
		widget->setRectParent(this->rect);
        if (reflow) reflowWidgets();
    }
    
    void removeWidgetFromList(list<ofxUIWidget*>::iterator it, bool reflow = true) {
        // If you know you are going to be calling removeWidgetFromList many times in a row, you might opt to pass in reflow = false and then call reflowWidgets() at the end for efficiency
        ofxUIWidget * w = *it;
        listItems.erase(it);
        delete w;
        if (reflow) reflowWidgets();
    }
    
    list<ofxUIWidget*> * getWidgetList() {
        return &listItems;
    }
    
    void reflowWidgets() {
        float y = listPadding;
        for (list<ofxUIWidget*>::iterator it = listItems.begin(); it != listItems.end(); it++) {
            ofxUIWidget *w = (*it);
            w->getRect()->y = y;
            float h = MAX(w->getRect()->height, w->getPaddingRect()->height);
            y += h + listPadding;
        }
        setContentHeight(y); // TODO why this magic number??
    }
    
    void setContentHeight(float _contentHeight) {
        contentHeight = _contentHeight;
        if (_contentHeight <= init_h) {
            scrollbar_h = init_h;
        } else {
            scrollbar_h = CLAMP(init_h * init_h / contentHeight, OFX_UI_MIN_SCROLLBAR_H, contentHeight);
        }
        scrollbar->setHeight(scrollbar_h);
        scrollTop = init_y + scrollbar->height/2;
        scrollBottom = init_y + init_h - scrollbar->height/2;
    }
    
    float getContentHeight() {
        return contentHeight;
    }
    
    // returns the bounds of the area that is not obscured behind the scroll bar
    ofRectangle getAvailableSpace()  {
        return ofRectangle(rect->x, rect->y, rect->width - scrollbar_w, rect->height);
    }
    
    void draw() {
        
        ofxUIPushStyle();
        
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        ofxUISetRectMode(OFX_UI_RECTMODE_CORNER);
        ofSetLineWidth(1.0);
        
        drawPadded();
        
        drawPaddedOutline();
        
        drawBack();
        
        drawFill();
        
        drawFillHighlight();
        
        drawOutline();
        
        drawOutlineHighlight();
        
        for(list<ofxUIWidget*>::iterator it = listItems.begin(); it != listItems.end(); ++it)
        {
            if((*it)->isVisible() && (*it)->getRect()->rInside(*sRect))
            {
                ofxUIWidget * w = (*it);
                w->draw();
                if (w->hasLabel() &&
                    w->getKind() != OFX_UI_WIDGET_TEXTAREA) {
                    ((ofxUIWidgetWithLabel*)w)->getLabelWidget()->draw();
                }
                //TODO handle embedded widgets
            }
		}
        scrollbar->draw();
        
        ofxUIPopStyle();
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

    // dragging on the scrollbar will scroll that way
    // dragging on the canvas will scroll according to ofxUICanvas scrolling,
    // and then update the scrollbar's position accordingly
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

    // This method assumes that the sort function you give it will work for every pair comparison of all widgets in listItems.
    void sortWidgets(bool (*f)(const ofxUIWidget *, const ofxUIWidget *)) {
        // std::sort(listItems.begin(), listItems.end(), f);
        listItems.sort(f);
        reflowWidgets();
    }
    
    
protected:
    //TODO I think I can use sRect dimensions instead of init_*
    float init_x, init_y, init_w, init_h;
    float listPadding;
    float scrollbar_w, scrollbar_h;
    float contentHeight;
    float scrollTop, scrollBottom;
    ofxUIDraggableRect * scrollbar;
    ofRectangle * scrollbarTrack;
    std::list<ofxUIWidget*> listItems;
};




#endif
