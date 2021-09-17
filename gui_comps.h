////////////////////////////////////
// GuiComps version 0.05          //
// Created by Jonathan Ravinovich //
////////////////////////////////////

///////////////////////////////////////////////////
// Update Log for 0.05:                          //
//                                               //
// Added Circle Scrollbar                        //
// Added TextEntry with basic functionality      //
// Added this update log                         //
// Did code refactoring                          //
// Added exceptions in some areas for edge cases //
///////////////////////////////////////////////////

// need to add namespace

#include <SFML/Graphics.hpp>
#include <deque>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>  // not needed other than debugging
#include <exception>
#include <math.h>

#define ACCEL_FAST 0
#define ACCEL_SLOW 1

#define CENTER 0
#define LEFT 1
#define RIGHT 2
#define UP 3
#define DOWN 4
#define NONE 5

#define HORIZONTAL 0
#define VERTICAL 1

#define RECT_SLIDER 0
#define CIRCLE_SLIDER 1


// Exceptions

// Value error - used when giving invalid value of a variable, like type or integer
class ValueError : public std::exception {
private:
    std::string msg;

public:
    ValueError(std::string message) {
        msg = message;
    }

    const char* what() const noexcept override {
        return msg.c_str();
    }
};

// Load error - used when failing to load assets like sprites or fonts
class LoadError : public std::exception {
private:
    std::string msg;

public:
    LoadError(std::string message) {
        msg = message;
    }

    const char* what() const noexcept override {
        return msg.c_str();
    }   
};


// custom data types
typedef struct Key {
    sf::Int16 code;
    bool alt;
    bool control;
    bool shift;
    bool system;
} Key;

// components

class Component {
protected:
    sf::Vector2f pos;
    sf::Vector2f size;
    sf::Color fillColor;
    sf::Color borderColor;
    std::deque<sf::Vector2f> moves;

public:
    Component(sf::Vector2f position, sf::Vector2f size){
        pos = position;
        this->size = size;
        fillColor = sf::Color::White;
        borderColor = sf::Color::White;
    }

    ~Component(){}
    
    // maybe could use a refrence here to make things faster
    void update(sf::Vector2i& mPos, sf::Vector3i& mBtns, Key& key) {
        if(moves.size() > 0) {
            pos = moves[0];
            moves.pop_front();
        }
    }

    virtual void draw(sf::RenderWindow& window) = 0;

    void move(sf::Vector2f pos, sf::Vector2f accelVec={2,2}, sf::Uint16 numOfSteps=10, sf::Uint8 type=ACCEL_FAST) {
        if(numOfSteps == 0)
            throw ValueError("The value of steps must be 1 or above");
        
        float newX = pos.x;
        float newY = pos.y;
        float tmpX = this->pos.x;
        float tmpY = this->pos.y;
        float nextX = 0;
        float nextY = 0;
        std::vector<sf::Vector2f> tmpMoves;

        // calculating the path of the object using this formula: accel*(end_pos - current_pos)/(num_of_steps - current_step)
        for(sf::Uint16 i=0; i<numOfSteps; i++) {
            nextX = accelVec.x*(newX - tmpX)/(numOfSteps - i);
            nextY = accelVec.y*(newX - tmpY)/(numOfSteps - i);
            tmpX += nextX;
            tmpY += nextY;
            tmpMoves.push_back({nextX, nextY});
        }

        tmpX = this->pos.x;
        tmpY = this->pos.y;

        if(type == ACCEL_SLOW) {
            std::reverse(tmpMoves.begin(), tmpMoves.end());
        }
        
        // retracing the path
        for(auto move : tmpMoves) {
            tmpX += move.x;
            tmpY += move.y;
            moves.push_back({tmpX, tmpY});
        }
    }

    void setPosition(sf::Vector2f position) {
        pos = position;
    }

    void setSize(sf::Vector2f size) {
        this->size = size;
    }

