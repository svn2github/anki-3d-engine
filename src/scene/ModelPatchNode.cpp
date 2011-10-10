#include <boost/array.hpp>
#include "ModelPatchNode.h"
#include "resource/Material.h"
#include "resource/MeshData.h"
#include "resource/ModelPatch.h"
#include "ModelNode.h"
#include "ModelNode.h"


//==============================================================================
// Constructor                                                                 =
//==============================================================================
ModelPatchNode::ModelPatchNode(const ModelPatch& modelPatch, ModelNode& parent)
:	PatchNode(modelPatch, SNF_NONE, parent)
{
	VboArray vboArr;

	for(uint i = 0; i < Mesh::VBOS_NUM; i++)
	{
		vboArr[i] = &rsrc.getMesh().getVbo((Mesh::Vbos)i);
	}


	for(uint i = 0; i < PASS_TYPES_NUM; i++)
	{
		createVao(rsrc.getMaterial(), vboArr, vaos[i]);
	}
}


//==============================================================================
// moveUpdate                                                                  =
//==============================================================================
void ModelPatchNode::moveUpdate()
{
	visibilityShapeWSpace =
		rsrc.getMesh().getVisibilityShape().getTransformed(
		getWorldTransform());
}
