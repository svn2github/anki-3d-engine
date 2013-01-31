#include "anki/scene/Sector.h"
#include "anki/scene/Spatial.h"
#include "anki/scene/SceneNode.h"
#include "anki/scene/VisibilityTestResults.h"
#include "anki/scene/Frustumable.h"
#include "anki/scene/Scene.h"
#include "anki/core/Logger.h"
#include "anki/renderer/Renderer.h"

namespace anki {

//==============================================================================
// Portal                                                                      =
//==============================================================================

//==============================================================================
Portal::Portal()
{
	sectors[0] = sectors[1] = nullptr;
}

//==============================================================================
// Sector                                                                      =
//==============================================================================

//==============================================================================
Sector::Sector(SectorGroup* group_, const Aabb& box)
	: group(group_), octree(this, box, 3),
		portals(group->getScene().getAllocator())
{}

//==============================================================================
Bool Sector::placeSceneNode(SceneNode* sn)
{
	// XXX Optimize

	if(!sn->getSpatial()->getAabb().collide(octree.getRoot().getAabb()))
	{
		return false;
	}

	octree.placeSceneNode(sn);
	return true;
}

//==============================================================================
// SectorGroup                                                                 =
//==============================================================================

//==============================================================================
SectorGroup::SectorGroup(Scene* scene_)
	: scene(scene_)
{
	ANKI_ASSERT(scene != nullptr);
}

//==============================================================================
SectorGroup::~SectorGroup()
{
	for(Sector* sector : sectors)
	{
		ANKI_DELETE(sector, scene->getAllocator());
	}

	for(Portal* portal : portals)
	{
		ANKI_DELETE(portal, scene->getAllocator());
	}
}

//==============================================================================
void SectorGroup::placeSceneNode(SceneNode* sn)
{
	ANKI_ASSERT(sn != nullptr);
	Spatial* sp = sn->getSpatial();
	ANKI_ASSERT(sp);
	const Aabb& spAabb = sp->getAabb();

	// Find the candidates first. Sectors overlap, chose the smaller(??!!??)
	Sector* sector = nullptr;
	for(Sector* s : sectors)
	{
		// Spatial inside the sector?
		if(s->getAabb().collide(spAabb))
		{
			// No other candidate?
			if(sector == nullptr)
			{
				sector = s;
			}
			else
			{
				// Other candidata so chose the smaller
				F32 lengthSqA = (sector->getAabb().getMax()
					- sector->getAabb().getMin()).getLengthSquared();

				F32 lengthSqB = (s->getAabb().getMax()
					- s->getAabb().getMin()).getLengthSquared();

				if(lengthSqB < lengthSqA)
				{
					sector = s;
				}
			}
		}
	}

	// Ask the octree to place it
	if(sector != nullptr)
	{
		sector->octree.placeSceneNode(sn);
	}
	else
	{
		ANKI_LOGW("Spatial outside all sectors");
	}
}

//==============================================================================
void SectorGroup::doVisibilityTests(SceneNode& sn, VisibilityTest test,
	Renderer* r)
{
	Frustumable* fr = sn.getFrustumable();
	ANKI_ASSERT(fr != nullptr);

	//
	// Find the visible sectors
	//

	SceneVector<Sector*> visibleSectors(scene->getFrameAllocator());

	Spatial* sp = sn.getSpatial();
	ANKI_ASSERT(sp != nullptr);

	// Find the sector that contains the frustumable
	Sector& containerSector = sp->getOctreeNode().getOctree().getSector();
	visibleSectors.push_back(&containerSector);

	// Loop all portals and add other sectors
	for(Portal* portal : portals)
	{
		// Get the "other" sector of that portal
		Sector* testAgainstSector;

		if(portal->sectors[0] == &containerSector)
		{
			testAgainstSector = portal->sectors[1];
		}
		else
		{
			ANKI_ASSERT(portal->sectors[1] == &containerSector);
			testAgainstSector = portal->sectors[0];
		}

		// Search if portal is in the container from another portal
		SceneVector<Sector*>::iterator it = std::find(visibleSectors.begin(),
			visibleSectors.end(), testAgainstSector);

		if(it == visibleSectors.end())
		{
			// Not found so test the portal

			// Portal is visible
			if(fr->insideFrustum(portal->shape))
			{
				if(r == nullptr || r->doVisibilityTests(portal->shape))
				{
					visibleSectors.push_back(testAgainstSector);
				}
			}
		}
	}


	/// Create the visibility container
	VisibilityTestResults* visible =
		ANKI_NEW(VisibilityTestResults, scene->getFrameAllocator(),
		scene->getFrameAllocator());

	fr->visible = visible;
}

} // end namespace anki
