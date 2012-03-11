#include "anki/renderer/SceneDbgDrawer.h"
#include "anki/renderer/Dbg.h"
#include "anki/scene/Octree.h"
#include "anki/scene/Frustumable.h"
#include "anki/scene/Spatial.h"


namespace anki {


//==============================================================================
void SceneDbgDrawer::draw(const SceneNode& node)
{
	if(isFlagEnabled(DF_FRUSTUMABLE) && node.getFrustumable())
	{
		draw(*node.getFrustumable());
	}

	if(isFlagEnabled(DF_SPATIAL) && node.getSpatial())
	{
		draw(*node.getSpatial());
	}
}


//==============================================================================
void SceneDbgDrawer::draw(const Frustumable& fr) const
{
	const Frustum& fs = fr.getFrustum();

	switch(fs.getFrustumType())
	{
		case Frustum::FT_PERSPECTIVE:
			draw(static_cast<const PerspectiveFrustum&>(fs), dbg);
			break;

		case Frustum::FT_ORTHOGRAPHIC:
			draw(static_cast<const OrthographicFrustum&>(fs), dbg);
			break;

		default:
			ANKI_ASSERT(0 && "WTF?");
			break;
	}
}


//==============================================================================
void SceneDbgDrawer::draw(const PerspectiveFrustum& pf, Dbg& dbg) const
{
	dbg.setColor(Vec4(1.0, 0.0, 1.0, 1.0));

	float camLen = pf.getFar();
	float tmp0 = camLen / tan((Math::PI - cam.getFovX()) * 0.5) + 0.001;
	float tmp1 = camLen * tan(cam.getFovY() * 0.5) + 0.001;

	Vec3 points[] = {
		Vec3(0.0, 0.0, 0.0), // 0: eye point
		Vec3(-tmp0, tmp1, -camLen), // 1: top left
		Vec3(-tmp0, -tmp1, -camLen), // 2: bottom left
		Vec3(tmp0, -tmp1, -camLen), // 3: bottom right
		Vec3(tmp0, tmp1, -camLen) // 4: top right
	};

	const uint indeces[] = {0, 1, 0, 2, 0, 3, 0, 4, 1, 2, 2, 3, 3, 4, 4, 1};

	dbg.begin();
		for(uint i = 0; i < sizeof(indeces) / sizeof(uint); i++)
		{
			dbg.pushBackVertex(points[indeces[i]]);
		}
	dbg.end();
}


//==============================================================================
void SceneDbgDrawer::draw(const OrthographicFrustum& of) const
{
	dbg.setColor(Vec4(0.0, 1.0, 0.0, 1.0));

	float left = ocam.getLeft();
	float right = ocam.getRight();
	float zNear = ocam.getZNear();
	float zFar = ocam.getZFar();
	float top = ocam.getTop();
	float bottom = ocam.getBottom();

	boost::array<Vec3, 8> positions = {{
		Vec3(right, top, -zNear),
		Vec3(left, top, -zNear),
		Vec3(left, bottom, -zNear),
		Vec3(right, bottom, -zNear),
		Vec3(right, top, -zFar),
		Vec3(left, top, -zFar),
		Vec3(left, bottom, -zFar),
		Vec3(right, bottom, -zFar)
	}};

	boost::array<uint, 24> indeces = {{
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7}};

	dbg.begin();
		//BOOST_FOREACH(uint i, indeces)
		for(uint i = 0; i < 24; i++)
		{
			dbg.pushBackVertex(positions[indeces[i]]);
		}
	dbg.end();
}


//==============================================================================
void SceneDbgDrawer::drawLight(const Light& light) const
{
	dbg.setColor(light.getDiffuseColor());
	dbg.setModelMat(Mat4(light.getWorldTransform()));
	dbg.drawSphere(0.1);
}


//==============================================================================
void SceneDbgDrawer::drawParticleEmitter(const ParticleEmitterNode& pe) const
{
	dbg.setColor(Vec4(1.0));
	dbg.setModelMat(Mat4(pe.getWorldTransform()));
	dbg.drawCube();
}


//==============================================================================
void SceneDbgDrawer::drawSkinNodeSkeleton(const SkinNode& sn) const
{
	dbg.setModelMat(Mat4(sn.getWorldTransform()));
	dbg.begin();
	for(uint i = 0; i < sn.getHeads().size(); i++)
	{
		dbg.setColor(Vec4(1.0, 0.0, 0.0, 1.0));
		dbg.pushBackVertex(sn.getHeads()[i]);
		dbg.setColor(Vec4(1.0));
		dbg.pushBackVertex(sn.getTails()[i]);
	}
	dbg.end();
}


//==============================================================================
void SceneDbgDrawer::drawOctree(const Octree& octree) const
{
	dbg.setColor(Vec3(1.0));
	drawOctreeNode(octree.getRoot(), 0, octree);
}


//==============================================================================
void SceneDbgDrawer::drawOctreeNode(const OctreeNode& octnode, uint depth,
	const Octree& octree) const
{
	Vec3 color = Vec3(1.0 - float(depth) / float(octree.getMaxDepth()));
	dbg.setColor(color);

	CollisionDbgDrawer v(dbg);
	octnode.getAabb().accept(v);

	// Children
	for(uint i = 0; i < 8; ++i)
	{
		if(octnode.getChildren()[i] != NULL)
		{
			drawOctreeNode(*octnode.getChildren()[i], depth + 1, octree);
		}
	}
}


} // end namespace
