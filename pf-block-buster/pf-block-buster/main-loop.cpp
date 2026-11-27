#include<iostream>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include<string>
using namespace sf;
using namespace std;

// gameState values: 0 = menu, 1 = playing, 2 = game over, 3 = you win
// menuChoice values: 0 = new game, 1 = continue, 2 = exit
// puType values: 1 = wide paddle, 2 = extra life, 3 = slow ball

bool loadLevel(int bricks[][10], int& Rows, int& Cols)
{
    ifstream fin("level.txt");
    if (!fin) 
        return false;

    Rows = 0;
    Cols = 0;
    if (!(fin >> Rows >> Cols)) 
        return false;

    if (Rows > 8)  
        Rows = 8;
    if (Cols > 10) 
        Cols = 10;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            bricks[i][j] = 0;
        }
    }

    for (int i = 0; i < Rows; i++)
    {
        for (int j = 0; j < Cols; j++)
        {
            fin >> bricks[i][j];
        }
    }
    fin.close();
    return true;
}

void drawBricks(sf::RenderWindow& window, int bricks[][10], int& Rows, int& Cols)
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

bool saveGame(int score, int lives, int bricks[][10], int Rows, int Cols)
{
    ofstream fout("savegame.txt");
    if (!fout) return 
        false;

    fout << score << " " << lives << "\n";
    for (int r = 0; r < Rows; r++)
    {
        for (int c = 0; c < Cols; c++)
        {
            fout << bricks[r][c];
            if (c < Cols - 1) 
                fout << " ";
        }
        fout << "\n";
    }
    return true;
}

void clearSave()
{
    ofstream f("savegame.txt");
}

bool loadSave(int& gameState, int& score, int& lives, int bricks[][10],int& Rows, int& Cols,float& paddleX, float& paddleW, float& ballX, float& ballY,bool& isLaunched, float& dx, float& dy)
{
    ifstream fin("savegame.txt");
    if (!fin) 
        return false;

    int s = 0, l = 0;
    if (!(fin >> s >> l)) 
        return false;

    int temp[8][10];
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 10; c++)
        {
            if (!(fin >> temp[r][c])) 
                return false;
        }
    }
    fin.close();

    score = s;
    lives = l;
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 10; c++)
        {
            bricks[r][c] = temp[r][c];
        }
    }

    paddleX = 250.f;
    paddleW = 100.f;
    isLaunched = false;
    ballX = paddleX + (paddleW - 12.f) * 0.5f;
    ballY = 650.f - 12.f;
    dx = 0.f;
    dy = 0.f;
    gameState = 1;
    return true;
}

bool startNewGame(int& gameState, int& score, int& lives, int bricks[][10],int& Rows, int& Cols,float& paddleX, float& paddleW, float& ballX, float& ballY,bool& isLaunched, float& dx, float& dy,bool puActive[], int& wideTimer, int& slowTimer)
{
    score = 0;
    lives = 3;
    paddleX = 250.f;
    paddleW = 100.f;
    isLaunched = false;
    dx = 0.f;
    dy = 0.f;
    for (int i = 0; i < 8; i++) 
        puActive[i] = false;
    wideTimer = 0;
    slowTimer = 0;

    if (!loadLevel(bricks, Rows, Cols)) 
        return false;

    ballX = paddleX + (paddleW - 12.f) * 0.5f;
    ballY = 650.f - 12.f;
    gameState = 1;
    return true;
}

void spawnPowerUp(float cx, float cy, bool puActive[], float puX[], float puY[], int puType[])
{
    if (rand() % 100 >= 25) 
        return;
    for (int i = 0; i < 8; i++)
    {
        if (!puActive[i])
        {
            puActive[i] = true;
            puX[i] = cx - 10.f;
            puY[i] = cy - 10.f;
            puType[i] = 1 + (rand() % 3);
            return;
        }
    }
}

