/* --------------------------------------------------------
 *    File: BreakoutGame.cpp
 *  Author: Justin Rubio
 *   Class: Computer Security
 * Purpose: The classic breakout desktop game (improved)
 * -------------------------------------------------------- */

#include <SFML/Graphics.hpp>
#include <cmath>                //  for trig/geometry/linear functions
#include "breakout_defs.h"
#include <chrono>
#include <iostream>

using namespace std::chrono;

// Function declarations
// --------------------------------------------------------

void setup(Ball &refBall, Borders &refBorder, MovingBlock &refPaddle, Brick bricks[BRICK_ROWS][BRICK_COLUMNS]);
Direction processInput() ;

bool update(Direction &input, Ball &ball, float delta, Borders walls,
            MovingBlock &paddle, bool &started, Brick bricks[BRICK_ROWS][BRICK_COLUMNS]);

void render(sf::RenderWindow &window, Ball &ball, float delta, Borders walls, MovingBlock paddle,
            Brick bricks[BRICK_ROWS][BRICK_COLUMNS]);

int getCollisionPoint(Ball *pBall, Block *pBlock);
bool checkBlockCollision(Block moving, Block stationary);
bool collisionCheck(Ball *pBall, Block *pBlock);
bool doCollisionChecks (Ball &ball, MovingBlock &paddle, Borders walls, Brick bricks[BRICK_ROWS][BRICK_COLUMNS]);

//-----------------------------------------------------------


/**
 * The main application
 * @return OS status message (0=Success)
 */
int main() {

    // render a 2d graphics window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Break Out!");
    window.clear(WINDOW_COLOR);

    // declarations
    Borders walls;
    Ball theBall;
    MovingBlock paddle;
    Brick bricks[BRICK_ROWS][BRICK_COLUMNS];

    // set up the game components
    // ------------------------------------------------
    setup(theBall, walls, paddle, bricks);

    // time variables for the main game loop
    sf::Clock clock;
    sf::Time startTime = clock.getElapsedTime();
    sf::Time stopTime = startTime;
    float delta = 0.0;

    // tracks how long game was running for
    auto start = high_resolution_clock::now();

    bool started = false;
    bool gameOver = false;
    while (!gameOver)
    {
        // calculates the frame time
        stopTime = clock.getElapsedTime();
        delta += (stopTime.asMilliseconds() - startTime.asMilliseconds());
        startTime = stopTime;

        // process events
        sf::Event event;
        while (!gameOver && window.pollEvent(event)) {

            if (event.type == sf::Event::Closed) //  closes the window
                gameOver = true;
        }

        // Process user input
        // ------------------------------------------------
        Direction userInput = processInput();
        if (userInput == Exit)
            gameOver = true;

        // Process Updates
        // ------------------------------------------------
        if (delta >= FRAME_RATE) {

            if (!paused)
                gameOver = update(userInput, theBall, delta, walls, paddle, started, bricks);

            // subtract the frame-rate from the current frame-time for each full frame covered by this update
            while (delta >= FRAME_RATE)
                delta -= FRAME_RATE;
        }

        // Render the window
        // ------------------------------------------------
        render(window, theBall, delta, walls, paddle, bricks);

    } // end main game loop

    //get time when game ended
    auto stop = high_resolution_clock::now();

    // get difference from stop - start = timeElapsed
    auto timeElapsed = duration_cast<seconds>(stop - start);

    std::cout<<"\nTime Elapsed: "<< timeElapsed.count()<< " seconds\n";

    // close graphics window
    window.close();

    return 0;
} // end main


/**
 * Initializes game window and blocks
 * @param refBall = rendered ball
 * @param refBorder = reference to borders to render
 * @param refPaddle = reference to paddle to render
 * @param bricks = array of bricks to render
 */
