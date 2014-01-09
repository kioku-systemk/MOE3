//
//  Stream.h
//


#ifndef INCLUDE_MOE_STREAM_H
#define INCLUDE_MOE_STREAM_H

#include "Type.h"
#include <string>
#include <map>

namespace MOE {
class KdbImporter;

class NoncopyableStream
{
	public:
		NoncopyableStream(){}
		
	private:
		void operator =(const NoncopyableStream& src) const {}
		NoncopyableStream(const NoncopyableStream& src) {}
};
	
class Stream : public NoncopyableStream
{
private:
	enum internalMode{
		MODE_FLAG_INPUT    = 0,
		MODE_FLAG_OUTPUT   = 1,
		MODE_FLAG_BINARY   = 0,
		MODE_FLAG_TEXT     = 2,
		MODE_FLAG_FROMDISK = 0,
		MODE_FLAG_ONMEMORY = 4,
	};
	
public:
	enum Mode {
		MODE_INPUT_BINARY           = MODE_FLAG_INPUT  | MODE_FLAG_BINARY,
		MODE_INPUT_TEXT             = MODE_FLAG_INPUT  | MODE_FLAG_TEXT,
		MODE_INPUT_BINARY_ONMEMORY  = MODE_FLAG_INPUT  | MODE_FLAG_BINARY | MODE_FLAG_ONMEMORY,
		MODE_INPUT_TEXT_ONMEMORY    = MODE_FLAG_INPUT  | MODE_FLAG_TEXT   | MODE_FLAG_ONMEMORY,
		MODE_OUTPUT_BINARY          = MODE_FLAG_OUTPUT | MODE_FLAG_BINARY,
		MODE_OUTPUT_TEXT            = MODE_FLAG_OUTPUT | MODE_FLAG_TEXT,
		MODE_OUTPUT_BINARY_ONMEMORY = MODE_FLAG_OUTPUT | MODE_FLAG_BINARY | MODE_FLAG_ONMEMORY,
		MODE_OUTPUT_TEXT_ONMEMORY   = MODE_FLAG_OUTPUT | MODE_FLAG_TEXT   | MODE_FLAG_ONMEMORY,
	};

	enum DataType{
		TYPE_DATA_UNKNOWN       = 0,
		TYPE_DATA_TEXT			= 1,
		TYPE_DATA_BINARY		= 2,
		TYPE_DATA_XML			= 3,
		TYPE_DATA_ZIP			= 4
	};

	
	Stream() {
		m_ptr = 0;
		m_size = 0;
        m_refptr = false;
	}
	Stream(const u8* ptr, u32 size);       // for buffer
	Stream(const s8* filename, Mode mode);
	~Stream();
	
	const void* GetData() const {
		return m_ptr;
	}
	s32 GetSize() const {
		return m_size;
	}
	
	// for Text
	void Write(const std::string& s);
	std::string Read() const;
	void Ignore(const char* ignoreChars) const;
	
	// for Binary
    /**
     * @param offset A pointer to start output. Ignored if output to a file.
     */
	void Write(const void* buf, u32 bufsize, u32 offset = 0);
	u32 Read(void* buf, u32 bufsize, u32 offset = 0) const;
	void Alloc(u32 bufsize);
	
	// DataType 
	const DataType& GetDataType(void) const;
	
	const std::string& GetFileName() const;
	const std::string& GetFilePath() const;
	
	b8 IsOpened() const;

	static const std::map<std::string,u32>& GetFileList() { return m_filelist; }
    static bool LoadKDB(const s8* kdbfile);
    
private:
	u8* m_ptr;
    b8 m_refptr;
    FILE *m_filePtr;
	u32 m_size;
	Mode m_mode;
	std::string m_filename;
	std::string m_filepath;
	DataType m_dataType;
	b8 m_opened;

    // kdb
    static std::map<std::string,u32> m_filelist;
    static KdbImporter* m_kdb;
    
	b8 isInputMode ()   const { return !(m_mode & MODE_FLAG_OUTPUT); }
	b8 isOutputMode()   const { return  (m_mode & MODE_FLAG_OUTPUT) ? true : false;  }
	b8 isBinaryMode()   const { return !(m_mode & MODE_FLAG_TEXT);   }
	b8 isTextMode()     const { return  (m_mode & MODE_FLAG_TEXT) ? true : false;    }
	b8 isOnMemoryMode() const { return  (m_mode & MODE_FLAG_ONMEMORY) ? true : false;}

};
    
} // namespace MOE

#endif // INCLUDE_MOE_STREAM_H