void handleEvents(RenderWindow& window, int& gameState, int& menuChoice, string& menuMessage,bool& isLaunched, float& dx, float& dy,int& score, int& lives, int bricks[][10], int& Rows, int& Cols,float& paddleX, float& paddleW, float& ballX, float& ballY,bool puActive[], int& wideTimer, int& slowTimer)
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<Event::Closed>())
        {
            window.close();
        }

        // Launching the ball — your original style (isKeyPressed inside event loop)
        if (gameState == 1 && isLaunched == false)
        {
            if (Keyboard::isKeyPressed(Keyboard::Key::Space))
            {
                isLaunched = true;
                dy = -5.f;
                if (rand() % 2 == 0) dx = -3.f;
                else                 dx = 3.f;
            }
        }

        if (const auto* keyPressed = event->getIf<Event::KeyPressed>())
        {
            if (gameState == 0)  // menu
            {
                if (keyPressed->code == Keyboard::Key::Up)
                {
                    menuChoice = (menuChoice + 2) % 3;
                    menuMessage = "";
                }
                else if (keyPressed->code == Keyboard::Key::Down)
                {
                    menuChoice = (menuChoice + 1) % 3;
                    menuMessage = "";
                }
                else if (keyPressed->code == Keyboard::Key::Enter)
                {
                    if (menuChoice == 0)  // new game
                    {
                        if (!startNewGame(gameState, score, lives, bricks, Rows, Cols,
                            paddleX, paddleW, ballX, ballY, isLaunched, dx, dy,
                            puActive, wideTimer, slowTimer))
                            menuMessage = "level.txt not found";
                        else
                            menuMessage = "";
                    }
                    else if (menuChoice == 1)  // continue
                    {
                        if (!loadSave(gameState, score, lives, bricks, Rows, Cols,
                            paddleX, paddleW, ballX, ballY, isLaunched, dx, dy))
                            menuMessage = "No saved game";
                        else
                            menuMessage = "";
                    }
                    else if (menuChoice == 2)  // exit
                    {
                        window.close();
                        return;
                    }
                }
            }
            else if (gameState == 1)  // playing
            {
                if (keyPressed->code == Keyboard::Key::Escape)
                {
                    saveGame(score, lives, bricks, Rows, Cols);
                    gameState = 0;
                    menuChoice = 0;
                    menuMessage = "Game saved";
                }
            }
            else  // game over or you win
            {
                if (keyPressed->code == Keyboard::Key::Enter)
                {
                    clearSave();
                    gameState = 0;
                    menuChoice = 0;
                    menuMessage = "";
                }
            }
        }
    }
}

void updatePaddle(float& paddleX, float paddleY, float paddleW, int windX, RectangleShape& paddle)
{
    if (Keyboard::isKeyPressed(Keyboard::Key::Left) || Keyboard::isKeyPressed(Keyboard::Key::A))
    {
        paddleX = paddleX - 8.f;
    }
    if (Keyboard::isKeyPressed(Keyboard::Key::Right) || Keyboard::isKeyPressed(Keyboard::Key::D))
    {
        paddleX = paddleX + 8.f;
    }

    if (paddleX < 0.f) 
        paddleX = 0.f;
    if (paddleX > windX - paddleW) 
        paddleX = windX - paddleW;

    paddle.setPosition({ paddleX, paddleY });
}