void setup(Ball &refBall, Borders &refBorder, MovingBlock &refPaddle, Brick bricks[BRICK_ROWS][BRICK_COLUMNS]){

    // paddle
    refPaddle.block.left = (WINDOW_WIDTH - PADDLE_WIDTH) / 2.0;
    refPaddle.block.top = WINDOW_HEIGHT - (2.0 * PADDLE_THICKNESS);
    refPaddle.block.width = PADDLE_WIDTH;
    refPaddle.block.height = PADDLE_THICKNESS;
    refPaddle.block.color = PADDLE_COLOR;
    refPaddle.block.rectangle.setSize(sf::Vector2(PADDLE_WIDTH, PADDLE_THICKNESS));
    refPaddle.block.rectangle.setFillColor(PADDLE_COLOR);
    refPaddle.velocityX = 0.0;
    refPaddle.velocityY = 0.0;


    // walls
    //------------------------------------------------------
    // left border
    refBorder.leftBlock.left = 0.0;
    refBorder.leftBlock.top = 0.0;
    refBorder.leftBlock.width = WALL_THICKNESS;
    refBorder.leftBlock.height = WINDOW_HEIGHT;
    refBorder.leftBlock.rectangle.setSize(sf::Vector2(refBorder.leftBlock.width,refBorder.leftBlock.height));
    refBorder.leftBlock.rectangle.setPosition(refBorder.leftBlock.left, refBorder.leftBlock.top);
    refBorder.leftBlock.rectangle.setFillColor(WALL_COLOR);

    // top border
    refBorder.topBlock.left = 0.0;
    refBorder.topBlock.top = 0.0;
    refBorder.topBlock.width = WINDOW_WIDTH;
    refBorder.topBlock.height = WALL_THICKNESS;
    refBorder.topBlock.rectangle.setSize(sf::Vector2(refBorder.topBlock.width,refBorder.topBlock.height));
    refBorder.topBlock.rectangle.setPosition(refBorder.topBlock.left, refBorder.topBlock.top);
    refBorder.topBlock.rectangle.setFillColor(WALL_COLOR);

    // right border
    refBorder.rightBlock.left = WINDOW_WIDTH - WALL_THICKNESS;
    refBorder.rightBlock.top = 0.0;
    refBorder.rightBlock.width = WALL_THICKNESS;
    refBorder.rightBlock.height = WINDOW_HEIGHT;
    refBorder.rightBlock.rectangle.setSize(sf::Vector2(refBorder.rightBlock.width,refBorder.rightBlock.height));
    refBorder.rightBlock.rectangle.setPosition(refBorder.rightBlock.left, refBorder.rightBlock.top);
    refBorder.rightBlock.rectangle.setFillColor(WALL_COLOR);

    // bottom border
    refBorder.bottomBlock.left = 0.0;
    refBorder.bottomBlock.top = WINDOW_HEIGHT - WALL_THICKNESS;
    refBorder.bottomBlock.width = WINDOW_WIDTH;
    refBorder.bottomBlock.height = WALL_THICKNESS;
    refBorder.bottomBlock.rectangle.setSize(sf::Vector2(refBorder.bottomBlock.width,refBorder.bottomBlock.height));
    refBorder.bottomBlock.rectangle.setPosition(refBorder.bottomBlock.left, refBorder.bottomBlock.top);
    refBorder.bottomBlock.rectangle.setFillColor(WALL_COLOR);
    //--------------------------------------------------------
    // end of borders


    // the ball
    refBall.radius = BALL_RADIUS;
    refBall.coordinateX = refPaddle.block.left + (PADDLE_WIDTH / 2.0);
    refBall.coordinateY = refPaddle.block.top - BALL_RADIUS - 1;
    refBall.velocityX = 0.0;
    refBall.velocityY = 0.0;
    refBall.color = BALL_COLOR;


    // Bricks setup
    float bricksTop = FIRST_BRICK; // start at lowest brick row

    Brick *pNextBrick = &bricks[0][0]; // pointer to first brick

    for (int row = 0; row < BRICK_ROWS; row++)  // for ROWS
    {

        float bricksLeft = BRICKS_LEFT; // far left bricks

        for (int column = 0; column < BRICK_COLUMNS; column++)  // for COLUMNS
        {

            // offset left and top add by 1
            pNextBrick->block.left = bricksLeft + 1;
            pNextBrick->block.top = bricksTop + 1;

            //subtract 2 from width & height for the space for 1-pixel
            pNextBrick->block.width = BRICK_WIDTH - 2;
            pNextBrick->block.height = BRICK_HEIGHT - 2;

            // row properties
            if (row < 2)
            {
                pNextBrick->block.color = sf::Color::White;
                pNextBrick->points = 1;
                pNextBrick-> speedAdjust = 0;
            }
            else if (row < 4)
            {
                pNextBrick->block.color = sf::Color(112, 121, 121); // Light Grey
                pNextBrick->points = 4;
                pNextBrick-> speedAdjust = 3;
            }
            else if (row < 6)
            {
                pNextBrick->block.color = sf::Color(70, 70, 80); // dark grey
                pNextBrick->points = 8;
                pNextBrick-> speedAdjust = 4;
            }
            else
            {
                pNextBrick->block.color = sf::Color(255, 153, 51); // Light orange
                pNextBrick->points = 12;
                pNextBrick-> speedAdjust = 6;
            }


            // drawing properties
            pNextBrick->block.rectangle.setSize(sf::Vector2f(pNextBrick->block.width, pNextBrick->block.height));
            pNextBrick->block.rectangle.setFillColor((pNextBrick->block.color));
            pNextBrick->block.rectangle.setPosition(pNextBrick->block.left, pNextBrick->block.top);

            pNextBrick->hit = false;
            pNextBrick++;
            bricksLeft += BRICK_WIDTH;
        } // brick columns
        bricksTop -= BRICK_HEIGHT;
    } // brick rows
}

