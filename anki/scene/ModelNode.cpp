#include <boost/foreach.hpp>
#include "anki/scene/ModelNode.h"
#include "anki/resource/Model.h"
#include "anki/resource/Skeleton.h"


namespace anki {


//==============================================================================
ModelNode::ModelNode(Scene& scene, ulong flags, SceneNode* parent)
	: SceneNode(SNT_MODEL_NODE, scene, flags, parent)
{}


//==============================================================================
ModelNode::~ModelNode()
{}


//==============================================================================
void ModelNode::init(const char* filename)
{
	model.load(filename);

	BOOST_FOREACH(const ModelPatch& patch, model->getModelPatches())
	{
		patches.push_back(new ModelPatchNode(&patch, this));
	}
}


//==============================================================================
void ModelNode::moveUpdate()
{
	// Update bounding shape
	visibilityShapeWSpace = model->getVisibilityShape().getTransformed(
		getWorldTransform());
}


} // end namespace
