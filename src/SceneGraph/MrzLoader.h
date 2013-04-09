//
//  MrzLoader.h
//
//  Created by kioku on 11/09/16.
//  Copyright 2011 System K. All rights reserved.
//

#ifndef INCLUDE_MOE_MRZLOADER_H
#define INCLUDE_MOE_MRZLOADER_H

#include "../Core/Type.h"
#include "BaseLoader.h"

namespace MOE {
	
class MrzLoader : public BaseLoader
{
public:
	MrzLoader();
	~MrzLoader();
	
	SceneGraph::Node* Load(const Stream* st);
};
	
} // namespace MOE

#endif // INCLUDE_MOE_MRZLOADER_H