void updateBall(bool& isLaunched, float& ballX, float& ballY, float ballD,float paddleX, float paddleY, float paddleW, float paddleH,int windX, int windY,float& dx, float& dy, CircleShape& ball,int rows, int cols, int bricks[][10], int& score,bool puActive[], float puX[], float puY[], int puType[],int slowTimer)
{
    if (isLaunched == false)
    {
        ballX = paddleX + (paddleW - ballD) * 0.5f;
        ballY = paddleY - ballD;
        ball.setPosition({ ballX, ballY });
        return;
    }

    // Slow-motion step size
    float stepX = dx;
    float stepY = dy;
    if (slowTimer > 0)
    {
        stepX = dx * 0.5f;
        stepY = dy * 0.5f;
    }

    // ---- X step ----
    ballX = ballX + stepX;

    if (ballX > windX - ballD || ballX < 0.f)
    {
        ballX = ballX - stepX;
        dx = -dx;
    }

    float cornersX[4] = { ballX, ballX + ballD, ballX + ballD, ballX };
    float cornersY[4] = { ballY, ballY, ballY + ballD, ballY + ballD };

    int ids[4] = { -1, -1, -1, -1 };
    int nIds = 0;
    bool hitX = false;

    for (int i = 0; i < 4; i++)
    {
        float cx = cornersX[i];
        float cy = cornersY[i];
        if (cx < 600 && cx >= 0 && cy < 300 && cy >= 100)
        {
            int row = (int)((cy - 100) / 25);
            int col = (int)(cx / 60);
            if (bricks[row][col] > 0)
            {
                int id = row * 10 + col;
                bool dup = false;
                for (int k = 0; k < nIds; k++)
                {
                    if (ids[k] == id) 
                    { 
                        dup = true; 
                        break; 
                    }
                }
                if (dup) 
                { 
                    hitX = true; 
                    continue;
                }

                ids[nIds] = id;
                nIds++;

                bricks[row][col] = bricks[row][col] - 1;
                if (bricks[row][col] == 0)
                {
                    score = score + 50;
                    spawnPowerUp(col * 60.f + 30.f, 100.f + row * 25.f + 12.5f,
                        puActive, puX, puY, puType);
                }
                else
                {
                    score = score + 10;
                }
                hitX = true;
            }
        }
    }
    if (hitX == true)
    {
        ballX = ballX - stepX;
        dx = -dx;
    }

    // ---- Y step ----
    ballY = ballY + stepY;

    if (ballY < 50.f)
    {
        ballY = ballY - stepY;
        dy = -dy;
    }

    float cornersX2[4] = { ballX, ballX + ballD, ballX + ballD, ballX };
    float cornersY2[4] = { ballY, ballY, ballY + ballD, ballY + ballD };

    int ids2[4] = { -1, -1, -1, -1 };
    int nIds2 = 0;
    bool hitY = false;

    for (int i = 0; i < 4; i++)
    {
        float cx = cornersX2[i];
        float cy = cornersY2[i];
        if (cx < 600 && cx >= 0 && cy < 300 && cy >= 100)
        {
            int row = (int)((cy - 100) / 25);
            int col = (int)(cx / 60);
            if (bricks[row][col] > 0)
            {
                int id = row * 10 + col;
                bool dup = false;
                for (int k = 0; k < nIds2; k++)
                {
                    if (ids2[k] == id) 
                    { 
                        dup = true; 
                        break; 
                    }
                }
                if (dup) 
                { 
                    hitY = true; 
                    continue; 
                }

                ids2[nIds2] = id;
                nIds2++;

                bricks[row][col] = bricks[row][col] - 1;
                if (bricks[row][col] == 0)
                {
                    score = score + 50;
                    spawnPowerUp(col * 60.f + 30.f, 100.f + row * 25.f + 12.5f,
                        puActive, puX, puY, puType);
                }
                else
                {
                    score = score + 10;
                }
                hitY = true;
            }
        }
    }
    if (hitY == true)
    {
        ballY = ballY - stepY;
        dy = -dy;
    }

    // ---- Paddle collision ----
    if (ballY + ballD >= paddleY && ballX + ballD >= paddleX && ballX <= paddleX + paddleW && dy > 0.f && ballY <= paddleY + paddleH)
    {
        ballY = 638.f;

        int part = (int)((ballX + 6 - paddleX) * 4.f / paddleW);
        if (part < 0) 
            part = 0;
        else if (part > 3) 
            part = 3;

        if (part == 0)      
            dx = -6.f;
        else if (part == 1) 
            dx = -3.f;
        else if (part == 2) 
            dx = 3.f;
        else                
            dx = 6.f;

        dy = -5.f;
    }

    ball.setPosition({ ballX, ballY });
}

void lifecount(int& lives, bool& isLaunched, float& ballX, float& ballY,float paddleX, float paddleY, float paddleW, float ballD, int windY,bool puActive[], int& wideTimer, int& slowTimer)
{
    if (ballY > windY)
    {
        if (lives > 0) 
            lives = lives - 1;

        ballX = paddleX + (paddleW - ballD) * 0.5f;
        ballY = paddleY - ballD;
        isLaunched = false;

        for (int i = 0; i < 8; i++) 
            puActive[i] = false;
        wideTimer = 0;
        slowTimer = 0;
    }
}

void checkWinLose(int& gameState, int lives, int bricks[][10])
{
    if (gameState != 1) 
        return;

    if (lives <= 0)
    {
        gameState = 2;
        clearSave();
        return;
    }

    bool allGone = true;
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 10; c++)
        {
            if (bricks[r][c] > 0) 
            { 
                allGone = false; 
                break; 
            }
        }
        if (allGone == false) 
            break;
    }
    if (allGone == true)
    {
        gameState = 3;
        clearSave();
    }
}

void updatePowerUps(bool puActive[], float puX[], float puY[], int puType[],float paddleX, float paddleY, float paddleH,float& paddleW, int& lives, int& wideTimer, int& slowTimer)
{
    for (int i = 0; i < 8; i++)
    {
        if (!puActive[i]) 
            continue;

        puY[i] = puY[i] + 3.f;

        bool caught = (puX[i] + 20.f > paddleX) &&
            (puX[i] < paddleX + paddleW) &&
            (puY[i] + 20.f > paddleY) &&
            (puY[i] < paddleY + paddleH);

        if (caught)
        {
            if (puType[i] == 1)      
                wideTimer = 600;
            else if (puType[i] == 2) 
                lives = lives + 1;
            else if (puType[i] == 3) 
                slowTimer = 600;

            puActive[i] = false;
            continue;
        }

        if (puY[i] > 700.f) 
            puActive[i] = false;
    }

    if (wideTimer > 0) 
        wideTimer = wideTimer - 1;
    if (slowTimer > 0) 
        slowTimer = slowTimer - 1;

    if (wideTimer > 0) 
        paddleW = 150.f;
    else               
        paddleW = 100.f;
}

