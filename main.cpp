#include <iostream>
#include <raylib.h>

using namespace std;

Color Green = Color{38, 185, 154, 255};
Color Dark_Green = Color{20, 160, 133, 255};
Color Light_Green = Color{129, 204, 184, 255};
Color Yellow = Color{243, 213, 91, 255};

int CPU_score = 0;
int player1_score = 0;

class Ball // creates the ball of the game
{
public:
    float x, y;         // coordinates of the ball when its moving through the board
    int speedX, speedY; // speed of ball when moving through the board
    int radius;         // radius of the ball
    void Draw()
    {
        DrawCircle(x, y, radius, Yellow); // creates the yellow ball with specific coordinates and radius length in pixels
    };

    void Update()
    { // updates ball's position by defined speed in pixels
        x += speedX;
        y += speedY;
        // this if statement changes direction of the ball's movement if it collides with the rectangle that delimits the board
        if (((y + radius) >= GetScreenHeight()) || ((y - radius) <= 0))
        {
            speedY *= -1;
        }
        // if the ball makes it past either of the paddles and reaches the left and right boundaries of the game,
        // a point will be added to the team that scored and the ball will reset in the middle for a new round.
        if (((x + radius) >= GetScreenWidth()))
        {

            player1_score++;
            ResetBall();
        }
        if ((x - radius) <= 0)
        {
            CPU_score++;
            ResetBall();
        }
    }

    void ResetBall()
    {
        // after  either player scores, the ball resets back to the middle and it direction of speed in both horizontal and vertical positions is randomized
        y = GetScreenHeight() / 2;
        x = GetScreenWidth() / 2;
        int speed_choices[2] = {-1, 1}; // possible directions of speed(negative is to the left and positive is to the right)
        speedX *= speed_choices[GetRandomValue(0, 1)];
        speedY *= speed_choices[GetRandomValue(0, 1)];
    }
};

class Paddle // creates the paddle the player will play with
{
protected:
    void LimitMovement() // limits movement of the paddles so that they stay inside the board during the game
    {
        if (y <= 0)
        { // if paddle moves below screen(0), it's moved back into the screen at the bottom
            y = 0;
        }
        if (y + height >= GetScreenHeight())
        { // If the top of the paddle leaves the board(the top is the position of the rectangle plus the rectangles height), the paddle will be moved back into the board at the top of the creen
            y = GetScreenHeight() - height;
        }
    }

public:
    float x, y;          // position of paddle on the board
    float width, height; // details size of the paddle
    int speed;           // the paddle only moves in the y direction(up and down);

    void Draw()
    {
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, WHITE); // gives the paddles rounded edges
    }

    void Update()
    {
        if (IsKeyDown(KEY_UP)) // if user presses key up, the paddle moves up at defined speed
        {
            y = y - speed;
        }
        if (IsKeyDown(KEY_DOWN)) // if user press key down, the paddle moves down at defined speed
        {
            y = y + speed;
        }

        LimitMovement(); // checks to make sure paddle stays inside board limits
    }
};

class CpuPaddle : public Paddle
{ // CpuPaddle class will inherit the Paddle class
public:
    /*this update method allows the CPU paddle to track the ball's ongoing movement
  and adjust the position of the paddle accordingly so that when the ball comes to the cpu
  paddle, the CPU is able to hit the ball back to the player. The CPU paddle will also stay within the boards limits
  throughout its entire movement  */
    void Update(int ballY)
    { // the vertical position of the ball is passed in through a parameter
        if (y + height / 2 > ballY)
        {
            y = y - speed;
        }
        if (y + height / 2 <= ballY)
        {
            y = y + speed;
        }

        LimitMovement(); // checks to make sure paddle stays inside the board limits
    }
};

Ball ball;
Paddle player; // player paddle will be on the left
CpuPaddle CPU; // cpu paddle will be on the right

int main()
{

    const int screen_width = 1280;
    const int screen_height = 800;
    InitWindow(screen_width, screen_height, "Pong Game");
    SetTargetFPS(60); // game runs at 60 fps

    ball.radius = 20;           // radius of ball
    ball.x = screen_width / 2;  // starting x coordinate of ball(in the middle of the board)
    ball.y = screen_height / 2; // starting y coordinate of ball(in the middle of the board)
    ball.speedX = 7;            // speed in horizontal direction of ball in pixels
    ball.speedY = 7;            // speed in vertical direction of ball in pixels

    player.width = 25;                                // width of player paddle
    player.height = 120;                              // height of player paddle
    player.speed = 6;                                 // speed in vertical direction of paddle in pixels(only moves up and down, hence the need for only one speed)
    player.x = 10;                                    // starting x coordinate of paddle, 10 pixels to the right of left boundary of board
    player.y = screen_height / 2 - player.height / 2; // starting y coordinate of paddle, the middle of the of the board

    CPU.height = 120;                           // height of CPU paddle
    CPU.width = 25;                             // width of CPU paddle
    CPU.speed = 4;                              // speed in vertical direction of paddle in pixels(only moves up and down, hence the need for only one speed)
    CPU.x = screen_width - CPU.width - 10;      // starting x coordinate of paddle, 10 pixels to the left of the right boundary of board
    CPU.y = screen_height / 2 - CPU.height / 2; // starting y coordinate of paddle, the middle of the of the board

    while (WindowShouldClose() == false)
    { // if the escape key or X(close button on top right of window) is pressed,
        // windowShouldClose() close will return true and the while loop will end(thus ending the game by closing the window)
        BeginDrawing();     // starts to draw the game board and its elements
        ball.Update();      // updates the ball movement on board by specified speed
        player.Update();    // updates the paddle movement on board
        CPU.Update(ball.y); // updates the CPU paddle's position based on the ball vertical position

        // checking for collisions between the 2 paddles and ball and changing horizontal speed of ball if a collision occurs
        // checking collision for player paddle
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height}))
        {
            ball.speedX *= -1;
        }
        // checking collision for CPU paddle
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{CPU.x, CPU.y, CPU.width, CPU.height}))
        {
            ball.speedX *= -1;
        }

        ClearBackground(Dark_Green);                                           // makes background color dark green removes any trace of the balls previous position, showing only shows it's current position
        DrawRectangle(0, 0, screen_width / 2, screen_height, Green);           // makes the background color of player's half side light green
        DrawCircle(screen_width / 2, screen_height / 2, 150, LIGHTGRAY);       // draws center circle where ball will start moving from
        DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE); // draw halfway line in middle of board in white
        ball.Draw();                                                           // draws ball into screen
        player.Draw();                                                         // draws player paddle
        CPU.Draw();
        DrawText(TextFormat("%i", player1_score), screen_width / 4 - 20, 20, 80, WHITE); // displays player score on board
        DrawText(TextFormat("%i", CPU_score), 3 * screen_width / 4 - 20, 20, 80, WHITE); // displays CPU score on board
        EndDrawing();
    }

    CloseWindow(); // closes the window
    return 0;
}
