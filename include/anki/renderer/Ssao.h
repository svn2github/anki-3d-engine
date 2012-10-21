#ifndef ANKI_RENDERER_SSAO_H
#define ANKI_RENDERER_SSAO_H

#include "anki/renderer/RenderingPass.h"
#include "anki/resource/ShaderProgramResource.h"
#include "anki/resource/TextureResource.h"
#include "anki/resource/Resource.h"
#include "anki/gl/Fbo.h"
#include "anki/gl/Texture.h"
#include "anki/gl/Ubo.h"
#include "anki/core/Timestamp.h"

namespace anki {

/// Screen space ambient occlusion pass
///
/// Three passes:
/// @li Calc ssao factor
/// @li Blur vertically
/// @li Blur horizontally
/// @li Repeat from 2
class Ssao: public SwitchableRenderingPass
{
public:
	Ssao(Renderer* r_)
		: SwitchableRenderingPass(r_)
	{}

	void init(const RendererInitializer& initializer);
	void run();

	/// @name Accessors
	/// @{
	float getRenderingQuality() const
	{
		return renderingQuality;
	}

	const Texture& getFai() const
	{
		return fai;
	}
	/// @}

private:
	Texture ssaoFai; ///< It contains the unblurred SSAO factor
	Texture hblurFai;
	Texture fai;  ///< AKA vblurFai The final FAI
	F32 renderingQuality;
	U32 blurringIterationsCount;
	Fbo ssaoFbo;
	Fbo hblurFbo;
	Fbo vblurFbo;
	TextureResourcePointer noiseMap;
	ShaderProgramResourcePointer ssaoSProg;
	ShaderProgramResourcePointer hblurSProg;
	ShaderProgramResourcePointer vblurSProg;
	U32 width, height;
	U32 commonUboUpdateTimestamp = Timestamp::getTimestamp();
	Ubo commonUbo;

	void createFbo(Fbo& fbo, Texture& fai);
	void initInternal(const RendererInitializer& initializer);
};

} // end namespace anki

#endif
