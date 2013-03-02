/*
 * SceneGraph/GeometryCache.h
 *
 * SceneGraph Library
 * Created by kioku on 12/09/02.
 *
 */

#ifndef INCLUDE_SCENEGPRAH_GEOMETRYCACHE_H
#define INCLUDE_SCENEGRAPH_GEOMETRYCACHE_H

#include "../Core/Type.h"

#include "Geometry.h"
#include "Transform.h"
#include "../Core/Math.h"
#include <float.h>

namespace MOE
{
	namespace SceneGraph
	{
		class GeometryCache : public Geometry
		{
		public:
			GeometryCache() : Geometry(NODETYPE_GEOMETRYCACHE){
				m_vertexWeightBuffer = 0;
				m_matrixNode = 0;
				m_geo = 0;
			}
			~GeometryCache()
			{
				delete [] m_vertexWeightBuffer;
				delete [] m_matrixNode;
			}
			
			enum {
				MAX_WEIGHT_NUM = 8
			};

			class VertexWeightFormat
			{
			public:
				s8 wid[MAX_WEIGHT_NUM];
				float weight[MAX_WEIGHT_NUM];
			};
					
			void Alloc(u32 vertexCount, u32 indexCount){
				assert(vertexCount < 65536);
				Geometry::Alloc(vertexCount, indexCount);
				m_vertexWeightBuffer = mnew VertexWeightFormat[vertexCount];
			}
			
			void AllocNodeMatrixTable(s32 num){
				m_matrixNodeCount = num;
				m_matrixNode = mnew Joint*[num];
				for (s32 i = 0; i < num; ++i)
					m_matrixNode[i] = 0;
			}
			s32 GetNodeMatrixTableCount() const {
				return m_matrixNodeCount;
			}
			
			void SetNodeMatrixTable(s32 i, Joint* node)
			{
				if (i >= m_matrixNodeCount)
					return;
				m_matrixNode[i] = node;
			}
			Joint* GetNodeMatrixTable(s32 i) const
			{
				if (i >= m_matrixNodeCount)
					return 0;
				return m_matrixNode[i];
			}
			
			VertexWeightFormat* GetVertexWeight()
			{
				return m_vertexWeightBuffer;
			}
			
			const VertexWeightFormat* GetVertexWeight() const
			{
				return m_vertexWeightBuffer;
			}
			
			void SetTargetGeometry(Geometry* geo)
			{
				m_geo = geo;
			}
			Geometry* GetTargetGeometry() const
			{
				return m_geo;
			}
		private:
			VertexWeightFormat* m_vertexWeightBuffer;
			Joint** m_matrixNode;
			s32 m_matrixNodeCount;
			MOE::SceneGraph::Geometry* m_geo;
		};
		
	} // namespace SceneGraph
} // namespace MOE

#endif // INCLUDE_SCENEGPRAH_GEOMETRYCACHE_H
