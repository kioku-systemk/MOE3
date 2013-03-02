/*
 * SceneGraph/Transform.h
 *
 * SceneGraph Library
 * Created by kioku on 11/09/15.
 *
 */

#ifndef INCLUDE_SCENEGPRAH_TRANSFORM_H
#define INCLUDE_SCENEGPRAH_TRANSFORM_H

#include "../Core/Type.h"
#include "../Core/Math.h"
#include "Node.h"

namespace MOE
{
	namespace SceneGraph
	{
		class Transform : public Group
		{
		public:
			Transform(NODETYPE nodetype = NODETYPE_TRANSFORM) : Group(nodetype)
			{
				m_mat = Math::Identity();
			}
			~Transform(){}
			
			void SetMatrix(const Math::matrix& mat)
			{
				m_mat = mat;
			}
			const Math::matrix GetMatrix() const
			{
				return m_mat;
			}

		private:
			Math::matrix m_mat;
		};
		
		class Joint : public Transform
		{
		public:
			Joint() : Transform(NODETYPE_JOINT)
			{
				m_mat_orig_inverse = Math::Identity();
			}
			~Joint(){}
			
			void SetOrigInverseMatrix(const Math::matrix& mat)
			{
				m_mat_orig_inverse = mat;
			}
			const Math::matrix GetOrigInverseMatrix() const
			{
				return m_mat_orig_inverse;
			}
		private:
			Math::matrix m_mat_orig_inverse;
		};

	} // namespace SceneGraph
		
} // namespace MOE

#endif // INCLUDE_SCENEGRAPH_TRANSFORM_H
