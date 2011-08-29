#include <stdio.h>
#include <iostream>
#include <fstream>

#include "i/Input.h"
#include "scene/PerspectiveCamera.h"
#include "scene/OrthographicCamera.h"
#include "m/Math.h"
#include "r/Renderer.h"
#include "ui/Painter.h"
#include "core/App.h"
#include "rsrc/Mesh.h"
#include "scene/Light.h"
#include "scene/PointLight.h"
#include "scene/SpotLight.h"
#include "rsrc/Material.h"
#include "scene/Scene.h"
#include "util/scanner/Scanner.h"
#include "rsrc/SkelAnim.h"
#include "rsrc/LightRsrc.h"
#include "misc/Parser.h"
#include "scene/ParticleEmitterNode.h"
#include "phys/Character.h"
#include "r/Renderer.h"
#include "r/RendererInitializer.h"
#include "r/MainRenderer.h"
#include "phys/Character.h"
#include "phys/RigidBody.h"
#include "script/Engine.h"
#include "core/StdinListener.h"
#include "scene/ModelNode.h"
#include "rsrc/Model.h"
#include "core/Logger.h"
#include "util/Util.h"
#include "util/HighRezTimer.h"
#include "scene/SkinNode.h"
#include "rsrc/Skin.h"
#include "scene/MaterialRuntime.h"
#include "core/Globals.h"
#include "ui/FtFontLoader.h"
#include "ui/Font.h"
#include "event/Manager.h"
#include "event/SceneColor.h"
#include "event/MainRendererPpsHdr.h"
#include "rsrc/ShaderProgramPrePreprocessor.h"
#include "rsrc/Material.h"
#include "core/parallel/Manager.h"
#include "r/PhysDbgDrawer.h"


// map (hard coded)
ModelNode* floor__,* sarge,* horse,* crate,* pentagram;
SkinNode* imp;
//SkelModelNode* imp;
PointLight* point_lights[10];
SpotLight* spot_lights[2];
ParticleEmitterNode* partEmitter;
phys::Character* character;

ui::Painter* painter;


// Physics
Vec<btRigidBody*> boxes;

#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_Z 5

#define MAX_PROXIES (ARRAY_SIZE_X*ARRAY_SIZE_Y*ARRAY_SIZE_Z + 1024)

#define SCALING 1.
#define START_POS_X -5
#define START_POS_Y -5
#define START_POS_Z -3


void initPhysics()
{
	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.),btScalar(50.),btScalar(50.)));

	Transform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(Vec3(0,-50, 0));

	phys::RigidBody::Initializer init;
	init.mass = 0.0;
	init.shape = groundShape;
	init.startTrf = groundTransform;

	new phys::RigidBody(SceneSingleton::get().getPhysMasterContainer(), init);


	/*{
		btCollisionShape* colShape = new btBoxShape(btVector3(SCALING*1,SCALING*1,SCALING*1));

		float start_x = START_POS_X - ARRAY_SIZE_X/2;
		float start_y = START_POS_Y;
		float start_z = START_POS_Z - ARRAY_SIZE_Z/2;

		for (int k=0;k<ARRAY_SIZE_Y;k++)
		{
			for (int i=0;i<ARRAY_SIZE_X;i++)
			{
				for(int j = 0;j<ARRAY_SIZE_Z;j++)
				{
					//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
					MeshNode* crate = new MeshNode;
					crate->init("models/crate0/crate0.mesh");
					crate->getLocalTransform().setScale(1.11);

					Transform trf(SCALING*Vec3(2.0*i + start_x, 20+2.0*k + start_y, 2.0*j + start_z), Mat3::getIdentity(), 1.0);
					new RigidBody(1.0, trf, colShape, crate, Physics::CG_MAP, Physics::CG_ALL);
				}
			}
		}
	}*/
}



