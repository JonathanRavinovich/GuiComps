#include "gui_comps.h"
#include <thread>

void func() {
    std::cout << "hello" << std::endl;
}

bool running = true;
TextButton comp({100,100}, {100,60}, "hello", &func);
ScrollBar comp2({100,200}, {30,100}, VERTICAL);
ScrollBar comp3({100,350}, {100,30}, HORIZONTAL);
sf::Mouse mouse;
sf::Vector2i mouse_pos = {0,0};
sf::Vector3i mouse_buttons = {0,0,0};

void update() {
    while(running) {
        comp.update(mouse_pos, mouse_buttons);
        comp2.update(mouse_pos, mouse_buttons);
        comp3.update(mouse_pos, mouse_buttons);
        sf::sleep(sf::Time(sf::milliseconds(16)));
    }
}

int main() {
    std::cout << (sizeof(ScrollBar)) << std::endl;
    sf::RenderWindow window(sf::VideoMode(800,600), "Window");

    comp.loadFont("ARIAL.TTF");
    comp.setTextSize(30);
    comp.setTextAnchor(CENTER);
    comp.setTextColor(sf::Color::White);
    comp.setFillColor(sf::Color::Black);
    comp.setBorderColor(sf::Color::Black);
    comp.setHlFillColor(sf::Color::Black);
    comp.setHlTextColor(sf::Color::Green);
    comp.setTextTransition(10);
    comp.setHlBorderColor(sf::Color::Green);
    comp.setBorderThickness(3);

    comp3.setValueRange({-10,10});
    comp3.setValue(-4);

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
                comp.move({400,400}, {8,4}, 100, ACCEL_FAST);
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
        comp2.draw(window);
        comp3.draw(window);
        window.display();

        std::cout << comp3.getValue() << std::endl;

    }
    t.join();
    return 0;
}
