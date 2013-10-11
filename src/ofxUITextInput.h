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

// TODO make it work even when you don't give it the x and y initially

// TODO better way to do this; I think this is mac-only
#define OFXUI_KEY_BACK_QUOTE 161

#include "ofxUIWidgetWithLabel.h"
#include "ofxUITextArea.h"

class ofxUITextInput : public ofxUIWidget
{
public:
    
    /* CONSTRUCTORS ***********************************************************/
    
    ofxUITextInput(string _name, string _textstring, float w, float h = 0, float x = 0, float y = 0, int _size = OFX_UI_FONT_MEDIUM_SIZE) : ofxUIWidget()
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
        textArea = new ofxUITextArea(_name, _textstring, rect->width, rect->height, rect->x, rect->y, _size);
        textArea->setParent(this);
        
		clicked = false;
        autoclear = true;
        triggerOnClick = true;
        
		triggerType = OFX_UI_TEXTINPUT_ON_FOCUS;

		cursorWidth = textArea->getLabelWidget()->getStringWidth(".");
        cursorOffset = 2.0;
		theta = 0;

        cursorChar = 0;
        cursorLine = 0;
        
        shiftKeyPressed = false;
        
        // TODO fix things so I don't need this hack
        xCorrection = 0;
        yCorrection = 0;
    }
    
    /* CURSOR ARITHMETIC ******************************************************/
    
    void moveCursorBackward() {
        if(cursorChar > 0) {
            cursorChar--;
        } else if (cursorLine > 0) {
            cursorLine--;
            cursorChar = textArea->textLines[cursorLine].size() - 1;
        }
        cursorLine = CLAMP(cursorLine, 0, textArea->textLines.size() - 1);
        cursorChar = CLAMP(cursorChar, 0, textArea->textLines[cursorLine].size());
    }

    void moveCursorForward() {
        if(textArea->textstring == "" ||
           cursorChar < textArea->textLines[cursorLine].size() - 1) {
            cursorChar++;
        } else if (cursorLine < textArea->textLines.size() &&
                   cursorLine < textArea->textLines.size() - 1) {
            cursorLine++;
            cursorChar = 0;
        }
        cursorLine = CLAMP(cursorLine, 0, textArea->textLines.size() - 1);
        cursorChar = CLAMP(cursorChar, 0, textArea->textLines[cursorLine].size());
    }
    
    int getStringIndex() {
        // returns the index in textArea->textstring corresponding with the
        // cursor position specified by cursorChar and cursorLine
        // cursorChar x is between textArea->textstring[x-1] and textArea->textstring[x]
        // cursorChar = 0 is before textArea->textstring[0]
        int i = 0;
        int currentLineIndex = 0;
        while (currentLineIndex < cursorLine) {
            i += textArea->textLines[currentLineIndex++].size();
        }
        i += cursorChar;
        i = CLAMP(i, 0, textArea->textstring.size());
        return i;
    }
    
    int setCursorPosition(int stringIndex) {
        // sets cursorChar and cursorLine to correspond with the stringIndex
        if (textArea->textLines.size() == 0) {
            cursorLine = 0;
            cursorChar = 0;
            return;
        }
        
        int count = 0; // count up to stringIndex
        int i = 0;
        
        for (int i = 0; i < textArea->textLines.size(); i++) {
            if (count + textArea->textLines[i].size() >= stringIndex) {
                cursorLine = i;
                cursorChar = stringIndex - count;
                break;
            } else {
                count += textArea->textLines[i].size();
            }
        }
        cursorLine = CLAMP(cursorLine, 0, textArea->textLines.size() - 1);
        cursorChar = CLAMP(cursorChar, 0, textArea->textLines[cursorLine].size());
    }
    
    void clearText() {
        textArea->textstring.clear();
        textArea->formatDisplayString();
        cursorChar = 0;
        cursorLine = 0;
    }
    
    /* DRAWING ****************************************************************/
    
    virtual void drawCursor() {
        ofxUILabel * label = textArea->getLabelWidget();

        string beforeCursor = "";
        if (textArea->textLines.size() > 0) {
            if (textArea->password) {
                beforeCursor = textArea->passwordString(cursorChar);
            } else {
                beforeCursor = textArea->textLines[cursorLine].substr(0, cursorChar);
            }
        }

        // we need to put "." on both ends of the string so that the width of
        // white space at the beginning or end of the string is included
        float xOffset = label->getStringWidth("." + beforeCursor + ".") - label->getStringWidth("..");
        
        float x = textArea->getRect()->getX() + xOffset;
        float y = textArea->getLineTopY(cursorLine);
        
        // cursor color oscillates
        ofxUIFill();
        ofxUISetColor(textArea->getLabelWidget()->getColorFillHighlight(), 255.0*fabs(cos(theta)));
        theta +=0.05;
        ofxUIDrawRect(x + xCorrection, y + yCorrection, cursorWidth, textArea->lineHeight);
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
            textArea->getLabelWidget()->drawString(rect->x + textArea->xCorrection + 2.0, yCorrection + textArea->getLineBottomY(0), defaultstring);
		}
    }

    /* EVENT CALLBACKS ********************************************************/
    
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
            focus();
        else
            unfocus();
    }
    
    void focus() {
        clicked = true;
        theta = 0;
        hit = true;
        
        setCursorPosition(textArea->textstring.size());
        
        state = OFX_UI_STATE_DOWN;
        triggerType = OFX_UI_TEXTINPUT_ON_FOCUS;
        
        if(triggerOnClick) triggerEvent(this);
        
        stateChange();
    }
    
    void unfocus() {
        state = OFX_UI_STATE_NORMAL;
        if(clicked) unClick();
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
   
    void keyPressed(int key) {

		if(clicked)
		{
            switch (key)
			{
				case OF_KEY_BACKSPACE:
                {
                    if (textArea->textstring.size() > 0) {
                        int i = getStringIndex();
                        if (i > 0) {
                            textArea->textstring.erase(i - 1, 1);
                            textArea->formatDisplayString();
                            setCursorPosition(i - 1);
                        }
                    }
                }
					break;

                case OF_KEY_DEL:
                {
					if (textArea->textstring.size() > 0) {
                        int i = getStringIndex();
                        if (i < textArea->textstring.size()) {
                            textArea->textstring.erase(i, 1);
                            textArea->formatDisplayString();
                            setCursorPosition(i);
                        }
                    }
                }
					break;
					
				case OF_KEY_RETURN:
                    triggerType = OFX_UI_TEXTINPUT_ON_ENTER;
					triggerEvent(this);
					if(autoclear) {
                        string input = textArea->textstring.substr();
                        ofNotifyEvent(inputSubmitted, input, this);
						clearText();
                        clicked = true;
					}
					break;
					
				case OF_KEY_RIGHT:
                    moveCursorForward();
					break;					

                case OF_KEY_LEFT:
                    moveCursorBackward();
					break;
                    
                case OF_KEY_DOWN:
                    if (cursorLine < textArea->textLines.size() - 1) {
                        cursorLine++;
                        cursorChar = CLAMP(cursorChar, 0, textArea->textLines[cursorLine].size());
                    }
                    break;
                    
                case OF_KEY_UP:
                    if (cursorLine > 0) {
                        cursorLine--;
                        cursorChar = CLAMP(cursorChar, 0, textArea->textLines[cursorLine].size());
                    }
                    break;

#if (OF_VERSION_MINOR > 7)
                case OF_KEY_TAB:
                case OF_KEY_COMMAND:
                case OF_KEY_CONTROL:
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
                break;
                  
                case OF_KEY_LEFT_SHIFT:
                case OF_KEY_RIGHT_SHIFT:
                case OF_KEY_SHIFT:
                    shiftKeyPressed = true;
                    break;
                    
				default:
                {
                    char c = key;
                    if (key == OFXUI_KEY_BACK_QUOTE) { //TODO better way to do this
                        c = '`';
                    }
                    if (shiftKeyPressed) {
                        if (key == OFXUI_KEY_BACK_QUOTE) c = '~';
                        if (key == '1') c = '!';
                        if (key == '2') c = '@';
                        if (key == '3') c = '#';
                        if (key == '4') c = '$';
                        if (key == '5') c = '%';
                        if (key == '6') c = '^';
                        if (key == '7') c = '&';
                        if (key == '8') c = '*';
                        if (key == '9') c = '(';
                        if (key == '0') c = ')';
                        if (key == '-') c = '_';
                        if (key == '=') c = '+';
                        if (key == '[') c = '{';
                        if (key == ']') c = '}';
                        if (key == '\\') c = '|';
                        if (key == ';') c = ':';
                        if (key == '\'') c = '"';
                        if (key == ',') c = '<';
                        if (key == '.') c = '>';
                        if (key == '/') c = '?';
                    }
                    int i = getStringIndex();
                    textArea->textstring.insert(i, 1, c);
                    textArea->formatDisplayString();
                    setCursorPosition(i + 1);
                }
					break;
			}
		}
    }
    
    void keyReleased(int key) {

        if (clicked) {
            switch (key) {
                case OF_KEY_LEFT_SHIFT:
                case OF_KEY_RIGHT_SHIFT:
                case OF_KEY_SHIFT:
                    shiftKeyPressed = false;
                    break;
                default:
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
	
    /* GETTERS & SETTERS ******************************************************/
    
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
    
    bool isPassword() {
        return textArea->password;
    }
    
    void isPassword(bool val) {
        textArea->password = val;
    }
    
    ofEvent<string> inputSubmitted;
    
    float xCorrection, yCorrection;
    
protected:
    // text
    ofxUITextArea * textArea;
	string defaultstring; 

    // state and events
	bool clicked, autoclear, triggerOnClick;
    int triggerType;
    
    // drawing the cursor
	float theta, cursorWidth, cursorOffset;
    int cursorChar, cursorLine;
    
    // whether the shift key is pressed
    bool shiftKeyPressed;

}; 

#endif
