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

#ifndef OFXUI_TEXT_INPUT
#define OFXUI_TEXT_INPUT

#include "ofxUIWidgetWithLabel.h"
#include "ofxUITextArea.h"

class ofxUITextInput : public ofxUIWidget
{
public:
    ofxUITextInput(string _name, string _textstring, float w, float h = 0, float x = 0, float y = 0, int _size = OFX_UI_FONT_SMALL) : ofxUIWidget()
    {
        init(_name, _textstring, w, h, x, y, _size);
    }
    
    // DON'T USE THE NEXT CONSTRUCTORS
    // This is maintained for backward compatibility and will be removed on future releases
    
    ofxUITextInput(float x, float y, float w, string _name, string _textstring, int _size, float h = 0) : ofxUIWidget() 
    {
        init(_name, _textstring, w, h, x, y, _size);
//        ofLogWarning("OFXUITEXTINPUT: DON'T USE THIS CONSTRUCTOR. THIS WILL BE REMOVED ON FUTURE RELEASES.");
    }
    
    ofxUITextInput(float w, string _name, string _textstring, int _size, float h = 0) : ofxUIWidget()
    {
        init(_name, _textstring, w, h, 0, 0, _size);
//        ofLogWarning("OFXUITEXTINPUT: DON'T USE THIS CONSTRUCTOR. THIS WILL BE REMOVED ON FUTURE RELEASES.");
    }
    
    void init(string _name, string _textstring, float w, float h = 0, float x = 0, float y = 0, int _size = OFX_UI_FONT_SMALL)
    {
        // TODO why won't ofxUI things show up in their own canvas even when I make the paddedRect the same dimensions as the ofxUICanvas -- where is this extra margin_guess coming from?
        float margin_guess = 2;
        paddedRect = new ofxUIRectangle(x + margin_guess, y + margin_guess, w - 2.0 * margin_guess, h - 2.0 * margin_guess);
        rect = new ofxUIRectangle(x + padding + margin_guess, y + padding + margin_guess, w - 2.0 * padding - 2.0 * margin_guess, h - 2.0 * padding - 2.0 * margin_guess);
        paddedRect->setParent(rect);

		name = string(_name);  		
		kind = OFX_UI_WIDGET_TEXTINPUT;

		defaultstring = _textstring;;
        textArea = new ofxUITextArea(_name, _textstring, w, h, x, y, _size);
        
		clicked = false;
        autoclear = true;
        triggerOnClick = true;
        
		triggerType = OFX_UI_TEXTINPUT_ON_FOCUS;

		cursorWidth = textArea->getLabelWidget()->getStringWidth(".");
        cursorOffset = 2.0;
		theta = 0;

        cursorChar = 0;
    }
    
    virtual void setDrawPadding(bool _draw_padded_rect)
	{
		draw_padded_rect = _draw_padded_rect; 
        textArea->setDrawPadding(false);
	}
    
    virtual void setDrawPaddingOutline(bool _draw_padded_rect_outline)
	{
		draw_padded_rect_outline = _draw_padded_rect_outline; 
        textArea->setDrawPaddingOutline(false);
	}  
    
    void boundCursorChar() {
        cursorChar = MIN(cursorChar, textArea->textstring.size());
        cursorChar = MAX(0, cursorChar);
    }
    
    virtual void setCursorChar(int charIndex, int lineIndex) {
        int i = 0;
        int currentLineIndex = 0;
        while (currentLineIndex < lineIndex) {
            i += textArea->textLines[currentLineIndex++].size();
        }
        i += charIndex;
        cursorChar = i;
    }
    
    virtual ofVec2f getCharLineIndices() {
        boundCursorChar();
        int charIndex = 0;
        int lineIndex = 0;
        int i = 0;
        while (i < cursorChar) {
            i++;
            if (charIndex <= textArea->textLines[lineIndex].size()) {
                charIndex++;
            } else {
                lineIndex++;
                charIndex = 0;
            }
        }
        ofVec2f res(charIndex, lineIndex);
        return res;
    }

