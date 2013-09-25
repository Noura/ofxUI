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
    ofxUITextInput(string _name, string _textstring, float w, float h = 0, float x = 0, float y = 0, int _size = OFX_UI_FONT_SMALL) : ofxUIWidgetWithLabel()
    {
        init(_name, _textstring, w, h, x, y, _size);
    }
    
    // DON'T USE THE NEXT CONSTRUCTORS
    // This is maintained for backward compatibility and will be removed on future releases
    
    ofxUITextInput(float x, float y, float w, string _name, string _textstring, int _size, float h = 0) : ofxUIWidgetWithLabel() 
    {
        init(_name, _textstring, w, h, x, y, _size);
//        ofLogWarning("OFXUITEXTINPUT: DON'T USE THIS CONSTRUCTOR. THIS WILL BE REMOVED ON FUTURE RELEASES.");
    }
    
    ofxUITextInput(float w, string _name, string _textstring, int _size, float h = 0) : ofxUIWidgetWithLabel()
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

		cursorWidth = 1.0;
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
    
    virtual void setCursorChar(charIndex, lineIndex) {
        
    }
    
    virtual void getCharLineIndices() {
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
        
        string beforeCursor = textArea->textLines[line_index].substr(0, char_index);
        
        float x = textArea->rect->getX() + textArea->getLabelWidget()->getStringWidth(beforeCursor);
        float y = textArea->getLineTopY(line_index);
        
        // cursor color oscillates
        ofxUIFill();
        ofxUISetColor(label->getColorFillHighlight(), 255.0*fabs(cos(theta)));
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
            textArea->getLabelWidget()->drawString(textArea->rect->getX(), textArea->getLineBottomY(0), defaultstring);
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
					if (textstring.size() > 0 && cursorChar > 0) {
                        cursorChar--;
                        textArea->textstring.erase(cursorPosition, 1);
                        textArea->formatDisplayString();
					}
					break;

                case OF_KEY_DEL:
					if (textstring.size() > 0 && cursorChar < textstring.length()) {
                        textArea->textstring.erase(cursorPosition, 1);
                        textArea->formatDisplayString();
                    }
					break;
					
				case OF_KEY_RETURN:
                    triggerType = OFX_UI_TEXTINPUT_ON_ENTER;
					triggerEvent(this);
					if(autoclear)
					{
						textArea->textstring.clear();
                        textArea->formatDisplayString();
					}
                    clicked = false;
					break;
					
				case OF_KEY_RIGHT:
                    if(cursorChar < textArea->textstring.length())
                    {
                        cursorChar++;
                        textArea->formatDisplayString();
                    }
					break;					
                    :
                case OF_KEY_LEFT:
                    if(cursorChar > 0)
                    {
                        cursorChar--;
                        textArea->formatDisplayString();
                    }
					break;
                    
                case OF_KEY_DOWN:

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
                    textstring.insert(cursorPosition, 1, key);
                    cursorPosition++;
                    recalculateDisplayString();
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
				label->unfocus(); 								
            }
                break;
            case OFX_UI_STATE_OVER:
            {
                draw_fill_highlight = false;            
                draw_outline_highlight = true;  
				label->focus(); 								
            }
                break;
            case OFX_UI_STATE_DOWN:
            {
                draw_fill_highlight = false;            
                draw_outline_highlight = true;             
				label->focus(); 					
            }
                break;
            case OFX_UI_STATE_SUSTAINED:
            {
                draw_fill_highlight = false;            
                draw_outline_highlight = false;                         
				label->unfocus(); 								
            }
                break;                            
            default:
                break;
        }        
    }
	
    void setVisible(bool _visible)
    {
        visible = _visible; 
        label->setVisible(visible); 
    }
    
	bool isClicked()
	{
		return clicked;
	}
	
	ofxUILabel *getLabel()
	{
		return label; 
	}
	
	string getTextString()
	{
		return textstring; 
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
		textstring = ""; 
		string temp = ""; 
		
        int length = s.length(); 
        
        if(length > 0)
        {
            for(int i = 0; i < length; i++)
            {
                temp+=s.at(i); 
                float newWidth = label->getStringWidth(temp); 
                
                if(newWidth < rect->width-padding*4.0)
                {
                    textstring+=s.at(i); 
                    label->setLabel(textstring); 
                }				
            }		
        }
        else
        {
            textstring = s; 
            label->setLabel(textstring);                
        }
        displaystring = textstring; 
	}
	
	void setParent(ofxUIWidget *_parent)
	{
		parent = _parent; 
        if(rect->height == 0 || rect->height < label->getPaddingRect()->height+padding*2.0)
        {
            rect->height = label->getPaddingRect()->height+padding*2.0; 
        }
        label->setLabel(textstring);
		ofxUIRectangle *labelrect = label->getRect(); 
		float h = labelrect->getHeight(); 
		float ph = rect->getHeight(); 	
		
		labelrect->y = ph/2.0 - h/2.0; 
		defaultY = labelrect->y+labelrect->getHeight(); 
		defaultX = labelrect->x; 
 		
		paddedRect->height = rect->height+padding*2.0; 
		
		cursorWidth = label->getStringWidth("."); 
        
        while(label->getStringWidth(textstring) > rect->width-padding*4.0)
        {
            string::iterator it;
            it=textstring.begin();
            textstring.erase (it);                    
        }
        
        defaultstring = textstring; 
		displaystring = textstring; 
        setTextString(textstring);        
	}	
	
	void setAutoClear(bool _autoclear)
	{
		autoclear = _autoclear; 
	}
    
    void setTriggerOnClick(bool _triggerOnClick)
    {
        triggerOnClick = _triggerOnClick;
    }
    
    void recalculateDisplayString()
    {
        // the maximum width of the displaystring
        float maxWidth = rect->width-padding*4.0;
        
        displaystring = textstring;
        string stringBeforeCursor = displaystring.substr(0, cursorPosition);
        string stringBeforeLabel =  displaystring.substr(0, firstVisibleCharacterIndex);
        
        // if the cursoroffset - length of the (invisible) string before the label < 0, we have to shift our string to the left to get our cursor in the label
        while(label->getStringWidth(stringBeforeCursor) - label->getStringWidth(stringBeforeLabel) < 0){
            firstVisibleCharacterIndex --;
            stringBeforeLabel =  displaystring.substr(0, firstVisibleCharacterIndex);
        }

        // if the cursoroffset - length of the (invisible) string before the label is > maximum width, we have to shift to the right
        while(label->getStringWidth(stringBeforeCursor) - label->getStringWidth(stringBeforeLabel) > maxWidth){
            firstVisibleCharacterIndex ++;
            stringBeforeLabel =  displaystring.substr(0, firstVisibleCharacterIndex);
        }
        
        // we now know how long the string before the label should be, so trim it off
        displaystring = displaystring.substr(MIN(firstVisibleCharacterIndex, displaystring.length()));
        
        // trim off the end of the string until it fits
        while(label->getStringWidth(displaystring) > maxWidth && displaystring.length() > 0)
        {
            displaystring = displaystring.substr(0, displaystring.size()-1);
        }
        
        label->setLabel(displaystring);
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
