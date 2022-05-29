#include "raylib.h"
#include "math.h"

class Input {
private:
    int keyA;
    int keyB;
public:
    Input() {
        keyA = KEY_A;
        keyB = KEY_B;
    }
    Input(int A, int B) {
        keyA = A;
        keyB = B;
    }
    int GetInput() {
        return (int)IsKeyDown(keyA) - (int)IsKeyDown(keyB);
    }
};

class Ball {
private:
    Vector2 position = Vector2{ 0, 0 };
    Vector2 velocity = Vector2{ 0, 0 };
    float radius = 10.0;
    Color color = WHITE;
public:
    Ball() { }
    Ball(Vector2 pos, float velX, float velY, float r) {
        position = pos;
        velocity.x = velX;
        velocity.y = velY;
        radius = r;
    }
    Vector2 GetPosition() { return position; }
    void SetPosition(Vector2 pos) { position = pos; }
    Vector2 GetVelocity() { return velocity; }
    void SetVelocity(Vector2 vel) { velocity = vel; }
    float GetRadius() { return radius; }
    void Draw() {
        Vector2 currentPosition = position;
        Vector2 currentVelocity = velocity;
        currentPosition.x += currentVelocity.x * GetFrameTime();
        currentPosition.y += currentVelocity.y * GetFrameTime();

        if (currentPosition.y < 5) {
            currentPosition.y = 5;
            currentVelocity.y *= -1.0;
        }
        if (currentPosition.y > GetScreenHeight() - 5) {
            currentPosition.y = GetScreenHeight() - 5;
            currentVelocity.y *= -1.0;
        }

        position = currentPosition;
        velocity = currentVelocity;
        DrawCircleV(position, radius, color);
    }
};

class Paddle {
private:
    Vector2 position = Vector2{ 0, 0 };
    Vector2 size = Vector2{ 1, 1 };
    Input input = Input{ KEY_A, KEY_B };
    Color color = WHITE;
    float speed = 1000.0;
    Ball ball;
public:
    Paddle() { }
    Paddle(Vector2 pos, Vector2 _size, Input _input) {
        position = pos;
        size = _size;
        input = _input;
    }
    Rectangle GetRect() {
        return Rectangle{ position.x, position.y, size.x, size.y };
    }
    Vector2 GetSize() { return size; }
    Vector2 GetPosition() { return position; }
    void SetPosition(Vector2 pos) { position = pos; }
    void Draw() {
        Vector2 currentPosition = position;
        currentPosition.y -= input.GetInput() * speed * GetFrameTime();

        if (currentPosition.y < 0) {
            currentPosition.y = 0;
        }
        if (currentPosition.y > GetScreenHeight() - size.y) {
            currentPosition.y = GetScreenHeight() - size.y;
        }

        position = currentPosition;
        DrawRectangleRec(GetRect(), color);
    }
};

class Game {
private:
    Input player1Input = Input(KEY_W, KEY_S);
    Input player2Input = Input(KEY_UP, KEY_DOWN);
    Vector2 playerLeftStartingPos = Vector2{ 50, (float)GetScreenHeight() / 2 - 125 };
    Vector2 playerRightStartingPos = Vector2{ (float)GetScreenWidth() - 20 - 50, (float)GetScreenHeight() / 2 - 125 };
    Vector2 ballStartingPos = Vector2{ (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 };
    Vector2 paddleSize = Vector2{ 20, 250 };
    float ballStartSpeed = 500.0;
    int scoreLeft, scoreRight;
    Paddle leftPaddle, rightPaddle;
    Ball ball;
    bool menu = true;
public:
    Game() {
        scoreLeft = 0;
        scoreRight = 0;
        ball = Ball(ballStartingPos, ballStartSpeed, ballStartSpeed, 10.0);
        leftPaddle = Paddle(playerLeftStartingPos, paddleSize, player1Input);
        rightPaddle = Paddle(playerRightStartingPos, paddleSize, player2Input);
    }
    void Reset(){
        //leftPaddle.SetPosition(playerLeftStartingPos);
        //rightPaddle.SetPosition(playerRightStartingPos);
        //TODO random start direction;
        //TODO Speed limit
        ball.SetPosition(ballStartingPos);
        ball.SetVelocity(Vector2{ ballStartSpeed, ballStartSpeed });
    }
    void Draw() {

        BeginDrawing();
        ClearBackground(BLACK);

        Vector2 ballPosition = ball.GetPosition();
        Vector2 ballVelocity = ball.GetVelocity();

        bool collisionLeft = CheckCollisionCircleRec(ballPosition, ball.GetRadius(), leftPaddle.GetRect());
        bool collisionRight = CheckCollisionCircleRec(ballPosition, ball.GetRadius(), rightPaddle.GetRect());
        if (collisionLeft) {
            if (ballVelocity.x < 0) {
                Vector2 newVelocity = Vector2{ ballVelocity.x * -1.1f, (ballPosition.y - (leftPaddle.GetPosition().y + (paddleSize.y / 2))) / (paddleSize.y / 2) * ballVelocity.x };
                ball.SetVelocity(newVelocity);
            }
        }
        if (collisionRight) {
            if (ballVelocity.x > 0) {
                Vector2 newVelocity = Vector2{ ballVelocity.x * -1.1f, (ballPosition.y - (rightPaddle.GetPosition().y + (paddleSize.y / 2))) / (paddleSize.y / 2) * -ballVelocity.x };
                ball.SetVelocity(newVelocity);
            }
        }
      
        if (ballPosition.x > GetScreenWidth()) {
            scoreLeft += 1;
            Reset();
        }
        if (ballPosition.x < 0) {
            scoreRight += 1;
            Reset();
        }

        if (!menu)
        {
            ball.Draw();
            leftPaddle.Draw();
            rightPaddle.Draw();
            DrawText(TextFormat("%1i : %1i", scoreLeft, scoreRight), GetScreenWidth() / 2, GetScreenHeight() / 2, 40, LIGHTGRAY);
            DrawText(TextFormat("%02.02f", (ballVelocity.x)), GetScreenWidth() / 2 - 100, GetScreenHeight() / 2 - 100, 40, LIGHTGRAY);
        } else {
            DrawText(TextFormat("Press Space to continue"), GetScreenWidth() / 2 - 200, GetScreenHeight() / 2, 40, LIGHTGRAY);
            DrawText(TextFormat("Press R to restart"), GetScreenWidth() / 2 - 200, GetScreenHeight() / 2 - 100, 40, LIGHTGRAY);
            if (IsKeyPressed(KEY_R)) {
                scoreLeft = 0;
                scoreRight = 0;
                Reset();
            }
        }

        if (IsKeyPressed(KEY_SPACE))
        {
            menu = !menu;
        }
        
        EndDrawing();
    }
};

int main()
{
    // Window init
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "Pong");
    SetTargetFPS(60);

    //Game init
    Game game;

    // Main game loop
    while (!WindowShouldClose())
    {
        game.Draw();
    }

    CloseWindow();
    return 0;
};