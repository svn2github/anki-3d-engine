#ifndef _BUFFEROBJECT_H_
#define _BUFFEROBJECT_H_

#include "Common.h"
#include <GL/glew.h>


/**
 * A wrapper for OpenGL buffer objects (vertex arrays, texture buffers etc) to prevent us from making idiotic errors
 */
class BufferObject
{
	protected:
		uint glId; ///< The OpenGL id of the BO
		GLenum target; ///< Used in glBindBuffer(target, glId) and its for easy access so we wont have to query the GL driver
		GLenum usage; ///< GL_STREAM_DRAW or GL_STATIC_DRAW or GL_DYNAMIC_DRAW

	public:
		BufferObject();
		virtual ~BufferObject();

		/**
		 * Accessor. Throws an assertion error in BO is not created
		 * @return The OpenGL ID of the buffer
		 */
		uint getGlId() const;

		/**
		 * Accessor. Throws an assertion error in BO is not created
		 * @return
		 */
		GLenum getBufferTarget() const;

		/**
		 * Accessor. Throws an assertion error in BO is not created
		 * @return GL_STREAM_DRAW or GL_STATIC_DRAW or GL_DYNAMIC_DRAW
		 */
		GLenum getBufferUsage() const;

		/**
		 * @brief Checks if BO is created
		 * @return True if BO is already created
		 */
		bool isCreated() const;

		/**
		 * Creates a new BO with the given params and checks if everything went OK
		 * @param target_ Depends on the BO
		 * @param sizeInBytes The size of the buffer that we will allocate in bytes
		 * @param dataPtr Points to the data buffer to copy to the VGA memory. Put NULL if you want just to allocate memory
		 * @param usage_ It should be: GL_STREAM_DRAW or GL_STATIC_DRAW or GL_DYNAMIC_DRAW only!!!!!!!!!
		 * @return True on success
		 */
		bool create(GLenum target_, uint sizeInBytes, const void* dataPtr, GLenum usage_);

		/**
		 * Throws an assertion error in BO is not created
		 */
		void bind() const;

		/**
		 * Throws an assertion error in BO is not created
		 */
		void unbind() const;

		/**
		 * Self explanatory. Throws an assertion error in BO is not created
		 */
		void deleteBuff();
};


//======================================================================================================================
// Inlines                                                                                                             =
//======================================================================================================================

inline BufferObject::BufferObject():
	glId(0)
{}


inline BufferObject::~BufferObject()
{
	if(glId!=0)
		deleteBuff();
}

inline uint BufferObject::getGlId() const
{
	DEBUG_ERR(!isCreated());
	return glId;
}


inline GLenum BufferObject::getBufferTarget() const
{
	DEBUG_ERR(!isCreated());
	return target;
}


inline GLenum BufferObject::getBufferUsage() const
{
	DEBUG_ERR(!isCreated());
	return usage;
}


inline bool BufferObject::isCreated() const
{
	return glId != 0;
}


inline bool BufferObject::create(GLenum target_, uint sizeInBytes, const void* dataPtr, GLenum usage_)
{
	DEBUG_ERR(isCreated()); // BO already initialized
	DEBUG_ERR(usage_!=GL_STREAM_DRAW && usage_!=GL_STATIC_DRAW && usage_!=GL_DYNAMIC_DRAW); // unacceptable usage_
	DEBUG_ERR(sizeInBytes < 1); // unacceptable sizeInBytes

	usage = usage_;
	target = target_;

	glGenBuffers(1, &glId);
	bind();
	glBufferData(target, sizeInBytes, dataPtr, usage);

	// make a check
	int bufferSize = 0;
	glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufferSize);
	if(sizeInBytes != (uint)bufferSize)
	{
		deleteBuff();
		ERROR("Data size mismatch");
		return false;
	}

	unbind();
	return true;
}


inline void BufferObject::bind() const
{
	DEBUG_ERR(!isCreated());
	glBindBuffer(target, glId);
}


inline void BufferObject::unbind() const
{
	DEBUG_ERR(!isCreated());
	glBindBuffer(target, 0);
}


inline void BufferObject::deleteBuff()
{
	DEBUG_ERR(!isCreated());
	glDeleteBuffers(1, &glId);
	glId = 0;
}

#endif
