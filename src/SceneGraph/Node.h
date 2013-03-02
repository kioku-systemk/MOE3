/*
 * SceneGraph/Node.h
 *
 * SceneGraph Library
 * Created by kioku on 11/09/15.
 *
 */

#ifndef INCLUDE_SCENEGPRAH_NODE_H
#define INCLUDE_SCENEGPRAH_NODE_H

#include "../Core/Type.h"
#include "NodeType.h"

#include <assert.h>
#include <string>

namespace MOE
{
	namespace SceneGraph
	{
		class RefPtr
		{
		protected:
			RefPtr()           { m_ref = 0; }
			virtual ~RefPtr()  {};
		public:	
			s32 Ref()          { return ++m_ref; }
			s32 GetRef() const { return m_ref; }
			s32 Unref()        {
				const s32 ref = --m_ref;
				assert(ref>=0);
				
				if (!ref)
					delete this;
				return ref;
			}
			
		private:
			s32 m_ref;
		};
		
		class Group;
		class Node : public RefPtr
		{
		protected:
			Node(const NODETYPE type) : RefPtr(), m_type(type), m_visible(true) ,m_parent(0){}
			void setParent(Group* parent)
			{
				m_parent = parent;
			}
			friend class Group;
		public:
			virtual ~Node(){}
			
			NODETYPE GetType() const { return m_type; };
			void SetName(const std::string& name)
			{
				m_name = name;
			}
			const std::string& GetName() const
			{
				return m_name;
			}
			void SetVisible(b8 visible)
			{
				m_visible = visible;
			}
			b8 GetVisible() const
			{
				return m_visible;
			}
			Group* GetParent() const
			{
				return m_parent;
			}
		private:
			const NODETYPE m_type;
            std::string m_name;
			b8 m_visible;
			Group* m_parent;
		};
		
		template<class T>
		class NodeRefPtr
		{
		public:
			NodeRefPtr()
			{
				m_p = 0;
			}
			NodeRefPtr(T* s)
			{
				m_p = s;
				if (m_p)
					m_p->Ref();
			}
			NodeRefPtr(const NodeRefPtr& p)
			{
				m_p = p;
				if (m_p)
					m_p->Ref();
			}
			~NodeRefPtr()
			{
				if (m_p)
					m_p->Unref();
			}
			
			T* Get() const
			{
				return m_p;
			}
			
			operator T*() const
			{
				return m_p;
			}
			T& operator *() const
			{
				assert(m_p);
				return *m_p;
			}
			T* operator->() const
			{
				assert(m_p);
				return m_p;
			}
			bool operator !() const
			{
				return m_p == 0;
			}
			bool operator ==(T* p) const
			{
				return m_p == p;
			}
			bool operator !=(T* p) const
			{
				return m_p != p;
			}
			bool operator <(T* p) const // for STL container
			{
				return m_p < p;
			}
			
			//equal operator
			T* operator =(T* p)
			{
				if (p)
					p->Ref();
				if (m_p)
					m_p->Unref();
				m_p = p;
				return m_p;
			}
			const NodeRefPtr& operator =(const NodeRefPtr& r)
			{
				if (this->m_p == r.m_p)
					return *this;
				
				if (r.m_p)
					r.m_p->Ref();
				if (m_p)
					m_p->Unref();
				m_p = r.m_p;
				
				return *this;
			}
			
		private:
			T* m_p;
		};
		
	} // namespace SceneGraph
    
} // namespace MOE

#endif // INCLUDE_SCENEGPRAH_NODE_H