//==============================================================================
// init                                                                        =
//==============================================================================
void init()
{
	INFO("Other init...");

	/*Material mtl;
	mtl.load("lala.mtl");*/

	srand(unsigned(time(NULL)));

	painter = new ui::Painter(Vec2(AppSingleton::get().getWindowWidth(),
	                               AppSingleton::get().getWindowHeight()));
	painter->setFont("engine-rsrc/ModernAntiqua.ttf", 25, 25);

	// camera
	PerspectiveCamera* cam = new PerspectiveCamera(false, NULL);
	//cam->setAll(toRad(100.0), toRad(100.0) / r::MainRendererSingleton::get().getAspectRatio(), 0.5, 200.0);
	cam->setAll(r::MainRendererSingleton::get().getAspectRatio()*toRad(60.0), toRad(60.0), 0.5, 200.0);
	cam->moveLocalY(3.0);
	cam->moveLocalZ(5.7);
	cam->moveLocalX(-0.3);
	AppSingleton::get().setActiveCam(cam);
	INFO(cam->getSceneNodeName());

	OrthographicCamera* ocam = new OrthographicCamera(false, NULL);
	ocam->setAll(-1, 1, 1.0, -1.0, 0.1, 10.0);

	// lights
	point_lights[0] = new PointLight(false, NULL);
	point_lights[0]->init("maps/temple/light0.light");
	point_lights[0]->setLocalTransform(Transform(Vec3(-1.0, 2.4, 1.0), Mat3::getIdentity(), 1.0));
	point_lights[1] = new PointLight(false, NULL);
	point_lights[1]->init("maps/temple/light1.light");
	point_lights[1]->setLocalTransform(Transform(Vec3(2.5, 1.4, 1.0), Mat3::getIdentity(), 1.0));

	spot_lights[0] = new SpotLight(false, NULL);
	spot_lights[0]->init("maps/temple/light2.light");
	spot_lights[0]->setLocalTransform(Transform(Vec3(1.3, 4.3, 3.0), Mat3(Euler(toRad(-20), toRad(20), 0.0)), 1.0));
	spot_lights[1] = new SpotLight(false, NULL);
	spot_lights[1]->init("maps/temple/light3.light");
	spot_lights[1]->setLocalTransform(Transform(Vec3(-2.3, 6.3, 2.9), Mat3(Euler(toRad(-70), toRad(-20), 0.0)), 1.0));


	// horse
	horse = new ModelNode(false, NULL);
	horse->init("meshes/horse/horse.mdl");
	horse->setLocalTransform(Transform(Vec3(-2, 0, 0), Mat3::getIdentity(), 1.0));

	// Pentagram
	/*pentagram = new ModelNode(false, NULL);
	pentagram->init("models/pentagram/pentagram.mdl");
	pentagram->setLocalTransform(Transform(Vec3(2, 0, 0), Mat3::getIdentity(), 1.0));

	// Sponza
	ModelNode* sponza = new ModelNode(false, NULL);
	//sponza->init("maps/sponza/sponza.mdl");
	sponza->init("maps/sponza-crytek/sponza_crytek.mdl");
	sponza->setLocalTransform(Transform(Vec3(0.0), Mat3::getIdentity(), 0.05));


	// Imp
	imp = new SkinNode(false, NULL);
	imp->setLocalTransform(Transform(Vec3(0.0, 2.0, 0.0), Mat3::getIdentity(), 0.7));
	imp->init("models/imp/imp.skin");
	imp->skelAnimModelNodeCtrl = new SkelAnimModelNodeCtrl(*imp);
	imp->skelAnimModelNodeCtrl->set(imp->getSkin().getSkelAnims()[0].get());
	imp->skelAnimModelNodeCtrl->setStep(0.8);

	imp->addChild(*cam);*/


	// sarge
	/*sarge = new MeshNode();
	sarge->init("meshes/sphere/sphere16.mesh");
	//sarge->setLocalTransform(Vec3(0, -2.8, 1.0), Mat3(Euler(-m::PI/2, 0.0, 0.0)), 1.1);
	sarge->setLocalTransform(Transform(Vec3(0, 2.0, 2.0), Mat3::getIdentity(), 0.4));
	
	// floor
	floor__ = new MeshNode();
	floor__->init("maps/temple/Cube.019.mesh");
	floor__->setLocalTransform(Transform(Vec3(0.0, -0.19, 0.0), Mat3(Euler(-m::PI/2, 0.0, 0.0)), 0.8));*/

	// imp	
	/*imp = new SkelModelNode();
	imp->init("models/imp/imp.smdl");
	//imp->setLocalTransform(Transform(Vec3(0.0, 2.11, 0.0), Mat3(Euler(-m::PI/2, 0.0, 0.0)), 0.7));
	SkelAnimCtrl* ctrl = new SkelAnimCtrl(*imp->meshNodes[0]->meshSkelCtrl->skelNode);
	ctrl->skelAnim.loadRsrc("models/imp/walk.imp.anim");
	ctrl->step = 0.8;*/

	// cave map
	/*for(int i=1; i<21; i++)
	{
		MeshNode* node = new MeshNode();
		node->init(("maps/cave/rock." + lexical_cast<string>(i) + ".mesh").c_str());
		node->setLocalTransform(Transform(Vec3(0.0, -0.0, 0.0), Mat3::getIdentity(), 0.01));
	}*/

	// sponza map
	/*MeshNode* node = new MeshNode();
	node->init("maps/sponza/floor.mesh");
	node = new MeshNode();
	node->init("maps/sponza/walls.mesh");
	node = new MeshNode();
	node->init("maps/sponza/light-marbles.mesh");
	node = new MeshNode();
	node->init("maps/sponza/dark-marbles.mesh");*/
	//node->setLocalTransform(Transform(Vec3(0.0, -0.0, 0.0), Mat3::getIdentity(), 0.01));

	// particle emitter
	/*
	XXX
	partEmitter = new ParticleEmitterNode(false, NULL);
	partEmitter->init("asdf");
	partEmitter->getLocalTransform().setOrigin(Vec3(3.0, 0.0, 0.0));*/

	return;

	// character
	/*PhyCharacter::Initializer init;
	init.sceneNode = imp;
	init.startTrf = Transform(Vec3(0, 40, 0), Mat3::getIdentity(), 1.0);
	character = new PhyCharacter(SceneSingleton::get().getPhysics(), init);*/

	// crate
	/*crate = new MeshNode;
	crate->init("models/crate0/crate0.mesh");
	crate->scaleLspace = 1.0;*/


	//
	//floor_ = new floor_t;
	//floor_->material = RsrcMngr::materials.load("materials/default.mtl");


	initPhysics();

	//INFO("Engine initialization ends (" << (App::getTicks() - ticks) << ")");
}