    void setFillColor(sf::Color color) {
        fillColor = color;
    }

    void setBorderColor(sf::Color color) {
        borderColor = color;
    }

    sf::Vector2f getPosition() {
        return pos;
    }

    sf::Vector2f getSize() {
        return size;
    }
};


class TextComponent : public Component {
protected:
    std::string text;
    sf::Font font;
    sf::Uint8 textSize;
    sf::Vector2f textPos;
    sf::Vector2f textDrawPos;  // used for the text anchor
    sf::Color textColor;
    bool loadedFont;

public:
    TextComponent(sf::Vector2f position, sf::Vector2f size) : Component(position, size){
        text = "";
        textSize = 10;
        textPos = {0,0};
        textDrawPos = textPos;
        textColor = sf::Color::Black;
        loadedFont = false;
    }

    void setText(std::string text) {
        this->text = text;
    }

    void loadFont(std::string path) {
        if(!font.loadFromFile(path))
            throw LoadError("Failed to load font from " + path);
        else
            loadedFont = true;
    }

    void setTextSize(sf::Uint8 size) {
        textSize = size;
    }

    void setTextColor(sf::Color color) {
        textColor = color;
    }

    void setTextPosition(sf::Vector2f position) {
        textPos = position;
    }

    void setTextAnchor(sf::Uint8 anchor) {
        // creating the text to calculate its size
        sf::Text t;
        t.setFont(font);
        t.setString(text);
        t.setCharacterSize(textSize);

        // getting the text dimentions
        float tWidth = t.getLocalBounds().width;
        float tHeight = t.getLocalBounds().height;
        float tTop = t.getLocalBounds().top;
        float tLeft = t.getLocalBounds().left;

        switch(anchor) {
            case CENTER:
                textDrawPos = {textPos.x + (size.x-tWidth)/2 - tLeft, textPos.y + (size.y-textSize)/2 - tTop};
                break;

            case LEFT:
                textDrawPos = {textPos.x - tLeft, textPos.y + (size.y-tHeight)/2 - tTop};
                break;

            case RIGHT:
                textDrawPos = {textPos.x + (size.x-tWidth-tLeft), textPos.y + (size.y-tHeight)/2 - tTop};
                break;

            case UP:
                textDrawPos = {textPos.x + (size.x-tWidth)/2 - tLeft, textPos.y - tTop};
                break;

            case DOWN:
                textDrawPos = {textPos.x + (size.x-tWidth)/2 - tLeft, textPos.y + (size.y-tHeight-tTop)};
                break;

            default:
                textDrawPos = {textPos.x - tLeft, textPos.y - tTop};
                break;
            }
    }

    std::string getText() {
        return text;
    }

    sf::Uint16 getTextSize() {
        return textSize;
    }

    sf::Vector2f getTextPosition() {
        return textPos;
    }
};


class Label : public TextComponent {
public:
    Label(sf::Vector2f position, sf::Vector2f size, std::string text) : TextComponent(position, size) {
        this->text = text;
    }

    ~Label() {}

    void update(sf::Vector2i& mPos, sf::Vector3i& mBtns, Key& key) {
        TextComponent::update(mPos, mBtns, key);
    }

    void draw(sf::RenderWindow& window) {
        if(!loadedFont) {
            throw ValueError("None font is loaded");
        }
        else {
            sf::RectangleShape shape;
            shape.setSize(size);
            shape.setPosition(pos);
            shape.setFillColor(fillColor);
            shape.setOutlineColor(borderColor);
            window.draw(shape);

            sf::Text t;
            t.setFont(font);
            t.setString(text);
            t.setFillColor(textColor);
            t.setCharacterSize(textSize);
            t.setPosition(textDrawPos + pos);
            window.draw(t);
        }
    }
};


