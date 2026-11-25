#include<iostream>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace sf;
using namespace std;

void drawBricks(sf::RenderWindow& window, int bricks[][100], int& Rows, int& Cols)
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



void handleEvents(RenderWindow& window, bool& isLaunched, float& dx, float& dy)
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<Event::Closed>()) {
            window.close();
        }
        if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
            if (keyPressed->code == Keyboard::Key::Backspace) {
                window.close();
            }
        }

        // Launching The Ball
        if (isLaunched == false)
        {
            if (Keyboard::isKeyPressed(Keyboard::Key::Space))
            {
                isLaunched = true;
                dy = -5.f;
                if (rand() % 2 == 0)
                {
                    dx = -3.f;
                }
                else
                {
                    dx = 3.f;
                }
            }
        }

    }
}

void updatePaddle(float& paddleX, float paddleY, float paddleW, int windX, RectangleShape& paddle)
{
    //PADDLE MOVEMENT
    if (Keyboard::isKeyPressed(Keyboard::Key::Left) || Keyboard::isKeyPressed(Keyboard::Key::A))
    {
        paddleX = paddleX - 8.f;
        paddle.setPosition({ paddleX, paddleY });
        if (paddleX < 3.f)
        {
            paddleX = 3.f;
            paddle.setPosition({ paddleX, paddleY });

        }

    }
    if (Keyboard::isKeyPressed(Keyboard::Key::Right) || Keyboard::isKeyPressed(Keyboard::Key::D))
    {
        paddleX = paddleX + 8.f;
        paddle.setPosition({ paddleX, paddleY });
        if (paddleX > windX - paddleW - 3.f)
        {
            paddleX = windX - paddleW - 3.f;
            paddle.setPosition({ paddleX, paddleY });

        }
    }
}

void updateBall(bool isLaunched, float& ballX, float& ballY, float ballD, float paddleX, float paddleY, float paddleW, int windX, int windY, float& dx, float& dy, CircleShape& ball, float& paddleH, int rows, int cols, int bricks[][100], int& score)
{

    int row, col;

    //BALL MOVEMENT BEFORE LAUNCH
    if (isLaunched == false)
    {
        ballX = paddleX + 44.f;
        ballY = paddleY - ballD;
        ball.setPosition({ ballX, ballY });
    }

    //BALL MOVEMENT AFTER LAUNCH
    if (isLaunched == true)
    {
        int hitRow = -1, hitCol = -1;
        float cx = ballX, cy = ballY;
        // Horizontal Reversing or Continuation
        ballX = ballX + dx;

        bool hit = false;

        //Left/Right Walls Check
        if (ballX > windX - ballD || ballX < 0.f)
        {
            ballX = ballX - dx;
            dx = -dx;
        }

        float cornersX[4] = { ballX, ballX + ballD, ballX + ballD, ballX };
        float cornersY[4] = { ballY, ballY, ballY + ballD, ballY + ballD };

        for (int i = 0; i < 4; i++)
        {
            cx = cornersX[i];
            cy = cornersY[i];
            if (cx < 600 && cx >= 0 && cy < 300 && cy >= 100)
            {
                row = (cy - 100) / 25;
                col = (cx) / 60;
                if (bricks[row][col] > 0)
                {
                    if (row != hitRow || col != hitCol)
                    {
                        bricks[row][col]--;
                        if (bricks[row][col] == 0)
                        {
                            score = score + 50;
                        }
                        else
                        {
                            score = score + 10;
                        }
                        cout << "Score: " << score << endl;
                        hitRow = row;
                        hitCol = col;
                    }
                    hit = true;
                }

            }
        }
        if (hit == true)
        {
            ballX = ballX - dx;
            dx = -dx;
        }

        //Vertical Reversing or Continuation
        hitRow = -1;
        hitCol = -1;
        ballY = ballY + dy;
        cx = ballX, cy = ballY;
        hit = false;

        //Ceiling Check
        if (ballY < 50.f)
        {
            ballY = ballY - dy;
            dy = -dy;
        }

        float cornersX2[4] = { ballX, ballX + ballD, ballX + ballD, ballX };
        float cornersY2[4] = { ballY, ballY, ballY + ballD, ballY + ballD };

        for (int i = 0; i < 4; i++)
        {
            cy = cornersY2[i];
            cx = cornersX2[i];
            if (cx < 600 && cx >= 0 && cy < 300 && cy >= 100)
            {
                row = (cy - 100) / 25;
                col = (cx) / 60;
                if (bricks[row][col] > 0)
                {
                    if (row != hitRow || col != hitCol)
                    {
                        bricks[row][col]--;
                        if (bricks[row][col] == 0)
                        {
                            score = score + 50;
                        }
                        else
                        {
                            score = score + 10;
                        }
                        cout << "Score: " << score << endl;
                        hitRow = row;
                        hitCol = col;
                    }
                    hit = true;
                }


            }
        }
        if (hit == true)
        {
            ballY = ballY - dy;
            dy = -dy;
        }




        // Reversing After Pad Collision
        if (ballY + ballD >= paddleY && ballX + ballD >= paddleX && ballX <= paddleX + paddleW && dy > 0.f && ballY <= paddleY + paddleH)
        {

            ballY = 638.f;

            //Paddle Part Measurement
            int part = (ballX + 6 - paddleX) * 4 / paddleW;
            if (part < 0)
            {
                part = 0;
            }
            else if (part > 3)
            {
                part = 3;
            }
            if (part == 0)
                dx = -6.f;
            else if (part == 1)
                dx = -3.f;
            else if (part == 2)
                dx = 3.f;
            else if (part == 3)
                dx = 6.f;
            dy = -dy;
        }
        ball.setPosition({ ballX, ballY });
    }
}

