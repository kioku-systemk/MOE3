//
// Memory
//

#define MOEMEMORY_CPP
#include "Memory.h"

#ifndef NOTUSE_MOEMEMORY

namespace
{

struct mem_list
{
	const char* file;
	int line;
	MEMSIZE_T size; // WARNING max 4GB.
	mem_list* next;
};

void memoryErrorDeleteReport(void* p);

}// namespace


#ifndef NOTUSE_MOEMEMORY
#include <stdlib.h>

namespace MOE {

void* Memory::m_memory = 0;
MEMSIZE_T Memory::m_allocedsize = 0;

void* Memory::internal_new(MEMSIZE_T size, const char* file, const int line)
{
	const void* ptr = malloc(size + sizeof(mem_list));

	mem_list* cur = (mem_list*)(ptr);
	cur->next = 0;
	cur->file = file;
	cur->line = line;
	cur->size = size;
	m_allocedsize += size;
	if (!m_memory)
	{
		m_memory = cur;
	}
	else
	{
		mem_list* last = reinterpret_cast<mem_list*>(m_memory);
		while (last->next)
			last = last->next;
		last->next = cur;
	}
	
	return (void*)((char*)ptr + sizeof(mem_list));
}

void Memory::internal_delete(void* p, const char* file, const int line)
{
	if (!p)
		return;

	mem_list* cur = (mem_list*)((const char*)p - sizeof(mem_list));
	if (m_memory == cur)
	{
		m_memory = reinterpret_cast<void*>(cur->next);
		m_allocedsize -= cur->size;
		free(cur);
		return;
	}
	else
	{
		for (mem_list* ptr = reinterpret_cast<mem_list*>(m_memory); ptr; ptr = ptr->next)
		{
			if (ptr->next == cur)
			{
				ptr->next = cur->next;
				m_allocedsize -= cur->size;
				free(cur);
				return;
			}
		}
	}
	
	// not find list
	memoryErrorDeleteReport(p);
}

} // namespace MOE
	
void* CDECL operator new(MEMSIZE_T size, const char* file, const int line)  { return MOE::Memory::internal_new(size, file, line); }
void  CDECL operator delete(void* p, const char* file, const int line)      {        MOE::Memory::internal_delete(p, file, line); }
void* CDECL operator new[](MEMSIZE_T size, const char* file, const int line){ return MOE::Memory::internal_new(size, file, line); }
void  CDECL operator delete[](void* p, const char* file, const int line)    {        MOE::Memory::internal_delete(p, file, line); }



#endif // #ifndef NOTUSE_MOEMEMORY


#ifdef _WIN32
#include <windows.h>
void MOE::Memory::MemoryLeakReport()
{
	if (!m_memory)
		return;

	OutputDebugStringA("-------Memory Leak Report Start ------\n");
	for (mem_list* ptr = reinterpret_cast<mem_list*>(m_memory); ptr; ptr = ptr->next)\
	{
		char buf[1024];
		if (!ptr->file || !ptr->line)
			wsprintfA(buf, "File: ???, Line(?): %d Bytes\n", ptr->size);
		else
			wsprintfA(buf, "File: %s, Line(%d): %d Bytes\n", ptr->file, ptr->line, ptr->size);
		OutputDebugStringA(buf);
	}
	OutputDebugStringA("-------Memory Leak Report End------\n");
}

namespace
{
void memoryErrorDeleteReport(void* p)
{
	char buf[128];
	wsprintfA(buf, "Not find memory list: %X\n", p);
	OutputDebugStringA(buf);
}
}// namespace

#else // Windows

#include <iostream>
#include <cstdlib>

void MOE::Memory::MemoryLeakReport()
{
	if (!m_memory)
		return;

	std::cout << "-------Memory Leak Report Start ------" << std::endl;
	for (mem_list* ptr = reinterpret_cast<mem_list*>(m_memory); ptr; ptr = ptr->next)\
	{
		if (!ptr->file || !ptr->line)
			std::cout << "File: " << "???" << ", Line(" << "?" << "): " << ptr->size<< "Bytes" << std::endl;
		else
			std::cout << "File: " << ptr->file << ", Line(" << ptr->line << "): " << ptr->size<< "Bytes" << std::endl;
	}
	std::cout << "-------Memory Leak Report End------" << std::endl;
}

namespace
{
void memoryErrorDeleteReport(void* p)
{
	std::cout << "Not find memory list: " << p << std::endl;
}
}// namespace

#endif 


MEMSIZE_T MOE::Memory::AllocedSize()
{
#if 1
	return m_allocedsize;
#else
	if (!m_memory)
		return 0;
	
	MEMSIZE_T s = 0;
	for (mem_list* ptr = reinterpret_cast<mem_list*>(m_memory); ptr; ptr = ptr->next)\
	{
		s += ptr->size;
	}
	return s;
#endif
}

#endif // NOTUSE_MOEMEMORY


