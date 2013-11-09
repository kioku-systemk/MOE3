/*
 * SceneGraph/Instance.h
 *
 * SceneGraph Library
 * Created by kioku on 11/09/15.
 *
 */

#ifndef INCLUDE_SCENEGRAPH_INSTANCE_H
#define INCLUDE_SCENEGRAPH_INSTANCE_H

#include "../Core/Type.h"

#include "Node.h"

namespace MOE
{
	namespace SceneGraph
	{
        class Instance : public Node
		{
		public:
			Instance(NODETYPE nodetype = NODETYPE_INSTANCE) : Node(nodetype), m_instnode(0){}
			~Instance(){}
            
            void SetInstanceName(const std::string& name)
            {
                m_instname = name;
            }
            
            const std::string GetInstanceName() const
            {
                return m_instname;
            }
            
            void SetInstanceNode(Node* node)
            {
                m_instnode = node;
            }
            
            Node* GetInstanceNode() const
            {
                return m_instnode;
            }
			
		protected:
            std::string m_instname;
            Node* m_instnode;
		};
		
	} // namespace SceneGraph
} // namespace MOE

#endif // INCLUDE_SCENEGRAPH_INSTANCE_H
