/*
 *  Memory.h
 *
 */

#ifndef INCLUDE_MOE_MEMORY_HEADER
#define INCLUDE_MOE_MEMORY_HEADER

#ifndef NOTUSE_MOEMEMORY

#if defined(WIN32) || defined(WIN64)
#ifndef CDECL
#define CDECL __cdecl
#endif
#pragma warning(disable : 4985) // ignore SAL Attribute
#else
#define CDECL //__attribute__((__cdecl__))
#endif

#ifdef _WIN32

#define ALWAYS_INLINE 
#define MEMSIZE_T size_t

#else // Mac

#define ALWAYS_INLINE __attribute__ ((always_inline))

#define MEMSIZE_T long unsigned int

#endif // Mac

namespace MOE {
    
class Memory
{
public:
	static void MemoryLeakReport();
	static MEMSIZE_T AllocedSize();
	
	static void* internal_new(MEMSIZE_T size, const char* file, const int line);
	static void  internal_delete(void* p, const char* file, const int line);
private:
	static void* m_memory;
	static MEMSIZE_T m_allocedsize;
};

} // namespace MOE

void* CDECL operator new(MEMSIZE_T size, const char* file, int line);
void  CDECL operator delete(void* p, const char* file, int line);
void* CDECL operator new[](MEMSIZE_T size, const char* file, int line);
void  CDECL operator delete[](void* p, const char* file, int line);

#ifndef MOEMEMORY_CPP

#include <new>

// Global new/delete override
/*inline void* CDECL operator new(MEMSIZE_T size) throw(std::bad_alloc)  ALWAYS_INLINE;
inline void* CDECL operator new(MEMSIZE_T size) throw(std::bad_alloc)
{
	return MOE::Memory::internal_new(size, 0, 0);
}
inline void  CDECL operator delete(void* p) throw() ALWAYS_INLINE;
inline void  CDECL operator delete(void* p) throw()
{
    MOE::Memory::internal_delete(p, 0, 0);
}

// Global new[]/delete[] override
inline void* CDECL operator new[](MEMSIZE_T size) throw(std::bad_alloc)  ALWAYS_INLINE;
inline void* CDECL operator new[](MEMSIZE_T size) throw(std::bad_alloc)
{
	return MOE::Memory::internal_new(size, 0, 0);
}
inline void  CDECL operator delete[](void* p) throw() ALWAYS_INLINE;
inline void  CDECL operator delete[](void* p) throw()
{
    MOE::Memory::internal_delete(p, 0, 0);
}*/

#define mnew             new(__FILE__,__LINE__)
template<typename T> void mdelete(T* p)
{
	if (p){
		p->~T();
		operator delete((void*)p,__FILE__,__LINE__);
	}
}
template<class T> void mdeleteArray(T* p)
{
	if (p) {
		//p->~T();// TODO
		operator delete[](p,__FILE__,__LINE__);
	}
}

#endif // MOEMEMORY_CPP

#else // NOTUSE_MOEMEMORY

#define mnew new
#include <new>

template<typename T> void MOEdelete(T* p)
{
	delete p;
}
template<class T> void MOEdeleteArray(T* p)
{
	delete [] p;
}

#define MEMSIZE_T size_t

namespace MOE {
    
class Memory
{
public:
	static void MemoryLeakReport() {};
	static MEMSIZE_T AllocedSize() {return 0;};
};

} // namespace MOE

#endif // NOTUSE_MOEMEMORY

#endif // INCLUDE_MOEMEMORY_HEADER
