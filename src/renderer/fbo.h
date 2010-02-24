#ifndef _FBO_H_
#define _FBO_H_

#include "common.h"
#include "renderer.h"


/// The class is created as a wrapper to avoid common mistakes
class fbo_t
{
	PROPERTY_R( uint, glId, getGlId ) ///< OpenGL idendification

	public:
		fbo_t(): glId(0) {}

		/// Creates a new FBO
		void Create()
		{
			DEBUG_ERR( glId != 0 ); // FBO allready initialized
			glGenFramebuffers( 1, &glId );
		}

		/// Binds FBO
		void Bind() const
		{
			DEBUG_ERR( glId == 0 );  // FBO unitialized
			glBindFramebuffer( GL_FRAMEBUFFER, glId );
		}

		/// Unbinds the FBO. Actualy unbinds all FBOs
		static void Unbind() { glBindFramebuffer( GL_FRAMEBUFFER, 0 ); }

		/**
		 * Checks the status of an initialized FBO
		 * @return True if FBO is ok and false if not
		 */
		bool IsGood() const
		{
			DEBUG_ERR( glId == 0 );  // FBO unitialized
			DEBUG_ERR( GetCurrentFBO() != glId ); // another FBO is binded

			return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
		}

		/// Set the number of color attachements of the FBO
		void SetNumOfColorAttachements( uint num ) const
		{
			DEBUG_ERR( glId == 0 );  // FBO unitialized
			DEBUG_ERR( GetCurrentFBO() != glId ); // another FBO is binded

			if( num == 0 )
			{
				glDrawBuffer( GL_NONE );
				glReadBuffer( GL_NONE );
			}
			else
			{
				static GLenum color_attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
				                                      GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
				glDrawBuffers( num, color_attachments );
			}
		}

		/**
		 * Returns the GL id of the current attached FBO
		 * @return Returns the GL id of the current attached FBO
		 */
		static uint GetCurrentFBO()
		{
			int fbo_glId;
			glGetIntegerv( GL_FRAMEBUFFER_BINDING, &fbo_glId );
			return (uint)fbo_glId;
		}
};

#endif
