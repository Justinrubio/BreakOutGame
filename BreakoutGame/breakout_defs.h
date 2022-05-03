/* --------------------------------------------------------
 *    File: BreakoutPaddles.cpp
 *  Author: Justin Rubio
 *   Class: COP 2001
 * Purpose: The classic breakout desktop game.
 * -------------------------------------------------------- */

#include <SFML/Graphics.hpp>
#ifndef BREAKOUTPADDLES_CPP_BREAKOUT_DEFS_H
#define BREAKOUTPADDLES_CPP_BREAKOUT_DEFS_H

//ball properties
const float BALL_RADIUS = 10.0;

//paddle properties
const float PADDLE_WIDTH = 80.0;
const float PADDLE_THICKNESS = 10.0;
const sf::Color PADDLE_COLOR = sf::Color::White;
const float PADDLE_SPEED = PADDLE_WIDTH / 10.0 / 1000.0 ; //adjust the 10 to change paddle speed


//border properties
const float WALL_THICKNESS = 15.0;
const sf::Color WALL_COLOR = sf::Color(255, 119, 0); // darker orange

// window properties
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 480;
const sf::Color WINDOW_COLOR = sf::Color::Black;

// drawing properties
const float FRAME_RATE = (1.0/30.0) * 1000.0;  // FPS in ms
const sf::Color BALL_COLOR = sf::Color(255, 153, 51); // light orange

//brick properties
const int BRICK_ROWS = 8;
const int BRICK_COLUMNS = 14;
const float BRICK_WIDTH = WINDOW_WIDTH / BRICK_COLUMNS;
const float BRICK_HEIGHT = PADDLE_THICKNESS * 2;
const float BRICKS_HEIGHT = BRICK_ROWS * BRICK_HEIGHT;
const float BRICKS_TOP = WINDOW_HEIGHT/2.0 - BRICKS_HEIGHT * 0.75;
const float BRICKS_LEFT = WALL_THICKNESS;
const float FIRST_BRICK = BRICKS_TOP + (BRICK_ROWS - 1) * BRICK_HEIGHT;


// These are just for fun
// speed that the can accelerate at to span window in
// n-seconds (in ms) broken up for each frame
const float SPEED_TIME = (3.0 * 1000.0) * 30.0;             //
const float BALL_SPEED_X = BALL_RADIUS * 10.0 / 1000.0;    // speed horizontally
const float BALL_SPEED_Y = BALL_RADIUS * 8.5 / 1000.0;   // span  vertically


// Type definitions
// --------------------------------------------------------

// <direction enum here>
enum Direction{
    Exit =-1,
    None,
    Left,
    Up,
    Right,
    Down,
    Start
};


// ball properties
// <define structure properties here>
struct Ball {
    float radius; // size of ball
    float coordinateX; // current horizontal position of ball X
    float coordinateY; // current vertical position of ball Y
    float velocityX;   // current horizontal speed X
    float velocityY;   // current vertical speed Y
    sf::Color color;   // fill color

};

//wall structure
struct Block {
    float left;
    float top;
    float width;
    float height;
    sf::Color color;
    sf::RectangleShape rectangle;
};

// MOVING BLOCK STRUCT
struct MovingBlock {
    Block block;
    sf::RectangleShape rectangle;
    float velocityX;   // current horizontal speed X
    float velocityY;   // current vertical speed Y

};

//borders for walls
struct Borders{
    Block leftBlock;
    Block topBlock;
    Block rightBlock;
    Block bottomBlock;
};

struct Brick{
    Block block;
    bool hit;
    int points;
    float speedAdjust;
};


#endif //BREAKOUTPADDLES_CPP_BREAKOUT_DEFS_H