/**
 * convert user keyboard input into recognized integer values
 * for left=1/up=2/right=3/down=4
 * @return int - user input (default no-input=0, quit=-1, start=6)
 */
Direction processInput() {
    Direction input = None;  // no input

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        input = Left;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        input = Up;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        input = Right;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        input = Down;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
        input = Exit;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        input = Start;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
        input = Pause;
    }

    return input;
} // end getUserInput


/**
 * update the state of game objects
 * @param input - user keyboard input
 * @param ball  - update ball position and speed
 * @param delta - current frame time
 * @param walls - the borders
 * @param paddle - user paddle block
 * @param started - game start check
 * @param bricks - structure array for bricks (passed to another function within)
 * @return bool - returns true if a game-ending collision occured
 */
bool update(Direction &input, Ball &ball, float delta, Borders walls,
            MovingBlock &paddle, bool &started, Brick bricks[BRICK_ROWS][BRICK_COLUMNS]){

    bool gameOver = false;
    // adjust velocity directions for user input
    if (input) {
        switch (input) {
            case Down:
                paddle.velocityX = 0; // paddle stop
                break;
            case Left:
                paddle.velocityX -= PADDLE_SPEED;
                break;
                //case Up:
                //    break;
            case Right:
                paddle.velocityX += PADDLE_SPEED;
                break;
                //case Down:
            case Start:
                if (!started)
                {
                    ball.velocityX = BALL_SPEED_X;
                    ball.velocityY = BALL_SPEED_Y * -1;
                    if ((int(delta * 10) & 1) % 2)
                    {
                        ball.velocityX *= -1;
                    }
                    started = true;
                }
                break;
                //case Start
            case Pause:
                paused();
                break;

        }
        // clear input
        input = None;
    }
    // adjust the location of the ball for speed * time
    paddle.block.left += paddle.velocityX * delta;

    if (started)
    {
        ball.coordinateX += (ball.velocityX * delta);
        ball.coordinateY += (ball.velocityY * delta);
    }
    else
    {
        ball.coordinateX = paddle.block.left + (PADDLE_WIDTH / 2.0);
        ball.coordinateY = paddle.block.top - BALL_RADIUS - 1;
    }

    gameOver = doCollisionChecks(ball, paddle, walls, bricks);

    return gameOver;
} // end update


