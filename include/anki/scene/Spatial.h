#ifndef ANKI_SCENE_SPATIAL_H
#define ANKI_SCENE_SPATIAL_H

#include "anki/collision/Collision.h"
#include "anki/util/Flags.h"
#include "anki/core/Timestamp.h"

namespace anki {

class OctreeNode;

/// @addtogroup Scene
/// @{

/// Spatial "interface" for scene nodes. It indicates scene nodes that need to 
/// be placed in the scene's octree and they participate in the visibility 
/// tests
class Spatial: public Flags<uint32_t>
{
public:
	/// Spatial flags
	enum SpatialFlag
	{
		SF_NONE = 0,
		SF_VISIBLE = 1 ///< Visible or not. The visibility tester sets it
	};

	/// Pass the collision shape here so we can avoid the virtuals
	Spatial(CollisionShape* cs)
		: spatialCs(cs)
	{}

	/// @name Accessors
	/// @{
	const CollisionShape& getSpatialCollisionShape() const
	{
		return *spatialCs;
	}

	const Aabb& getAabb() const
	{
		return aabb;
	}

	U32 getSpatialTimestamp() const
	{
		return timestamp;
	}

	OctreeNode* getOctreeNode()
	{
		return octreeNode;
	}
	const OctreeNode* getOctreeNode() const
	{
		return octreeNode;
	}
	void setOctreeNode(OctreeNode* x)
	{
		octreeNode = x;
	}
	/// @}

	/// The derived class has to manually set when the collision shape got
	/// updated
	void spatialMarkUpdated()
	{
		timestamp = Timestamp::getTimestamp();
		spatialCs->toAabb(aabb);
	}

protected:
	CollisionShape* spatialCs = nullptr;

private:
	U32 timestamp = Timestamp::getTimestamp();
	OctreeNode* octreeNode = nullptr; ///< What octree node includes this
	Aabb aabb; ///< A faster shape
};
/// @}

} // namespace anki

#endif
