#ifndef PERSPECTIVE_CAMERA_H
#define PERSPECTIVE_CAMERA_H

#include "Camera.h"
#include "cln/Collision.h"


/// @todo
class PerspectiveCamera: public Camera
{
	public:
		PerspectiveCamera(Scene& scene, ulong flags, SceneNode* parent);

		/// @name Accessors
		/// @{
		float getFovX() const
		{
			return fovX;
		}
		void setFovX(float fovx);

		float getFovY() const
		{
			return fovY;
		}
		void setFovY(float fovy);
		/// @}

		void moveUpdate()
		{
			Camera::moveUpdate();
			wspaceCShape =
				lspaceCShape.getCollisionShapeType(getWorldTransform());
		}

		/// @copydoc SceneNode::getVisibilityCollisionShapeWorldSpace
		const CollisionShape* getVisibilityCollisionShapeWorldSpace() const
		{
			return &wspaceCShape;
		}

		void init(const char*)
		{}

		void setAll(float fovx, float fovy, float znear, float zfar);

	private:
		/// @name Data
		/// @{
		PerspectiveCameraShape lspaceCShape;
		PerspectiveCameraShape wspaceCShape;

		/// fovX is the angle in the y axis (imagine the cam positioned in
		/// the default OGL pos) Note that fovX > fovY (most of the time) and
		/// aspectRatio = fovX/fovY
		float fovX;
		float fovY; /// @see fovX
		/// @}

		/// Implements Camera::calcLSpaceFrustumPlanes
		void calcLSpaceFrustumPlanes();

		/// Implements Camera::calcProjectionMatrix
		void calcProjectionMatrix();

		/// Update:
		/// - The projection matrix
		/// - The planes
		/// - The collision shape
		void updateLocals()
		{
			calcProjectionMatrix();
			calcLSpaceFrustumPlanes();
			lspaceCShape.setAll(fovX, fovY, zNear, zFar,
				Transform::getIdentity());
		}
};


inline PerspectiveCamera::PerspectiveCamera(Scene& scene, ulong flags,
	SceneNode* parent)
:	Camera(CT_PERSPECTIVE, scene, flags, parent)
{
	name = "PerspectiveCamera:" + name;
}


inline void PerspectiveCamera::setFovX(float fovx_)
{
	fovX = fovx_;
	update();
}


inline void PerspectiveCamera::setFovY(float fovy_)
{
	fovY = fovy_;
	calcProjectionMatrix();
	calcLSpaceFrustumPlanes();
}


#endif
