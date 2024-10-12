/**
* Author: [Your name here]
* Assignment: Pong Clone
* Date due: 2024-10-12, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

enum AppStatus { RUNNING, TERMINATED };

constexpr int WINDOW_WIDTH  = 1100,
              WINDOW_HEIGHT = 600;

constexpr float LEFT_BORDER = -5.22;
constexpr float RIGHT_BORDER = 5.22;
constexpr float BOTTOM_BORDER = -2.85;
constexpr float TOP_BORDER = 2.85;

// Set up the background color
constexpr float BG_RED     = 0.25f,
                BG_BLUE    = 0.25f,
                BG_GREEN   = 0.25f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X      = 0,
              VIEWPORT_Y      = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
// Sources:

// File Path:
constexpr char TENNIS_COURT_SPRITE_FILEPATH[] = "tennis_court.png";
constexpr char LEFT_TENNIS_RACKET_SPRITE_FILEPATH[] = "left_tennis_racket.png";
constexpr char RIGHT_TENNIS_RACKET_SPRITE_FILEPATH[] = "right_tennis_racket.png";
constexpr char TENNIS_BALL_SPRITE_FILEPATH[] = "tennis_ball.png";
constexpr char LEFT_RACKET_WIN_SPRITE_FILEPATH[] = "red_racket_win_image.png";
constexpr char RIGHT_RACKET_WIN_SPRITE_FILEPATH[] = "pink_racket_win_image.png";


// ID
GLuint g_tennis_court_id, g_left_racket_id, g_right_racket_id, g_tennis_ball_id_1, g_tennis_ball_id_2, g_tennis_ball_id_3, g_left_win_id, g_right_win_id;

constexpr int TRIANGLE_RED     = 1.0,
              TRIANGLE_BLUE    = 0.4,
              TRIANGLE_GREEN   = 0.4,
              TRIANGLE_OPACITY = 1.0;

AppStatus g_app_status = RUNNING;
SDL_Window* g_display_window;

ShaderProgram g_shader_program = ShaderProgram();

glm::mat4 g_view_matrix,
          g_model_matrix_left_paddle,
          g_model_matrix_right_paddle,
          g_model_matrix_ball_1,
          g_model_matrix_ball_2,
          g_model_matrix_ball_3,
          g_model_matrix_court,
          g_model_matrix_left_win,
          g_model_matrix_right_win,
          g_projection_matrix;

// ——————————— GLOBAL VARS AND CONSTS FOR OBJECTS' TRANSFORMATIONS ——————————— //

// Transformation for Left Paddle
constexpr float LEFT_PADDLE_SPEED = 5.0f;


// Transformation for Right Paddle
constexpr float RIGHT_PADDLE_SPEED = 5.0f;

// Transformation for Ball
constexpr float BALL_SPEED = 2.0f;


// The movement vectors for all the objects
glm::vec3 g_model_matrix_left_paddle_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_model_matrix_left_paddle_movement = glm::vec3(0.0f, 0.0f, 0.0f);

constexpr float INIT_RIGHT_PADDLE_ANGLE = glm::radians(180.0);
glm::vec3 g_model_matrix_right_paddle_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_model_matrix_right_paddle_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_model_matrix_ball_position_1 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_model_matrix_ball_movement_1 = glm::vec3(-1.2f, 0.8f, 0.0f);

glm::vec3 g_model_matrix_ball_position_2 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_model_matrix_ball_movement_2 = glm::vec3(-1.2f, 0.8f, 0.0f);

glm::vec3 g_model_matrix_ball_position_3 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_model_matrix_ball_movement_3 = glm::vec3(-1.2f, 0.8f, 0.0f);

// Initial position for all the objects
constexpr glm::vec3 LEFT_PADDLE_INIT_POS = glm::vec3(-4.0f, 0.0f, 0.0f);
constexpr glm::vec3 RIGHT_PADDLE_INIT_POS = glm::vec3(4.0f, 0.0f, 0.0f);
constexpr glm::vec3 BALL_INIT_POS = glm::vec3(2.0f, -3.0f, 0.0f);

bool ifGameStop = false;
bool singlePlayerModeOpened = false;
bool ifTouchedHeightBorder = false;
bool ifLeftWin = true;
float right_paddle_movement_single_mode = 1.0f;
float ballnumber = 1;

// Initial scale for all the objects
constexpr glm::vec3 LEFT_PADDLE_INIT_SCALE = glm::vec3(0.4f, 1.8f, 0.0f);
constexpr glm::vec3 RIGHT_PADDLE_INIT_SCALE = glm::vec3(0.4f, 1.8f, 0.0f);
constexpr glm::vec3 BALL_INIT_SCALE = glm::vec3(0.2f, 0.3f, 0.0f);
constexpr glm::vec3 TENNIS_COURT_INIT_SCALE = glm::vec3(17.1f, 12.0f, 0.0f);
constexpr glm::vec3 WINNING_MESSAGE_INIT_SCALE = glm::vec3(4.5f, 3.2f, 0.0f);

// Delta time global variable for recording the previous frame
float g_previous_ticks = 0.0f;

// Texture required global variables
constexpr int NUMBER_OF_TEXTURES = 1; // to be generated, that is
constexpr GLint LEVEL_OF_DETAIL = 0; // base image level; Level n is the nth mipmap reduction image
constexpr GLint TEXTURE_BORDER = 0; // this value MUST be zero

constexpr float MILLISECONDS_IN_SECOND = 1000.0f;

// —————————————————————————————————————————————————————————————————— //

GLuint load_texture(const char* filepath) {
    // Try to load the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    // Quit if it fails
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }
    GLuint textureID;                               // declaration
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);  // assignment
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);
        
    return textureID;
}


void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("CS3113_Assignment2_Pong Clone!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
    if (g_display_window == nullptr) {
        std::cerr << "ERROR: SDL Window could not be created.\n";
        g_app_status = TERMINATED;
            
        SDL_Quit();
        exit(1);
    }


#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_model_matrix_left_paddle = glm::mat4(1.0f);
    g_model_matrix_right_paddle = glm::mat4(1.0f);
    g_model_matrix_ball_1 = glm::mat4(1.0f);
    g_model_matrix_ball_2 = glm::mat4(1.0f);
    g_model_matrix_ball_3 = glm::mat4(1.0f);
    g_model_matrix_court = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED,
                 BG_BLUE,
                 BG_GREEN,
                 BG_OPACITY);
    
    // Load Texture ID Here:
    g_left_racket_id = load_texture(LEFT_TENNIS_RACKET_SPRITE_FILEPATH);
    g_right_racket_id = load_texture(RIGHT_TENNIS_RACKET_SPRITE_FILEPATH);
    g_tennis_ball_id_1 = load_texture(TENNIS_BALL_SPRITE_FILEPATH);
    g_tennis_ball_id_2 = load_texture(TENNIS_BALL_SPRITE_FILEPATH);
    g_tennis_ball_id_3 = load_texture(TENNIS_BALL_SPRITE_FILEPATH);
    g_tennis_court_id = load_texture(TENNIS_COURT_SPRITE_FILEPATH);
    g_left_win_id = load_texture(LEFT_RACKET_WIN_SPRITE_FILEPATH);
    g_right_win_id = load_texture(RIGHT_RACKET_WIN_SPRITE_FILEPATH);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
            case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_t:
                            singlePlayerModeOpened = !singlePlayerModeOpened;
                            break;
                        case SDLK_1:
                            ballnumber = 1;
                            break;
                        case SDLK_2:
                            ballnumber = 2;
                            break;
                        case SDLK_3:
                            ballnumber = 3;
                            break;
                    }
                    break;
        }
        
    }
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    if (key_state[SDL_SCANCODE_W] && !ifGameStop)
    {
        if(g_model_matrix_left_paddle_position.y < TOP_BORDER){
            g_model_matrix_left_paddle_movement.y = 1.0f;
        }else{
            g_model_matrix_left_paddle_movement.y = 0.0f;
        }
        
    }
    else if (key_state[SDL_SCANCODE_S] && !ifGameStop)
    {
        if(g_model_matrix_left_paddle_position.y > BOTTOM_BORDER){
            g_model_matrix_left_paddle_movement.y = -1.0f;
        }else{
            g_model_matrix_left_paddle_movement.y = 0.0f;
        }
    }else {
        g_model_matrix_left_paddle_movement.y = 0.0f;
    }
    
    if (key_state[SDL_SCANCODE_UP] && !ifGameStop && !singlePlayerModeOpened)
    {
        if(g_model_matrix_right_paddle_position.y < TOP_BORDER){
            g_model_matrix_right_paddle_movement.y = 1.0f;
        }else{
            g_model_matrix_right_paddle_movement.y = 0.0f;
        }
    }
    else if (key_state[SDL_SCANCODE_DOWN] && !ifGameStop && !singlePlayerModeOpened)
    {
        if(g_model_matrix_right_paddle_position.y > BOTTOM_BORDER){
            g_model_matrix_right_paddle_movement.y = -1.0f;
        }else{
            g_model_matrix_right_paddle_movement.y = 0.0f;
        }
    }else if (!singlePlayerModeOpened){
        g_model_matrix_right_paddle_movement.y = 0.0f;
    }
 
    
    // Normalization is not necessary here
    if (glm::length(g_model_matrix_left_paddle_movement) > 1.0f)
    {
         g_model_matrix_left_paddle_movement = glm::normalize(g_model_matrix_left_paddle_movement);
    } else if (glm::length(g_model_matrix_right_paddle_movement) > 1.0f)
    {
        g_model_matrix_right_paddle_movement = glm::normalize(g_model_matrix_right_paddle_movement);
        
    }
}


void update()
{
    std::cout << singlePlayerModeOpened << std::endl;
    std::cout << g_model_matrix_right_paddle_movement.y << std::endl;
    // ——————————— TRANSFORMATION FOR ALL THE OBJECTS ——————————— //
    
    // Settle the delta time for each frame
    float g_ticks = (float) SDL_GetTicks() / 1000.0f;  // get the current number of g_ticks
    float g_delta_time = g_ticks - g_previous_ticks;  // the delta time is the difference from the last frame
    g_previous_ticks = g_ticks; // modify the previous ticks global variable
    
    
    g_model_matrix_left_paddle_position += g_model_matrix_left_paddle_movement * LEFT_PADDLE_SPEED * g_delta_time;
    g_model_matrix_right_paddle_position += g_model_matrix_right_paddle_movement * RIGHT_PADDLE_SPEED * g_delta_time;
    if(ballnumber == 1){
        g_model_matrix_ball_position_1 += g_model_matrix_ball_movement_1 * BALL_SPEED * g_delta_time;
    } else if(ballnumber == 2){
        g_model_matrix_ball_position_1 += g_model_matrix_ball_movement_1 * BALL_SPEED * g_delta_time;
        g_model_matrix_ball_position_2 += g_model_matrix_ball_movement_2 * BALL_SPEED * g_delta_time;
    } else if(ballnumber == 3){
        g_model_matrix_ball_position_1 += g_model_matrix_ball_movement_1 * BALL_SPEED * g_delta_time;
        g_model_matrix_ball_position_2 += g_model_matrix_ball_movement_2 * BALL_SPEED * g_delta_time;
        g_model_matrix_ball_position_3 += g_model_matrix_ball_movement_3 * BALL_SPEED * g_delta_time;
    }
    
    
    // Model matrix reset
    g_model_matrix_left_paddle = glm::mat4(1.0f);
    
    // Apply transformations
    g_model_matrix_left_paddle = glm::translate(g_model_matrix_left_paddle, LEFT_PADDLE_INIT_POS);
    g_model_matrix_left_paddle = glm::translate(g_model_matrix_left_paddle, g_model_matrix_left_paddle_position);
    
    g_model_matrix_right_paddle = glm::mat4(1.0f);
    g_model_matrix_right_paddle = glm::translate(g_model_matrix_right_paddle, RIGHT_PADDLE_INIT_POS);
    g_model_matrix_right_paddle = glm::translate(g_model_matrix_right_paddle, g_model_matrix_right_paddle_position);
    g_model_matrix_right_paddle = glm::rotate(g_model_matrix_right_paddle, INIT_RIGHT_PADDLE_ANGLE, glm::vec3(0.0f, 1.0f, 0.0f));
    
    g_model_matrix_ball_1 = glm::mat4(1.0f);
    g_model_matrix_ball_2 = glm::mat4(1.0f);
    g_model_matrix_ball_3 = glm::mat4(1.0f);
    if(ballnumber == 1){
        g_model_matrix_ball_1 = glm::translate(g_model_matrix_ball_1, BALL_INIT_POS);
        g_model_matrix_ball_1 = glm::translate(g_model_matrix_ball_1, g_model_matrix_ball_position_1);
    } else if(ballnumber == 2){
        g_model_matrix_ball_1 = glm::translate(g_model_matrix_ball_1, BALL_INIT_POS);
        g_model_matrix_ball_1 = glm::translate(g_model_matrix_ball_1, g_model_matrix_ball_position_1);
        g_model_matrix_ball_2 = glm::translate(g_model_matrix_ball_2, BALL_INIT_POS);
        g_model_matrix_ball_2 = glm::translate(g_model_matrix_ball_2, g_model_matrix_ball_position_2);
    } else if(ballnumber == 3){
        g_model_matrix_ball_1 = glm::translate(g_model_matrix_ball_1, BALL_INIT_POS);
        g_model_matrix_ball_1 = glm::translate(g_model_matrix_ball_1, g_model_matrix_ball_position_1);
        g_model_matrix_ball_2 = glm::translate(g_model_matrix_ball_2, BALL_INIT_POS);
        g_model_matrix_ball_2 = glm::translate(g_model_matrix_ball_2, g_model_matrix_ball_position_2);
        g_model_matrix_ball_3 = glm::translate(g_model_matrix_ball_3, BALL_INIT_POS);
        g_model_matrix_ball_3 = glm::translate(g_model_matrix_ball_3, g_model_matrix_ball_position_3);
    }
    
    
    g_model_matrix_court = glm::mat4(1.0f);
    
    if(ifGameStop && ifLeftWin){
        g_model_matrix_left_win = glm::mat4(1.0f);
        g_model_matrix_left_win = glm::scale(g_model_matrix_left_win, WINNING_MESSAGE_INIT_SCALE);
    }
    
    if(ifGameStop && !ifLeftWin){
        g_model_matrix_right_win = glm::mat4(1.0f);
        g_model_matrix_right_win = glm::scale(g_model_matrix_right_win, WINNING_MESSAGE_INIT_SCALE);
    }
    
    g_model_matrix_court = glm::scale(g_model_matrix_court, TENNIS_COURT_INIT_SCALE);
    g_model_matrix_left_paddle = glm::scale(g_model_matrix_left_paddle, LEFT_PADDLE_INIT_SCALE);
    g_model_matrix_right_paddle = glm::scale(g_model_matrix_right_paddle, RIGHT_PADDLE_INIT_SCALE);
    
    if(ballnumber == 1){
        g_model_matrix_ball_1 = glm::scale(g_model_matrix_ball_1, BALL_INIT_SCALE);
    } else if(ballnumber == 2){
        g_model_matrix_ball_1 = glm::scale(g_model_matrix_ball_1, BALL_INIT_SCALE);
        g_model_matrix_ball_2 = glm::scale(g_model_matrix_ball_2, BALL_INIT_SCALE);
    } else if (ballnumber == 3){
        g_model_matrix_ball_1 = glm::scale(g_model_matrix_ball_1, BALL_INIT_SCALE);
        g_model_matrix_ball_2 = glm::scale(g_model_matrix_ball_2, BALL_INIT_SCALE);
        g_model_matrix_ball_3 = glm::scale(g_model_matrix_ball_3, BALL_INIT_SCALE);
    }

    
    // Detect collison for bounce off
    float x_distance_left_paddle_1 = fabs(g_model_matrix_ball_position_1.x + BALL_INIT_POS.x - (g_model_matrix_left_paddle_position.x + LEFT_PADDLE_INIT_POS.x)) - ((LEFT_PADDLE_INIT_SCALE.x + BALL_INIT_SCALE.x) / 2.0f);
    float y_distance_left_paddle_1 = fabs(g_model_matrix_ball_position_1.y + BALL_INIT_POS.y - (g_model_matrix_left_paddle_position.y + LEFT_PADDLE_INIT_POS.y)) - ((BALL_INIT_SCALE.y + LEFT_PADDLE_INIT_SCALE.y) / 2.0f);
    
    float x_distance_right_paddle_1 = fabs(g_model_matrix_ball_position_1.x + BALL_INIT_POS.x - (g_model_matrix_right_paddle_position.x + RIGHT_PADDLE_INIT_POS.x)) - ((RIGHT_PADDLE_INIT_SCALE.x + BALL_INIT_SCALE.x) / 2.0f);
    float y_distance_right_paddle_1 = fabs(g_model_matrix_ball_position_1.y + BALL_INIT_POS.y - (g_model_matrix_right_paddle_position.y + RIGHT_PADDLE_INIT_POS.y)) - ((BALL_INIT_SCALE.y + RIGHT_PADDLE_INIT_SCALE.y) / 2.0f);
    
    float x_distance_left_paddle_2 = fabs(g_model_matrix_ball_position_2.x + BALL_INIT_POS.x - (g_model_matrix_left_paddle_position.x + LEFT_PADDLE_INIT_POS.x)) - ((LEFT_PADDLE_INIT_SCALE.x + BALL_INIT_SCALE.x) / 2.0f);
    float y_distance_left_paddle_2 = fabs(g_model_matrix_ball_position_2.y + BALL_INIT_POS.y - (g_model_matrix_left_paddle_position.y + LEFT_PADDLE_INIT_POS.y)) - ((BALL_INIT_SCALE.y + LEFT_PADDLE_INIT_SCALE.y) / 2.0f);
    
    float x_distance_right_paddle_2 = fabs(g_model_matrix_ball_position_2.x + BALL_INIT_POS.x - (g_model_matrix_right_paddle_position.x + RIGHT_PADDLE_INIT_POS.x)) - ((RIGHT_PADDLE_INIT_SCALE.x + BALL_INIT_SCALE.x) / 2.0f);
    float y_distance_right_paddle_2 = fabs(g_model_matrix_ball_position_2.y + BALL_INIT_POS.y - (g_model_matrix_right_paddle_position.y + RIGHT_PADDLE_INIT_POS.y)) - ((BALL_INIT_SCALE.y + RIGHT_PADDLE_INIT_SCALE.y) / 2.0f);
    
    float x_distance_left_paddle_3 = fabs(g_model_matrix_ball_position_3.x + BALL_INIT_POS.x - (g_model_matrix_left_paddle_position.x + LEFT_PADDLE_INIT_POS.x)) - ((LEFT_PADDLE_INIT_SCALE.x + BALL_INIT_SCALE.x) / 2.0f);
    float y_distance_left_paddle_3 = fabs(g_model_matrix_ball_position_3.y + BALL_INIT_POS.y - (g_model_matrix_left_paddle_position.y + LEFT_PADDLE_INIT_POS.y)) - ((BALL_INIT_SCALE.y + LEFT_PADDLE_INIT_SCALE.y) / 2.0f);
    
    float x_distance_right_paddle_3 = fabs(g_model_matrix_ball_position_3.x + BALL_INIT_POS.x - (g_model_matrix_right_paddle_position.x + RIGHT_PADDLE_INIT_POS.x)) - ((RIGHT_PADDLE_INIT_SCALE.x + BALL_INIT_SCALE.x) / 2.0f);
    float y_distance_right_paddle_3 = fabs(g_model_matrix_ball_position_3.y + BALL_INIT_POS.y - (g_model_matrix_right_paddle_position.y + RIGHT_PADDLE_INIT_POS.y)) - ((BALL_INIT_SCALE.y + RIGHT_PADDLE_INIT_SCALE.y) / 2.0f);
    

    // If any collison detected, set the movement for the ball for the next frame
    if (g_model_matrix_ball_position_1.y + BALL_INIT_POS.y - 0.8f >= TOP_BORDER){
        float ball_movement_x_record = g_model_matrix_ball_movement_1.x;
        g_model_matrix_ball_movement_1.x = g_model_matrix_ball_movement_1.y;
        g_model_matrix_ball_movement_1.y = -ball_movement_x_record;
        g_model_matrix_ball_position_1 += g_model_matrix_ball_movement_1 * BALL_SPEED * g_delta_time;
    } else if (g_model_matrix_ball_position_1.y + BALL_INIT_POS.y + 0.8f <= BOTTOM_BORDER){
        float ball_movement_x_record = g_model_matrix_ball_movement_1.x;
        g_model_matrix_ball_movement_1.x = g_model_matrix_ball_movement_1.y;
        g_model_matrix_ball_movement_1.y = -ball_movement_x_record;
        g_model_matrix_ball_position_1 += g_model_matrix_ball_movement_1 * BALL_SPEED * g_delta_time;
    }
    
    if (g_model_matrix_ball_position_2.y + BALL_INIT_POS.y - 0.8f >= TOP_BORDER){
        float ball_movement_x_record = g_model_matrix_ball_movement_2.x;
        g_model_matrix_ball_movement_2.x = g_model_matrix_ball_movement_2.y;
        g_model_matrix_ball_movement_2.y = -ball_movement_x_record;
        g_model_matrix_ball_position_2 += g_model_matrix_ball_movement_2 * BALL_SPEED * g_delta_time;
    } else if (g_model_matrix_ball_position_2.y + BALL_INIT_POS.y + 0.8f <= BOTTOM_BORDER){
        float ball_movement_x_record = g_model_matrix_ball_movement_2.x;
        g_model_matrix_ball_movement_2.x = g_model_matrix_ball_movement_2.y;
        g_model_matrix_ball_movement_2.y = -ball_movement_x_record;
        g_model_matrix_ball_position_2 += g_model_matrix_ball_movement_2 * BALL_SPEED * g_delta_time;
    }
    
    if (g_model_matrix_ball_position_3.y + BALL_INIT_POS.y - 0.8f >= TOP_BORDER){
        float ball_movement_x_record = g_model_matrix_ball_movement_3.x;
        g_model_matrix_ball_movement_3.x = g_model_matrix_ball_movement_3.y;
        g_model_matrix_ball_movement_3.y = -ball_movement_x_record;
        g_model_matrix_ball_position_3 += g_model_matrix_ball_movement_3 * BALL_SPEED * g_delta_time;
    } else if (g_model_matrix_ball_position_3.y + BALL_INIT_POS.y + 0.8f <= BOTTOM_BORDER){
        float ball_movement_x_record = g_model_matrix_ball_movement_3.x;
        g_model_matrix_ball_movement_3.x = g_model_matrix_ball_movement_3.y;
        g_model_matrix_ball_movement_3.y = -ball_movement_x_record;
        g_model_matrix_ball_position_3 += g_model_matrix_ball_movement_3 * BALL_SPEED * g_delta_time;
    }
    
    
    
    if((x_distance_right_paddle_1 < 0.0f && y_distance_right_paddle_1 < 0.0f) || (x_distance_left_paddle_1 < 0.0f && y_distance_left_paddle_1 < 0.0f)){
        float ball_movement_x_record = g_model_matrix_ball_movement_1.x;
        g_model_matrix_ball_movement_1.x = g_model_matrix_ball_movement_1.y;
        g_model_matrix_ball_movement_1.y = -ball_movement_x_record;
        g_model_matrix_ball_position_1 += g_model_matrix_ball_movement_1 * BALL_SPEED * g_delta_time;
    }
    
    if((x_distance_right_paddle_2 < 0.0f && y_distance_right_paddle_2 < 0.0f) || (x_distance_left_paddle_2 < 0.0f && y_distance_left_paddle_2 < 0.0f)){
        float ball_movement_x_record = g_model_matrix_ball_movement_2.x;
        g_model_matrix_ball_movement_2.x = g_model_matrix_ball_movement_2.y;
        g_model_matrix_ball_movement_2.y = -ball_movement_x_record;
        g_model_matrix_ball_position_2 += g_model_matrix_ball_movement_2 * BALL_SPEED * g_delta_time;
    }
    
    if((x_distance_right_paddle_3 < 0.0f && y_distance_right_paddle_3 < 0.0f) || (x_distance_left_paddle_3 < 0.0f && y_distance_left_paddle_3 < 0.0f)){
        float ball_movement_x_record = g_model_matrix_ball_movement_3.x;
        g_model_matrix_ball_movement_3.x = g_model_matrix_ball_movement_3.y;
        g_model_matrix_ball_movement_3.y = -ball_movement_x_record;
        g_model_matrix_ball_position_3 += g_model_matrix_ball_movement_3 * BALL_SPEED * g_delta_time;
    }
    
    // Game Over
    if((g_model_matrix_ball_position_1.x + BALL_INIT_POS.x + 0.34f >= RIGHT_BORDER) || (g_model_matrix_ball_position_2.x + BALL_INIT_POS.x + 0.34f >= RIGHT_BORDER) || (g_model_matrix_ball_position_3.x + BALL_INIT_POS.x + 0.34f >= RIGHT_BORDER)){
        g_model_matrix_ball_movement_1.x = 0.0f;
        g_model_matrix_ball_movement_1.y = 0.0f;
        g_model_matrix_ball_movement_2.x = 0.0f;
        g_model_matrix_ball_movement_2.y = 0.0f;
        g_model_matrix_ball_movement_3.x = 0.0f;
        g_model_matrix_ball_movement_3.y = 0.0f;
        ifGameStop = true;
        ifLeftWin = true;
    } else if((g_model_matrix_ball_position_1.x + BALL_INIT_POS.x - 0.34f <= LEFT_BORDER) || (g_model_matrix_ball_position_2.x + BALL_INIT_POS.x - 0.34f <= LEFT_BORDER) || (g_model_matrix_ball_position_3.x + BALL_INIT_POS.x - 0.34f <= LEFT_BORDER)){
        g_model_matrix_ball_movement_1.x = 0.0f;
        g_model_matrix_ball_movement_1.y = 0.0f;
        g_model_matrix_ball_movement_2.x = 0.0f;
        g_model_matrix_ball_movement_2.y = 0.0f;
        g_model_matrix_ball_movement_3.x = 0.0f;
        g_model_matrix_ball_movement_3.y = 0.0f;
        ifGameStop = true;
        ifLeftWin = false;
    }
    
    // Check the mode switch
    if(singlePlayerModeOpened){
        if(g_model_matrix_right_paddle_position.y >= TOP_BORDER){
            g_model_matrix_right_paddle_position.y = TOP_BORDER;
            right_paddle_movement_single_mode = -right_paddle_movement_single_mode;
        }
        if(g_model_matrix_right_paddle_position.y <= BOTTOM_BORDER){
            g_model_matrix_right_paddle_position.y = BOTTOM_BORDER;
            right_paddle_movement_single_mode = -right_paddle_movement_single_mode;
        }
        
        if(ifGameStop){
            g_model_matrix_right_paddle_movement.y = 0.0f;
        }else {
            g_model_matrix_right_paddle_movement.y = right_paddle_movement_single_mode;
        }
    }
    
    // ————————————————————————————————————————————————————————————————— //
}

void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render() {
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    float vertices[] = {
            -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
            -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };
    
    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(),
                          2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
 
    // Render for Tennis Court as the background
    draw_object(g_model_matrix_court, g_tennis_court_id);
    if(ballnumber == 1){
        draw_object(g_model_matrix_ball_1, g_tennis_ball_id_1);
    }else if(ballnumber == 2){
        draw_object(g_model_matrix_ball_1, g_tennis_ball_id_1);
        draw_object(g_model_matrix_ball_2, g_tennis_ball_id_2);
    } else if(ballnumber ==3){
        draw_object(g_model_matrix_ball_1, g_tennis_ball_id_1);
        draw_object(g_model_matrix_ball_2, g_tennis_ball_id_2);
        draw_object(g_model_matrix_ball_3, g_tennis_ball_id_3);
    }
    draw_object(g_model_matrix_left_paddle, g_left_racket_id);
    draw_object(g_model_matrix_right_paddle, g_right_racket_id);
    if(ifGameStop && ifLeftWin){
        draw_object(g_model_matrix_left_win, g_left_win_id);
    }
    
    if(ifGameStop && !ifLeftWin){
        draw_object(g_model_matrix_right_win, g_right_win_id);
    }
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}


void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}
