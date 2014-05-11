#ifndef ANKI_GL_GL_SYNC_H
#define ANKI_GL_GL_SYNC_H

#include "anki/gl/GlCommon.h"
#include "anki/util/Thread.h"

namespace anki {

/// @addtogroup opengl_private
/// @{

/// Sync with the client
class GlClientSync
{
public:
	GlClientSync()
		: m_barrier(2)
	{}

	/// Wait 
	void wait()
	{
		m_barrier.wait();
	}

private:
	Barrier m_barrier;
};

/// @}

} // end namespace anki

#endif

