// Copyright (C) 2014, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#ifndef ANKI_GL_GL_ERROR_H
#define ANKI_GL_GL_ERROR_H

#include "anki/Config.h"
#include "anki/gl/GlCommon.h"

namespace anki {

/// @addtogroup opengl_private
/// @{

// Enable the exception on debug. Calling glGetError calls serialization

#if ANKI_DEBUG

/// The function throws an exception if there is an OpenGL error. Use it with
/// the ANKI_CHECK_GL_ERROR macro
void glConditionalThrowException(const char* file, int line, const char* func);

#	define ANKI_CHECK_GL_ERROR() \
		glConditionalThrowException(ANKI_FILE, __LINE__, ANKI_FUNC)
#else
#	define ANKI_CHECK_GL_ERROR() ((void)0)
#endif

/// @}

} // end namespace anki

#endif
