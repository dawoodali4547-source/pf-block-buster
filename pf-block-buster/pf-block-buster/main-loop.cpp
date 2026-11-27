#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <optional>
using namespace sf;
using namespace std;

// ============ Constants from spec ============
const float WINDOW_W = 600.f, WINDOW_H = 700.f;
const int   ROWS = 8, COLS = 10;
const float BRICK_W = 60.f, BRICK_H = 25.f, BRICKS_Y = 100.f;
const float PADDLE_W = 200.f, PADDLE_H = 14.f, PADDLE_Y = 650.f;
const float PADDLE_SPEED = 20.f;
const float BALL_R = 6.f, BALL_D = 12.f;
const int   HUD_H = 50;
const int   START_LIVES = 3;

// Power-up constants (bonus)
const int   MAX_POWERUPS = 8;
const int   PU_WIDE = 1, PU_LIFE = 2, PU_SLOW = 3;
const float PU_SPEED = 3.f;
const float PU_SIZE = 20.f;
const int   PU_DURATION = 600;

// Game states
const int MENU = 0, PLAYING = 1, GAME_OVER = 2, YOU_WIN = 3;
const int OPT_NEW = 0, OPT_CONT = 1, OPT_EXIT = 2;

// ============ Data ============
struct PowerUp {
    float x, y;
    int type;
    bool active;
};

struct Game {
    int bricks[ROWS][COLS];
    float paddleX, ballX, ballY;
    float paddleWidth;
    float dx, dy;
    float slowMul;
    bool isLaunched;
    int score, lives, state, menuChoice;
    string menuMessage;
    PowerUp powerups[MAX_POWERUPS];
    int wideTimer, slowTimer;
};

// ============ Helpers ============
bool loadLevel(int bricks[ROWS][COLS]) 
{
    ifstream fin("level.txt");
    if (!fin) 
        return false;
    int r, c;
    if (!(fin >> r >> c)) 
        return false;
    for (int i = 0; i < ROWS; i++)      //initializing 2d array bricks with 0
        for (int j = 0; j < COLS; j++) 
            bricks[i][j] = 0;
    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++)     //getting values from file & putting in array 
        {
            int v;
            if (!(fin >> v))            //if any value is missing in a file, it left the next part of file without reading
                return true;            // to protect it from reading garbage.
            if (i < ROWS && j < COLS) 
                bricks[i][j] = v;
        }
    return true;
}

bool anyBricksLeft(int b[ROWS][COLS]) 
{
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            if (b[i][j] > 0) 
                return true;
    return false;
}

void clearPowerUps(Game& g) 
{
    for (int i = 0; i < MAX_POWERUPS; i++) 
        g.powerups[i].active = false;

    g.wideTimer = 0;
    g.slowTimer = 0;
    g.paddleWidth = PADDLE_W;
    g.slowMul = 1.0f;
}

void resetBall(Game& g) 
{
    g.isLaunched = false;
    g.ballX = g.paddleX + (g.paddleWidth - BALL_D) * 0.5f;         //ball to centre of paddle x
    g.ballY = PADDLE_Y - BALL_D;                                   //ball to centre of paddle y
    g.dx = 0.f;
    g.dy = 0.f;
}

void startNewGame(Game& g) {
    g.score = 0;
    g.lives = START_LIVES;
    g.paddleX = 250.f;

    if (!loadLevel(g.bricks)) {              // if now file found
        g.state = MENU;
        g.menuMessage = "level.txt is missing";
        return;
    }

    clearPowerUps(g);
    resetBall(g);
    g.state = PLAYING;
    g.menuMessage = "";
}

bool saveGame(Game& g) {
    ofstream fout("savegame.txt");
    if (!fout)
        return false;
    fout << g.score << " " << g.lives << "\n";
    for (int i = 0; i < ROWS; i++) 
    {
        for (int j = 0; j < COLS; j++) 
        {
            fout << g.bricks[i][j];
            if (j < COLS - 1) 
                fout << " ";
        }
        fout << "\n";
    }
    return true;
}

