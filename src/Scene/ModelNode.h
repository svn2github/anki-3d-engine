#ifndef MODEL_NODE_H
#define MODEL_NODE_H

#include <boost/array.hpp>
#include "SceneNode.h"
#include "RsrcPtr.h"
#include "Accessors.h"
#include "ModelPatchNode.h"
#include "Vec.h"
#include "Obb.h"


class Model;


/// The model scene node
class ModelNode: public SceneNode
{
	public:
		ModelNode(): SceneNode(SNT_MODEL, true, NULL) {}

		/// @name Accessors
		/// @{
		GETTER_RW(Vec<ModelPatchNode*>, patches, getModelPatchNodes)
		const Model& getModel() const {return *model;}
		const Obb& getVisibilityShapeWSpace() const {return visibilityShapeWSpace;}
		/// @}

		/// Initialize the node
		/// - Load the resource
		void init(const char* filename);

		/// Update the bounding shape
		void moveUpdate();

		void frameUpdate() {}

	private:
		RsrcPtr<Model> model;
		Vec<ModelPatchNode*> patches;
		Obb visibilityShapeWSpace;
};


#endif
