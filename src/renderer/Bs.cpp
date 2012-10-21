#include "anki/renderer/Bs.h"
#include "anki/renderer/Renderer.h"
#include "anki/resource/ShaderProgramResource.h"

namespace anki {

//==============================================================================
Bs::~Bs()
{}

//==============================================================================
void Bs::createFbo()
{
#if 0
	try
	{
		fbo.create();

		fbo.setColorAttachments({&r->getPps().getPrePassFai()});
		fbo.setOtherAttachment(GL_DEPTH_STENCIL_ATTACHMENT, 
			r->getMs().getDepthFai());
	}
	catch(std::exception& e)
	{
		throw ANKI_EXCEPTION("Failed to create blending stage FBO") << e;
	}
#endif
}

//==============================================================================
void Bs::createRefractFbo()
{
	try
	{
		refractFbo.create();

		refractFbo.setColorAttachments({&refractFai});
		refractFbo.setOtherAttachment(GL_DEPTH_STENCIL_ATTACHMENT, 
			r->getMs().getDepthFai());
	}
	catch(std::exception& e)
	{
		throw ANKI_EXCEPTION("Failed to create blending stage refract FBO") 
			<< e;
	}
}

//==============================================================================
void Bs::init(const RendererInitializer& /*initializer*/)
{
	createFbo();
	Renderer::createFai(r->getWidth(), r->getHeight(), GL_RGBA8, GL_RGBA,
		GL_FLOAT, refractFai);
	createRefractFbo();
	refractSProg.load("shaders/BsRefract.glsl");
}

//==============================================================================
void Bs::run()
{
	/// XXX
}

} // end namespace
