/*
	Game Name: Pong - Space WAR
	Cody Jiang
	zj2247@nyu.edu

	Press space bar to start
	Press W,S and UP,DOWN to move
	3 Points to WIN
*/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

#include <time.h>
using namespace std;

enum Coordinate {
	x_coordinate,
	y_coordinate
};

#define LOG(statement) std::cout << statement << '\n';

const int WINDOW_WIDTH = 1080,
WINDOW_HEIGHT = 720;

const float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f;
const float BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;


const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const char START_SPRITE[] = "start.png";
const char WINDOWS_SPRITE[] = "windows.png";
const char paddle_SPRITE[] = "paddle1.png";
const char paddle_SPRITE_2[] = "paddle2.png";
const char ball_SPRITE[] = "ball.png";
const char PLAYER1_WINS_SPRITE[] = "msg-player1-wins.png";
const char PLAYER2_WINS_SPRITE[] = "msg-player2-wins.png";

GLuint start_texture_id;
GLuint windows_texture_id;
GLuint paddle_texture_id;
GLuint paddle_texture_id_2;
GLuint ball_texture_id;
GLuint player1_wins_texture_id;
GLuint player2_wins_texture_id;

SDL_Window* display_window;
bool game_is_running = true;

ShaderProgram program;
glm::mat4 view_matrix;
glm::mat4 projection_matrix;
glm::mat4 model_matrix_start;
glm::mat4 model_matrix_p1;
glm::mat4 model_matrix_p2;
glm::mat4 model_matrix_ball;
glm::mat4 result_model_matrix;
glm::mat4 windows_model_matrix;

const char MAT_SEP = '\t';

/*
// Start at 0, 0, 0
glm::vec3 player_position = glm::vec3(0, 0, 0);

// Don't go anywhere (yet)
glm::vec3 player_movement = glm::vec3(0, 0, 0);
*/

// Paddle1:
glm::vec3 p1_position = glm::vec3(-4.75, 0, 0);
glm::vec3 p1_movement = glm::vec3(0, 0, 0);
float p1_speed = 3.5f;

// Paddle2:
glm::vec3 p2_position = glm::vec3(4.75, 0, 0);
glm::vec3 p2_movement = glm::vec3(0, 0, 0);
float p2_speed = 3.5f;

// Paddles
glm::vec3 p_scale = glm::vec3(0.75f, 1.55f, 1.0f);
float p_height = 1.0f * p_scale.y;
float p_width = 1.0f * p_scale.x;

// Ball:
glm::vec3 ball_position = glm::vec3(0, 0, 0);
glm::vec3 ball_movement = glm::vec3(0, 0, 0);
glm::vec3 ball_scale = glm::vec3(0.35f, 0.35f, 1.0f);
float ball_height = 1.0f * ball_scale.y;
float ball_width = 1.0f * ball_scale.x;

float ball_speed = 5.5f;
float ball_rotate = 1.0f;
bool ball_path_reversed = false;

SDL_Joystick* playerOneController;


const float GROWTH_FACTOR = 1.01f;  // growth rate of 1.0% per frame
const float SHRINK_FACTOR = 0.99f;  // growth rate of -1.0% per frame
const int MAX_FRAME = 40;           // this value is, of course, up to you

const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0; // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0; // this value MUST be zero


