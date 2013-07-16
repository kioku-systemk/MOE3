//
//  AnimLoader.h
//
//  Created by kioku on 12/06/21.
//  Copyright 2011 System K. All rights reserved.
//

#ifndef INCLUDE_MOE_ANIMLOADER_H
#define INCLUDE_MOE_ANIMLOADER_H

#include "../Core/Type.h"


namespace MOE {
class Stream;
namespace SceneGraph {
	class Node;
}
	
class Animation
{
public:
	class internalAnimation;
	Animation(internalAnimation* internalAnim);
	~Animation();
	void Animate(MOE::SceneGraph::Node* node, double time);
	f64 GetMaxAnimTime();
	
private:
	internalAnimation* anim;
	
};

class AnimLoader
{
public:
	AnimLoader();
	~AnimLoader();
	
	Animation* Load(const Stream* st);
};
	
} // namespace MOE

#endif // INCLUDE_MOE_ANIMLOADER_H