void showScore(RenderWindow& window, int score, sf::Font& font, int lives)
{
    sf::RectangleShape strip({ 600.f, 50.f });
    strip.setFillColor(sf::Color(10, 10, 30));
    window.draw(strip);

    sf::Text scoreText(font);
    scoreText.setString("SCORE  " + std::to_string(score));
    scoreText.setCharacterSize(22);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({ 20.f, 12.f });
    window.draw(scoreText);

    sf::Text livesText(font);
    livesText.setString("LIVES  " + std::to_string(lives));
    livesText.setCharacterSize(22);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition({ 460.f, 12.f });
    window.draw(livesText);
}

void drawPowerUps(RenderWindow& window, bool puActive[], float puX[], float puY[], int puType[], sf::Font& font)
{
    for (int i = 0; i < 8; i++)
    {
        if (!puActive[i]) 
            continue;

        sf::Color col;
        string label;
        if (puType[i] == 1) { col = sf::Color(80, 180, 255);  label = "W"; }
        else if (puType[i] == 2) { col = sf::Color(80, 255, 120);  label = "+"; }
        else { col = sf::Color(255, 160, 60);  label = "S"; }

        sf::RectangleShape r({ 20.f, 20.f });
        r.setFillColor(col);
        r.setPosition({ puX[i], puY[i] });
        window.draw(r);

        sf::Text t(font);
        t.setString(label);
        t.setCharacterSize(16);
        t.setFillColor(sf::Color::Black);
        t.setPosition({ puX[i] + 6.f, puY[i] + 1.f });
        window.draw(t);
    }
}

void drawMenu(RenderWindow& window, int menuChoice, string menuMessage, sf::Font& font)
{
    sf::RectangleShape bg({ 600.f, 700.f });
    bg.setFillColor(sf::Color(15, 15, 40));
    window.draw(bg);

    sf::Text title(font);
    title.setString("BLOCK BUSTER");
    title.setCharacterSize(48);
    title.setStyle(sf::Text::Bold);
    title.setFillColor(sf::Color(100, 210, 255));
    title.setPosition({ 110.f, 130.f });
    window.draw(title);

    string labels[3] = { "New Game", "Continue", "Exit" };
    for (int i = 0; i < 3; i++)
    {
        sf::Text t(font);
        t.setString(labels[i]);
        t.setCharacterSize(32);
        if (i == menuChoice) t.setFillColor(sf::Color(100, 255, 150));
        else                 t.setFillColor(sf::Color(200, 200, 220));
        t.setPosition({ 210.f, 300.f + i * 60.f });
        window.draw(t);
    }

    if (menuMessage != "")
    {
        sf::Text m(font);
        m.setString(menuMessage);
        m.setCharacterSize(20);
        m.setFillColor(sf::Color(255, 200, 100));
        m.setPosition({ 200.f, 540.f });
        window.draw(m);
    }

    sf::Text hint(font);
    hint.setString("Up / Down to move    Enter to select");
    hint.setCharacterSize(18);
    hint.setFillColor(sf::Color(140, 140, 170));
    hint.setPosition({ 100.f, 640.f });
    window.draw(hint);
}

void drawEndScreen(RenderWindow& window, int gameState, int score, sf::Font& font)
{
    bool win = (gameState == 3);

    sf::RectangleShape bg({ 600.f, 700.f });
    if (win) bg.setFillColor(sf::Color(10, 40, 10));
    else     bg.setFillColor(sf::Color(45, 10, 10));
    window.draw(bg);

    sf::Text title(font);
    if (win) title.setString("YOU WIN!");
    else     title.setString("GAME OVER");
    title.setCharacterSize(56);
    title.setStyle(sf::Text::Bold);
    if (win) title.setFillColor(sf::Color(100, 255, 150));
    else     title.setFillColor(sf::Color(255, 100, 100));
    title.setPosition({ 150.f, 240.f });
    window.draw(title);

    sf::Text sc(font);
    sc.setString("Final Score: " + std::to_string(score));
    sc.setCharacterSize(30);
    sc.setFillColor(sf::Color::White);
    sc.setPosition({ 180.f, 380.f });
    window.draw(sc);

    sf::Text p(font);
    p.setString("Press Enter to return to menu");
    p.setCharacterSize(22);
    p.setFillColor(sf::Color(200, 200, 220));
    p.setPosition({ 140.f, 490.f });
    window.draw(p);
}