bool loadSave(Game& g) {
    ifstream fin("savegame.txt");

    if (!fin) 
        return false;

    int s, l;

    if (!(fin >> s >> l)) 
        return false;

    int temp[ROWS][COLS];
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            if (!(fin >> temp[i][j])) 
                return false;
    g.score = s;
    g.lives = l;
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++) 
            g.bricks[i][j] = temp[i][j];

    g.paddleX = 250.f;
    clearPowerUps(g);
    resetBall(g);
    g.state = PLAYING;
    g.menuMessage = "";
    return true;
}

void clearSave() {
    ofstream f("savegame.txt"); //called when user win,lose
    //it clear the save file
}

int pointBrick(float cx, float cy, int b[ROWS][COLS]) 
{
    if (cx < 0 || cx >= WINDOW_W) 
        return -1;
    if (cy < BRICKS_Y || cy >= BRICKS_Y + ROWS * BRICK_H) 
        return -1;
    int col = (int)(cx / BRICK_W);
    int row = (int)((cy - BRICKS_Y) / BRICK_H);
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) 
        return -1;
    if (b[row][col] <= 0) 
        return -1;
    return row * COLS + col;
}

void spawnPowerUp(Game& g, float cx, float cy) 
{
    if (rand() % 100 >= 25) 
        return;
    for (int i = 0; i < MAX_POWERUPS; i++) 
    {
        if (!g.powerups[i].active) 
        {
            g.powerups[i].active = true;
            g.powerups[i].x = cx - PU_SIZE * 0.5f;
            g.powerups[i].y = cy - PU_SIZE * 0.5f;
            g.powerups[i].type = PU_WIDE + (rand() % 3);
            return;
        }
    }
}

bool damageCorners(Game& g) 
{
    float cx[4];
    float cy[4];
    cx[0] = g.ballX;            cy[0] = g.ballY;               //four edges of ball (x,y)
    cx[1] = g.ballX + BALL_D;   cy[1] = g.ballY;
    cx[2] = g.ballX + BALL_D;   cy[2] = g.ballY + BALL_D;
    cx[3] = g.ballX;            cy[3] = g.ballY + BALL_D;

    int ids[4] = { -1, -1, -1, -1 };
    int n = 0;
    bool any = false;

    for (int i = 0; i < 4; i++) 
    {
        int id = pointBrick(cx[i], cy[i], g.bricks);
        if (id < 0) 
            continue;
        any = true;

        bool dup = false;
        for (int k = 0; k < n; k++) 
        {
            if (ids[k] == id) 
            { 
                dup = true; 
                break; 
            }
        }
        if (dup) 
            continue;

        ids[n] = id;
        n = n + 1;

        int r = id / COLS;
        int c = id % COLS;
        g.bricks[r][c] = g.bricks[r][c] - 1;

        if (g.bricks[r][c] == 0) 
        {
            g.score = g.score + 50;
            spawnPowerUp(g,c * BRICK_W + BRICK_W * 0.5f,BRICKS_Y + r * BRICK_H + BRICK_H * 0.5f);
        }
        else 
        {
            g.score = g.score + 10;
        }
    }
    return any;
}

// ============ Input ============
void handleInput(RenderWindow& window, Game& g) {
    optional<Event> opt = window.pollEvent();
    while (opt) {
        Event ev = *opt;   // copy event, 

        if (ev.is<Event::Closed>()) {
            window.close();
            return;
        }
        if (ev.is<Event::KeyPressed>()) {
            
            Keyboard::Key code = ev.getIf<Event::KeyPressed>()->code;

            if (g.state == MENU) {
                if (code == Keyboard::Key::Up)   g.menuChoice = (g.menuChoice + 2) % 3;
                if (code == Keyboard::Key::Down) g.menuChoice = (g.menuChoice + 1) % 3;
                if (code == Keyboard::Key::Enter) {
                    if (g.menuChoice == OPT_NEW) {
                        startNewGame(g);
                    }
                    else if (g.menuChoice == OPT_CONT) {
                        if (!loadSave(g)) g.menuMessage = "No saved game";
                    }
                    else if (g.menuChoice == OPT_EXIT) {
                        window.close();
                        return;
                    }
                }
            }
            else if (g.state == PLAYING) {
                if (code == Keyboard::Key::Space) {
                    if (!g.isLaunched) {
                        g.isLaunched = true;
                        g.dy = -5.f;
                        if (rand() % 2 == 0) g.dx = -3.f;
                        else                 g.dx = 3.f;
                    }
                }
                if (code == Keyboard::Key::Escape) {
                    saveGame(g);
                    g.state = MENU;
                    g.menuChoice = 0;
                    g.menuMessage = "Game saved";
                }
            }
            else {
                if (code == Keyboard::Key::Enter) {
                    clearSave();
                    g.state = MENU;
                    g.menuChoice = 0;
                    g.menuMessage = "";
                }
            }
        }

        opt = window.pollEvent();
    }
}

