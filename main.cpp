#include "gui_comps.h"
#include <thread>


bool running = true;

void update() {
    while(running) {
        comp.update();
        sf::sleep(sf::Time(sf::milliseconds(16)));
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800,600), "Window");
    std::thread t(update);
    bool lock = false;
    
    while(window.isOpen()) {
        sf::Event event;

        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                window.close();
                running = false;
            }
        }  

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
            if(lock == false && comp.getPos().x==0 && comp.getPos().y==0) {
                comp.accelerate({400,400}, {8,4}, 100, ACCEL_SLOW);
                lock = true;
            }
        }          

        window.clear(sf::Color::Black);
        comp.draw(window);
        window.display();
    }
    t.join();
    return 0;
}
