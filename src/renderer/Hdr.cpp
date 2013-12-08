#include "anki/renderer/Hdr.h"
#include "anki/renderer/Renderer.h"
#include <sstream>

namespace anki {

//==============================================================================
Hdr::~Hdr()
{}

//==============================================================================
void Hdr::initFbo(Fbo& fbo, Texture& fai)
{
	fai.create2dFai(width, height, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);

	// Set to bilinear because the blurring techniques take advantage of that
	fai.setFiltering(Texture::TFT_LINEAR);

	// create FBO
	fbo.create({{&fai, GL_COLOR_ATTACHMENT0}});
}

//==============================================================================
void Hdr::initInternal(const RendererInitializer& initializer)
{
	enabled = initializer.get("pps.hdr.enabled");

	if(!enabled)
	{
		return;
	}

	const F32 renderingQuality = initializer.get("pps.hdr.renderingQuality");

	width = renderingQuality * (F32)r->getWidth();
	height = renderingQuality * (F32)r->getHeight();
	exposure = initializer.get("pps.hdr.exposure");
	blurringDist = initializer.get("pps.hdr.blurringDist");
	blurringIterationsCount = 
		initializer.get("pps.hdr.blurringIterationsCount");

	initFbo(hblurFbo, hblurFai);
	initFbo(vblurFbo, vblurFai);

	// init shaders
	Vec4 block(exposure, 0.0, 0.0, 0.0);
	commonUbo.create(GL_UNIFORM_BUFFER, sizeof(Vec4), &block, GL_DYNAMIC_DRAW);

	toneSProg.load("shaders/PpsHdr.glsl");
	toneSProg->findUniformBlock("commonBlock").setBinding(0);

	const char* SHADER_FILENAME = "shaders/VariableSamplingBlurGeneric.glsl";

	std::stringstream pps;
	pps << "#define HPASS\n"
		"#define COL_RGB\n"
		"#define BLURRING_DIST float(" << blurringDist << ")\n"
		"#define IMG_DIMENSION " << height << "\n"
		"#define SAMPLES " << (U)initializer.get("pps.hdr.samples") << "\n";
	hblurSProg.load(ShaderProgramResource::createSrcCodeToCache(
		SHADER_FILENAME, pps.str().c_str(), "r_").c_str());

	pps.str("");
	pps << "#define VPASS\n"
		"#define COL_RGB\n"
		"#define BLURRING_DIST float(" << blurringDist << ")\n"
		"#define IMG_DIMENSION " << width << "\n"
		"#define SAMPLES " << (U)initializer.get("pps.hdr.samples") << "\n";
	vblurSProg.load(ShaderProgramResource::createSrcCodeToCache(
		SHADER_FILENAME, pps.str().c_str(), "r_").c_str());

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
	vblurFbo.bind(true);
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
		hblurFbo.bind(true);
		hblurSProg->bind();
		if(i == 0)
		{
			hblurSProg->findUniformVariable("img").set(vblurFai);
		}
		r->drawQuad();

		// vpass
		vblurFbo.bind(true);
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
