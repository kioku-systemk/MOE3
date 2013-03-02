/*
 *
 * MatrixStack.h
 * 
 */


#ifndef __MOE_MATRIXSTACK_H__
#define __MOE_MATRIXSTACK_H__

#include "Type.h"
#include <vector>

namespace MOE {
	
	class MatrixStack
	{
	public:
		MatrixStack()
		{
			Clear();
		}
		
		const Math::matrix& GetMatrix()
		{
			return m_mat[m_mat.size() - 1];
		}
		void Push(const Math::matrix& mat)
		{
			const Math::matrix m = mat * GetMatrix();
			m_mat.push_back(m);
		}
		void Pop()
		{
			if (m_mat.size()>1)
				m_mat.pop_back();
		}
		void Clear()
		{
			m_mat.clear();
			Math::matrix E = Math::Identity();
			m_mat.push_back(E);
		}
	private: 
		std::vector<Math::matrix> m_mat;
	};

} // namespace MOE

#endif // __MOE_MATRIXSTACK_H__