class TextButton : public TextComponent {
private:
    // Highlight color(when mouse hovering on button)
    sf::Color hlFillColor;
    sf::Color hlBorderColor;
    sf::Color hlTextColor;
    // Transition steps used for gradient change(also can be used for instant change)
    sf::Uint16 tFillSteps;
    sf::Uint16 tBorderSteps;
    sf::Uint16 tTextSteps;
    // Count the time of the mouse hovering on the button for gradient change calculations
    sf::Uint16 currFillStep;
    sf::Uint16 currBorderStep;
    sf::Uint16 currTextStep;
    // Used for debouncing the button when clicked
    sf::Uint16 clickTimer;

    sf::Uint16 borderThickness;
    void (*action)();

    bool isSelected(sf::Vector2i mPos) {
        if(pos.x <= mPos.x && mPos.x <= pos.x+size.x) {
            return (pos.y <= mPos.y && mPos.y <= pos.y+size.y);
        }
        return false;
    }

public:
    TextButton(sf::Vector2f position, sf::Vector2f size, std::string text, void (*function)()) : TextComponent(position, size) {
        this->text = text;
        hlFillColor = fillColor;
        hlBorderColor = borderColor;
        hlTextColor = textColor;
        tFillSteps = 1;
        tBorderSteps = 1;
        tTextSteps = 1;
        currFillStep = 0;
        currBorderStep = 0;
        currTextStep = 0;
        action = function;
        clickTimer = 0;
        borderThickness = 0;
    }

    ~TextButton() {}

    void update(sf::Vector2i& mPos, sf::Vector3i& mBtns, Key& key) {
        TextComponent::update(mPos, mBtns, key);
        
        if(clickTimer > 0)
            clickTimer--;

        if(isSelected(mPos)) {
            if(mBtns.x && clickTimer <= 0) {
                action();
                clickTimer = 20;
            }

            if(currFillStep < tFillSteps)
                currFillStep++;
            
            if(currBorderStep < tBorderSteps)
                currBorderStep++;
            
            if(currTextStep < tTextSteps)
                currTextStep++;
        }
        else {
            if(currFillStep > 0)
                currFillStep--;
            
            if(currBorderStep > 0)
                currBorderStep--;
            
            if(currTextStep > 0)
                currTextStep--;
        } 
    }

    void draw(sf::RenderWindow& window) {
        if(!loadedFont) {
            throw ValueError("None font is loaded");
        }
        else {
            sf::RectangleShape shape;
            shape.setSize(size);
            shape.setPosition(pos);
            
            sf::Color tmpFillColor;
            tmpFillColor.r = (fillColor.r*(tFillSteps-currFillStep)+hlFillColor.r*currFillStep)/tFillSteps;
            tmpFillColor.g = (fillColor.g*(tFillSteps-currFillStep)+hlFillColor.g*currFillStep)/tFillSteps;
            tmpFillColor.b = (fillColor.b*(tFillSteps-currFillStep)+hlFillColor.b*currFillStep)/tFillSteps;
            shape.setFillColor(tmpFillColor);
            
            sf::Color tmpBorderColor;
            tmpBorderColor.r = (borderColor.r*(tBorderSteps-currBorderStep)+hlBorderColor.r*currBorderStep)/tBorderSteps;
            tmpBorderColor.g = (borderColor.g*(tBorderSteps-currBorderStep)+hlBorderColor.g*currBorderStep)/tBorderSteps;
            tmpBorderColor.b = (borderColor.b*(tBorderSteps-currBorderStep)+hlBorderColor.b*currBorderStep)/tBorderSteps;
            shape.setOutlineColor(tmpBorderColor);
            shape.setOutlineThickness(borderThickness);
            window.draw(shape);

            sf::Text t;
            t.setFont(font);
            t.setString(text);
            t.setCharacterSize(textSize);
            t.setPosition(textDrawPos + pos);
        
            sf::Color tmpTextColor;
            tmpTextColor.r = (textColor.r*(tTextSteps-currTextStep)+hlTextColor.r*currTextStep)/tTextSteps;
            tmpTextColor.g = (textColor.g*(tTextSteps-currTextStep)+hlTextColor.g*currTextStep)/tTextSteps;
            tmpTextColor.b = (textColor.b*(tTextSteps-currTextStep)+hlTextColor.b*currTextStep)/tTextSteps;
            t.setFillColor(tmpTextColor);

            window.draw(t);
        }
    }

