//
//  ofxUIMultiLineTextInput.h
//
//  Created by Noura Howell on 9/24/13.
//
//

#ifndef OFXUI_MULTI_LINE_TEXT_INPUT
#define OFXUI_MULTI_LINE_TEXT_INPUT

class ofxUIMultiLineTextInput : public ofxUITextArea {
public:
    ofxUIMultiLineTextInput(string _name, string _textstring, float x, float y, float w, float h, int _size = OFX_UI_FONT_SMALL)
    : ofxUITextArea(_name, _textstring, w, h, x, y, _size)
    , cursorWidth(1.0)
    , cursorChar(0)
    , cursorLine(0)
    , topLine(0) {
        kind = OFX_UI_WIDGET_MULTILINETEXTINPUT;
        ofAddListener(ofEvents().keyPressed, this, &ofxUIMultiLineTextInput::onKeyPressed);
    }
    
    ~ofxUIMultiLineTextInput() {
        ofRemoveListener(ofEvents().keyPressed, this, &ofxUIMultiLineTextInput::onKeyPressed);
    }
    
    void onKeyPressed(ofKeyEventArgs &key) {
        
        bool stop_here = true;
        cout << "KEY PRESSED " << key << endl;
        cout << "KEY PRESSED .KEY " << key.key << endl;
        textstring += (char)(key.key);
    }

    float cursorWidth;
    int cursorChar, cursorLine, topLine;
    
};

#endif
