#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <deque>
#include <vector>
#include <algorithm>
#include <iostream>

#define ACCEL_FAST 0
#define ACCEL_SLOW 1

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

    virtual ~Component(){}
    
    void update() {
        if(moves.size() > 0) {
            pos = moves[0];
            moves.pop_front();
        }
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape shape(size);
        shape.setPosition(pos);
        shape.setFillColor(color);
        window.draw(shape);
    }

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

