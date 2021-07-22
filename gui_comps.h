////////////////////////////////////
// GuiComps version 0.01          //
// Created by Jonathan Ravinovich //
////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <deque>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>

#define ACCEL_FAST 0
#define ACCEL_SLOW 1

#define CENTER 0
#define LEFT 1
#define RIGHT 2
#define UP 3
#define DOWN 4
#define NONE 5

class Component {
protected:
    sf::Vector2f pos;
    sf::Vector2f size;
    sf::Color fillColor;
    sf::Color borderColor;
    std::deque<sf::Vector2f> moves;

public:
    Component(sf::Vector2f position={0,0}, sf::Vector2f size={30,30}){
        pos = position;
        this->size = size;
        fillColor = sf::Color::White;
        borderColor = sf::Color::White;
    }

    ~Component(){}
    
    void update(sf::Mouse mouse) {
        if(moves.size() > 0) {
            pos = moves[0];
            moves.pop_front();
        }
    }

    virtual void draw(sf::RenderWindow& window) = 0;

    void accelerate(sf::Vector2f pos, sf::Vector2f accelVec={2,2}, int32_t numOfSteps=10, char type=ACCEL_FAST) {
        float newX = pos.x;
        float newY = pos.y;
        float tmpX = this->pos.x;
        float tmpY = this->pos.y;
        float nextX = 0;
        float nextY = 0;
        std::vector<sf::Vector2f> tmp_moves;

        //calculating the path of the object using accel*(end_pos - current_pos)/(num_of_steps - current_step)
        for(int32_t i=0; i<numOfSteps; i++) {
            nextX = accelVec.x*(newX - tmpX)/(numOfSteps - i);
            nextY = accelVec.y*(newX - tmpY)/(numOfSteps - i);
            tmpX += nextX;
            tmpY += nextY;
            tmp_moves.push_back({nextX, nextY});
        }

        tmpX = this->pos.x;
        tmpY = this->pos.y;

        if(type == ACCEL_SLOW) {
            std::reverse(tmp_moves.begin(), tmp_moves.end());
        }
        
        //retracing the path
        for(auto move : tmp_moves) {
            tmpX += move.x;
            tmpY += move.y;
            moves.push_back({tmpX, tmpY});
        }
    }

    void setPosition(sf::Vector2f position) {
        pos = position;
    }

    void setFillColor(sf::Color color) {
        fillColor = color;
    }

    void setBorderColor(sf::Color color) {
        borderColor = color;
    }

    void setSize(sf::Vector2f size) {
        this->size = size;
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
    sf::Font font;
    sf::Int16 textSize;
    sf::Vector2f textPos;
    sf::Color textColor;
    sf::Uint8 anchor;
    bool loadedFont;

public:
    std::string text;

    TextComponent(sf::Vector2f position={0,0}, sf::Vector2f size={30,30}) : Component(position, size){
        text = "";
        textSize = 10;
        textPos = {0,0};
        textColor = sf::Color::Black;
        anchor = NONE;
        loadedFont = false;
    }

    void setTextSize(sf::Int16 size) {
        textSize = size;
    }

    void setTextColor(sf::Color color) {
        textColor = color;
    }

    void setTextPos(sf::Vector2f position) {
        textPos = position;
    }

    void setTextAnchor(sf::Uint8 anchor) {
        this->anchor = anchor;
    }

    void loadFont(std::string path) {
        if(!font.loadFromFile(path))
            std::cout << "error loading font" << std::endl;
        else
            loadedFont = true;
    }

    sf::Vector2f getTextPos() {
        return textPos;
    }
};


class Label : public TextComponent {
public:
    Label(std::string text, sf::Vector2f position={0,0}, sf::Vector2f size={30,30}) : TextComponent(position, size) {
        this->text = text;
    }

    ~Label() {}

    void draw(sf::RenderWindow& window) {
        if(!loadedFont) {
            std::cout << "Error, font not loaded" << std::endl;
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

            float tWidth = t.getLocalBounds().width;
            float tHeight = t.getLocalBounds().height;
            float tTop = t.getLocalBounds().top;
            float tLeft = t.getLocalBounds().left;

            switch(anchor) {
            case CENTER:
                t.setPosition({pos.x+textPos.x+(size.x-tWidth)/2 - tLeft, pos.y+textPos.y+(size.y-textSize)/2 - tTop});
                break;

            case LEFT:
                t.setPosition({pos.x+textPos.x - tLeft, pos.y+textPos.y+(size.y-tHeight)/2 - tTop});
                break;

            case RIGHT:
                t.setPosition({pos.x+textPos.x+(size.x-tWidth-tLeft), pos.y+textPos.y+(size.y-tHeight)/2 - tTop});
                break;

            case UP:
                t.setPosition({pos.x+textPos.x+(size.x-tWidth)/2 - tLeft, pos.y+textPos.y - tTop});
                break;

            case DOWN:
                t.setPosition({pos.x+textPos.x+(size.x-tWidth)/2 - tLeft, pos.y+textPos.y+(size.y-tHeight-tTop)});
                break;

            default:
                t.setPosition({pos.x+textPos.x - tLeft, pos.y+textPos.y - tTop});
                break;
            }

            window.draw(t);
        }
    }
};


class TextButton : public TextComponent {
private:
    sf::Color hlFillColor;
    sf::Color hlBorderColor;
    sf::Color hlTextColor;
    sf::Uint16 tFillSteps;
    sf::Uint16 tBorderSteps;
    sf::Uint16 tTextSteps;
    sf::Uint16 currFillStep;
    sf::Uint16 currBorderStep;
    sf::Uint16 currTextStep;
    bool isSelected;

    bool selected(sf::Vector2i m_pos) {
        if(pos.x <= m_pos.x && m_pos.x <= pos.x+size.x) {
            return (pos.y <= m_pos.y && m_pos.y <= pos.y+size.y);
        }
    }

public:
    TextButton(std::string text, sf::Vector2f position={0,0}, sf::Vector2f size={30,30}) : TextComponent(position, size) {
        this->text = text;
    }

    ~TextButton() {}

    void update(sf::Mouse mouse) {
        TextComponent::update(mouse);
        
        if(selected(mouse.getPosition())) {

        }
        
    }

    void draw(sf::RenderWindow& window) {
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

        float tWidth = t.getLocalBounds().width;
        float tHeight = t.getLocalBounds().height;
        float tTop = t.getLocalBounds().top;
        float tLeft = t.getLocalBounds().left;

        switch(anchor) {
        case CENTER:
            t.setPosition({pos.x+textPos.x+(size.x-tWidth)/2 - tLeft, pos.y+textPos.y+(size.y-textSize)/2 - tTop});
            break;

        case LEFT:
            t.setPosition({pos.x+textPos.x - tLeft, pos.y+textPos.y+(size.y-tHeight)/2 - tTop});
            break;

        case RIGHT:
            t.setPosition({pos.x+textPos.x+(size.x-tWidth-tLeft), pos.y+textPos.y+(size.y-tHeight)/2 - tTop});
            break;

        case UP:
            t.setPosition({pos.x+textPos.x+(size.x-tWidth)/2 - tLeft, pos.y+textPos.y - tTop});
            break;

        case DOWN:
            t.setPosition({pos.x+textPos.x+(size.x-tWidth)/2 - tLeft, pos.y+textPos.y+(size.y-tHeight-tTop)});
            break;

        default:
            t.setPosition({pos.x+textPos.x - tLeft, pos.y+textPos.y - tTop});
            break;
        }

        window.draw(t);
    }
};
