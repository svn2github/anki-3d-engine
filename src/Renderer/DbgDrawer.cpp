#include "DbgDrawer.h"
#include "Dbg.h"
#include "Camera.h"
#include "Light.h"
#include "ParticleEmitter.h"


//======================================================================================================================
// drawCamera                                                                                                          =
//======================================================================================================================
void DbgDrawer::drawCamera(const Camera& cam) const
{
	dbg.setColor(Vec4(1.0, 0.0, 1.0, 1.0));
	dbg.setModelMat(Mat4(cam.getWorldTransform()));

	const float camLen = 1.0;
	float tmp0 = camLen / tan((M::PI - cam.getFovX()) * 0.5) + 0.001;
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


//======================================================================================================================
// drawLight                                                                                                           =
//======================================================================================================================
void DbgDrawer::drawLight(const Light& light) const
{
	dbg.drawSphere(0.1, light.getWorldTransform(), Vec4(light.getDiffuseCol(), 1.0));
}


//======================================================================================================================
// drawParticleEmitter                                                                                                 =
//======================================================================================================================
void DbgDrawer::drawParticleEmitter(const ParticleEmitter& pe) const
{
	dbg.setColor(Vec4(1.0));
	dbg.setModelMat(Mat4(pe.getWorldTransform()));
	dbg.drawCube();
}
