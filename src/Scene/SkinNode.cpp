#include <boost/foreach.hpp>
#include "SkinNode.h"
#include "Skin.h"
#include "SkinPatchNode.h"
#include "Skeleton.h"


//======================================================================================================================
// init                                                                                                                =
//======================================================================================================================
void SkinNode::init(const char* filename)
{
	skin.loadRsrc(filename);

	BOOST_FOREACH(const ModelPatch& patch, skin->getModelPatches())
	{
		patches.push_back(new SkinPatchNode(patch, this));
	}

	uint bonesNum = skin->getSkeleton().bones.size();
	tails.resize(bonesNum);
	heads.resize(bonesNum);
	boneRotations.resize(bonesNum);
	boneTranslations.resize(bonesNum);
}


//======================================================================================================================
// moveUpdate                                                                                                          =
//======================================================================================================================
void SkinNode::moveUpdate()
{
	visibilityShapeWSpace.set(tails);
	visibilityShapeWSpace = visibilityShapeWSpace.getTransformed(getWorldTransform());
}
