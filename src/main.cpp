#include <glad/gl.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <print>
#include <string>
#include <unordered_map>

const GLuint WIDTH = 800, HEIGHT = 600;

typedef unsigned int uint;

uint compileShader(const std::string& shaderSrc, const uint shaderType) {
	static std::unordered_map<uint, std::string> reverseGlEnum{
	    // {GL_COMPUTE_SHADER, "GL_COMPUTE_SHADER"},
	    {GL_VERTEX_SHADER,   "GL_VERTEX_SHADER"  },
	    // {GL_TESS_CONTROL_SHADER, "GL_TESS_CONTROL_SHADER"},
	    // {GL_TESS_EVALUATION_SHADER, "GL_TESS_EVALUATION_SHADER"},
	    {GL_GEOMETRY_SHADER, "GL_GEOMETRY_SHADER"},
	    {GL_FRAGMENT_SHADER, "GL_FRAGMENT_SHADER"},
	};

	auto asPtr = shaderSrc.c_str();
	uint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &asPtr, nullptr);
	glCompileShader(shader);
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (not success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		throw std::runtime_error(std::format("ERROR: failed to compile {} shader: {}.",
		                                     reverseGlEnum[shaderType], infoLog));
	}
	return shader;
}

int main(void) {
	// code without checking for errors
	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Window* window = SDL_CreateWindow("[glad] GL with SDL", WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	SDL_SetWindowResizable(window, true);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
	std::println("GLAD version: {}", version);

	glViewport(0, 0, WIDTH, HEIGHT);

	const char vertexShaderSrc[] = {
#embed "./shaders/vertexShader.vert.glsl"
	    , 0};
	const char fragmentShaderSrc[] = {
#embed "./shaders/fragmentShader.frag.glsl"
	    , 0};

	uint vertexShader = compileShader(std::string(vertexShaderSrc), GL_VERTEX_SHADER);
	uint fragmentShader = compileShader(std::string(fragmentShaderSrc), GL_FRAGMENT_SHADER);

	uint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (not success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		throw std::runtime_error(std::format("ERROR: failed to link shader program: {}.", infoLog));
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	constexpr float vertices[] = {
	    // clang-format off
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	    // clang-format on
	};

	uint vertBufObj;
	uint vertAttribObj;
	glGenVertexArrays(1, &vertAttribObj);
	glGenBuffers(1, &vertBufObj);

	glBindVertexArray(vertAttribObj);
	glBindBuffer(GL_ARRAY_BUFFER, vertBufObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	std::println("vbo: {}; vao: {}", vertBufObj, vertAttribObj);

	int exit = 0;
	while (not exit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT: exit = 1; break;
			case SDL_EVENT_KEY_UP:
				if (event.key.key == SDLK_ESCAPE) {
					exit = 1;
				}
				break;
			default: break;
			}
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(vertAttribObj);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		SDL_GL_SwapWindow(window);
		SDL_Delay(1);
	}

	glDeleteVertexArrays(1, &vertAttribObj);
	glDeleteBuffers(1, &vertBufObj);
	glDeleteProgram(shaderProgram);

	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