//==============================================================================
//                                                                             =
//==============================================================================
void mainLoopExtra()
{
	InputSingleton::get().handleEvents();

	float dist = 0.2;
	float ang = toRad(3.0);
	float scale = 0.01;

	// move the camera
	static SceneNode* mover = AppSingleton::get().getActiveCam();

	if(InputSingleton::get().getKey(SDL_SCANCODE_1)) mover = AppSingleton::get().getActiveCam();
	if(InputSingleton::get().getKey(SDL_SCANCODE_2)) mover = point_lights[0];
	if(InputSingleton::get().getKey(SDL_SCANCODE_3)) mover = spot_lights[0];
	if(InputSingleton::get().getKey(SDL_SCANCODE_4)) mover = point_lights[1];
	if(InputSingleton::get().getKey(SDL_SCANCODE_5)) mover = spot_lights[1];
	if(InputSingleton::get().getKey(SDL_SCANCODE_6)) mover = imp;
	if(InputSingleton::get().getKey(SDL_SCANCODE_7)) mover =
		SceneSingleton::get().getParticleEmitterNodes()[0];
	//if(InputSingleton::get().getKey(SDL_SCANCODE_M) == 1) InputSingleton::get().warpMouse = !InputSingleton::get().warpMouse;

	if(InputSingleton::get().getKey(SDL_SCANCODE_A)) mover->moveLocalX(-dist);
	if(InputSingleton::get().getKey(SDL_SCANCODE_D)) mover->moveLocalX(dist);
	if(InputSingleton::get().getKey(SDL_SCANCODE_LSHIFT)) mover->moveLocalY(dist);
	if(InputSingleton::get().getKey(SDL_SCANCODE_SPACE)) mover->moveLocalY(-dist);
	if(InputSingleton::get().getKey(SDL_SCANCODE_W)) mover->moveLocalZ(-dist);
	if(InputSingleton::get().getKey(SDL_SCANCODE_S)) mover->moveLocalZ(dist);
	if(!InputSingleton::get().warpMouse())
	{
		if(InputSingleton::get().getKey(SDL_SCANCODE_UP)) mover->rotateLocalX(ang);
		if(InputSingleton::get().getKey(SDL_SCANCODE_DOWN)) mover->rotateLocalX(-ang);
		if(InputSingleton::get().getKey(SDL_SCANCODE_LEFT)) mover->rotateLocalY(ang);
		if(InputSingleton::get().getKey(SDL_SCANCODE_RIGHT)) mover->rotateLocalY(-ang);
	}
	else
	{
		float accel = 44.0;
		mover->rotateLocalX(ang * InputSingleton::get().mouseVelocity.y() * accel);
		mover->rotateLocalY(-ang * InputSingleton::get().mouseVelocity.x() * accel);
	}
	if(InputSingleton::get().getKey(SDL_SCANCODE_Q)) mover->rotateLocalZ(ang);
	if(InputSingleton::get().getKey(SDL_SCANCODE_E)) mover->rotateLocalZ(-ang);
	if(InputSingleton::get().getKey(SDL_SCANCODE_PAGEUP)) mover->getLocalTransform().getScale() += scale ;
	if(InputSingleton::get().getKey(SDL_SCANCODE_PAGEDOWN)) mover->getLocalTransform().getScale() -= scale ;

	if(InputSingleton::get().getKey(SDL_SCANCODE_K))
		AppSingleton::get().getActiveCam()->lookAtPoint(point_lights[0]->getWorldTransform().getOrigin());

	if(InputSingleton::get().getKey(SDL_SCANCODE_I))
		character->moveForward(0.1);

	/*if(InputSingleton::get().getKey(SDL_SCANCODE_F) == 1)
	{
		Event::ManagerSingleton::get().createEvent(Event::MainRendererPpsHdr(HighRezTimer::getCrntTime() + 5,
			5, r::MainRendererSingleton::get().getPps().getHdr().getExposure() + 20.0, 3, 1.4));
	}*/


	if(InputSingleton::get().getKey(SDL_SCANCODE_O) == 1)
	{
		btRigidBody* body = static_cast<btRigidBody*>(boxes[0]);
		//body->getMotionState()->setWorldTransform(toBt(Mat4(Vec3(0.0, 10.0, 0.0), Mat3::getIdentity(), 1.0)));
		body->setWorldTransform(toBt(Mat4(Vec3(0.0, 10.0, 0.0), Mat3::getIdentity(), 1.0)));
		//body->clearForces();
		body->forceActivationState(ACTIVE_TAG);
	}

	if(InputSingleton::get().getKey(SDL_SCANCODE_Y) == 1)
	{
		INFO("Exec script");
		script::EngineSingleton::get().execScript(util::readFile("test.py").c_str());
	}

	mover->getLocalTransform().getRotation().reorthogonalize();

	//INFO(mover->getSceneNodeName())

	/*if(spot_lights[0]->getCamera().insideFrustum(spot_lights[1]->getCamera()))
	{
		INFO("in");
	}
	else
	{
		INFO("out");
	}*/
}


