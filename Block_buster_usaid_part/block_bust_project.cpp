#include <SFML/Graphics.hpp>
#include <iostream>
#include<fstream>
using namespace std;
void drawBricks(sf::RenderWindow& window, int bricks[][100],int &Rows,int &Cols) 
{
    const float BRICK_W = 60.f;
    const float BRICK_H = 25.f;
    const float BRICKS_Y = 100.f;
    sf::Color colors[4] = { sf::Color(25, 25, 45), 
                           sf::Color::Green, 
                           sf::Color::Yellow, 
                           sf::Color::Red };
    sf::RectangleShape brick({ BRICK_W - 2.f, BRICK_H - 2.f }); 

    for (int r = 0; r < Rows; r++)
    {
        for (int c = 0; c < Cols; c++) 
        {
            if (bricks[r][c] == 0) {
                continue;
            }

            brick.setFillColor(colors[bricks[r][c]]);
            brick.setPosition({ c * BRICK_W + 1.f, BRICKS_Y + r * BRICK_H + 1.f });
            window.draw(brick);
        }
    }
}
int main() {
    int rows =0, cols=0 ;
    sf::RenderWindow window(sf::VideoMode({ 600, 700 }), "Block Buster");
    window.setFramerateLimit(60);
	ifstream fin("bricks.txt");
    if (!fin)
    {
        cout << "Your file is not found" << endl;
        return 0;
    }
    int bricks[100][100];
    fin >> rows >> cols;
    for (int i = 0;i < rows;i++)
    {
        for (int j = 0;j < cols;j++)
        {
            fin >> bricks[i][j];
        }
    }
    fin.close();

    
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
        drawBricks(window, bricks, rows, cols);

        window.display();
    }

    return 0;
}