void renderGame(RenderWindow& window, int bricks[][100], int rows, int cols, RectangleShape& paddle, CircleShape& ball)
{
    window.clear(Color(15, 15, 40));

    drawBricks(window, bricks, rows, cols); // <-- DRAW BRICKS EVERY FRAME
    window.draw(paddle);
    window.draw(ball);

    window.display();
}

int main()
{
    int windX = 600, windY = 700;

    RenderWindow window(
        VideoMode({ 600, 700 }),
        "Block Buster - Testing Window"
    );

    // Draw temporary bricks
    int rows = 0, cols = 0;
    ifstream fin("bricks.txt");
    if (!fin)
    {
        cout << "Your file is not found" << endl;
        return 0;
    }
    int bricks[100][100];
    fin >> rows >> cols;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            fin >> bricks[i][j];
        }
    }
    fin.close();


    //Randomly Choosing Launching Left/Right Direction
    srand(time(0));

    window.setFramerateLimit(60);

    //paddle
    float paddleX = 250.f, paddleY = 650.f;
    float paddleW = 100.f, paddleH = 14.f;
    RectangleShape paddle({ paddleW, paddleH });
    paddle.setPosition({ paddleX, paddleY });
    paddle.setFillColor(Color::Cyan);

    //ball
    float ballD = 12.f;
    float ballX = paddleX + 44.f, ballY = paddleY - ballD;
    CircleShape ball(6.f);
    ball.setPosition({ ballX, ballY });
    ball.setFillColor(Color::White);


    //Time Measurement

    /*Clock gameclock;*/

    // FOR CKECKING FOR BALL ATTACHING AND DETACHING BEFORE AND AFTER LAUNCHING
    bool isLaunched = false;

    // Speed Variables
    float dx = 0.f, dy = 0.f;

    //Score Counting
    int score = 0;

    while (window.isOpen())
    {
        /* float dt = gameclock.restart().asSeconds();*/
        handleEvents(window, isLaunched, dx, dy);
        updatePaddle(paddleX, paddleY, paddleW, windX, paddle);
        updateBall(isLaunched, ballX, ballY, ballD, paddleX, paddleY, paddleW, windX, windY, dx, dy, ball, paddleH, rows, cols, bricks, score);
        renderGame(window, bricks, rows, cols, paddle, ball);
    }

    return 0;
}