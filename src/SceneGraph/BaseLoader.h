//
//  BaseLoader.h
//
//  Created by kioku on 11/09/16.
//  Copyright 2011 System K. All rights reserved.
//

#ifndef INCLUDE_MOE_BASELOADER_H
#define INCLUDE_MOE_BASELOADER_H

#include "../Core/Type.h"

namespace MOE {
	
class Stream;
namespace SceneGraph {
	class Node;
} // namespace SceneGraph

class BaseLoader
{
protected:
	BaseLoader(){};
	virtual ~BaseLoader(){};
	
public:
	virtual SceneGraph::Node* Load(const Stream* s) = 0;
};

} // namespace MOE


#endif // INCLUDE_MOE_BASELOADER_H