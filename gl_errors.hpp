#pragma once

#include "GL.hpp"
#include <iostream>

#define STR2(X) # X
#define STR(X) STR2(X)

inline void gl_errors(std::string const &where) {
	GLenum err = 0;
	while ((err = glGetError()) != GL_NO_ERROR) {
		#define CHECK( ERR ) \
			if (err == ERR) { \
				std::cerr << "WARNING: gl error '" #ERR "' at " << where << std::endl; \
			} else

		CHECK( GL_INVALID_ENUM )
		CHECK( GL_INVALID_VALUE )
		CHECK( GL_INVALID_OPERATION )
		CHECK( GL_INVALID_FRAMEBUFFER_OPERATION )
		CHECK( GL_OUT_OF_MEMORY )
#ifndef __EMSCRIPTEN__
		//GLES3 (and therefore WebGL2) has no stack-based GL state, so these two
		//enums do not exist there -- naming them is a compile error, not a warning:
		CHECK( GL_STACK_UNDERFLOW )
		CHECK( GL_STACK_OVERFLOW )
#endif
		{
			std::cerr << "WARNING: gl error '" << err << "'" << std::endl;
		}
		#undef CHECK
	}
}
#define GL_ERRORS() gl_errors(__FILE__  ":" STR(__LINE__) )