/**
 * draw the ball on the graphics window
 * @param window - handle to open graphics windowaa
 * @param ball   - structure variable with properties for the ball
 * @param delta  - amount of frame time plus lag (in ms)
 * @param walls - the borders
 * @param paddle - structure variable with properties for the paddle
 * @param bricks - structure array with properties for the bricks
 */
void render(sf::RenderWindow &window, Ball &ball, float delta, Borders walls, MovingBlock paddle,
            Brick bricks[BRICK_ROWS][BRICK_COLUMNS]){

    // Render drawing objects
    // ------------------------------------------------
    window.clear(WINDOW_COLOR);     // clear the window with the background color

    // draw the ball
    // ------------------------------------------------
    sf::CircleShape circle;
    circle.setFillColor(ball.color);
    circle.setRadius(ball.radius);
    circle.setOrigin(ball.radius,ball.radius);  // set screen coordinates relative to the center of the circle

    // calculate current drawing location relative to speed and frame-time
    float xCoordinate = (ball.coordinateX + (ball.velocityX * delta));
    float yCoordinate = (ball.coordinateY + (ball.velocityY * delta));

    float paddleXCoord = (paddle.block.left + (paddle.velocityX * delta));
    float paddleYCoord = (paddle.block.top + (paddle.velocityY * delta));

    // set paddles position
    //--------------------------------------------
    paddle.block.rectangle.setPosition(paddleXCoord, paddleYCoord);
    circle.setPosition(xCoordinate, yCoordinate);
    window.draw(circle);
    window.draw(paddle.block.rectangle);


    // draw the window walls
    window.draw(walls.leftBlock.rectangle);
    window.draw(walls.topBlock.rectangle);
    window.draw(walls.rightBlock.rectangle);
    window.draw(walls.bottomBlock.rectangle);


    // draw the bricks
    Brick *pBrick = &bricks[0][0];
    for (int row = 0; row < BRICK_ROWS; row++) // rows
    {

        for (int column = 0; column < BRICK_COLUMNS; column++) //columns
        {

            if (!pBrick->hit)
            {
                window.draw(pBrick->block.rectangle);
            }

            pBrick++;
        } // COLUMNS

    } //  ROWS

    // display new window
    window.display();
} // end render


/**
 * determines the point using for collision detection
 * @param pBall - structure variable with properties for the ball
 * @param pBlock - structure variable with properties for the block
 * @return int - returns angle of heading from 1-360 degrees
 */
int getCollisionPoint(Ball* pBall, Block* pBlock)
{
    int heading = 0;
    float checkPointX = 0.0;
    float checkPointY = 0.0;

    // horizontal collisions
    //---------------------------------------------------------------
    if (pBall->coordinateX < pBlock->left) // collision with left wall
    {
        checkPointX = pBlock->left;
    }
    else if (pBall->coordinateX > (pBlock->left + pBlock->width)) // collision with right wall
    {
        checkPointX = (pBlock->left + pBlock->width);

    }else{
        checkPointX = pBall->coordinateX;
    }

    // vertical collisions
    //---------------------------------------------------------------
    if (pBall->coordinateY < pBlock->top) // collision with top wall
    {
        checkPointY = pBlock->top;
    }
    else if (pBall->coordinateY > (pBlock->top + pBlock->height)) // collision with bottom wall
    {
        checkPointY = (pBlock->top + pBlock->height);

    }else{
        checkPointY = pBall->coordinateY;
    }

    float differenceX = checkPointX - pBall->coordinateX;
    float differenceY = ((WINDOW_HEIGHT - checkPointY) - (WINDOW_HEIGHT - pBall->coordinateY));
    double distance = sqrt(pow(differenceX, 2.0) + pow(differenceY, 2.0));

    if (distance <= pBall->radius)
    {
        double theta = atan2(differenceY, differenceX);
        double degrees = 90.0 - theta * 180 / M_PI;
        if (degrees <= 0)
        {
            degrees += 360;
        }
        heading = int(degrees);
    }
    return heading;
}


