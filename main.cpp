#include "gui_comps.h"
#include <thread>

///////////////////////////////////////////////////////
// This is a demo code to test the library functions //
// I will add better demo in the future              //
///////////////////////////////////////////////////////

void func() {
    std::cout << "hello" << std::endl;
}

bool running = true;
TextButton comp({100,100}, {100,60}, "hello", &func);
ScrollBar comp2({100,200}, {100,14}, HORIZONTAL, 20, {0, 7});
ScrollBar comp3({100,350}, {100,30}, HORIZONTAL, {30,30});
TextEntry comp4({100,450}, {180,60}, "hello");
sf::Mouse mouse;
sf::Vector2i mouse_pos = {0,0};
sf::Vector3i mouse_buttons = {0,0,0};
Key key;

void update() {
    while(running) {
        comp.update(mouse_pos, mouse_buttons, key);
        comp2.update(mouse_pos, mouse_buttons, key);
        comp3.update(mouse_pos, mouse_buttons, key);
        comp4.update(mouse_pos, mouse_buttons, key);
        
        // std::cout << key << std::endl;

        // if (comp2.getValue() >= 10 - 0.1)
        //     comp2.setValue(-10);
        // else
        //     comp2.setValue(comp2.getValue()+ 0.1);
        // std::cout << comp2.getValue() << std::endl;

        sf::sleep(sf::Time(sf::milliseconds(16)));
    }
}

int main() {
    std::cout << (sizeof(TextButton)) << std::endl;
    sf::RenderWindow window(sf::VideoMode(800,600), "Window");
    key = {-1,0,0,0,0};
    
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


    comp2.setValueRange({-10,10});
    comp2.setValue(-4);

    comp4.loadFont("ARIAL.TTF");
    comp4.setTextSize(30);
    comp4.setTextAnchor(LEFT);

    std::thread t(update);
    bool lock = false;
    // key.code = -1;
    
    while(window.isOpen()) {
        sf::Event event;

        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                window.close();
                running = false;
            }

            if(event.type == sf::Event::TextEntered) {
                // std::cout << event.text.unicode << std::endl;
            }

            if(event.type == sf::Event::KeyPressed) {
                key.code = event.key.code;
                key.alt = event.key.alt;
                key.control = event.key.control;
                key.shift = event.key.shift;
                key.system = event.key.system;
                std::cout << event.key.code << std::endl;
            }

            if(event.type == sf::Event::KeyReleased) {
                key.code = -1;
            }
        }       

        mouse_pos = mouse.getPosition(window);
        mouse_buttons = {mouse.isButtonPressed(mouse.Left),mouse.isButtonPressed(mouse.Middle),mouse.isButtonPressed(mouse.Right)};
        window.clear(sf::Color::Black);
        comp.draw(window);
        comp2.draw(window);
        comp3.draw(window);
        comp4.draw(window);
        window.display();

    }
    t.join();
    return 0;
}
