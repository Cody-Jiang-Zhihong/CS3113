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


#define LOG(statement) std::cout << statement << '\n';

const int WINDOW_WIDTH = 720,
WINDOW_HEIGHT = 540;

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

const char WINDOW_SPRITE[] = "window.png"; // window texture
const char PLAYER_SPRITE[] = "player.png";
const char PLAYER_SPRITE_2[] = "player2.png"; // second object texture

// effect texture
const char FIRE_SPRITE[] = "fire.png";
GLuint fire_texture_id;
glm::mat4 model_matrix_fire;

const char THUNDER_SPRITE[] = "thunder.png";
GLuint thunder_texture_id;
glm::mat4 model_matrix_thunder;

const char BALL_SPRITE[] = "pokeball.png";
GLuint ball_texture_id;
glm::mat4 model_matrix_ball;



GLuint window_texture_id;
GLuint player_texture_id;
GLuint player_texture_id_2;


SDL_Window* display_window;
bool game_is_running = true;

ShaderProgram program;
glm::mat4 view_matrix;

glm::mat4 model_matrix_window;
glm::mat4 model_matrix;
glm::mat4 model_matrix_2;

glm::mat4 projection_matrix;

const char MAT_SEP = '\t';

void print_matrix(glm::mat4& matrix, int size)
{
	for (auto row = 0; row < size; row++)
	{
		for (auto col = 0; col < size; col++)
		{
			// Print row
			std::cout << matrix[row][col] << MAT_SEP;
		}

		// Visually "move on" to the next row
		std::cout << "\n";
	}
}

/* Some code... */



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
	SDL_Init(SDL_INIT_VIDEO);


	display_window = SDL_CreateWindow("Pikachu VS Charizard",
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

	// Load our window and player image
	window_texture_id = load_texture(WINDOW_SPRITE);
	fire_texture_id = load_texture(FIRE_SPRITE);
	thunder_texture_id = load_texture(THUNDER_SPRITE);
	ball_texture_id = load_texture(BALL_SPRITE);

	player_texture_id = load_texture(PLAYER_SPRITE);
	player_texture_id_2 = load_texture(PLAYER_SPRITE_2);

	// enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	
	// Init our view, model, and proj, matrices
	view_matrix = glm::mat4(1.0f); // Identity 4x4 matrix

	model_matrix_window = glm::mat4(1.0f);

	model_matrix = glm::mat4(1.0f); // Identity 4x4 matrix
	model_matrix_2 = glm::mat4(1.0f); // Identity 4x4 matrix

	model_matrix_fire = glm::mat4(1.0f);
	model_matrix_thunder = glm::mat4(1.0f);
	model_matrix_ball = glm::mat4(1.0f);

	projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetViewMatrix(view_matrix);
	program.SetProjectionMatrix(projection_matrix);

	model_matrix_window = glm::scale(model_matrix_window, glm::vec3(35.0f, 35.0f, 35.0f)); // enlarge the window texture
	model_matrix = glm::scale(model_matrix, glm::vec3(3.0f, 3.0f, 3.0f));
	model_matrix_2 = glm::scale(model_matrix_2, glm::vec3(3.0f, 3.0f, 3.0f));
	model_matrix_fire = glm::scale(model_matrix_fire, glm::vec3(3.0f, 3.0f, 3.0f));
	model_matrix_thunder = glm::scale(model_matrix_thunder, glm::vec3(3.0f, 3.0f, 3.0f));
	//model_matrix_ball = glm::scale(model_matrix_ball, glm::vec3(3.0f, 3.0f, 3.0f));

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


const float ROT_ANGLE = glm::radians(1.5f); // Let's try a smaller angle
float TRANS_VALUE = 0.025f;


const float GROWTH_FACTOR = 0.01f;  // growth rate of 1.0% per frame
const float SHRINK_FACTOR = 0.99f;  // growth rate of -1.0% per frame
const int MAX_FRAME = 120;           // this value is, of course, up to you

int frame_counter = 0;
bool is_growing = true;
bool is_growing_rotate = true;


void update()
{
	glm::vec3 scale_vector;
	glm::vec3 scale_vector_window;
	frame_counter += 1;

	if (frame_counter >= MAX_FRAME)
	{
		//is_growing = !is_growing;
		//frame_counter = 0;
		is_growing = false;
	}

	scale_vector = glm::vec3(is_growing ? 0.01f : 0.0f, 0.0f, 0.0f);
	model_matrix = glm::translate(model_matrix, scale_vector);

	scale_vector = glm::vec3(is_growing ? -0.01f : 0.0f, 0.0f, 0.0f);
	model_matrix_2 = glm::translate(model_matrix_2, scale_vector);


	model_matrix_fire = glm::translate(model_matrix_fire, glm::vec3(-0.01f, 0.0f, 0.0f));
	model_matrix_thunder = glm::translate(model_matrix_thunder, glm::vec3(0.01f, 0.0f, 0.0f));
	model_matrix_ball = glm::translate(model_matrix_ball, glm::vec3(0.00f, 0.01f, 0.0f));

	float angle = glm::radians(5.0f);

	// Every frame, rotate my model by an angle of ROT_ANGLE on the z-axis
	model_matrix_fire = glm::rotate(model_matrix_fire, glm::radians(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model_matrix_thunder = glm::rotate(model_matrix_thunder, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	
	model_matrix_ball = glm::rotate(model_matrix_ball, angle, glm::vec3(0.0f, 0.0f, 1.0f));
	//model_matrix = glm::rotate(model_matrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));
	//model_matrix_2 = glm::rotate(model_matrix_2, angle, glm::vec3(0.0f, 0.0f, 1.0f));

	if (frame_counter >= MAX_FRAME)
	{
		is_growing_rotate = !is_growing_rotate;
		frame_counter = 0;
	}
	scale_vector = glm::vec3(is_growing_rotate ? 1.01f : 0.99f, is_growing_rotate ? 1.01f : 0.99f, 0.0f);
	model_matrix_ball = glm::scale(model_matrix_ball, scale_vector);
	//model_matrix_fire = glm::scale(model_matrix_fire, scale_vector);
	//model_matrix = glm::scale(model_matrix, scale_vector);
	//model_matrix_2 = glm::scale(model_matrix_2, scale_vector);
}

void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
	program.SetModelMatrix(object_model_matrix);
	glBindTexture(GL_TEXTURE_2D, object_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT);

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

	draw_object(model_matrix_window, window_texture_id);

	draw_object(model_matrix, player_texture_id);
	draw_object(model_matrix_2, player_texture_id_2);

	draw_object(model_matrix_fire, fire_texture_id);
	draw_object(model_matrix_thunder, thunder_texture_id);
	draw_object(model_matrix_ball, ball_texture_id);

	// We disable two attribute arrays now
	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

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
