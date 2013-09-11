/*
 *
 * Render.h
 *
 * pure virtual class
 * 
 */


#ifndef __MOE_RENDER_H__
#define __MOE_RENDER_H__

#include "../Core/Type.h"

namespace MOE {
	namespace SG {
		class Node;
	}
	namespace Math {
		struct matrix4x4;
	}
}

class IRender
{
protected:
	IRender(){};
	
public:
	virtual ~IRender(){};
	
	virtual void Draw(const MOE::SG::Node* node) = 0;
	virtual void Resize(u32 w, u32 h) = 0;

	virtual void SetViewMat(const MOE::Math::matrix4x4& mat) = 0;
	virtual void SetProjMat(const MOE::Math::matrix4x4& mat) = 0;
	
};

#endif // __MOE_RENDER_H__