    virtual void drawCursor() {
        
        ofVec2f char_and_line = getCharLineIndices();
        float char_index = char_and_line[0];
        float line_index = char_and_line[1];
        
        string beforeCursor = "";
        if (textArea->textLines.size() > 0) {
            beforeCursor = textArea->textLines[line_index].substr(0, char_index);
        }
        
        float x = textArea->getRect()->getX() + textArea->getLabelWidget()->getStringWidth(beforeCursor);
        float y = textArea->getLineTopY(line_index);
        
        // cursor color oscillates
        ofxUIFill();
        ofxUISetColor(textArea->getLabelWidget()->getColorFillHighlight(), 255.0*fabs(cos(theta)));
        theta +=0.05;
        ofxUIDrawRect(x, y, cursorWidth, textArea->lineHeight);
    }
    
    virtual void drawFill() 
    {
        if(draw_fill)
        {
            ofxUIFill(); 
            ofxUISetColor(color_fill); 
            rect->draw(); 
        }
        textArea->drawFill();
        if(clicked)
		{
            ofNoFill();
            ofxUISetColor(color_outline_highlight);
            rect->draw();
			drawCursor();
		}
		if(textArea->textstring == "" && !clicked)
		{
			ofxUIFill(); 
            ofxUISetColor(color_fill);
            textArea->getLabelWidget()->drawString(textArea->getRect()->getX(), textArea->getLineBottomY(0), defaultstring);
		}        
    }
	
    void mouseMoved(int x, int y ) 
    {
        if(rect->inside(x, y))
        {
            state = OFX_UI_STATE_OVER;         			
        }    
        else
        {
            state = OFX_UI_STATE_NORMAL;
        }
        stateChange();         
    }
    
    void mouseDragged(int x, int y, int button) 
    {
        if(hit)
        {
            state = OFX_UI_STATE_DOWN;         
        }    
        else
        {
            state = OFX_UI_STATE_NORMAL;  
        }
        stateChange();     
    }
    
    void mousePressed(int x, int y, int button) 
    {
        if(rect->inside(x, y))
        {
			if(state == OFX_UI_STATE_OVER)
			{
				clicked = true; 
				theta = 0; 
                hit = true; 
			}
#ifdef TARGET_OPENGLES
			clicked = true;
			theta = 0;
			hit = true;
#endif

            cursorChar = textArea->textstring.size();
            
            state = OFX_UI_STATE_DOWN;     
			triggerType = OFX_UI_TEXTINPUT_ON_FOCUS;
            
            if(triggerOnClick)
            {
                triggerEvent(this);
            }
        }
        else
        {
            state = OFX_UI_STATE_NORMAL;
            if(clicked)
            {
                unClick();  
            }
        }
        stateChange();         
    }
    
    void mouseReleased(int x, int y, int button) 
    {
        if(hit)
        {
#ifdef TARGET_OPENGLES
            state = OFX_UI_STATE_NORMAL;        
#else            
            state = OFX_UI_STATE_OVER; 
#endif 
//			triggerType = OFX_UI_TEXTINPUT_ON_UNFOCUS; 
//			triggerEvent(this); 
        }    
        else
        {
            state = OFX_UI_STATE_NORMAL;         
        }
        hit = false; 
        stateChange();         
    }
	
