// Old stuff
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES
#include <SDL.h>
#include <SDL_opengl.h>


// New stuff
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

// Constants
const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

// Background colours
const float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f;
const float BG_OPACITY = 1.0f;

// Viewport (i.e. our cameras)
const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

// Shader filepath
const char V_SHADER_PATH[] = "shaders/vertex.glsl",
F_SHADER_PATH[] = "shaders/fragment.glsl";




// Variables
// Old stuff
SDL_Window* display_window;
bool game_is_running = true;

// New stuff
ShaderProgram program;
glm::mat4 view_matrix; // Defines the position (location and direction) of the camera
glm::mat4 model_matrix; // Defines every single transformation we apply to a rendered object
glm::mat4 projection_matrix; // Defiens the characteristics of your camera (clip panes, field of view, projection method, etc.)


void initialise() {
	/* Old stuff */
	SDL_Init(SDL_INIT_VIDEO);
	display_window = SDL_CreateWindow("Hello, triangle",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(display_window);
	SDL_GL_MakeCurrent(display_window, context);

#ifdef _WINDOWS
	glewInit();
#endif

	/* New stuff */
	glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); // Init our camera
	program.Load(V_SHADER_PATH, F_SHADER_PATH); // Loads up our shaders

	// Init our view, model, and proj, matrices
	view_matrix = glm::mat4(1.0f); // Identity 4x4 matrix
	model_matrix = glm::mat4(1.0f); // Identity 4x4 matrix
	projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f); // Orthographic means perpendicular; our camera
																			 // will be looking perpendicularly down to our triangle
	// Once we've initialised these, we actually load them onto OpenGL
	program.SetViewMatrix(view_matrix);
	program.SetProjectionMatrix(projection_matrix);

	// Set the colour of our triangle
	program.SetColor(1.0f, 5.4f, 0.4f, 1.0f);

	glUseProgram(program.programID);

	glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
}

void process_input() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			// if the user quits or closes the window, our game loop is over
			game_is_running = false;
		}
	}
}

void update() { /* Nothing (yet), since there are no updates per frame to our scence */ }

void render() {
	// STEP 1
	glClear(GL_COLOR_BUFFER_BIT); // Use the background colour that we initialised

	// STEP 2
	program.SetModelMatrix(model_matrix); // load any current transformation donw onto a model

	// STEP 3
	float vertices[] = {
		0.5f, -0.5f,
		0.0f, 0.5f,
		-0.5f, -0.5f
	};
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(program.positionAttribute);

	// STEP 4
	SDL_GL_SwapWindow(display_window);
}

void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[]) {

	// STEP 1: Get everything set up and ready
	initialise();

	// STEP 2: Game loop
	while (game_is_running) {

		// STEP 3: Check for any input from the user
		process_input();

		// STEP 4: Update our scence, maybe using the nput from the user
		update();

		// STEP 5: Get those updates onto our current screen
		render();

	}

	// STEP 6: Shut our engine down
	shutdown();


	return 0;
}
