#pragma once
#include <vector>
#include <glew.h>
#include "Renderer.h"

struct VertexBufferElement
{
  unsigned int type;
  unsigned int count;
  unsigned char normalized;

  static unsigned int GetSizeOftype(unsigned int type)
  {
    switch (type)
    {
      case GL_FLOAT: return 4;
      case GL_UNSIGNED_BYTE: return 1;
      case GL_UNSIGNED_INT: return 4;
    }
    ASSERT(false);
    return 0;
  }
};

class VertexBufferLayout
{
private:
  std::vector<VertexBufferElement> m_Elements;
  unsigned int m_stride;

public:
  VertexBufferLayout()
    : m_stride(0) {};
  
  template<typename T>
  void Push(int count)
  {
    static_assert(false);
  }

  template<>
  void Push<float>(int count)
  {
    VertexBufferElement a = { GL_FLOAT, count, GL_FALSE };
    m_Elements.push_back(a);
    m_stride += (count * VertexBufferElement::GetSizeOftype(GL_FLOAT));
  }
  template<>
  void Push<unsigned int>(int count)
  {
    VertexBufferElement a = { GL_UNSIGNED_INT, count, GL_FALSE};
    m_Elements.push_back(a);
    m_stride += (count * VertexBufferElement::GetSizeOftype(GL_UNSIGNED_INT));
  }
  template<>
  void Push<char>(int count)
  {
    VertexBufferElement a = { GL_UNSIGNED_BYTE, count, GL_TRUE };
    m_Elements.push_back(a);
    m_stride += (count * VertexBufferElement::GetSizeOftype(GL_UNSIGNED_BYTE));
  }

  inline const std::vector<VertexBufferElement> 
    GetElements() const {return m_Elements;};
  inline unsigned int GetStride () const {return m_stride;};
};