    void setBorderThickness(sf::Uint16 thickness) {
        borderThickness = thickness;
    }

    void setHlFillColor(sf::Color color) {
        hlFillColor = color;
    }

    void setHlBorderColor(sf::Color color) {
        hlBorderColor = color;
    }

    void setHlTextColor(sf::Color color) {
        hlTextColor = color;
    }

    void setFillTransition(sf::Uint16 steps) {
        if(steps == 0)
            throw ValueError("The value of steps must be 1 or above");
        tFillSteps = steps;
    }

    void setBorderTransition(sf::Uint16 steps) {
        if(steps == 0)
            throw ValueError("The value of steps must be 1 or above");
        tBorderSteps = steps;
    }

    void setTextTransition(sf::Uint16 steps) {
        if(steps == 0)
            throw ValueError("The value of steps must be 1 or above");
        tTextSteps = steps;
    }
};


class ScrollBar : public Component {
private:
    sf::Uint8 type;
    sf::Uint8 sliderType;
    sf::Vector2f valRange;
    sf::Vector2f sliderPos;  // the position of slider is relative to the position of the bar
    sf::Vector2f sliderSize;
    float sliderRadius;  // used for the circle scrollbar
    sf::Color sldFillColor;  // slider fill color
    sf::Color sldBorderColor;  // slider border color  

    bool hold;  // used to detemine if the slider is held
    bool pressed;  // used for checking if the mouse was already pressed before hovering slider
    sf::Uint16 sldMouseDist;  // the slider mouse distance is used for calculating the position of the slider when following the mouse

    float slope;
    float intercept;

    bool selected(sf::Vector2i& mPos) {
        if(sliderType == RECT_SLIDER) {
            if(pos.x+sliderPos.x <= mPos.x && mPos.x <= pos.x+sliderPos.x+sliderSize.x) {
                return (pos.y+sliderPos.y <= mPos.y && mPos.y <= pos.y+sliderPos.y+sliderSize.y);
            }
        }
        else {
            return pow(mPos.x - sliderPos.x - pos.x, 2) + pow(mPos.y - sliderPos.y - pos.y, 2) <= pow(sliderRadius,2);
        }

        return false;
    }

public:
    // rectangle slider constructor
    ScrollBar(sf::Vector2f position, sf::Vector2f size, sf::Uint8 type, sf::Vector2f sliderSize, sf::Vector2f sliderPosition={0,0}) : Component(position, size) {
        this->type = type;
        sliderType = RECT_SLIDER;
        sliderPos = sliderPosition;
        this->sliderSize = sliderSize;
        valRange = {0,100};
        fillColor = sf::Color::Blue;
        borderColor = sf::Color::Blue;
        sldFillColor = {127,127,127}; // applying gray color
        sldBorderColor = {127,127,127};

        hold = false;
        pressed = false;

        // creating temporary variables for calculations
        float min_val = std::min(valRange.x, valRange.y);
        float max_val = std::max(valRange.x, valRange.y);

        // calculating the slope and the intercept for calculating slider value
        if(type == VERTICAL) {
            slope = (min_val - max_val)/(0 - (size.y - sliderSize.y));
            intercept = min_val - slope*0;
        }
        else {
            slope = (min_val - max_val)/(0 - (size.x - sliderSize.x));
            intercept = min_val - slope*0;
        }
    }

