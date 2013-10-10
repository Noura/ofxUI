/**********************************************************************************
 
 Copyright (C) 2012 Syed Reza Ali (www.syedrezaali.com)
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 **********************************************************************************/

#ifndef OFXUI_TEXT_AREA
#define OFXUI_TEXT_AREA

#include "ofxUIWidgetWithLabel.h"

class ofxUITextArea : public ofxUIWidgetWithLabel
{
public:
    ofxUITextArea(string _name, string _textstring, float w, float h = 0, float x = 0, float y = 0, int _size = OFX_UI_FONT_MEDIUM_SIZE) : ofxUIWidgetWithLabel()
    {
        init(_name, _textstring, w, h, x, y, _size);
    }
    
    void init(string _name, string _textstring, float w, float h, float x, float y, int _size)
    {
        paddedRect = new ofxUIRectangle(x, y, w, h);
        rect = new ofxUIRectangle(x + padding, y + padding, w - 2.0*padding, h - 2.0*padding);
        paddedRect->setParent(rect);
		name = string(_name); 
		kind = OFX_UI_WIDGET_TEXTAREA;
        setDrawFill(true);
        setDrawBack(false);
        drawShadow = false; 

		label = new ofxUILabel(x+padding,y+padding, name, _size);
		label->setParent(label);
		label->setRectParent(rect);
        label->setEmbedded(true);
        
        autoSize = (h == 0);
        
        lineHeight = label->getLineHeight() * 1.5;
        lineSpaceSize = lineHeight * 0.5;
        
        setTextString(_textstring);
        
        // TODO fix things so I don't need this hack
        xCorrection = 0;
        yCorrection = 0;
    }
    
    virtual void drawBack()
    {
        if(draw_back)
        {
            ofFill();
            ofSetColor(color_back);
            rect->draw();
        }
    }
    
    virtual void drawFill()
    {
        if(draw_fill)
        {
            if(drawShadow)
            {
                ofSetColor(color_back);
                for(unsigned int i = 0; i < textLines.size(); i++)
                {
                    label->drawStringShadow(xCorrection + rect->getX(), yCorrection + getLineBottomY(i), textLines[i]);
                }
            }
            
            ofSetColor(color_fill);
            for(unsigned int i = 0; i < textLines.size(); i++)
            {
                label->drawString(xCorrection + rect->getX(), yCorrection + getLineBottomY(i), textLines[i]);
            }
        }
    }
    
    float getLineBottomY(int line_index) {
        return rect->getY() + (line_index + 1) * (lineHeight + lineSpaceSize) - lineSpaceSize;
    }
    float getLineTopY(int line_index) {
        return getLineBottomY(line_index) - lineHeight;
    }
    
    void draw() {
        
        ofxUIWidget::draw();
    }
    
    void setVisible(bool _visible)
    {
        visible = _visible;
        label->setVisible(false);
    }
    
    string getTextString()
	{
		return textstring;
	}
	
	
	void setTextString(string s)
	{
        textLines.clear();        
        textstring = s;
        formatDisplayString();
	}
    
    void formatDisplayString()
    {        
        textLines.clear();
        
        float widthLimit = rect->getWidth() - label->getStringWidth("M");
        if (widthLimit < label->getStringWidth("M")) {
            return;
        }
        float heightLimit = rect->getHeight();
        
        int numLinesLimit;
        if (autoSize) {
            numLinesLimit = 0;
        } else {
            numLinesLimit = (int)heightLimit / (lineHeight + lineSpaceSize);
        }
        
        
        string line = "";
        for (int i = 0; i < textstring.size(); i++) {
            
            if (!autoSize && textLines.size() > numLinesLimit) {
                break;
            }
            if (label->getStringWidth(line) >= widthLimit) {
                if (line.size() == 0) {
                    break;
                }
                // try to break line at white space
                int whitespace_index = line.size() - 1;
                while (whitespace_index > 0 &&
                       !isspace(line.at(whitespace_index))) {
                    whitespace_index--;
                }
                if (whitespace_index <= 0) {
                    // white space not found, or found at first char
                    textLines.push_back(line.substr(0, line.size()-1));
                    line = line.substr(line.size()-1);
                } else {
                    // white space found
                    textLines.push_back(line.substr(0, whitespace_index));
                    line = line.substr(whitespace_index);
                }
            }
            char c = textstring.at(i);
            line += c;
        }
        if (line != "" && !(!autoSize && textLines.size() > numLinesLimit)) {
            textLines.push_back(line);
        }
        
        if (autoSize) {
            setHeightForContent();
        }
    }

    void setHeightForContent() {
        rect->setHeight(textLines.size() * (lineHeight + lineSpaceSize));
        paddedRect->setHeight(rect->getHeight() + 2.0*padding);
    }
    
	void setParent(ofxUIWidget *_parent)
	{
		parent = _parent;
        formatDisplayString();
	}
    
    void setDrawShadow(bool _drawShadow)
    {
        drawShadow = _drawShadow;
    }
	
	string textstring;
    vector<string> textLines;
    bool autoSize;
    bool drawShadow; 
    int lineSpaceSize;
    int lineHeight; 
    int offsetY;
    
    float xCorrection, yCorrection;
};

#endif