GLuint load_texture(const char* filepath)
{
	// STEP 1: Loading the image file
	int width, height, number_of_components;
	unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

	if (image == NULL)
	{
		LOG("Unable to load image. Make sure the path is correct.");
		assert(false);
	}

	// STEP 2: Generating and binding a texture ID to our image
	GLuint textureID;
	glGenTextures(NUMBER_OF_TEXTURES, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

	// STEP 3: Setting our texture filter parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// STEP 4: Releasing our file from memory and returning our texture id
	stbi_image_free(image);

	return textureID;
}

void initialise() {
	// Initialise video and joystick subsystem
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

	// Open the first controller found. Reruen null on error
	playerOneController = SDL_JoystickOpen(0);

	display_window = SDL_CreateWindow("Pong - Space WAR",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(display_window);
	SDL_GL_MakeCurrent(display_window, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); // Init our camera

	// Load the shaders for handling textures
	program.Load(V_SHADER_PATH, F_SHADER_PATH);

	// Load our player image
	start_texture_id = load_texture(START_SPRITE);
	windows_texture_id = load_texture(WINDOWS_SPRITE);
	paddle_texture_id = load_texture(paddle_SPRITE);
	paddle_texture_id_2 = load_texture(paddle_SPRITE_2);
	ball_texture_id = load_texture(ball_SPRITE);
	player1_wins_texture_id = load_texture(PLAYER1_WINS_SPRITE);
	player2_wins_texture_id = load_texture(PLAYER2_WINS_SPRITE);

	// enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Init our view, model, and proj, matrices
	view_matrix = glm::mat4(1.0f); // Identity 4x4 matrix
	model_matrix_ball = glm::mat4(1.0f); // Identity 4x4 matrix
	windows_model_matrix = glm::mat4(1.0f);
	result_model_matrix = glm::mat4(1.0f);
	model_matrix_start = glm::mat4(1.0f);
	
	projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
	
	model_matrix_start = glm::scale(model_matrix_start, glm::vec3(10.0f, 10.0f, 5.0f));
	windows_model_matrix = glm::scale(windows_model_matrix, glm::vec3(10.0f, 10.0f, 5.0f)); // enlarge the windows texture
	result_model_matrix = glm::scale(result_model_matrix, glm::vec3(5.0f, 1.0f, 1.0f)); // enlarge the result texture

	//model_matrix = glm::rotate(model_matrix, ROT_ANGLE, glm::vec3(0.0f, 0.0f, 1.0f));

	program.SetViewMatrix(view_matrix);
	program.SetProjectionMatrix(projection_matrix);

	glUseProgram(program.programID);

	glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

}


bool reachingTop(glm::vec3 position, float height, float top) {
	return !((position.y + (height / 2.0f)) < top);
}

bool reachingBottom(glm::vec3 position, float height, float bottom) {
	return !((position.y - (height / 2.0f)) > bottom);
}


void process_input() {

	p1_movement = glm::vec3(0);
	p2_movement = glm::vec3(0);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			game_is_running = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_LEFT:
				break;
			case SDLK_RIGHT:
				break;
			case SDLK_SPACE:
				break;
			}
			break;
		}
	}

	const Uint8* key_states = SDL_GetKeyboardState(NULL); // array of the key states [0, 0, 1, 0, 0, ...]

	// Ball
	if (key_states[SDL_SCANCODE_SPACE]) {
		srand(time(0));                     // random seed
		int slope = (rand() % 3) - 1;       // spice up the initial throw so it's not the same every time
		ball_movement.x = 1.0f * slope;
		ball_movement.y = 1.0f * slope;
	}
	if (glm::length(ball_movement) > 1.0f) {
		ball_movement = glm::normalize(ball_movement);
	}

	// Paddle 1
	if ((key_states[SDL_SCANCODE_W]) && !reachingTop(p1_position, p_height, 3.7f)) {
		p1_movement.y = 1.0f;
	}
	else if ((key_states[SDL_SCANCODE_S]) && !reachingBottom(p1_position, p_height, -3.7f)) {
		p1_movement.y = -1.0f;
	}
	if (glm::length(p1_movement) > 1.0f) {
		p1_movement = glm::normalize(p1_movement);
	}

	// Paddle 2
	if ((key_states[SDL_SCANCODE_UP]) && !reachingTop(p2_position, p_height, 3.7f)) {
		p2_movement.y = 1.0f;
	}
	else if ((key_states[SDL_SCANCODE_DOWN]) && !reachingBottom(p2_position, p_height, -3.7f)) {
		p2_movement.y = -1.0f;
	}
	if (glm::length(p2_movement) > 1.0f) {
		p2_movement = glm::normalize(p2_movement);
	}
}

bool Collision_Detection(glm::vec3 ball_position, glm::vec3 p_position) {

	float x_coord_a = ball_position.x;
	float y_coord_a = ball_position.y;
	float width_a = ball_width - 0.2f;
	float height_a = ball_height - 0.2f;


	float x_coord_b = p_position.x;
	float y_coord_b = p_position.y;
	float width_b = p_width;
	float height_b = p_height;


	float x_distance = fabs(x_coord_a - x_coord_b) - ((width_a + width_b) / 2.0f);
	float y_distance = fabs(y_coord_a - y_coord_b) - ((height_a + height_b) / 2.0f);

	return (x_distance < 0 && y_distance < 0);
}

int frame_counter = 0;
bool is_growing = true;

const float ROT_ANGLE = glm::radians(1.5f);
float TRANS_VALUE = 0.025f;

float MILLISECONDS_IN_SECOND = 1000.0f;
float previous_ticks = 0.0f;
float player_speed = 1.0f;


int p1_score, p2_score = 0;


