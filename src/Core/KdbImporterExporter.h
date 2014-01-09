/*
 *  KdbExporter.h
 *
 */

#ifndef INCLUDE_KDBEXPORTER_H
#define INCLUDE_KDBEXPORTER_H

#include "Type.h"
#include <string>
#include <map>

namespace MOE {

class KdbExporter
{
public:
    KdbExporter();
    ~KdbExporter();
    
    b8 Export(const std::string& packfile, const std::string& basepath);
};

class KdbImporter
{
public:
    KdbImporter();
    ~KdbImporter();
    
    bool Load(const std::string& kdbfile);
    u32 GetSize(const std::string& filepath);
    const void* GetData(const std::string& filepath);
    
private:
    struct FileInfo{
        u32 size;
        u8* fileptr;
    };
    std::map<std::string, FileInfo*> m_files;
    u8* m_filebuf;
    FileInfo* m_finfo;
};

} // namespace MOE
#endif // INCLUDE_KDBEXPORTER_H

