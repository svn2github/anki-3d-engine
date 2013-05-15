#include "anki/renderer/Hdr.h"
#include "anki/renderer/Renderer.h"

namespace anki {

//==============================================================================
Hdr::~Hdr()
{}

//==============================================================================
void Hdr::initFbo(Fbo& fbo, Texture& fai)
{
	Renderer::createFai(width, height, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, fai);

	// Set to bilinear because the blurring techniques take advantage of that
	fai.setFiltering(Texture::TFT_LINEAR);

	// create FBO
	fbo.create();
	fbo.setColorAttachments({&fai});
	if(!fbo.isComplete())
	{
		throw ANKI_EXCEPTION("Fbo not complete");
	}
}

//==============================================================================
void Hdr::initInternal(const Renderer::Initializer& initializer)
{
	enabled = initializer.pps.hdr.enabled;

	if(!enabled)
	{
		return;
	}

	const F32 renderingQuality = initializer.pps.hdr.renderingQuality;

	width = renderingQuality * (F32)r->getWidth();
	height = renderingQuality * (F32)r->getHeight();
	exposure = initializer.pps.hdr.exposure;
	blurringDist = initializer.pps.hdr.blurringDist;
	blurringIterationsCount = initializer.pps.hdr.blurringIterationsCount;

	initFbo(hblurFbo, hblurFai);
	initFbo(vblurFbo, vblurFai);

	// init shaders
	Vec4 block(exposure, 0.0, 0.0, 0.0);
	commonUbo.create(sizeof(Vec4), &block);

	toneSProg.load("shaders/PpsHdr.glsl");
	toneSProg->findUniformBlock("commonBlock").setBinding(0);

	const char* SHADER_FILENAME = "shaders/GaussianBlurGeneric.glsl";

	F32 blurringDistRealX = F32(blurringDist / width);// XXX
	F32 blurringDistRealY = F32(blurringDist / height);// XXX

	std::string pps =
		"#define HPASS\n"
		"#define COL_RGB\n"
		"#define BLURRING_DIST " + std::to_string(blurringDistRealX) + "\n" // XXX
		"#define IMG_DIMENSION " + std::to_string(height) + ".0\n";
	hblurSProg.load(ShaderProgramResource::createSrcCodeToCache(
		SHADER_FILENAME, pps.c_str()).c_str());

	pps =
		"#define VPASS\n"
		"#define COL_RGB\n"
		"#define BLURRING_DIST " + std::to_string(blurringDistRealY) + "\n"// XXX
		"#define IMG_DIMENSION " + std::to_string(width) + ".0\n";
	vblurSProg.load(ShaderProgramResource::createSrcCodeToCache(
		SHADER_FILENAME, pps.c_str()).c_str());

	// Set timestamps
	parameterUpdateTimestamp = getGlobTimestamp();
	commonUboUpdateTimestamp = getGlobTimestamp();
}

//==============================================================================
void Hdr::init(const RendererInitializer& initializer)
{
	try
	{
		initInternal(initializer);
	}
	catch(const std::exception& e)
	{
		throw ANKI_EXCEPTION("Failed to init PPS HDR") << e;
	}
}

//==============================================================================
void Hdr::run()
{
	ANKI_ASSERT(enabled);
	/*if(r->getFramesCount() % 2 == 0)
	{
		return;
	}*/

	GlStateSingleton::get().setViewport(0, 0, width, height);

	GlStateSingleton::get().disable(GL_BLEND);
	GlStateSingleton::get().disable(GL_DEPTH_TEST);

	// For the passes it should be NEAREST
	//vblurFai.setFiltering(Texture::TFT_NEAREST);

	// pass 0
	vblurFbo.bind();
	r->clearAfterBindingFbo(GL_COLOR_BUFFER_BIT);
	toneSProg->bind();

	if(parameterUpdateTimestamp > commonUboUpdateTimestamp)
	{
		Vec4 block(exposure, 0.0, 0.0, 0.0);
		commonUbo.write(&block);
		commonUboUpdateTimestamp = getGlobTimestamp();
	}
	commonUbo.setBinding(0);
	toneSProg->findUniformVariable("fai").set(r->getIs().getFai());
	r->drawQuad();

	// blurring passes
	for(U32 i = 0; i < blurringIterationsCount; i++)
	{
		// hpass
		hblurFbo.bind();
		r->clearAfterBindingFbo(GL_COLOR_BUFFER_BIT);
		hblurSProg->bind();
		if(i == 0)
		{
			hblurSProg->findUniformVariable("img").set(vblurFai);
		}
		r->drawQuad();

		// vpass
		vblurFbo.bind();
		r->clearAfterBindingFbo(GL_COLOR_BUFFER_BIT);
		vblurSProg->bind();
		if(i == 0)
		{
			vblurSProg->findUniformVariable("img").set(hblurFai);
		}
		r->drawQuad();
	}

	// For the next stage it should be LINEAR though
	//vblurFai.setFiltering(Texture::TFT_LINEAR);
}

} // end namespace anki