    // circle slider constructor
    ScrollBar(sf::Vector2f position, sf::Vector2f size, sf::Uint8 type, float sliderRadius, sf::Vector2f sliderPosition={0,0}) : Component(position, size) {
        this->type = type;
        sliderType = CIRCLE_SLIDER;
        sliderPos = sliderPosition;
        this->sliderRadius = sliderRadius;
        valRange = {0,100};
        fillColor = sf::Color::Blue;
        borderColor = sf::Color::Blue;
        sldFillColor = {127,127,127}; // applying gray color
        sldBorderColor = {127,127,127};

        hold = false;
        pressed = false;

        // creating temporary variables for calculations
        float min_val = std::min(valRange.x, valRange.y);
        float max_val = std::max(valRange.x, valRange.y);

        // calculating the slope and the intercept for calculating slider value
        if(type == VERTICAL) {
            slope = (min_val - max_val)/(0 - size.y);
            intercept = min_val - slope*0;
        }
        else {
            slope = (min_val - max_val)/(0 - size.x);
            intercept = min_val - slope*0;
        }
    }

    ~ScrollBar() {}

    void update(sf::Vector2i& mPos, sf::Vector3i& mBtns, Key& key) {
        Component::update(mPos, mBtns, key);

        // checking if mouse is on slider
        if(selected(mPos)) {
            // checking if mouse clicked on slider
            if(mBtns.x && !hold && !pressed) {
                if(type == VERTICAL)
                    sldMouseDist = mPos.y - sliderPos.y;
                else
                    sldMouseDist = mPos.x - sliderPos.x;

                hold = true;
            }
        }
        else {
            if(mBtns.x)
                pressed = true;
        }

        // if the left button of mouse is not on hold removing grip from slider 
        if(!mBtns.x) {
            hold = false;
            pressed = false;
        }

        // if the mouse holding the slider
        if(hold) {
            if(type == VERTICAL) {
                if(sliderType == RECT_SLIDER) {
                    // checking if the position of slider will be out of bounds
                    if(mPos.y - sldMouseDist < 0)
                        sliderPos.y = 0;
                    else if(mPos.y - sldMouseDist > size.y - sliderSize.y)
                        sliderPos.y = size.y - sliderSize.y;
                    else
                        sliderPos.y = mPos.y - sldMouseDist;
                }
                else {
                    if(mPos.y - sldMouseDist < 0)
                        sliderPos.y = 0;
                    else if(mPos.y - sldMouseDist > size.y)
                        sliderPos.y = size.y;
                    else
                        sliderPos.y = mPos.y - sldMouseDist;
                }
            }
            else {
                if(sliderType == RECT_SLIDER) {
                    if(mPos.x - sldMouseDist < 0)
                        sliderPos.x = 0;
                    else if(mPos.x - sldMouseDist > size.x - sliderSize.x)
                        sliderPos.x = size.x - sliderSize.x;
                    else
                        sliderPos.x = mPos.x - sldMouseDist; 
                }
                else {
                    if(mPos.x - sldMouseDist < 0)
                        sliderPos.x = 0;
                    else if(mPos.x - sldMouseDist > size.x)
                        sliderPos.x = size.x;
                    else
                        sliderPos.x = mPos.x - sldMouseDist; 
                } 
            }      
        }
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape shape;
        shape.setSize(size);
        shape.setPosition(pos);
        shape.setFillColor(fillColor);
        shape.setOutlineColor(borderColor);
        window.draw(shape);

        if(sliderType == RECT_SLIDER) {
            shape.setSize(sliderSize);
            shape.setPosition(pos + sliderPos);
            shape.setFillColor(sldFillColor);
            shape.setOutlineColor(sldBorderColor);
            window.draw(shape);
        }
        else {
            sf::CircleShape shape;
            sf::Vector2f tmpSldRad = {sliderRadius,sliderRadius};
            shape.setRadius(sliderRadius);
            shape.setPosition(pos + sliderPos - tmpSldRad);
            shape.setFillColor(sldFillColor);
            shape.setOutlineColor(sldBorderColor);
            window.draw(shape);
        }
    }

