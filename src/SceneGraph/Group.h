/*
 * SceneGraph.h
 *
 * SceneGraph Library
 * Created by kioku on 11/09/15.
 *
 */

#ifndef INCLUDE_SCENEGRAPH_GROUP_H
#define INCLUDE_SCENEGRAPH_GROUP_H

#include "../Core/Type.h"
#include "Node.h"
#include <vector>

namespace MOE
{
	namespace SceneGraph
	{
		
		class Group : public Node
		{
		protected:
			Group(NODETYPE nt)  : Node(nt) {}
		public:
			Group()  : Node(NODETYPE_GROUP) {}
			~Group(){}
			
			void AddChild(Node* child)
			{
				if (!child)
					return;
			
				child->setParent(this);
				m_childs.push_back(child);
			}
			void RemoveChild(Node* child)
			{
				if (!child)
					return;
				
                std::vector< NodeRefPtr<Node> >::iterator it, eit = m_childs.end();
				for (it = m_childs.begin(); it != eit; ++it)
				{
					if (*it == child)
					{
						m_childs.erase(it);
						break;
					}
				}
			}
			s32 GetChildCount() const
			{
				return static_cast<s32>(m_childs.size());
			}
			Node* GetChild(s32 i)
			{
				if (static_cast<s32>(m_childs.size()) <= i)
					return 0;
				return m_childs[i];
			}
			const Node* GetChild(s32 i) const
			{
				if (static_cast<s32>(m_childs.size()) <= i)
					return 0;
				return m_childs[i];
			}
			
		private:
            std::vector< NodeRefPtr<Node> > m_childs;
		};
		
    } // namespace Group
} // namespace MOE

#endif // INCLUDE_SCENEGRAPH_TRANSFORM_H