void renderGame(RenderWindow& window, int gameState, int menuChoice, string menuMessage,int bricks[][10], int rows, int cols,RectangleShape& paddle, CircleShape& ball,int score, int lives, sf::Font& font, bool isLaunched,bool puActive[], float puX[], float puY[], int puType[])
{
    window.clear(Color(15, 15, 40));

    if (gameState == 0)
    {
        drawMenu(window, menuChoice, menuMessage, font);
    }
    else if (gameState == 1)
    {
        drawBricks(window, bricks, rows, cols);
        window.draw(paddle);
        window.draw(ball);
        drawPowerUps(window, puActive, puX, puY, puType, font);
        showScore(window, score, font, lives);

        if (isLaunched == false)
        {
            sf::Text h(font);
            h.setString("Press Space to launch");
            h.setCharacterSize(20);
            h.setFillColor(sf::Color(150, 200, 255));
            h.setPosition({ 175.f, 545.f });
            window.draw(h);
        }
    }
    else
    {
        drawEndScreen(window, gameState, score, font);
    }

    window.display();
}

int main()
{
    int windX = 600, windY = 700;

    RenderWindow window(
        VideoMode({ 600, 700 }),
        "Block Buster by bit hunters "
    );
    window.setFramerateLimit(60);

    srand(time(0));

    sf::Font font;
    if (!font.openFromFile("arial.ttf"))
    {
        font.openFromFile("C:/Windows/Fonts/arial.ttf");
    }

    int gameState = 0;
    int menuChoice = 0;
    string menuMessage = "";

    int bricks[8][10];
    int rows = 0, cols = 0;
    if (!loadLevel(bricks, rows, cols))
    {
        menuMessage = "level.txt not found";
    }

    // paddle
    float paddleX = 250.f, paddleY = 650.f;
    float paddleW = 100.f, paddleH = 14.f;
    RectangleShape paddle({ paddleW, paddleH });
    paddle.setPosition({ paddleX, paddleY });
    paddle.setFillColor(Color::Cyan);

    // ball
    float ballD = 12.f;
    float ballX = paddleX + (paddleW - ballD) * 0.5f;
    float ballY = paddleY - ballD;
    CircleShape ball(6.f);
    ball.setPosition({ ballX, ballY });
    ball.setFillColor(Color::White);

    bool isLaunched = false;
    float dx = 0.f, dy = 0.f;
    int score = 0;
    int lives = 3;

    bool puActive[8];
    float puX[8], puY[8];
    int puType[8];
    for (int i = 0; i < 8; i++) 
        puActive[i] = false;
    int wideTimer = 0, slowTimer = 0;

    while (window.isOpen())
    {
        handleEvents(window, gameState, menuChoice, menuMessage,isLaunched, dx, dy, score, lives, bricks, rows, cols,paddleX, paddleW, ballX, ballY,puActive, wideTimer, slowTimer);

        if (gameState == 1)
        {
            updatePaddle(paddleX, paddleY, paddleW, windX, paddle);
            updatePowerUps(puActive, puX, puY, puType,paddleX, paddleY, paddleH, paddleW, lives, wideTimer, slowTimer);
            updateBall(isLaunched, ballX, ballY, ballD,paddleX, paddleY, paddleW, paddleH,windX, windY, dx, dy, ball,rows, cols, bricks, score,puActive, puX, puY, puType, slowTimer);
            lifecount(lives, isLaunched, ballX, ballY,paddleX, paddleY, paddleW, ballD, windY,puActive, wideTimer, slowTimer);
            checkWinLose(gameState, lives, bricks);
        }

        // Keep paddle shape in sync with current paddleW every frame
        if (paddleX > windX - paddleW) 
            paddleX = windX - paddleW;
        if (paddleX < 0.f) 
            paddleX = 0.f;
        paddle.setSize({ paddleW, paddleH });
        paddle.setPosition({ paddleX, paddleY });

        renderGame(window, gameState, menuChoice, menuMessage,bricks, rows, cols, paddle, ball,score, lives, font, isLaunched,puActive, puX, puY, puType);
    }

    return 0;
}