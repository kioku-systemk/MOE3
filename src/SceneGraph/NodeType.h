/*
 * SceneGraph/Transform.h
 *
 * SceneGraph Library
 * Created by kioku on 11/09/15.
 *
 */

#ifndef INCLUDE_SCENEGPRAH_TRANSFORM_H
#define INCLUDE_SCENEGRAPH_TRANSFORM_H

#include "../Core/Type.h"
#include "Node.h"

namespace MOE
{
	namespace SceneGraph
	{
		enum NODETYPE
		{
			NODETYPE_UNKNOWN,
			NODETYPE_GROUP,
			NODETYPE_GEOMETRY,
			NODETYPE_MATERIAL,
			NODETYPE_TRANSFORM,
			NODETYPE_JOINT,
			NODETYPE_GEOMETRYCACHE,
			NODETYPE_WAVESOUND,
            NODETYPE_INSTANCE
		};
		
	} // namespace SceneGraph
		
} // namespace MOE

#endif // INCLUDE_SCENEGRAPH_TRANSFORM_H
