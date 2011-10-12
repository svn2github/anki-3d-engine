#include "anki/renderer/Deformer.h"
#include "anki/resource/ShaderProgram.h"
#include "anki/resource/Material.h"
#include "anki/scene/SkinPatchNode.h"
#include "anki/scene/SkinNode.h"
#include "anki/renderer/MainRenderer.h"


namespace anki {


//==============================================================================
// Constructors & destructor                                                   =
//==============================================================================

Deformer::Deformer(const MainRenderer& mainR_)
:	mainR(mainR_)
{
	init();
}


Deformer::~Deformer()
{}


//==============================================================================
// init                                                                        =
//==============================================================================
void Deformer::init()
{
	//
	// Load the shaders
	//
	tfHwSkinningAllSProg.loadRsrc("shaders/TfHwSkinningPosNormTan.glsl");
	tfHwSkinningPosSProg.loadRsrc("shaders/TfHwSkinningPos.glsl");
}


//==============================================================================
// deform                                                                      =
//==============================================================================
void Deformer::deform(SkinPatchNode& node) const
{
	ASSERT(node.getParent() != NULL); // The SkinPatchNodes always have parent
	ASSERT(node.getParent()->getSceneNodeType() ==
		SceneNode::SNT_SKIN_NODE); // And their parent must be SkinNode
	ASSERT(node.isFlagEnabled(SceneNode::SNF_VISIBLE)); // And it should be
	                                                    // visible

	SkinNode* skinNode = static_cast<SkinNode*>(node.getParent());

	GlStateMachineSingleton::get().enable(GL_RASTERIZER_DISCARD);

	// Chose sProg
	const ShaderProgram* sProg;
	const Material& mtl = node.getModelPatchRsrc().getMaterial();

	if(mtl.buildinVariableExits(MaterialBuildinVariable::MV_NORMAL) &&
	   mtl.buildinVariableExits(MaterialBuildinVariable::MV_TANGENT))
	{
		sProg = tfHwSkinningAllSProg.get();
	}
	else
	{
		sProg = tfHwSkinningPosSProg.get();
	}

	sProg->bind();

	// Uniforms
	sProg->getUniformVariableByName("skinningRotations").set(
		&skinNode->getBoneRotations()[0], skinNode->getBoneRotations().size());

	sProg->getUniformVariableByName("skinningTranslations").set(
		&skinNode->getBoneTranslations()[0],
		skinNode->getBoneTranslations().size());

	node.getTfVao().bind();

	// TF
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0,
		node.getTfVbo(SkinPatchNode::TFV_POSITIONS).getGlId());

	if(sProg == tfHwSkinningAllSProg.get())
	{
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1,
			node.getTfVbo(SkinPatchNode::TFV_NORMALS).getGlId());
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2,
			node.getTfVbo(SkinPatchNode::TFV_TANGENTS).getGlId());
	}

	//glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, this->Query);
	glBeginTransformFeedback(GL_POINTS);
		glDrawArrays(GL_POINTS, 0,
			node.getModelPatchRsrc().getMesh().getVertsNum());
	glEndTransformFeedback();
	//glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

	GlStateMachineSingleton::get().disable(GL_RASTERIZER_DISCARD);
}


} // end namespace