/**
 * checks for collision using the heading from getCollisionPoint
 * @param pBall - structure variable with properties for the ball
 * @param pBlock - structure variable with properties for the block
 * @return bool - returns true if collision detected, false if not
 */
bool collisionCheck(Ball *pBall, Block *pBlock)
{
    bool bCollided = false;
    int collision = getCollisionPoint(pBall, pBlock);

    if (collision)
    {
        bCollided = true;
        if (collision > 225 && collision < 315) // left
        {
            pBall->velocityX *= -1;
            pBall->coordinateX = (pBlock->left + pBlock->width + pBall->radius + 1);
        }
        else if (collision > 45 && collision < 135) // top
        {
            pBall->velocityX *= -1;
            pBall->coordinateX = (pBlock->left - pBall->radius - 1);
        }

        if (collision >= 315 || collision <= 45) // right
        {
            pBall->velocityY *= -1;
            pBall->coordinateY = (pBlock->top + pBlock->height + pBall->radius + 1);
        }
        else if (collision >= 135 && collision <= 225) // bottom
        {
            pBall->velocityY *= -1;
            pBall->coordinateY = (pBlock->top - pBall->radius - 1);
        }
    }
    return bCollided;
}


/**
 * checks for collision using the properties of moving and stationary blocks
 * @param moving - structure variable with properties for the moving block (paddle)
 * @param stationary - structure variable with properties for the stationary block (brick)
 * @return bool - returns true if collision detected, false if not
 */
bool checkBlockCollision(Block moving, Block stationary)
{
    bool collision = false;
    if (moving.left < stationary.left + stationary.width &&

        moving.left + moving.width > stationary.left &&

        moving.top < stationary.top + stationary.height &&

        moving.top + moving.height > stationary.top)

    {
        collision = true;
    }
    return collision;
}


/**
 * @param ball = ball for collision checks
 * @param paddle = paddle for collision checks
 * @param walls = game walls
 * @param bricks = point bricks to break
 * @return bool = returns true if a collision happened, false if not
 */
bool doCollisionChecks (Ball &ball, MovingBlock &paddle, Borders walls, Brick bricks[BRICK_ROWS][BRICK_COLUMNS])
{
    bool gameOver = false;
    // vertical collision checks
    //-----------------------------------------
    if (!collisionCheck(&ball, &paddle.block))
    {
        if (!collisionCheck(&ball, &walls.topBlock))
        {
            gameOver = collisionCheck(&ball, &walls.bottomBlock);
        }
    }

    // horizontal collision checks
    //-----------------------------------------
    if (!collisionCheck(&ball, &walls.leftBlock))
    {
        collisionCheck(&ball, &walls.rightBlock);
    }

    // paddle-wall collision checks
    //-----------------------------------------
    if (checkBlockCollision(paddle.block, walls.leftBlock))
    {
        paddle.block.left = walls.leftBlock.left + walls.leftBlock.width + 1;
        paddle.velocityX = 0;
    }

    else if (checkBlockCollision(paddle.block, walls.rightBlock))
    {
        paddle.block.left = walls.rightBlock.left - paddle.block.width - 1;
        paddle.velocityX = 0;
    }


    Brick *pBrick = &bricks[0][0];
    for (int row = 0; row < BRICK_ROWS; row++)
    {

        for (int column = 0; column < BRICK_COLUMNS; column++)
        {

            if (!pBrick->hit)
            {
                pBrick->hit = collisionCheck(&ball, &pBrick->block);
            }
            pBrick++;

        } // COLUMNS

    } // ROWS
    return gameOver;
}
