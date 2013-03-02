/*
 *  Util.h
 *
 */

#ifndef INCLUDE_SCENEGRAPH_UTIL_H
#define INCLUDE_SCENEGRAPH_UTIL_H

#include <float.h>

namespace MOE {
namespace SceneGraph{

	inline b8 _getbbox(const Node* node, Math::vec3& box_max, Math::vec3& box_min, const Math::matrix4x4& mat)
	{
		b8 r = false;
		NODETYPE t = node->GetType();
		if (t == NODETYPE_GROUP)
		{
			const Group* g = static_cast<const Group*>(node);
			const s32 n = g->GetChildCount();
			for (s32 i = 0; i < n; i++)
				r |= _getbbox(g->GetChild(i), box_max, box_min, mat);
		}
		else if (t == NODETYPE_TRANSFORM || t == NODETYPE_JOINT)
		{
			const Transform* trs = static_cast<const Transform*>(node);
			const s32 n = trs->GetChildCount();
			const Math::matrix4x4 lworld = mat * trs->GetMatrix();
			for (s32 i = 0; i < n; i++)
				r |= _getbbox(trs->GetChild(i), box_max, box_min, lworld);
		}
		else if (t == NODETYPE_GEOMETRY)
		{
			using namespace Math;
			const Geometry* g = static_cast<const Geometry*>(node);
			vec3 p[2] = {g->GetMax(), g->GetMin()};
			for (u32 j = 0; j < 2; ++j) {
				p[j] = (mat * vec4(p[j],1.0)).xyz();
				for (u32 i = 0; i < 3; ++i) {
					box_max[i] = max(p[j][i], box_max[i]);
					box_min[i] = min(p[j][i], box_min[i]);
				}
			}
			return true;
		}
		return r;
	}
	
	inline b8 GetBBox(const Node* node, Math::vec3& box_max, Math::vec3& box_min)
	{
		box_max = Math::vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		box_min = Math::vec3( FLT_MAX, FLT_MAX, FLT_MAX);
		Math::matrix4x4 world = Math::Identity();
		return _getbbox(node, box_max, box_min, world);
	}
	
	template<class T>
	inline void visitAllGeometry(const Node* node, T& func)
	{
		if (!node)
			return;
		
		NODETYPE t = node->GetType();
		if (t == NODETYPE_GROUP || t == NODETYPE_TRANSFORM || t == NODETYPE_JOINT)
		{
			const Group* g = static_cast<const Group*>(node);
			const s32 n = g->GetChildCount();
			for (s32 i = 0; i < n; i++)
				visitAllGeometry(g->GetChild(i), func);
		}
		else if (t == NODETYPE_GEOMETRY)
		{
			using namespace Math;
			const Geometry* g = static_cast<const Geometry*>(node);
			func(g);
		}
	}
	
	template<class T>
	inline void VisitAllGeometry(const Node* node, T& func)
	{
		visitAllGeometry(node, func);
	}

	template<class T>
	inline void visitAllGeometry(Node* node, T& func)
	{
		if (!node)
			return;
		
		NODETYPE t = node->GetType();
		if (t == NODETYPE_GROUP || t == NODETYPE_TRANSFORM || t == NODETYPE_JOINT)
		{
			Group* g = static_cast<Group*>(node);
			const s32 n = g->GetChildCount();
			for (s32 i = 0; i < n; i++)
				visitAllGeometry(g->GetChild(i), func);
		}
		else if (t == NODETYPE_GEOMETRY)
		{
			using namespace Math;
			Geometry* g = static_cast<Geometry*>(node);
			func(g);
		}
	}
	
	template<class T>
	inline void VisitAllGeometry(Node* node, T& func)
	{
		visitAllGeometry(node, func);
	}

	// --- VisitAllNode ----
	template<class T>
	inline void visitAllNode(const Node* node, T& func)
	{
		if (!node)
			return;

		func(node);
		
		NODETYPE t = node->GetType();
		if (t == NODETYPE_GROUP || t == NODETYPE_TRANSFORM || t == NODETYPE_JOINT)
		{
			const Group* g = static_cast<const Group*>(node);
			const s32 n = g->GetChildCount();
			for (s32 i = 0; i < n; i++)
				visitAllNode(g->GetChild(i), func);
		}
	}
	
	template<class T>
	inline void VisitAllNode(const Node* node, T& func)
	{
		visitAllNode(node, func);
	}

	template<class T>
	inline void visitAllNode(Node* node, T& func)
	{
		if (!node)
			return;
		
		func(node);
		
		NODETYPE t = node->GetType();
		if (t == NODETYPE_GROUP || t == NODETYPE_TRANSFORM || t == NODETYPE_JOINT)
		{
			Group* g = static_cast<Group*>(node);
			const s32 n = g->GetChildCount();
			for (s32 i = 0; i < n; i++)
				visitAllNode(g->GetChild(i), func);
		}
	}
	
	template<class T>
	inline void VisitAllNode(Node* node, T& func)
	{
		visitAllNode(node, func);
	}

} // SG
} // MOE

#endif // INCLUDE_SCENEGRAPH_UTIL_H
