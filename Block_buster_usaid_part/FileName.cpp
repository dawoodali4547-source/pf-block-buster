//#include <SFML/Graphics.hpp>
//#include <filesystem>
//#include <iostream>
//
//int main() {
//    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SFML 3 Font Example");
//
//    std::cout << "Working dir: " << std::filesystem::current_path() << '\n';
//     
//    sf::Font font;
//    if (!font.openFromFile("C:/Users/YourUsername/Desktop/Block_buster_usaid_part/GoogleSans.ttf") 
//    { // or "Oswald.ttf"
//        std::cerr << "Error loading font\n";
//        return -1;
//    }
//
//    sf::Text text(font, "Hello, SFML 3!", 32);
//    text.setFillColor(sf::Color::White);
//    text.setPosition({ 50.f, 50.f });
//
//    while (window.isOpen()) {
//        while (const auto event = window.pollEvent()) {
//            if (event->is<sf::Event::Closed>()) {
//                window.close();
//            }
//        }
//
//        window.clear();
//        window.draw(text);
//        window.display();
//    }
//}