// ============ Update ============
void updatePaddle(Game& g) {
    if (Keyboard::isKeyPressed(Keyboard::Key::Left) ||
        Keyboard::isKeyPressed(Keyboard::Key::A))
        g.paddleX = g.paddleX - PADDLE_SPEED;

    if (Keyboard::isKeyPressed(Keyboard::Key::Right) ||
        Keyboard::isKeyPressed(Keyboard::Key::D))
        g.paddleX = g.paddleX + PADDLE_SPEED;

    if (g.paddleX < 0.f) g.paddleX = 0.f;
    if (g.paddleX > WINDOW_W - g.paddleWidth)
        g.paddleX = WINDOW_W - g.paddleWidth;
}

void updateBall(Game& g) {
    if (!g.isLaunched) {
        g.ballX = g.paddleX + (g.paddleWidth - BALL_D) * 0.5f;
        g.ballY = PADDLE_Y - BALL_D;
        return;
    }

    float stepX = g.dx * g.slowMul;
    g.ballX = g.ballX + stepX;
    if (g.ballX < 0.f) {
        g.ballX = g.ballX - stepX;
        g.dx = -g.dx;
    }
    if (g.ballX + BALL_D > WINDOW_W) {
        g.ballX = g.ballX - stepX;
        g.dx = -g.dx;
    }
    if (damageCorners(g)) {
        g.ballX = g.ballX - stepX;
        g.dx = -g.dx;
    }

    float stepY = g.dy * g.slowMul;
    g.ballY = g.ballY + stepY;
    if (g.ballY < HUD_H) {
        g.ballY = g.ballY - stepY;
        g.dy = -g.dy;
    }
    if (damageCorners(g)) {
        g.ballY = g.ballY - stepY;
        g.dy = -g.dy;
    }

    bool touch = (g.ballX < g.paddleX + g.paddleWidth) &&
        (g.ballX + BALL_D > g.paddleX) &&
        (g.ballY < PADDLE_Y + PADDLE_H) &&
        (g.ballY + BALL_D > PADDLE_Y);

    if (touch && g.dy > 0.f) {
        g.ballY = PADDLE_Y - BALL_D;
        float center = g.ballX + BALL_R;
        int part = (int)((center - g.paddleX) * 4.f / g.paddleWidth);
        if (part < 0) part = 0;
        if (part > 3) part = 3;

        if (part == 0)      g.dx = -6.f;
        else if (part == 1) g.dx = -3.f;
        else if (part == 2) g.dx = 3.f;
        else                g.dx = 6.f;

        g.dy = -5.f;
    }

    if (g.ballY > WINDOW_H) {
        g.lives = g.lives - 1;
        if (g.lives <= 0) {
            g.lives = 0;
            g.state = GAME_OVER;
            clearSave();
        }
        else {
            clearPowerUps(g);
            resetBall(g);
        }
    }

    if (g.state == PLAYING && !anyBricksLeft(g.bricks)) {
        g.state = YOU_WIN;
        clearSave();
    }
}

void updatePowerUps(Game& g) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!g.powerups[i].active) continue;

        g.powerups[i].y = g.powerups[i].y + PU_SPEED;

        bool caught = (g.powerups[i].x + PU_SIZE > g.paddleX) &&
            (g.powerups[i].x < g.paddleX + g.paddleWidth) &&
            (g.powerups[i].y + PU_SIZE > PADDLE_Y) &&
            (g.powerups[i].y < PADDLE_Y + PADDLE_H);

        if (caught) {
            if (g.powerups[i].type == PU_WIDE) g.wideTimer = PU_DURATION;
            else if (g.powerups[i].type == PU_LIFE) g.lives = g.lives + 1;
            else if (g.powerups[i].type == PU_SLOW) g.slowTimer = PU_DURATION;
            g.powerups[i].active = false;
            continue;
        }

        if (g.powerups[i].y > WINDOW_H) g.powerups[i].active = false;
    }

    if (g.wideTimer > 0) g.wideTimer = g.wideTimer - 1;
    if (g.slowTimer > 0) g.slowTimer = g.slowTimer - 1;

    if (g.wideTimer > 0) g.paddleWidth = PADDLE_W + 50.f;
    else                 g.paddleWidth = PADDLE_W;

    if (g.slowTimer > 0) g.slowMul = 0.5f;
    else                 g.slowMul = 1.0f;
}

