/*
 * SceneGraph/Geometry.h
 *
 * SceneGraph Library
 * Created by kioku on 11/09/15.
 *
 */

#ifndef INCLUDE_SCENEGRAPH_GEOMETRY_H
#define INCLUDE_SCENEGRAPH_GEOMETRY_H

#include "../Core/Type.h"

#include "Node.h"
#include "../Core/Math.h"
#include <float.h>

namespace MOE
{
	namespace SceneGraph
	{
		typedef u16 Index;
		typedef u32 Index32;
		class Material;
		
		class Geometry : public Node
		{
		public:
			Geometry(NODETYPE nodetype = NODETYPE_GEOMETRY) : Node(nodetype){
				m_vertexBuffer = 0;
				m_indexBuffer  = 0;
				m_vertexCount  = 0;
				m_indexCount   = 0;
				m_material = 0;
				
				m_bbox[0] = m_bbox[1] = m_center = Math::vec3(0,0,0);
				m_radius = 0;
				
				m_needUpdate = false;
			}
			~Geometry()
			{
				delete [] m_vertexBuffer;
				delete [] m_indexBuffer;
			}
			
			class VertexFormat
			{
			public:
				Math::vec3 pos;
				u32        col;
				Math::vec3 normal;
				Math::vec2 uv;
			};
					
			virtual void Alloc(u32 vertexCount, u32 indexCount){
				delete [] m_vertexBuffer;
				delete [] m_indexBuffer;
				m_indexBuffer = 0;
				m_vertexBuffer = mnew VertexFormat[vertexCount];
				if (indexCount) {
					if(vertexCount < 65536)
						m_indexBuffer  = mnew Index[indexCount];
					else
						m_indexBuffer  = mnew Index[indexCount*2]; // for Index32
				}
				m_indexCount   = indexCount;
				m_vertexCount  = vertexCount;
				EnableNeedUpdate();
			}
			
			s32 GetIndexTypeSize() const {
				if (!m_indexCount)
					return 0;
				if (m_vertexCount < 65536)
					return 2; // Index
				else
					return 4; // Index32
			}
			
			void MinimizeIndex(u32 new_indexCount) {
				if (m_indexCount <= new_indexCount)
					return;
				
				m_indexCount = new_indexCount;
			}
			
			u32 GetVertexCount() const {
				return m_vertexCount;
			}
			
			u32 GetIndexCount() const {
				return m_indexCount;
			}
			
			const VertexFormat* GetVertex() const {
				return m_vertexBuffer;
			}
			
			VertexFormat* GetVertex() {
				return m_vertexBuffer;
			}
			
			void* GetIndex(){
				return m_indexBuffer;
			}
			
			const void* GetIndex() const {
				return m_indexBuffer;
			}
			
			void CalcBounds() {
				using namespace Math;
				m_bbox[0] = vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
				m_bbox[1] = vec3( FLT_MAX, FLT_MAX, FLT_MAX);
				for (u32 i = 0; i < m_vertexCount; i++) {
					for (u32 j = 0; j < 3; j++) {
						m_bbox[0][j] = max(m_bbox[0][j], m_vertexBuffer[i].pos[j]);
						m_bbox[1][j] = min(m_bbox[1][j], m_vertexBuffer[i].pos[j]);
					}
				}
				m_center = (m_bbox[0] + m_bbox[1]) * 0.5;
				m_radius = static_cast<f32>(length(m_bbox[0] - m_bbox[1]) * 0.5);
			}
			
			const Math::vec3& GetMax() const
			{
				return m_bbox[0];
			}
			
			const Math::vec3& GetMin() const
			{
				return m_bbox[1];
			}
			
			f32 GetRadius() const
			{
				return m_radius;
			}
			
			const Math::vec3& GetCenter() const
			{
				return m_center;
			}
			
			b8 GetNeedUpdate() const
			{
				return m_needUpdate;
			}
			
			void DisableNeedUpdate() const
			{
				m_needUpdate = false;
			}
			void EnableNeedUpdate() const
			{
				m_needUpdate = true;
			}
			
			void SetMaterial(Material* mat)
			{
				m_material = mat;
			}
			Material* GetMaterial() const
			{
				return m_material;
			}
			
		protected:
			mutable b8 m_needUpdate;

			u32 m_vertexCount;
			u32 m_indexCount;
			
			VertexFormat* m_vertexBuffer;
			u16* m_indexBuffer;
			Material* m_material;
			
			// BBox and BSphere
			Math::vec3 m_bbox[2];// 0 - max, 1 - min
			Math::vec3 m_center;
			f32 m_radius;
			
		};
		
	} // namespace SceneGraph
} // namespace MOE

#endif // INCLUDE_SCENEGRAPH_GEOMETRY_H
