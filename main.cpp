#include "gui_comps.h"
#include <thread>

void func() {
    std::cout << "hello" << std::endl;
}

bool running = true;
TextButton comp("hello", &func, {100,100}, {100,60});
sf::Mouse mouse;
sf::Vector2i mouse_pos = {0,0};
sf::Vector3i mouse_buttons = {0,0,0};

void update() {
    while(running) {
        comp.update(mouse_pos, mouse_buttons);
        sf::sleep(sf::Time(sf::milliseconds(16)));
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800,600), "Window");

    comp.loadFont("ARIAL.TTF");
    comp.setTextSize(30);
    comp.setTextAnchor(CENTER);
    comp.setTextPos({0,0});
    comp.setHlTextColor(sf::Color::Green);
    comp.setTextTransition(1);
    comp.setBorderThickness(3);
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

        mouse_pos = mouse.getPosition(window);
        mouse_buttons = {mouse.isButtonPressed(mouse.Left),mouse.isButtonPressed(mouse.Middle),mouse.isButtonPressed(mouse.Right)};
        window.clear(sf::Color::Black);
        comp.draw(window);
        window.display();
    }
    t.join();
    return 0;
}
