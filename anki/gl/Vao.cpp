#include "anki/gl/Vao.h"
#include "anki/gl/Vbo.h"
#include "anki/util/Exception.h"
#include "anki/resource/ShaderProgramAttributeVariable.h"


namespace anki {


//==============================================================================
// Destructor                                                                  =
//==============================================================================
Vao::~Vao()
{
	if(isCreated())
	{
		destroy();
	}
}


//==============================================================================
// attachArrayBufferVbo                                                        =
//==============================================================================
void Vao::attachArrayBufferVbo(const Vbo& vbo, uint attribVarLocation,
	GLint size, GLenum type, GLboolean normalized, GLsizei stride,
	const GLvoid* pointer)
{
	ASSERT(isCreated());
	if(vbo.getBufferTarget() != GL_ARRAY_BUFFER)
	{
		throw ANKI_EXCEPTION("Only GL_ARRAY_BUFFER is accepted");
	}

	ANKI_CHECK_GL_ERROR();

	bind();
	vbo.bind();
	glVertexAttribPointer(attribVarLocation, size, type, normalized,
		stride, pointer);
	glEnableVertexAttribArray(attribVarLocation);
	vbo.unbind();
	unbind();

	ANKI_CHECK_GL_ERROR();
}


//==============================================================================
// attachArrayBufferVbo                                                        =
//==============================================================================
void Vao::attachArrayBufferVbo(const Vbo& vbo,
	const ShaderProgramAttributeVariable& attribVar,
	GLint size, GLenum type, GLboolean normalized, GLsizei stride,
	const GLvoid* pointer)
{
	attachArrayBufferVbo(vbo, attribVar.getLocation(), size, type, normalized,
		stride, pointer);
}


//==============================================================================
// attachElementArrayBufferVbo                                                 =
//==============================================================================
void Vao::attachElementArrayBufferVbo(const Vbo& vbo)
{
	ASSERT(isCreated());
	if(vbo.getBufferTarget() != GL_ELEMENT_ARRAY_BUFFER)
	{
		throw ANKI_EXCEPTION("Only GL_ELEMENT_ARRAY_BUFFER is accepted");
	}

	bind();
	vbo.bind();
	unbind();
	ANKI_CHECK_GL_ERROR();
}


} // end namespace
