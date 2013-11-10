/*
 * SceneGraph/Transform.h
 *
 * SceneGraph Library
 * Created by kioku on 11/09/15.
 *
 */

#ifndef INCLUDE_SCENEGPRAH_CAMERA_H
#define INCLUDE_SCENEGPRAH_CAMERA_H

#include "../Core/Type.h"
#include "Node.h"

namespace MOE
{
	namespace SceneGraph
	{
		class Camera : public Node
		{
		public:
			Camera(NODETYPE nodetype = NODETYPE_CAMERA) : Node(nodetype)
			{
				m_holfov = 60.0f;
			}
			~Camera(){}
			
			void SetFov(f32 holfov)
			{
                m_holfov = holfov;
			}
			f32 GetFov() const
			{
				return m_holfov;
			}

		private:
            f32 m_holfov;
		};
    } // namespace SceneGraph
		
} // namespace MOE

#endif // INCLUDE_SCENEGPRAH_CAMERA_H
