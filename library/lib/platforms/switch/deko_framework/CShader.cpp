/*
** Sample Framework for deko3d Applications
**   CShader.cpp: Utility class for loading shaders from the filesystem
*/
#include "CShader.h"

struct DkshHeader
{
    uint32_t magic; // DKSH_MAGIC
    uint32_t header_sz; // sizeof(DkshHeader)
    uint32_t control_sz;
    uint32_t code_sz;
    uint32_t programs_off;
    uint32_t num_programs;
};

bool CShader::loadMem(CMemPool& pool, const u8* pointer, size_t size)
{
    DkshHeader const* hdr;

    m_codemem.destroy();

    hdr = reinterpret_cast<DkshHeader const*>(pointer);

    if (hdr->code_sz > size)
        return false;

    m_codemem = pool.allocate(hdr->code_sz, DK_SHADER_CODE_ALIGNMENT);
    if (!m_codemem)
        return false;

    memcpy(m_codemem.getCpuAddr(), pointer + hdr->control_sz, hdr->code_sz);

    dk::ShaderMaker { m_codemem.getMemBlock(), m_codemem.getOffset() }
        .setControl(pointer)
        .setProgramId(0)
        .initialize(m_shader);

    return true;
}
