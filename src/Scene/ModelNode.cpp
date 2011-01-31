#include "ModelNode.h"
#include "Model.h"
#include "Skeleton.h"


//======================================================================================================================
// init                                                                                                                =
//======================================================================================================================
void ModelNode::init(const char* filename)
{
	model.loadRsrc(filename);

	for(uint i = 0; i < model->getModelPatches().size(); i++)
	{
		patches.push_back(new ModelPatchNode(model->getModelPatches()[i], this));
	}
}


//======================================================================================================================
// updateTrf                                                                                                           =
//======================================================================================================================
void ModelNode::updateTrf()
{
	// Update bounding shape
	boundingShapeWSpace = model->getBoundingShape().getTransformed(worldTransform);
}
