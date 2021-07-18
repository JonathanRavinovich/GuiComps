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


class Component {
private:
    sf::Vector2f pos;
    sf::Vector2f size;
    sf::Color color = sf::Color::White;
    std::deque<sf::Vector2f> moves;

public:
    Component(sf::Vector2f position={0,0}, sf::Vector2f size={30,30}){
        pos = position;
        this->size = size;
    }

    ~Component(){}
    
    void update() {
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

    sf::Vector2f getPos() {
        return pos;
    }

    sf::Vector2f getSize() {
        return size;
    }

    void setColor(sf::Color color) {
        this->color = color;
    }
};


class TextComponent : public Component {
private:
    sf::Font font;
    sf::Int16 textSize;
    sf::Vector2f textPos;
    sf::Color textColor;
    sf::Uint8 mode;

public:
    std::string text;

    TextComponent(sf::Vector2f position={0,0}, sf::Vector2f size={30,30}) : Component(position, size){
        text = "";
        textSize = 10;
        textPos = {0,0};
        textColor = sf::Color::Black;
        mode = CENTER;
    }

    void setTextSize(sf::Int16 size) {
        textSize = size;
    }

    void setTextColor(sf::Color color) {
        textColor = color;
    }

    void loadFont(std::string path) {
        if(!font.loadFromFile(path))
            std::cout << "error loading path" << std::endl;
    }

    sf::Vector2f getTextPos() {
        return textPos;
    }
};