bool is_ball_die(glm::vec3 ball_position) {
	float left = -4.8f; 
	float right = 4.8f;

	if (ball_position.x > right) {
		p1_score += 1;
		return true;
	}
	else if (ball_position.x < left) {
		p2_score += 1;
		return true;
	}
	return false;
}

void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
	program.SetModelMatrix(object_model_matrix);
	glBindTexture(GL_TEXTURE_2D, object_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}


void update()
{

	if (is_ball_die(ball_position)) {
		if ((p1_score == 3 || p2_score == 3)) { // 3 points to win
			game_is_running = false;
		}
		else
			ball_position = glm::vec3(0, 0, 0);
	}

	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float delta_time = ticks - previous_ticks;
	previous_ticks = ticks;

	// ball
	model_matrix_ball = glm::mat4(1.0f);

	if ((reachingTop(ball_position, ball_height, 3.7f) || reachingBottom(ball_position, ball_height, -3.7f))) {
		ball_movement.y *= -1.0f;
	}
	else if ((Collision_Detection(ball_position, p1_position)) || (Collision_Detection(ball_position, p2_position))) {
		ball_movement.x *= -1.0f;
	}

	ball_position += ball_movement * ball_speed * delta_time;
	model_matrix_ball = glm::translate(model_matrix_ball, ball_position);
	model_matrix_ball = glm::scale(model_matrix_ball, ball_scale);

	 // paddle1
	p1_position += p1_movement * p1_speed * delta_time;
	model_matrix_p1 = glm::mat4(1.0f);
	model_matrix_p1 = glm::translate(model_matrix_p1, p1_position);
	model_matrix_p1 = glm::scale(model_matrix_p1, p_scale);


	 // paddle2
	p2_position += p2_movement * p2_speed * delta_time;
	model_matrix_p2 = glm::mat4(1.0f);
	model_matrix_p2 = glm::translate(model_matrix_p2, p2_position);
	model_matrix_p2 = glm::scale(model_matrix_p2, p_scale);

}


void render() {
	glClear(GL_COLOR_BUFFER_BIT);

	//program.SetModelMatrix(model_matrix);

	// Vertices
	float vertices[] = {
		-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
		-0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
	};

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);

	// Textures
	float texture_coordinates[] = {
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
	};

	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates);
	glEnableVertexAttribArray(program.texCoordAttribute);

	draw_object(windows_model_matrix, windows_texture_id);
	draw_object(model_matrix_p1, paddle_texture_id);
	draw_object(model_matrix_p2, paddle_texture_id_2);
	draw_object(model_matrix_ball, ball_texture_id);


	// Bind texture
	//glBindTexture(GL_TEXTURE_2D, player_texture_id);
	//glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3

	// We disable two attribute arrays now
	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

	SDL_GL_SwapWindow(display_window);
}


void result_render() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//program.SetModelMatrix(model_matrix);

	// Vertices
	float vertices[] = {
		-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
		-0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
	};

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);

	// Textures
	float texture_coordinates[] = {
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
	};

	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates);
	glEnableVertexAttribArray(program.texCoordAttribute);

	if (p1_score > p2_score) {
		draw_object(result_model_matrix, player1_wins_texture_id);
	}
	else
		draw_object(result_model_matrix, player2_wins_texture_id);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

	SDL_GL_SwapWindow(display_window);
}

void start_render() {

	glClear(GL_COLOR_BUFFER_BIT);

	//program.SetModelMatrix(model_matrix);

	// Vertices
	float vertices[] = {
		-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
		-0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
	};

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);

	// Textures
	float texture_coordinates[] = {
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
	};

	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates);
	glEnableVertexAttribArray(program.texCoordAttribute);

	draw_object(model_matrix_start, start_texture_id);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

	SDL_GL_SwapWindow(display_window);
}

void shutdown() {
	SDL_JoystickClose(playerOneController);
	SDL_Quit();
}

void sleepcp(int milliseconds) // Cross-platform sleep function
{
	clock_t time_end;
	time_end = clock() + milliseconds * CLOCKS_PER_SEC / 1000;
	while (clock() < time_end)
	{
	}
}

int main(int argc, char* argv[]) {

	// STEP 1: Get everything set up and ready
	initialise();

	start_render();
	sleepcp(3500);

	// STEP 2: Game loop
	while (game_is_running) {

		// STEP 3: Check for any input from the user
		process_input();

		// STEP 4: Update our scence, maybe using the nput from the user

		update();
		// STEP 5: Get those updates onto our current screen
		render();

	}
	
	result_render(); 
	sleepcp(5000);

	// STEP 6: Shut our engine down
	shutdown();

	return 0;
}
