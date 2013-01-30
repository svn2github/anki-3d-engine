#ifndef ANKI_SCENE_SECTOR_H
#define ANKI_SCENE_SECTOR_H

#include "anki/scene/Octree.h"
#include "anki/collision/Collision.h"

namespace anki {

// Forward
class SceneNode;
class Scene;
class Sector;
class SectorGroup;

/// @addtogroup Scene
/// @{

/// 2 way Portal
struct Portal
{
	Array<Sector*, 2> sectors;
	Obb shape;

	Portal();
};

/// A sector. It consists of an octree and some portals
class Sector
{
	friend class SectorGroup;

public:
	/// Default constructor
	Sector(SectorGroup* group, const Aabb& box);

	/// Called when a node was moved or a change in shape happened
	Bool placeSceneNode(SceneNode* sp);

	const Aabb& getAabb() const
	{
		return octree.getRoot().getAabb();
	}

	const SectorGroup& getSectorGroup() const
	{
		return *group;
	}
	SectorGroup& getSectorGroup()
	{
		return *group;
	}

private:
	SectorGroup* group; ///< Know your father
	Octree octree;
	SceneVector<Portal*> portals;
};

/// Sector group. This is supposed to represent the whole scene
class SectorGroup
{
public:
	enum VisibilityTest
	{
		VT_RENDERABLES = 1 << 0,
		VT_ONLY_SHADOW_CASTERS = 1 << 1,
		VT_LIGHTS = 1 << 2
	};

	/// Default constructor
	SectorGroup(Scene* scene);

	/// Destructor
	~SectorGroup();

	const Scene& getScene() const
	{
		return *scene;
	}
	Scene& getScene()
	{
		return *scene;
	}

	/// Called when a node was moved or a change in shape happened. The node 
	/// must be Spatial
	void placeSceneNode(SceneNode* sp);

	/// XXX
	void doVisibilityTests(Frustumable& fr, VisibilityTest test);

private:
	Scene* scene; ///< Keep it here to access various allocators
	SceneVector<Sector*> sectors;
	SceneVector<Portal*> portals;
};

/// @}

} // end namespace anki

#endif
