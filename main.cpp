#include "gui_comps.h"
#include <thread>


bool running = true;
Label comp("hello", {100,100}, {120,80});
sf::Mouse mouse;

void update() {
    while(running) {
        comp.update(mouse);
        sf::sleep(sf::Time(sf::milliseconds(16)));
    }
}

int main() {
    comp.loadFont("ARIAL.TTF");
    comp.setTextSize(30);
    comp.setTextAnchor(CENTER);
    comp.setTextPos({0,-10});
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
            if(lock == false) {
                comp.accelerate({400,400}, {8,4}, 100, ACCEL_FAST);
                lock = true;
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
            if(lock) {
                comp.setPosition({100,100});
                lock = false;
            }
        }          

        window.clear(sf::Color::Black);
        comp.draw(window);
        window.display();
    }
    t.join();
    return 0;
}