//==============================================================================
// mainLoop                                                                    =
//==============================================================================
void mainLoop()
{
	INFO("Entering main loop");

	HighRezTimer mainLoopTimer;
	mainLoopTimer.start();
	HighRezTimer::Scalar prevUpdateTime = HighRezTimer::getCrntTime();
	HighRezTimer::Scalar crntTime = prevUpdateTime;

	while(1)
	{
		HighRezTimer timer;
		timer.start();

		prevUpdateTime = crntTime;
		crntTime = HighRezTimer::getCrntTime();

		//
		// Update
		//
		mainLoopExtra();
		void execStdinScpripts();
		execStdinScpripts();
		SceneSingleton::get().getPhysMasterContainer().update(prevUpdateTime, crntTime);
		SceneSingleton::get().updateAllWorldStuff(prevUpdateTime, crntTime);
		SceneSingleton::get().doVisibilityTests(*AppSingleton::get().getActiveCam());
		SceneSingleton::get().updateAllControllers();
		event::ManagerSingleton::get().updateAllEvents(prevUpdateTime, crntTime);
		r::MainRendererSingleton::get().render(*AppSingleton::get().getActiveCam());

		painter->setPosition(Vec2(0.0, 0.1));
		painter->setColor(Vec4(1.0));
		//painter->drawText("A");
		const r::MainRenderer& r = r::MainRendererSingleton::get();
		std::stringstream ss;
		ss << "MS: " << r.getMsTime() * 1000000 << " IS: " <<
			r.getIsTime() * 1000000 << " BS: " << r.getBsTime() * 1000000 <<
			" PPS: " << r.getPpsTime() * 1000000 << " DBG: " <<
			r.getDbgTime() * 1000000;

		ss << "\n" << AppSingleton::get().getActiveCam()->
			getVisibleMsRenderableNodes().size();
		painter->drawText(ss.str());

		if(InputSingleton::get().getKey(SDL_SCANCODE_ESCAPE))
		{
			break;
		}

		if(InputSingleton::get().getKey(SDL_SCANCODE_F11))
		{
			AppSingleton::get().togleFullScreen();
		}

		if(InputSingleton::get().getKey(SDL_SCANCODE_F12) == 1)
		{
			r::MainRendererSingleton::get().takeScreenshot("gfx/screenshot.jpg");
		}

		AppSingleton::get().swapBuffers();


		//
		// Async resource loading
		//
		if(ResourceManagerSingleton::get().getAsyncLoadingRequestsNum() > 0)
		{
			HighRezTimer::Scalar a = timer.getElapsedTime();
			HighRezTimer::Scalar b = AppSingleton::get().getTimerTick();
			HighRezTimer::Scalar timeToSpendForRsrcPostProcess;
			if(a < b)
			{
				timeToSpendForRsrcPostProcess = b - a;
			}
			else
			{
				timeToSpendForRsrcPostProcess = 0.001;
			}
			ResourceManagerSingleton::get().postProcessFinishedLoadingRequests(timeToSpendForRsrcPostProcess);
		}

		//
		// Sleep
		//
		timer.stop();
		if(timer.getElapsedTime() < AppSingleton::get().getTimerTick())
		{
			SDL_Delay((AppSingleton::get().getTimerTick() - timer.getElapsedTime()) * 1000.0);
		}

		/*if(r::MainRendererSingleton::get().getFramesNum() == 100)
		{
			break;
		}*/
	}

	INFO("Exiting main loop (" << mainLoopTimer.getElapsedTime() << " sec)");
}


