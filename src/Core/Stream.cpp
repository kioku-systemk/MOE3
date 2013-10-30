//
//  Stream.cpp
//
//  Created by kioku on 11/09/14.
//  Copyright 2011 System K. All rights reserved.
//


#include "MOE.h"

#include "Stream.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include <assert.h>

namespace  {
	std::string parseFilename(const s8* fpath, const s8* splitChar)
	{
		std::string f(fpath);
		size_t p = f.rfind(splitChar);
		if (p != std::string::npos)
			f.erase(f.begin(), f.begin() + p + 1);
		return f;
	}
}

namespace MOE {
	
Stream::Stream(const u8* ptr, u32 size){
	m_ptr = mnew u8[size+1];
    m_ptr[size] = 0;
    m_filePtr = NULL;
	m_size = size;
	m_mode = MODE_INPUT_BINARY_ONMEMORY;
	memcpy(m_ptr, ptr, size);
	m_opened = false;
}

Stream::Stream(const s8* filename, Mode mode){
    m_filePtr = NULL;
    m_ptr = NULL;
	m_opened = false;
	m_size = 0;
	m_mode = mode;
	
	m_filepath = filename;
	m_filename = parseFilename(filename, "/");

	if (!isOnMemoryMode())
		assert(1); // TODO implimentation
	
	if (isInputMode())
	{
		// -- Read Mode --
		if (!filename)
			assert(1);
		
		s32 m = 0;
		const s8* mode[] = {"rb", "r"};
		if (isBinaryMode())
		{
			m = 0;
			m_dataType = TYPE_DATA_BINARY;
		}
		else
		{
			m = 1;
			m_dataType = TYPE_DATA_TEXT;
		}
		
		FILE* fp = fopen(filename, mode[m]);
		if (fp)
		{
			m_opened = true;
			fseek(fp, 0, SEEK_END);
			m_size = static_cast<u32>(ftell(fp));
			fseek(fp, 0, SEEK_SET);
			m_ptr = mnew u8[m_size+1];
            m_ptr[m_size] = 0;
			fread(m_ptr, m_size, 1, fp);
			fclose(fp);
		}
	}
	else
	{
		 // -- Write mode --
        if (!filename)
            assert(1);
        
		s32 m = 0;
		const s8* mode[] = {"wb", "w"};
		if (isBinaryMode())
		{
            m = 0;
			m_dataType = TYPE_DATA_BINARY;
		}
		else
		{
            m = 1;
			m_dataType = TYPE_DATA_TEXT;
		}

        m_filePtr = fopen(filename, mode[m]);
        if (m_filePtr) {
            m_opened = true;
        }
	}
}
	
Stream::~Stream(){
	if (m_ptr)
		mdeleteArray( m_ptr );
    if (m_filePtr)
    {
        fclose(m_filePtr);
        m_filePtr = NULL;
    }
	m_size = 0;
}
	
// for Text
void Stream::Write(const std::string& s){
	// TODO
}
	
std::string Stream::Read() const {
	return std::string();	// TODO
}
	
void Stream::Ignore(const char* ignoreChars) const {
	// TODO
}

// for Binary
void Stream::Write(const void* buf, u32 bufsize, u32 offset) {
	if (!isBinaryMode() || !isOutputMode())
		return;
	
    if (m_filename.empty())
    {
        // to memory
        if (offset > m_size)
            return;
        u32 endpos = offset + bufsize;
        if (endpos > m_size)
            endpos = m_size;
    
        memcpy(&m_ptr[offset], buf, endpos - offset);
    }
    else
    {
        // to file
        if (!m_opened)
            return;
        fwrite(buf, bufsize, 1, m_filePtr);
    }
}
	
u32 Stream::Read(void* buf, u32 bufsize, u32 offset) const {
	if (isBinaryMode() || isInputMode())
		return 0;
	
	if (offset > m_size)
		return 0;
	u32 endpos = offset + bufsize;
	if (endpos > m_size)
		endpos = m_size;
	
	memcpy(buf, &m_ptr[offset], endpos - offset);
	return endpos - offset;
}
	
void Stream::Alloc(u32 bufsize)
{
	m_size = bufsize;
	m_ptr = mnew u8[m_size];
	return;
}

const Stream::DataType& Stream::GetDataType(void) const {
	return m_dataType;
}
	
const std::string& Stream::GetFileName() const {
	return m_filename;
}

const std::string& Stream::GetFilePath() const {
	return m_filepath;
}

bool Stream::IsOpened() const {
	return m_opened;
}
	
} // namespace MOE