    void setType(sf::Uint8 type) {
        if(type != VERTICAL && type != HORIZONTAL)
            throw ValueError("Type "+std::to_string(type)+" is invalid type");
        this->type = type;
    }

    void setValueRange(sf::Vector2f range) {
        valRange = range;

        // creating temporary variables for calculations
        float min_val = std::min(valRange.x, valRange.y);
        float max_val = std::max(valRange.x, valRange.y);

        // calculating the slope and the intercept for calculating slider value
        if(type == VERTICAL) {
            slope = (min_val - max_val)/(0 - (size.y - sliderSize.y));
            intercept = min_val - slope*0;
        }
        else {
            slope = (min_val - max_val)/(0 - (size.x - sliderSize.x));
            intercept = min_val - slope*0;
        }
    }
    
    void setValue(float value) {
        float min_val = std::min(valRange.x, valRange.y);
        float max_val = std::max(valRange.x, valRange.y);

        // need to find a cleaner way to do the exception
        if(value > max_val || value < min_val)
            throw ValueError("The value "+std::to_string(value)+" is outside the range of "+std::to_string(min_val)+" to "+std::to_string(max_val));

        if(type == VERTICAL)
            sliderPos.y = (value - intercept)/slope;
        else
            sliderPos.x = (value - intercept)/slope;
    }

    void setSliderFillColor(sf::Color color) {
        sldFillColor = color;
    }

    void setSliderBorderColor(sf::Color color) {
        sldBorderColor = color;
    }

    float getValue() {
        if(type == VERTICAL)
            return slope*sliderPos.y + intercept;
        else 
            return slope*sliderPos.x + intercept;
    }
};


// NOTES: 
// entry only works with ascii right now because key pressed doesnt support unicode and text entered dont support arrow keys
// auto resizing is buggy and doesnt work good(some characters either appear weird or dont appear at all)
class TextEntry : public TextComponent {
private:
    bool selected;
    char cursor;
    sf::Uint8 blinkDelay;
    sf::Uint8 blinkTimer;
    sf::Uint8 cursorPos;
    sf::Uint8 keyDelay;
    sf::Uint8 ogKeyDelay;  // need this because dynamic change of keyDelay
    sf::Uint8 keyTimer;
    sf::Int8 cursorOffsetX;  // both offsets used for reposition the cursor relative to the text
    sf::Int8 cursorOffsetY;
    std::deque<Key> keys; 
    sf::Int16 lastKey;
    bool repeat;
    sf::Uint8 repeatDelay;
    sf::Uint8 repeatTimer;
    const char* symbolPattern = ")!@#$%^&*(";  // currently temporary solution

    bool isSelected(sf::Vector2i& mPos) {
        if(pos.x <= mPos.x && mPos.x <= pos.x + size.x)
            return (pos.y <= mPos.y && mPos.y <= pos.y + size.y);
        return false;
    }

    void processKey(Key key) {
        switch(key.code) {
            case sf::Keyboard::Right:
                if(cursorPos < text.length())
                    cursorPos++;
            break;

            case sf::Keyboard::Left:
                if(cursorPos > 0)
                    cursorPos--;
            break;

            case sf::Keyboard::BackSpace:
                if(text.length() > 0 && cursorPos > 0) {
                    text.erase(cursorPos-1, 1);
                    cursorPos--;
                }
            break;

            case sf::Keyboard::Space:
                text.insert(text.begin()+cursorPos, ' ');
                cursorPos++;
            break;

            default:
                if(sf::Keyboard::A <= key.code && key.code <= sf::Keyboard::Z) {
                    // 'a'/'A' since sf::Keyboard::A = 0

                    if(key.shift)
                        text.insert(text.begin()+cursorPos, (char)(key.code + 'A'));
                    else
                        text.insert(text.begin()+cursorPos, (char)(key.code + 'a'));
                    cursorPos++;
                }

                if(sf::Keyboard::Num0 <= key.code && key.code <= sf::Keyboard::Num9) {
                    // '0' - sf::Keyboard::Num0 = 22
                    // using the symbol pattern to remap key code values for shift combination
                    if(key.shift)
                        text.insert(text.begin()+cursorPos, (char)symbolPattern[key.code-26]);
                    else 
                        text.insert(text.begin()+cursorPos, (char)(key.code + 22));
                    cursorPos++;
                }
            break;
        }
    }

public:
    TextEntry(sf::Vector2f position, sf::Vector2f size, std::string text) : TextComponent(position, size) {
        this->text = text;
        selected = false;
        cursor = ' ';
        blinkDelay = 30;
        blinkTimer = 0;
        cursorPos = 0;
        keyDelay = 4;
        ogKeyDelay = keyDelay;
        keyTimer = 0;
        cursorOffsetX = -1;
        cursorOffsetY = -2;
        lastKey = -1;
        repeat = false;
        repeatDelay = 4;
        repeatTimer = 0;
    }

