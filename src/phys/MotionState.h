#ifndef MOTION_STATE_H
#define MOTION_STATE_H

#include <LinearMath/btMotionState.h>
#include "scene/SceneNode.h"


/// A custom motion state
class MotionState: public btMotionState
{
	public:
		MotionState(const Transform& initialTransform, SceneNode* node_);
		~MotionState() {}

		/// @name Bullet implementation of virtuals
		/// @{
		void getWorldTransform(btTransform& worldTrans) const;
		const btTransform& getWorldTransform() const;
		void setWorldTransform(const btTransform& worldTrans);
		/// @}

	private:
		btTransform worldTransform;
		SceneNode* node; ///< Pointer cause it may be NULL
};


inline MotionState::MotionState(const Transform& initialTransform,
	SceneNode* node_)
:	worldTransform(toBt(initialTransform)),
	node(node_)
{}


inline void MotionState::getWorldTransform(btTransform& worldTrans) const
{
	worldTrans = worldTransform;
}


inline const btTransform& MotionState::getWorldTransform() const
{
	return worldTransform;
}


inline void MotionState::setWorldTransform(const btTransform& worldTrans)
{
	worldTransform = worldTrans;

	if(node)
	{
		Transform& nodeTrf = node->getLocalTransform();
		float originalScale = nodeTrf.getScale();
		nodeTrf = toAnki(worldTrans);
		nodeTrf.setScale(originalScale);
	}
}


#endif