void updateGame(Game& g) {
    if (g.state != PLAYING) return;
    updatePaddle(g);
    updateBall(g);
    updatePowerUps(g);
}

// ============ Draw ============
void centerText(Text& t, float x, float y) {
    FloatRect b = t.getLocalBounds();
    t.setOrigin({ b.size.x / 2.f + b.position.x,
                  b.size.y / 2.f + b.position.y });
    t.setPosition({ x, y });
}

void drawBricks(RenderWindow& w, int b[ROWS][COLS]) {
    Color colors[4];
    colors[0] = Color(0, 0, 0);
    colors[1] = Color::Green;
    colors[2] = Color::Yellow;
    colors[3] = Color::Red;

    RectangleShape brick({ BRICK_W - 2.f, BRICK_H - 2.f });
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (b[r][c] <= 0) continue;
            brick.setFillColor(colors[b[r][c]]);
            brick.setPosition({ c * BRICK_W + 1.f, BRICKS_Y + r * BRICK_H + 1.f });
            w.draw(brick);
        }
    }
}

void drawPowerUps(RenderWindow& w, Game& g, Font& font) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!g.powerups[i].active) continue;

        Color col;
        string label;
        if (g.powerups[i].type == PU_WIDE) { col = Color(80, 180, 255);  label = "W"; }
        else if (g.powerups[i].type == PU_LIFE) { col = Color(80, 255, 120);  label = "+"; }
        else { col = Color(255, 160, 60);  label = "S"; }

        RectangleShape r({ PU_SIZE, PU_SIZE });
        r.setFillColor(col);
        r.setPosition({ g.powerups[i].x, g.powerups[i].y });
        w.draw(r);

        Text t(font, label, 16);
        t.setFillColor(Color::Black);
        centerText(t, g.powerups[i].x + PU_SIZE * 0.5f,
            g.powerups[i].y + PU_SIZE * 0.5f);
        w.draw(t);
    }
}

void drawHUD(RenderWindow& w, Game& g, Font& font) {
    RectangleShape strip({ WINDOW_W, (float)HUD_H });
    strip.setFillColor(Color(10, 10, 30));
    w.draw(strip);

    Text score(font, "SCORE  " + to_string(g.score), 22);
    score.setFillColor(Color::White);
    score.setPosition({ 15, 12 });
    w.draw(score);

    Text lives(font, "LIVES", 22);
    lives.setFillColor(Color::White);
    lives.setPosition({ WINDOW_W - 130, 12 });
    w.draw(lives);

    for (int i = 0; i < g.lives && i < 6; i++) {
        CircleShape dot(7.f);
        dot.setFillColor(Color::White);
        dot.setPosition({ WINDOW_W - 55 + i * 18.f, 18.f });
        w.draw(dot);
    }

    float bx = 15.f;
    if (g.wideTimer > 0) {
        Text t(font, "WIDE " + to_string(g.wideTimer / 60 + 1), 16);
        t.setFillColor(Color(80, 180, 255));
        t.setPosition({ bx, 26 });
        w.draw(t);
        bx = bx + 80.f;
    }
    if (g.slowTimer > 0) {
        Text t(font, "SLOW " + to_string(g.slowTimer / 60 + 1), 16);
        t.setFillColor(Color(255, 160, 60));
        t.setPosition({ bx, 26 });
        w.draw(t);
    }
}

