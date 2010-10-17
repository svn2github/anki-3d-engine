#ifndef SCENE_H
#define SCENE_H

#include "Object.h"
#include "skybox.h"
#include "Physics.h"
#include "Exception.h"


class SceneNode;
class Light;
class Camera;
class MeshNode;
class SkelNode;
class Controller;
class ParticleEmitter;


/// The Scene contains all the dynamic entities
class Scene: public Object
{
	//PROPERTY_RW(Vec3, ambientCol, setAmbientCol, getAmbientCol) ///< The global ambient color
	PROPERTY_RW(Vec3, sunPos, setSunPos, getSunPos)
	//PROPERTY_R(Physics*, phyWorld, getPhysics) ///< Connection with bullet

	public:
		/// The container template class. Extends vector
		template<typename Type> class Container: public Vec<Type*>
		{};

		// Containers of scene's data
		Container<SceneNode> nodes;
		Container<Light> lights;
		Container<Camera> cameras;
		Container<MeshNode> meshNodes;
		Container<SkelNode> skelNodes;
		Container<Controller> controllers;
		Container<ParticleEmitter> particleEmitters;
		Skybox skybox; // ToDo to be removed

		// The funcs
		Scene(Object* parent);
		~Scene() throw() {}

		void registerNode(SceneNode* node); ///< Put a node in the appropriate containers
		void unregisterNode(SceneNode* node);
		void registerController(Controller* controller);
		void unregisterController(Controller* controller);

		void updateAllWorldStuff();
		void updateAllControllers();

		/// @name Accessors
		/// @{
		Vec3& getAmbientCol() {return ambientCol;}
		void setAmbientCol(const Vec3& col) {ambientCol = col;}
		Physics& getPhysics();
		/// @}

	private:
		Vec3 ambientCol; ///< The global ambient color
		Physics* physics; ///< Connection with Bullet wrapper

		/// Adds a node in a container
		template<typename ContainerType, typename Type>
		void putBackNode(ContainerType& container, Type* x);

		/// Removes a node from a container
		template<typename ContainerType, typename Type>
		void eraseNode(ContainerType& container, Type* x);
};


template<typename ContainerType, typename Type>
inline void Scene::putBackNode(ContainerType& container, Type* x)
{
	RASSERT_THROW_EXCEPTION(std::find(container.begin(), container.end(), x) != container.end());
	container.push_back(x);
}


template<typename ContainerType, typename Type>
inline void Scene::eraseNode(ContainerType& container, Type* x)
{
	typename ContainerType::iterator it = std::find(container.begin(), container.end(), x);
	RASSERT_THROW_EXCEPTION(it == container.end());
	container.erase(it);
}


inline Physics& Scene::getPhysics()
{
	RASSERT_THROW_EXCEPTION(physics == NULL);
	return *physics;
}


#endif
