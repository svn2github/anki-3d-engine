#include "Smo.h"
#include "Renderer.h"
#include "Light.h"
#include "LightProps.h"
#include "Camera.h"


//======================================================================================================================
// Statics                                                                                                             =
//======================================================================================================================
float Smo::sMOUvSCoords[] = {-0.000000, 0.000000, -1.000000, 0.500000, 0.500000, -0.707107, 0.707107, 0.000000, -0.707107, 0.500000, 0.500000, 0.707107, 0.000000, 0.000000, 1.000000, 0.707107, 0.000000, 0.707107, -0.000000, 0.707107, 0.707107, 0.000000, 0.000000, 1.000000, 0.500000, 0.500000, 0.707107, -0.000000, 0.000000, -1.000000, -0.000000, 0.707107, -0.707107, 0.500000, 0.500000, -0.707107, -0.000000, 0.000000, -1.000000, -0.500000, 0.500000, -0.707107, -0.000000, 0.707107, -0.707107, -0.500000, 0.500000, 0.707107, 0.000000, 0.000000, 1.000000, -0.000000, 0.707107, 0.707107, -0.707107, -0.000000, 0.707107, 0.000000, 0.000000, 1.000000, -0.500000, 0.500000, 0.707107, -0.000000, 0.000000, -1.000000, -0.707107, -0.000000, -0.707107, -0.500000, 0.500000, -0.707107, -0.000000, 0.000000, -1.000000, -0.500000, -0.500000, -0.707107, -0.707107, -0.000000, -0.707107, -0.500000, -0.500000, 0.707107, 0.000000, 0.000000, 1.000000, -0.707107, -0.000000, 0.707107, 0.000000, -0.707107, 0.707107, 0.000000, 0.000000, 1.000000, -0.500000, -0.500000, 0.707107, -0.000000, 0.000000, -1.000000, 0.000000, -0.707107, -0.707107, -0.500000, -0.500000, -0.707107, -0.000000, 0.000000, -1.000000, 0.500000, -0.500000, -0.707107, 0.000000, -0.707107, -0.707107, 0.500000, -0.500000, 0.707107, 0.000000, 0.000000, 1.000000, 0.000000, -0.707107, 0.707107, 0.707107, 0.000000, 0.707107, 0.000000, 0.000000, 1.000000, 0.500000, -0.500000, 0.707107, -0.000000, 0.000000, -1.000000, 0.707107, 0.000000, -0.707107, 0.500000, -0.500000, -0.707107, 0.500000, -0.500000, -0.707107, 0.707107, 0.000000, -0.707107, 1.000000, 0.000000, -0.000000, 0.500000, -0.500000, -0.707107, 1.000000, 0.000000, -0.000000, 0.707107, -0.707107, 0.000000, 0.707107, -0.707107, 0.000000, 1.000000, 0.000000, -0.000000, 0.707107, 0.000000, 0.707107, 0.707107, -0.707107, 0.000000, 0.707107, 0.000000, 0.707107, 0.500000, -0.500000, 0.707107, 0.000000, -1.000000, 0.000000, 0.707107, -0.707107, 0.000000, 0.500000, -0.500000, 0.707107, 0.000000, -1.000000, 0.000000, 0.500000, -0.500000, 0.707107, 0.000000, -0.707107, 0.707107, 0.000000, -0.707107, -0.707107, 0.500000, -0.500000, -0.707107, 0.707107, -0.707107, 0.000000, 0.000000, -0.707107, -0.707107, 0.707107, -0.707107, 0.000000, 0.000000, -1.000000, 0.000000, -0.500000, -0.500000, -0.707107, 0.000000, -0.707107, -0.707107, -0.707107, -0.707107, 0.000000, 0.000000, -0.707107, -0.707107, 0.000000, -1.000000, 0.000000, -0.707107, -0.707107, 0.000000, -0.707107, -0.707107, 0.000000, 0.000000, -1.000000, 0.000000, 0.000000, -0.707107, 0.707107, -0.707107, -0.707107, 0.000000, 0.000000, -0.707107, 0.707107, -0.500000, -0.500000, 0.707107, -1.000000, -0.000000, 0.000000, -0.707107, -0.707107, 0.000000, -0.500000, -0.500000, 0.707107, -1.000000, -0.000000, 0.000000, -0.500000, -0.500000, 0.707107, -0.707107, -0.000000, 0.707107, -0.707107, -0.000000, -0.707107, -0.500000, -0.500000, -0.707107, -0.707107, -0.707107, 0.000000, -0.707107, -0.000000, -0.707107, -0.707107, -0.707107, 0.000000, -1.000000, -0.000000, 0.000000, -0.500000, 0.500000, -0.707107, -0.707107, -0.000000, -0.707107, -1.000000, -0.000000, 0.000000, -0.500000, 0.500000, -0.707107, -1.000000, -0.000000, 0.000000, -0.707107, 0.707107, 0.000000, -0.707107, 0.707107, 0.000000, -1.000000, -0.000000, 0.000000, -0.707107, -0.000000, 0.707107, -0.707107, 0.707107, 0.000000, -0.707107, -0.000000, 0.707107, -0.500000, 0.500000, 0.707107, -0.000000, 1.000000, 0.000000, -0.707107, 0.707107, 0.000000, -0.500000, 0.500000, 0.707107, -0.000000, 1.000000, 0.000000, -0.500000, 0.500000, 0.707107, -0.000000, 0.707107, 0.707107, -0.000000, 0.707107, -0.707107, -0.500000, 0.500000, -0.707107, -0.707107, 0.707107, 0.000000, -0.000000, 0.707107, -0.707107, -0.707107, 0.707107, 0.000000, -0.000000, 1.000000, 0.000000, 0.500000, 0.500000, -0.707107, -0.000000, 0.707107, -0.707107, -0.000000, 1.000000, 0.000000, 0.500000, 0.500000, -0.707107, -0.000000, 1.000000, 0.000000, 0.707107, 0.707107, 0.000000, 0.707107, 0.707107, 0.000000, -0.000000, 1.000000, 0.000000, -0.000000, 0.707107, 0.707107, 0.707107, 0.707107, 0.000000, -0.000000, 0.707107, 0.707107, 0.500000, 0.500000, 0.707107, 1.000000, 0.000000, -0.000000, 0.707107, 0.707107, 0.000000, 0.500000, 0.500000, 0.707107, 1.000000, 0.000000, -0.000000, 0.500000, 0.500000, 0.707107, 0.707107, 0.000000, 0.707107, 0.707107, 0.000000, -0.707107, 0.500000, 0.500000, -0.707107, 0.707107, 0.707107, 0.000000, 0.707107, 0.000000, -0.707107, 0.707107, 0.707107, 0.000000, 1.000000, 0.000000, -0.000000};
Vbo Smo::sMOUvSVbo;


