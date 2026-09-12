//Mode.hpp declares the "Mode::current" static member variable, which is used to decide where event-handling, updating, and drawing events go:
#include "Mode.hpp"

//The 'PlayMode' mode plays the game:
#include "PlayMode.hpp"

//For asset loading:
#include "Load.hpp"

//GL.hpp will include a non-namespace-polluting set of opengl prototypes:
#include "GL.hpp"

//for screenshots: (not built for web -- see the PrintScreen handler below)
#ifndef __EMSCRIPTEN__
#include "load_save_png.hpp"
#endif

//Includes for libSDL:
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

//...and for c++ standard library functions:
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <algorithm>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef _WIN32
extern "C" { uint32_t GetACP(); }
#endif

//These live at file scope because the web build hands one_frame() to the browser,
//which calls it long after main() has returned:
glm::uvec2 window_size; //size of window (layout pixels)
glm::uvec2 drawable_size; //size of drawable (physical pixels)
//On non-highDPI displays, window_size will always equal drawable_size.

//called whenever the window is resized; updates window_size and drawable_size:
void on_resize() {
	int w,h;
	SDL_GetWindowSize(Mode::window, &w, &h);
	window_size = glm::uvec2(w, h);
	SDL_GetWindowSizeInPixels(Mode::window, &w, &h);
	drawable_size = glm::uvec2(w, h);
	glViewport(0, 0, drawable_size.x, drawable_size.y);
}

//one pass through the game loop; creates one frame of output:
void one_frame() {
	if (!Mode::current) return;

	{ //(1) process any events that are pending
		static SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			//ignore key repeat:
			if (evt.type == SDL_EVENT_KEY_DOWN && evt.key.repeat) {
				continue;
			}
			//handle resizing:
			if (evt.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
				on_resize();
			}
			//handle input:
			if (Mode::current && Mode::current->handle_event(evt, window_size)) {
				// mode handled it; great
			} else if (evt.type == SDL_EVENT_QUIT) {
				Mode::set_current(nullptr);
				break;
#ifndef __EMSCRIPTEN__
			} else if (evt.type == SDL_EVENT_KEY_DOWN && evt.key.key == SDLK_PRINTSCREEN) {
				// --- screenshot key --- (no front buffer to read in WebGL2, so web build skips this)
				std::string filename = "screenshot.png";
				std::cout << "Saving screenshot to '" << filename << "'." << std::endl;
				glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
				glReadBuffer(GL_FRONT);
				int w,h;
				SDL_GetWindowSizeInPixels(Mode::window, &w, &h);
				std::vector< glm::u8vec4 > data(w*h);
				glReadPixels(0,0,w,h, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
				for (auto &px : data) {
					px.a = 0xff;
				}
				save_png(filename, glm::uvec2(w,h), data.data(), LowerLeftOrigin);
#endif
			}
		}
		if (!Mode::current) return;
	}

	{ //(2) call the current mode's "update" function to deal with elapsed time:
		auto current_time = std::chrono::high_resolution_clock::now();
		static auto previous_time = current_time;
		float elapsed = std::chrono::duration< float >(current_time - previous_time).count();
		previous_time = current_time;

		//if frames are taking a very long time to process,
		//lag to avoid spiral of death:
		elapsed = std::min(0.1f, elapsed);

		Mode::current->update(elapsed);
		if (!Mode::current) return;
	}

	{ //(3) call the current mode's "draw" function to produce output:

		Mode::current->draw(drawable_size);
	}

	//Wait until the recently-drawn frame is shown before doing it all again:
	SDL_GL_SwapWindow(Mode::window);
}

int main(int argc, char **argv) {
#ifdef _WIN32
	{ //when compiled on windows, check that code page is forced to utf-8 (makes file loading/saving work right):
		//see: https://docs.microsoft.com/en-us/windows/apps/design/globalizing/use-utf8-code-page
		uint32_t code_page = GetACP();
		if (code_page == 65001) {
			std::cout << "Code page is properly set to UTF-8." << std::endl;
		} else {
			std::cout << "WARNING: code page is set to " << code_page << " instead of 65001 (UTF-8). Some file handling functions may fail." << std::endl;
		}
	}

	//when compiled on windows, unhandled exceptions don't have their message printed, which can make debugging simple issues difficult.
	try {
#endif

	//------------  initialization ------------

	//Initialize SDL library:
	SDL_Init(SDL_INIT_VIDEO);

	//Ask for an OpenGL context version 3.3, core profile, enable debug:
	SDL_GL_ResetAttributes();
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	//create window:
	Mode::window = SDL_CreateWindow(
		"gp26 game1: Watch for The Droplet",
		2*PPU466::ScreenWidth + 8, 2*PPU466::ScreenHeight + 8,
		SDL_WINDOW_OPENGL
		| SDL_WINDOW_RESIZABLE //uncomment to allow resizing
		| SDL_WINDOW_HIGH_PIXEL_DENSITY //uncomment for full resolution on high-DPI screens
	);

	//prevent exceedingly tiny windows when resizing:
	SDL_SetWindowMinimumSize(Mode::window, PPU466::ScreenWidth, PPU466::ScreenHeight);

	if (!Mode::window) {
		std::cerr << "Error creating SDL window: " << SDL_GetError() << std::endl;
		return 1;
	}

	//Create OpenGL context:
	SDL_GLContext context = SDL_GL_CreateContext(Mode::window);

	if (!context) {
		SDL_DestroyWindow(Mode::window);
		std::cerr << "Error creating OpenGL context: " << SDL_GetError() << std::endl;
		return 1;
	}

	//On windows, load OpenGL entrypoints: (does nothing on other platforms)
	//Emscripten links the GL entry points directly, so there is nothing to look up:
#ifndef __EMSCRIPTEN__
	init_GL();
#endif

	//Set VSYNC + Late Swap (prevents crazy FPS):
	if (!SDL_GL_SetSwapInterval(-1)) {
		std::cerr << "NOTE: couldn't set vsync + late swap tearing (" << SDL_GetError() << ")." << std::endl;
		if (!SDL_GL_SetSwapInterval(1)) {
			std::cerr << "NOTE: couldn't set vsync (" << SDL_GetError() << ")." << std::endl;
		}
	}

	//Hide mouse cursor (note: showing can be useful for debugging):
	//SDL_ShowCursor(SDL_DISABLE);

	//------------ load assets --------------
	call_load_functions();

	//------------ create game mode + make current --------------
	Mode::set_current(std::make_shared< PlayMode >());

	//------------ main loop ------------

	on_resize();

#ifdef __EMSCRIPTEN__
	//A browser tab runs JS, layout and painting on one thread. Blocking in a loop here
	//would never hand that thread back, so the canvas would never paint. Instead, give
	//the browser one_frame and return; it calls it ~60x/sec via requestAnimationFrame.
	//Teardown below is unreachable on purpose -- the window must outlive main():
	emscripten_set_main_loop(one_frame, 0, 0);
	return 0;
#else
	//This will loop until the current mode is set to null:
	while (Mode::current) {
		one_frame();
	}
#endif


	//------------  teardown ------------

	SDL_GL_DestroyContext(context);
	context = 0;

	SDL_DestroyWindow(Mode::window);
	Mode::window = NULL;

	return 0;

#ifdef _WIN32
	} catch (std::exception const &e) {
		std::cerr << "Unhandled exception:\n" << e.what() << std::endl;
		return 1;
	} catch (...) {
		std::cerr << "Unhandled exception (unknown type)." << std::endl;
		throw;
	}
#endif
}
