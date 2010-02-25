#ifndef _SKEL_NODE_H_
#define _SKEL_NODE_H_

#include "Common.h"
#include "Node.h"
#include "Controller.h"
#include "Math.h"

class Skeleton;
class SkelAnimCtrl;


/// Skeleton node
class SkelNode: public Node
{
	public:
		Skeleton* skeleton; ///< The skeleton resource
		SkelAnimCtrl* skelAnimCtrl; ///< Hold the controller here as well

		SkelNode();
		void render();
		void init( const char* filename );
		void deinit();
};


#endif