//======================================================================================================================
// init                                                                                                                =
//======================================================================================================================
void Smo::init(const RendererInitializer& /*initializer*/)
{
	sProg.loadRsrc("shaders/IsSmo.glsl");

	if(!sMOUvSVbo.isCreated())
	{
		sMOUvSVbo.create(GL_ARRAY_BUFFER, sizeof(sMOUvSCoords), sMOUvSCoords, GL_STATIC_DRAW);
	}
}


//======================================================================================================================
// run [PointLight]                                                                                                    =
//======================================================================================================================
void Smo::run(const PointLight& light)
{
	// set GL
	glStencilFunc(GL_ALWAYS, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glColorMask(false, false, false, false);
	glClear(GL_STENCIL_BUFFER_BIT);

	glDisable(GL_CULL_FACE);

	// set shared prog
	const float scale = 1.2; // we scale the sphere a little
	sProg->bind();
	Mat4 modelMat = Mat4(light.getWorldTransform().origin, Mat3::getIdentity(), light.getRadius() * scale);
	Mat4 trf = r.getCamera().getProjectionMatrix() * Mat4::combineTransformations(r.getCamera().getViewMatrix(), modelMat);
	sProg->findUniVar("modelViewProjectionMat")->setMat4(&trf);

	// render sphere to the stencil buffer
	sMOUvSVbo.bind();
	const int loc = 0;
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glDrawArrays(GL_TRIANGLES, 0, sizeof(sMOUvSCoords)/sizeof(float)/3);
	glDisableVertexAttribArray(loc);
	sMOUvSVbo.unbind();

	// restore GL
	glEnable(GL_CULL_FACE);
	glColorMask(true, true, true, true);

	glStencilFunc(GL_EQUAL, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}


//======================================================================================================================
// run [SpotLight]                                                                                                     =
//======================================================================================================================
void Smo::run(const SpotLight& light)
{
	// set GL state
	glStencilFunc(GL_ALWAYS, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glClear(GL_STENCIL_BUFFER_BIT);

	glColorMask(false, false, false, false);
	glDisable(GL_CULL_FACE);

	// calc camera shape
	const Camera& lcam = light.camera;
	float x = lcam.getZFar() / tan((PI-lcam.getFovX())/2);
	float y = tan(lcam.getFovY()/2) * lcam.getZFar();
	float z = -lcam.getZFar();

	const int TRIS_NUM = 6;

	float verts[TRIS_NUM][3][3] = {
		{{0.0, 0.0, 0.0}, {x, -y, z}, {x,  y, z}}, // right triangle
		{{0.0, 0.0, 0.0}, {x,  y, z}, {-x,  y, z}}, // top
		{{0.0, 0.0, 0.0}, {-x,  y, z}, {-x, -y, z}}, // left
		{{0.0, 0.0, 0.0}, {-x, -y, z}, {x, -y, z}}, // bottom
		{{x, -y, z}, {-x,  y, z}, {x,  y, z}}, // front up right
		{{x, -y, z}, {-x, -y, z}, {-x,  y, z}}, // front bottom left
	};

	// shader prog
	sProg->bind();
	Mat4 modelMat = Mat4(lcam.getWorldTransform());
	Mat4 trf = r.getCamera().getProjectionMatrix() * Mat4::combineTransformations(r.getCamera().getViewMatrix(), modelMat);
	sProg->findUniVar("modelViewProjectionMat")->setMat4(&trf);

	// render camera shape to stencil buffer
	const int loc = 0;
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, false, 0, verts);
	glDrawArrays(GL_TRIANGLES, 0, TRIS_NUM * 3);
	glDisableVertexAttribArray(loc);

	// restore GL state
	glEnable(GL_CULL_FACE);
	glColorMask(true, true, true, true);

	glStencilFunc(GL_EQUAL, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}