//==============================================================================
// initSubsystems                                                              =
//==============================================================================
void initSubsystems(int argc, char* argv[])
{
	// App
	AppSingleton::get().init(argc, argv);

	// Main renderer
	r::RendererInitializer initializer;
	initializer.ms.ez.enabled = true;
	initializer.dbg.enabled = true;
	initializer.is.sm.bilinearEnabled = true;
	initializer.is.sm.enabled = true;
	initializer.is.sm.pcfEnabled = true;
	initializer.is.sm.resolution = 1024;
	initializer.is.sm.level0Distance = 3.0;
	initializer.pps.hdr.enabled = true;
	initializer.pps.hdr.renderingQuality = 0.25;
	initializer.pps.hdr.blurringDist = 1.0;
	initializer.pps.hdr.blurringIterationsNum = 2;
	initializer.pps.hdr.exposure = 4.0;
	initializer.pps.ssao.blurringIterationsNum = 4;
	initializer.pps.ssao.enabled = true;
	initializer.pps.ssao.renderingQuality = 0.3;
	initializer.pps.bl.enabled = true;
	initializer.pps.bl.blurringIterationsNum = 2;
	initializer.pps.bl.sideBlurFactor = 1.0;
	initializer.mainRendererQuality = 1.0;

	r::MainRendererSingleton::get().init(initializer);

	// Scripting engine
	const char* commonPythonCode =
		"import sys\n"
		"from Anki import *\n"
		"\n"
		"class StdoutCatcher:\n"
		"    def write(self, str_):\n"
		"        if str_ == \"\\n\": return\n"
		"        line = sys._getframe(1).f_lineno\n"
		"        file = sys._getframe(1).f_code.co_filename\n"
		"        func = sys._getframe(1).f_code.co_name\n"
		"        LoggerSingleton.get().write(file, line, "
		"func, str_ + \"\\n\")\n"
		"\n"
		"class StderrCatcher:\n"
		"    def write(self, str_):\n"
		"        line = sys._getframe(1).f_lineno\n"
		"        file = sys._getframe(1).f_code.co_filename\n"
		"        func = sys._getframe(1).f_code.co_name\n"
		"        LoggerSingleton.get().write(file, line, func, str_)\n"
		"\n"
		"sys.stdout = StdoutCatcher()\n"
		"sys.stderr = StderrCatcher()\n";

	script::EngineSingleton::get().execScript(commonPythonCode);

	// Stdin listener
	StdinListenerSingleton::get().start();

	// Parallel jobs
	parallel::ManagerSingleton::get().init(4);

	// Add drawer to physics
	SceneSingleton::get().getPhysMasterContainer().setDebugDrawer(
		new r::PhysDbgDrawer(r::MainRendererSingleton::get().getDbg()));
}


//==============================================================================
// execStdinScpripts                                                           =
//==============================================================================
/// The func pools the stdinListener for string in the console, if
/// there are any it executes them with scriptingEngine
void execStdinScpripts()
{
	while(1)
	{
		std::string cmd = StdinListenerSingleton::get().getLine();

		if(cmd.length() < 1)
		{
			break;
		}

		try
		{
			script::EngineSingleton::get().execScript(cmd.c_str(),
				"command line input");
		}
		catch(Exception& e)
		{
			ERROR(e.what());
		}
	}
}

//==============================================================================
// main                                                                        =
//==============================================================================
int main(int argc, char* argv[])
{
	int exitCode;

	try
	{
		initSubsystems(argc, argv);
		init();

		mainLoop();

		INFO("Exiting...");
		AppSingleton::get().quit(EXIT_SUCCESS);
		exitCode = 0;
	}
	catch(std::exception& e)
	{
		//ERROR("Aborting: " << e.what());
		std::cerr << "Aborting: " << e.what() << std::endl;
		//abort();
		exitCode = 1;
	}

	return exitCode;
}