void drawMenu(RenderWindow& w, Game& g, Font& font) {
    RectangleShape bg({ WINDOW_W, WINDOW_H });
    bg.setFillColor(Color(15, 15, 40));
    w.draw(bg);

    Text title(font, "BLOCK BUSTER", 48);
    title.setStyle(Text::Bold);
    title.setFillColor(Color(100, 210, 255));
    centerText(title, WINDOW_W / 2.f, 150.f);
    w.draw(title);

    string labels[3];
    labels[0] = "New Game";
    labels[1] = "Continue";
    labels[2] = "Exit";

    for (int i = 0; i < 3; i++) {
        Text t(font, labels[i], 32);
        if (i == g.menuChoice) t.setFillColor(Color(100, 255, 150));
        else                   t.setFillColor(Color(200, 200, 220));
        centerText(t, WINDOW_W / 2.f, 320.f + i * 60.f);
        w.draw(t);

        if (i == g.menuChoice) {
            CircleShape dot(6.f);
            dot.setFillColor(Color(100, 255, 150));
            dot.setPosition({ WINDOW_W / 2.f - 130, 320.f + i * 60.f - 14 });
            w.draw(dot);
        }
    }

    if (g.menuMessage != "") {
        Text m(font, g.menuMessage, 20);
        m.setFillColor(Color(255, 200, 100));
        centerText(m, WINDOW_W / 2.f, 560.f);
        w.draw(m);
    }

    Text hint(font, "Up / Down to move    Enter to select", 18);
    hint.setFillColor(Color(140, 140, 170));
    centerText(hint, WINDOW_W / 2.f, 660.f);
    w.draw(hint);
}

void drawEndScreen(RenderWindow& w, Game& g, Font& font) {
    bool win = (g.state == YOU_WIN);

    RectangleShape bg({ WINDOW_W, WINDOW_H });
    if (win) bg.setFillColor(Color(10, 40, 10));
    else     bg.setFillColor(Color(45, 10, 10));
    w.draw(bg);

    Text title(font, win ? "YOU WIN!" : "GAME OVER", 56);
    title.setStyle(Text::Bold);
    if (win) title.setFillColor(Color(100, 255, 150));
    else     title.setFillColor(Color(255, 100, 100));
    centerText(title, WINDOW_W / 2.f, 260.f);
    w.draw(title);

    Text sc(font, "Final Score: " + to_string(g.score), 30);
    sc.setFillColor(Color::White);
    centerText(sc, WINDOW_W / 2.f, 390.f);
    w.draw(sc);

    Text p(font, "Press Enter to return to menu", 22);
    p.setFillColor(Color(200, 200, 220));
    centerText(p, WINDOW_W / 2.f, 500.f);
    w.draw(p);
}

void drawGame(RenderWindow& w, Game& g, Font& font) {
    w.clear(Color(15, 15, 40));

    if (g.state == MENU) {
        drawMenu(w, g, font);
    }
    else if (g.state == PLAYING) {
        drawBricks(w, g.bricks);

        RectangleShape paddle({ g.paddleWidth, PADDLE_H });
        paddle.setFillColor(Color(80, 200, 255));
        paddle.setPosition({ g.paddleX, PADDLE_Y });
        w.draw(paddle);

        CircleShape ball(BALL_R);
        ball.setFillColor(Color::White);
        ball.setPosition({ g.ballX, g.ballY });
        w.draw(ball);

        drawPowerUps(w, g, font);
        drawHUD(w, g, font);

        if (!g.isLaunched) {
            Text h(font, "Press Space to launch", 20);
            h.setFillColor(Color(150, 200, 255));
            centerText(h, WINDOW_W / 2.f, 550.f);
            w.draw(h);
        }
    }
    else {
        drawEndScreen(w, g, font);
    }

    w.display();
}

// ============ Main ============
int main() {
    srand((unsigned)time(0));

    RenderWindow window(VideoMode({ (unsigned)WINDOW_W, (unsigned)WINDOW_H }), "Block Buster by bit hunters");
    window.setFramerateLimit(60);

    // Font loading 
    Font font;
    bool fontOk = font.openFromFile("C:/Windows/Fonts/arial.ttf");
    

    Game g{};
    g.state = MENU;
    g.menuChoice = OPT_NEW;
    g.paddleX = 250.f;
    g.lives = START_LIVES;
    clearPowerUps(g);
    resetBall(g);

    while (window.isOpen()) {
        handleInput(window, g);
        updateGame(g);
        drawGame(window, g, font);
    }
    return 0;
}