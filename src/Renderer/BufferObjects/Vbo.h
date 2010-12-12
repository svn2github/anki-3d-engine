#ifndef VBO_H
#define VBO_H

#include "BufferObject.h"


/// This is a wrapper for Vertex Buffer Objects to prevent us from making idiotic errors
class Vbo: public BufferObject
{
	public:
		Vbo() {}

		/// Adds an extra check in target_ @see BufferObject::BufferObject
		Vbo(GLenum target_, uint sizeInBytes, const void* dataPtr, GLenum usage_);

		/// Unbinds all VBOs, meaning both GL_ARRAY_BUFFER and GL_ELEMENT_ARRAY_BUFFER targets
		static void unbindAllTargets();

		/// @see BufferObject::create
		void create(GLenum target, uint sizeInBytes, const void* dataPtr, GLenum usage);
};


inline Vbo::Vbo(GLenum target_, uint sizeInBytes_, const void* dataPtr_, GLenum usage_)
{
	create(target_, sizeInBytes_, dataPtr_, usage_);
}


inline void Vbo::create(GLenum target_, uint sizeInBytes_, const void* dataPtr_, GLenum usage_)
{
	RASSERT_THROW_EXCEPTION(target_ != GL_ARRAY_BUFFER && target_ != GL_ELEMENT_ARRAY_BUFFER); // unacceptable target_
	BufferObject::create(target_, sizeInBytes_, dataPtr_, usage_);
}


inline void Vbo::unbindAllTargets()
{
	glBindBufferARB(GL_ARRAY_BUFFER, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
}


#endif
