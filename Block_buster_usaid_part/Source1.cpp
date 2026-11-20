//#include <SFML/Graphics.hpp>
//#include <iostream>
//using namespace sf;
//
//int main()
//{
//    RenderWindow window(VideoMode(800, 600), "Block Buster - Usaid");
//    window.setFramerateLimit(60);
//
//    // Paddle
//    RectangleShape paddle(Vector2f(120, 20));
//    paddle.setFillColor(Color::Cyan);
//    paddle.setPosition(340, 550);
//
//    // Ball
//    CircleShape ball(10);
//    ball.setFillColor(Color::White);
//    ball.setPosition(390, 400);
//    float ballSpeedX = 4, ballSpeedY = -4;
//    bool ballStarted = false;
//
//    // Bricks
//    const int rows = 5, cols = 8;
//    RectangleShape bricks[rows][cols];
//    bool brickAlive[rows][cols];
//    Color colors[5] = { Color::Red, Color::Yellow, Color::Green, Color::Magenta, Color::Blue };
//
//    for (int i = 0; i < rows; i++) {
//        for (int j = 0; j < cols; j++) {
//            bricks[i][j].setSize(Vector2f(85, 25));
//            bricks[i][j].setPosition(20 + j * 95, 50 + i * 35);
//            bricks[i][j].setFillColor(colors[i]);
//            brickAlive[i][j] = true;
//        }
//    }
//
//    int score = 0;
//    int lives = 3;
//    Font font;
//    // Font nahi hai to text skip ho jayega, game phir bhi chalegi
//    Text scoreText;
//    scoreText.setCharacterSize(20);
//    scoreText.setFillColor(Color::White);
//    scoreText.setPosition(10, 560);
//
//    bool gameOver = false;
//    bool gameWon = false;
//
//    while (window.isOpen())
//    {
//        Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == Event::Closed)
//                window.close();
//            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Space)
//                ballStarted = true;
//        }
//
//        if (!gameOver && !gameWon) {
//            // Paddle move
//            if (Keyboard::isKeyPressed(Keyboard::Left) && paddle.getPosition().x > 0)
//                paddle.move(-7, 0);
//            if (Keyboard::isKeyPressed(Keyboard::Right) && paddle.getPosition().x < 680)
//                paddle.move(7, 0);
//
//            if (!ballStarted) {
//                ball.setPosition(paddle.getPosition().x + 55, 530);
//            }
//            else {
//                ball.move(ballSpeedX, ballSpeedY);
//
//                // Wall collision
//                if (ball.getPosition().x <= 0 || ball.getPosition().x >= 780)
//                    ballSpeedX = -ballSpeedX;
//                if (ball.getPosition().y <= 0)
//                    ballSpeedY = -ballSpeedY;
//
//                // Paddle collision
//                if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
//                    ballSpeedY = -abs(ballSpeedY);
//                    ball.setPosition(ball.getPosition().x, 540);
//                }
//
//                // Brick collision
//                for (int i = 0; i < rows; i++) {
//                    for (int j = 0; j < cols; j++) {
//                        if (brickAlive[i][j] && ball.getGlobalBounds().intersects(bricks[i][j].getGlobalBounds())) {
//                            brickAlive[i][j] = false;
//                            ballSpeedY = -ballSpeedY;
//                            score += 10;
//                        }
//                    }
//                }
//
//                // Ball gir gayi
//                if (ball.getPosition().y > 600) {
//                    lives--;
//                    ballStarted = false;
//                    if (lives <= 0) gameOver = true;
//                }
//
//                // Win check
//                bool allBroken = true;
//                for (int i = 0; i < rows; i++)
//                    for (int j = 0; j < cols; j++)
//                        if (brickAlive[i][j]) allBroken = false;
//                if (allBroken) gameWon = true;
//            }
//        }
//
//        window.clear(Color::Black);
//        for (int i = 0; i < rows; i++)
//            for (int j = 0; j < cols; j++)
//                if (brickAlive[i][j]) window.draw(bricks[i][j]);
//
//        window.draw(paddle);
//        window.draw(ball);
//        window.display();
//    }
//    return 0;
//}