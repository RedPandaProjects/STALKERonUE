#include "XRayWallMarkArray.h"

FACTORY_PTR_INSTANCIATE(UIShader)

XRayWallMarkArray::XRayWallMarkArray()
{
}

void XRayWallMarkArray::Copy(IWallMarkArray & _in)
{
	*this = *(XRayWallMarkArray*)&_in;
}

void XRayWallMarkArray::AppendMark(LPCSTR s_textures)
{
	wm_shader s;
    s->create("effects\\wallmark", s_textures);
    m_CollideMarks.push_back(s);
}

void XRayWallMarkArray::clear()
{
	m_CollideMarks.clear();
}

bool XRayWallMarkArray::empty()
{
	return m_CollideMarks.empty();
}

wm_shader XRayWallMarkArray::GenerateWallmark()
{
    wm_shader res;
    if (!m_CollideMarks.empty())
        res->Copy(*m_CollideMarks[::Random.randI(0, m_CollideMarks.size())]);
    return res;
}
