
#include "KdbImporterExporter.h"
#include "Stream.h"
#include "Log.h"
#include <stdio.h>

namespace MOE {
    
KdbExporter::KdbExporter()
{
    
}

KdbExporter::~KdbExporter()
{
    
}

bool KdbExporter::Export(const std::string& packfile, const std::string& basepath)
{
    FILE* fp = fopen(packfile.c_str(), "wb");
    if (!fp)
        return false;
    
    const std::map<std::string,u32>& files = Stream::GetFileList();
    auto eit = files.end();
    
    // write header
    const u32 num = static_cast<u32>(files.size());
    fwrite("KDB", 4, 1, fp);
    fwrite(&num, sizeof(u32), 1, fp);
    for (auto it = files.begin(); it != eit; ++it)
    {
        std::string fpath = std::string(it->first);
        size_t p = fpath.find(basepath);
        if (p != std::string::npos) {
            fpath = fpath.substr(basepath.size(),fpath.size());
        }
        fwrite(&it->second, sizeof(u32), 1, fp);
        fwrite(fpath.c_str(), fpath.size()+1, 1, fp);
    }

    // write body
    for (auto it = files.begin(); it != eit; ++it)
    {
        const u32 fsize = it->second;
        Stream st(it->first.c_str(), Stream::MODE_INPUT_BINARY_ONMEMORY);
        if (fsize != st.GetSize()){
            fclose(fp);
            MOELogE("Need to reload file before Export.");
            return false;
        }
        fwrite(st.GetData(), st.GetSize(), 1, fp);
        const u8 term = 0;
        fwrite(&term, sizeof(u8), 1, fp);
    }

    fclose(fp);
    
    return true;
}

KdbImporter::KdbImporter()
{
}
    
KdbImporter::~KdbImporter()
{
    delete [] m_filebuf;
    delete [] m_finfo;
}

bool KdbImporter::Load(const std::string& kdbfile)
{
    FILE* fp = fopen(kdbfile.c_str(), "rb");
    if (!fp)
        return false;
    
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    m_filebuf = new u8[size];
    fread(m_filebuf, size, 1, fp);
    
    s8 header[4];
    memcpy(header, m_filebuf, 4); m_filebuf += 4;
    s32 num = 0;
    memcpy(&num, m_filebuf, 4); m_filebuf += sizeof(s32);
    m_finfo = new FileInfo[num];
    for (s32 i = 0; i < num; ++i)
    {
        memcpy(&m_finfo[i].size, m_filebuf, sizeof(u32));
        m_filebuf += sizeof(u32);
        const std::string filename = std::string(reinterpret_cast<const s8*>(m_filebuf));
        m_filebuf += filename.size()+1;

        m_files[filename] = &m_finfo[i];
    }
    for (s32 i = 0; i < num; ++i)
    {
        m_finfo[i].fileptr = m_filebuf;
        m_filebuf += m_finfo[i].size + 1;
    }
    fclose(fp);
    return true;
}
u32 KdbImporter::GetSize(const std::string& filepath)
{
    const auto fp = m_files.find(filepath);
    if (fp != m_files.end())
        return m_files[filepath]->size;
    else
        return 0;
}
const void* KdbImporter::GetData(const std::string& filepath)
{
    const auto fp = m_files.find(filepath);
    if (fp != m_files.end())
        return m_files[filepath]->fileptr;
    else
        return 0;
}
    
    
}