    ~TextEntry() {}

    void update(sf::Vector2i& mPos, sf::Vector3i& mBtns, Key& key) {
        TextComponent::update(mPos, mBtns, key);
        
        // processing the keys in deque
        if(keys.size() > 0) {
            processKey(keys[0]);
            keys.pop_front();
        }

        // checking for mouse left click
        if(mBtns.x) {
            if(isSelected(mPos))
                selected = true;
            else
                selected = false;
        }

        // checking if the entry is selected, if yes enabling blinking and key pressing
        if(selected) {
            // delay for blinking
            if(blinkTimer < blinkDelay) {
                blinkTimer++;
            }
            else {
                blinkTimer = 0;

                if(cursor == '|')
                    cursor = ' ';
                else
                    cursor = '|';
            }

            // using deque to not loss keys(not sure if useful), repeat is used for emulating long key pressing
            // timer is for debouncing presses
            if(keyTimer < keyDelay) {
                keyTimer++;
            }
            else {
                // checking if the key isnt invalid
                if(key.code != -1) {
                    // checking for long key press, first times of long press wont add keys to deque
                    if(key.code == lastKey) {
                        if(repeatTimer < repeatDelay && repeat == false) {
                            repeatTimer++;
                        }
                        else {
                            keys.push_back(key);
                            repeat = true;
                        }
                    }
                    else {
                        keys.push_back(key);
                        repeat = false;
                        repeatTimer = 0;
                    }

                    // long key press will have lower delay
                    if(repeat)
                        keyDelay = 2;
                    else
                        keyDelay = ogKeyDelay;
                    
                    keyTimer = 0;
                    lastKey = key.code;
                }
                else {
                    lastKey = key.code;
                    repeatTimer = 0;
                }
            }
        }
        else {
            // if the entry is not selected disabling cursor
            cursor = ' ';
        }
    }
    

    void draw(sf::RenderWindow& window) {
        if(!loadedFont) {
            throw ValueError("None font is loaded");
        }
        else {
            sf::RectangleShape shape;
            shape.setSize(size);
            shape.setPosition(pos);
            shape.setFillColor(fillColor);
            shape.setOutlineColor(borderColor);
            window.draw(shape);

            sf::Text t;
            t.setFont(font);
            t.setString(text);
            t.setFillColor(textColor);
            t.setCharacterSize(textSize);
            t.setPosition(textDrawPos + pos);
            window.draw(t);
  
            sf::Text tCursor;
            t.setString(text.substr(0, cursorPos));
            float tWidth = t.getLocalBounds().width;  
            sf::Vector2f cursorOffset = {tWidth + cursorOffsetX, cursorOffsetY};
            tCursor.setFont(font);
            tCursor.setString(cursor);
            tCursor.setFillColor(textColor);
            tCursor.setCharacterSize(textSize);
            tCursor.setPosition(textDrawPos + pos + cursorOffset);
            window.draw(tCursor);
        }
    }
};
