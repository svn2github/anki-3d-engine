#ifndef ANKI_SCENE_SCENE_NODE_H
#define ANKI_SCENE_SCENE_NODE_H

#include "anki/scene/Property.h"
#include "anki/scene/Common.h"
#include "anki/util/Object.h"

namespace anki {

class SceneGraph; // Don't include

// Components forward. Don't include
class MoveComponent;
class RenderComponent;
class FrustumComponent;
class SpatialComponent;
class Light;

/// @addtogroup Scene
/// @{

/// Interface class backbone of scene
class SceneNode
{
public:
	/// @name Constructors/Destructor
	/// @{

	/// The one and only constructor
	/// @param name The unique name of the node. If it's nullptr the the node
	///             is not searchable
	/// @param scene The scene that will register it
	explicit SceneNode(
		const char* name,
		SceneGraph* scene);

	/// Unregister node
	virtual ~SceneNode();
	/// @}

	/// @name Accessors
	/// @{

	/// Return the name. It may be nullptr for nodes that we don't want to 
	/// track
	const char* getName() const
	{
		return name.length() > 0 ? name.c_str() : nullptr;
	}

	SceneAllocator<U8> getSceneAllocator() const;

	SceneAllocator<U8> getSceneFrameAllocator() const;

	SceneGraph& getSceneGraph()
	{
		return *scene;
	}
	/// @}

	/// @name Accessors of components
	/// @{
	MoveComponent* getMoveComponent()
	{
		return sceneNodeProtected.moveC;
	}
	const MoveComponent* getMoveComponent() const
	{
		return sceneNodeProtected.moveC;
	}

	RenderComponent* getRenderComponent()
	{
		return sceneNodeProtected.renderC;
	}
	const RenderComponent* getRenderComponent() const
	{
		return sceneNodeProtected.renderC;
	}

	FrustumComponent* getFrustumComponent()
	{
		return sceneNodeProtected.frustumC;
	}
	const FrustumComponent* getFrustumComponent() const
	{
		return sceneNodeProtected.frustumC;
	}

	SpatialComponent* getSpatialComponent()
	{
		return sceneNodeProtected.spatialC;
	}
	const SpatialComponent* getSpatialComponent() const
	{
		return sceneNodeProtected.spatialC;
	}

	Light* getLight()
	{
		return sceneNodeProtected.lightC;
	}
	const Light* getLight() const 
	{
		return sceneNodeProtected.lightC;
	}
	/// @}

	/// This is called by the scene every frame after logic and before
	/// rendering. By default it does nothing
	/// @param[in] prevUpdateTime Timestamp of the previous update
	/// @param[in] crntTime Timestamp of this update
	/// @param[in] frame Frame number
	virtual void frameUpdate(F32 prevUpdateTime, F32 crntTime, I frame)
	{
		(void)prevUpdateTime;
		(void)crntTime;
		(void)frame;
	}

	/// Return the last frame the node was updated. It checks all components
	U32 getLastUpdateFrame() const;

	/// Mark a node for deletion
	void markForDeletion()
	{
		markedForDeletion = true;
	}
	Bool isMarkedForDeletion()
	{
		return markedForDeletion;
	}

protected:
	struct
	{
		MoveComponent* moveC = nullptr;
		RenderComponent* renderC = nullptr;
		FrustumComponent* frustumC = nullptr;
		SpatialComponent* spatialC = nullptr;
		Light* lightC = nullptr;
	} sceneNodeProtected;

private:
	SceneGraph* scene = nullptr;
	SceneString name; ///< A unique name
	Bool8 markedForDeletion;
};
/// @}

} // end namespace anki

#endif
