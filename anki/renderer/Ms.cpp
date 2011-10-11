#include <boost/foreach.hpp>
#include "anki/renderer/Ms.h"
#include "anki/renderer/Renderer.h"
#include "anki/scene/Camera.h"
#include "anki/renderer/Ez.h"
#include "anki/scene/RenderableNode.h"


//==============================================================================
// Constructor                                                                 =
//==============================================================================
Ms::Ms(Renderer& r_)
:	RenderingPass(r_),
	ez(r_)
{}


//==============================================================================
// Destructor                                                                  =
//==============================================================================
Ms::~Ms()
{}


//==============================================================================
// init                                                                        =
//==============================================================================
void Ms::init(const RendererInitializer& initializer)
{
	try
	{
		// create FBO
		fbo.create();
		fbo.bind();

		// inform in what buffers we draw
		fbo.setNumOfColorAttachements(3);

		// create the FAIs

		Renderer::createFai(r.getWidth(), r.getHeight(), GL_RGB16F,
			GL_RGB, GL_FLOAT, normalFai);
		Renderer::createFai(r.getWidth(), r.getHeight(), GL_RGB8,
			GL_RGB, GL_FLOAT, diffuseFai);
		Renderer::createFai(r.getWidth(), r.getHeight(), GL_RGBA8,
			GL_RGBA, GL_FLOAT, specularFai);
		Renderer::createFai(r.getWidth(), r.getHeight(),
			GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL,
			GL_UNSIGNED_INT_24_8, depthFai);

		// attach the buffers to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, normalFai.getGlId(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
			GL_TEXTURE_2D, diffuseFai.getGlId(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
			GL_TEXTURE_2D, specularFai.getGlId(), 0);

		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		//	GL_TEXTURE_2D, depthFai.getGlId(), 0);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
		//	GL_TEXTURE_2D, depthFai.getGlId(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
			GL_TEXTURE_2D, depthFai.getGlId(), 0);

		// test if success
		fbo.checkIfGood();

		// unbind
		fbo.unbind();
	}
	catch(std::exception& e)
	{
		throw ANKI_EXCEPTION("Cannot create deferred shading material stage FBO: " +
			e.what());
	}

	ez.init(initializer);
}


//==============================================================================
// run                                                                         =
//==============================================================================
void Ms::run()
{
	if(ez.getEnabled())
	{
		ez.run();
	}

	fbo.bind();

	if(!ez.getEnabled())
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	GlStateMachineSingleton::get().setViewport(0, 0,
		r.getWidth(), r.getHeight());

	//GlStateMachineSingleton::get().enable(GL_DEPTH_TEST, true);
	//app->getScene().skybox.Render(cam.getViewMatrix().getRotationPart());
	//glDepthFunc(GL_LEQUAL);

	// if ez then change the default depth test and disable depth writing
	if(ez.getEnabled())
	{
		glDepthMask(false);
		glDepthFunc(GL_EQUAL);
	}

	glClear(GL_COLOR_BUFFER_BIT);

	// render all
	BOOST_FOREACH(const RenderableNode* node,
		r.getCamera().getVisibleMsRenderableNodes())
	{
		r.getSceneDrawer().renderRenderableNode(*node, r.getCamera(),
			COLOR_PASS);
	}

	// restore depth
	if(ez.getEnabled())
	{
		glDepthMask(true);
		glDepthFunc(GL_LESS);
	}

	fbo.unbind();
	ON_GL_FAIL_THROW_EXCEPTION();
}