    void keyPressed(int key) 
    {
		if(clicked)            
		{
            switch (key) 
			{
				case OF_KEY_BACKSPACE:
					if (textArea->textstring.size() > 0 && cursorChar > 0) {
                        cursorChar--;
                        textArea->textstring.erase(cursorChar, 1);
                        textArea->formatDisplayString();
					}
					break;

                case OF_KEY_DEL:
					if (textArea->textstring.size() > 0 && cursorChar < textArea->textstring.length()) {
                        textArea->textstring.erase(cursorChar, 1);
                        textArea->formatDisplayString();
                    }
					break;
					
				case OF_KEY_RETURN:
                    triggerType = OFX_UI_TEXTINPUT_ON_ENTER;
					triggerEvent(this);
					if(autoclear) {
						textArea->textstring.clear();
                        textArea->formatDisplayString();
					}
                    clicked = false;
					break;
					
				case OF_KEY_RIGHT:
                    if(cursorChar < textArea->textstring.length()) {
                        cursorChar++;
                        textArea->formatDisplayString();
                    }
					break;					

                case OF_KEY_LEFT:
                    if(cursorChar > 0) {
                        cursorChar--;
                        textArea->formatDisplayString();
                    }
					break;
                    
                case OF_KEY_DOWN:
                {
                    ofVec2f char_and_line_indices = getCharLineIndices();
                    float charIndex = char_and_line_indices[0];
                    float lineIndex = char_and_line_indices[1];
                    if (lineIndex < textArea->textLines.size()) {
                        setCursorChar(charIndex, ++lineIndex); // TODO this will shift the cursor around horizontally depending on character widths - fix this
                    }
                }
                    break;
                    
                case OF_KEY_UP:
                {
                    ofVec2f char_and_line_indices = getCharLineIndices();
                    float charIndex = char_and_line_indices[0];
                    float lineIndex = char_and_line_indices[1];
                    if (lineIndex > 0) {
                        setCursorChar(charIndex, --lineIndex); // TODO same char shifting problem as in OF_KEY_DOWN case
                    }
                }
                    break;

#if (OF_VERSION_MINOR > 7)
                case OF_KEY_TAB:
                case OF_KEY_COMMAND:
                case OF_KEY_CONTROL:
                case OF_KEY_LEFT_SHIFT:
                case OF_KEY_RIGHT_SHIFT:
                case OF_KEY_LEFT_CONTROL:
                case OF_KEY_RIGHT_CONTROL:
                case OF_KEY_LEFT_ALT:
                case OF_KEY_RIGHT_ALT:
                case OF_KEY_LEFT_SUPER:
                case OF_KEY_RIGHT_SUPER:					
#endif
                case OF_KEY_F1:
                case OF_KEY_F2:
                case OF_KEY_F3:
                case OF_KEY_F4:
                case OF_KEY_F5:
                case OF_KEY_F6:
                case OF_KEY_F7:
                case OF_KEY_F8:
                case OF_KEY_F9:
                case OF_KEY_F10:
                case OF_KEY_F11:
                case OF_KEY_F12:
                case OF_KEY_PAGE_UP:
                case OF_KEY_PAGE_DOWN:
                case OF_KEY_HOME:
                case OF_KEY_END:
                case OF_KEY_INSERT:
                case OF_KEY_ALT:
                case OF_KEY_SHIFT:
                break;
                    
				default:
                {
                    textArea->textstring.insert(cursorChar, 1, key);
                    cursorChar++;
                    textArea->formatDisplayString();
                }
					break;
			}
		}
    }
        
    void unClick()
    {
        if(clicked)
        {
            clicked = false;          
			triggerType = OFX_UI_TEXTINPUT_ON_UNFOCUS; 
			triggerEvent(this);             
        }
    }
    void stateChange()
    {        
        switch (state) {
            case OFX_UI_STATE_NORMAL:
            {            
                draw_fill_highlight = false;             
                draw_outline_highlight = false;  
				textArea->getLabelWidget()->unfocus();
            }
                break;
            case OFX_UI_STATE_OVER:
            {
                draw_fill_highlight = false;            
                draw_outline_highlight = true;  
				textArea->getLabelWidget()->focus();
            }
                break;
            case OFX_UI_STATE_DOWN:
            {
                draw_fill_highlight = false;            
                draw_outline_highlight = true;             
				textArea->getLabelWidget()->focus();
            }
                break;
            case OFX_UI_STATE_SUSTAINED:
            {
                draw_fill_highlight = false;            
                draw_outline_highlight = false;                         
				textArea->getLabelWidget()->unfocus();
            }
                break;                            
            default:
                break;
        }        
    }
	
    void setVisible(bool _visible)
    {
        visible = _visible; 
        textArea->setVisible(visible);
    }
    
	bool isClicked()
	{
		return clicked;
	}
	
	ofxUITextArea *getTextArea()
	{
		return textArea;
	}
	
	string getTextString()
	{
		return textArea->textstring;
	}
	
    void setTriggerType(int _triggerType)
    {
        triggerType = _triggerType; 
    }
	
    int getTriggerType()
	{
		return triggerType; 
	}
	
	void setTextString(string s)	
	{
		textArea->setTextString(s);
        cursorChar = 0;
	}
	
	void setParent(ofxUIWidget *_parent)
	{
		parent = _parent;     
	}	
	
	void setAutoClear(bool _autoclear)
	{
		autoclear = _autoclear; 
	}
    
    void setTriggerOnClick(bool _triggerOnClick)
    {
        triggerOnClick = _triggerOnClick;
    }
    
protected:
    // text
    ofxUITextArea * textArea;
	string defaultstring; 

    // state and events
	bool clicked, autoclear, triggerOnClick;
    int triggerType;
    
    // drawing the cursor
	float theta, cursorWidth, cursorOffset;
    int cursorChar; // the char index in textArea->textstring immediately after
                    // which to draw the cursor

}; 

#endif
