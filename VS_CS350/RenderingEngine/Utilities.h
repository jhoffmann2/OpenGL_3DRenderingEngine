/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: utilities.h
Purpose: helpful utilities and additions to the glm namespace
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 9/28/2020
End Header --------------------------------------------------------*/

#pragma once
#include <istream>
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#pragma warning( disable : 4005 ) // ignore macro redefinition warning
#define FORCE_DISABLE_LOG false
#define LOG if(LOG_ACTIVE && !FORCE_DISABLE_LOG) std::cout

namespace glm
{
  template<length_t VecL, typename VecT>
  std::istream& operator>>(std::istream& is, vec<VecL, VecT>& v)
  {
    for (length_t i = 0; i < VecL; ++i)
    {
      if (!is.eof())
        is >> v[i];
      else 
        v[i] = (i+1 == VecL); // defaults all components to 0 except last which defaults to 1
    }
    return is;
  }

  template<length_t VecL, typename VecT>
  std::ostream& operator<<(std::ostream& os, const vec<VecL, VecT>& v)
  {
    os << '[';
    for (length_t i = 0; i < VecL; ++i)
      os << v[i] << ((i + 1 == VecL) ? "" : ", "); // puts comma and space between all but last num
    return os << ']';
  }

  // get a pointer to the underlying data of a glm::vec
  // useful for opengl buffer storage
  template<length_t VecL, typename VecT>
  VecT* data(const vec<VecL, VecT>& val)
  {
    return const_cast<VecT*>(&val[0]);
  }

  // get a pointer to the underlying data of a glm::mat
  // useful for opengl buffer storage
  template<length_t Matc, length_t MatR, typename MatT>
  MatT* data(const mat<Matc,MatR,MatT>& val)
  {
    return const_cast<MatT*>(&val[0][0]);
  }

  inline mat4 scale(const vec3 &s)
  {
    return scale(mat4(1), s);
  }

  inline mat4 scale(float s)
  {
    return scale(mat4(1), glm::vec3(s));
  }

  inline mat4 translate(const vec3& t)
  {
    return translate(mat4(1), t);
  }
}


namespace std
{
  template<glm::length_t vcl = 3, typename vct = float>
  constexpr vct *begin(glm::vec<vcl,vct> &v)
  {
    return glm::data(v);
  }

  template<glm::length_t vcl = 3, typename vct = float>
  constexpr vct* end(glm::vec<vcl, vct>& v)
  {
    return glm::data(v) + vcl;
  }

  template<glm::length_t vcl = 3, typename vct = float>
  constexpr const vct* begin(const glm::vec<vcl, vct>& v)
  {
    return glm::data(v);
  }

  template<glm::length_t vcl = 3, typename vct = float>
  constexpr const vct* end(const glm::vec<vcl, vct>& v)
  {
    return glm::data(v) + vcl;
  }

  template<glm::length_t vcl = 3, typename vct = float>
  constexpr const vct* cbegin(const glm::vec<vcl, vct>& v)
  {
    return glm::data(v);
  }

  template<glm::length_t vcl = 3, typename vct = float>
  constexpr const vct* cend(const glm::vec<vcl, vct>& v)
  {
    return glm::data(v) + vcl;
  }

  template<glm::length_t C, glm::length_t R, typename vct = float>
  constexpr glm::vec<C, vct> *begin(glm::mat<C,R,vct> &m)
  {
    return &m[0];
  }

  template<glm::length_t R, glm::length_t C, typename vct = float>
  constexpr glm::vec<C, vct> *end(glm::mat<R,C,vct> &m)
  {
    return &m[0] + R;
  }

  template<glm::length_t C, glm::length_t R, typename vct = float>
  constexpr const glm::vec<C, vct> *begin(const glm::mat<C,R,vct> &m)
  {
    return &m[0];
  }

  template<glm::length_t R, glm::length_t C, typename vct = float>
  constexpr const glm::vec<C, vct> *end(const glm::mat<R,C,vct> &m)
  {
    return &m[0] + R;
  }

  template<glm::length_t C, glm::length_t R, typename vct = float>
  constexpr const glm::vec<C, vct> *cbegin(const glm::mat<C,R,vct> &m)
  {
    return &m[0];
  }

  template<glm::length_t R, glm::length_t C, typename vct = float>
  constexpr const glm::vec<C, vct> *cend(const glm::mat<R,C,vct> &m)
  {
    return &m[0] + R;
  }
}

template<size_t alignment, typename T>
class AlignData
{
public:
  AlignData() : data() {}
  AlignData(T val) : data(val) {}

  operator T&() { return data; }
  T& operator()(){return data;}

  AlignData& operator=(T val) { data = val; return *this; }
  AlignData& operator=(AlignData other) { data = other.data; return *this; }

  bool operator==(T val) { return data == val; }
  bool operator==(AlignData other) { return data != other.data; }

  bool operator!=(T val) { return data != val; }
  bool operator!=(AlignData other) { return data != other.data; }
private:
  union
  {
    uint8_t padding[alignment * (1 + (sizeof(T) - 1) / alignment)];
    T data;
  };
};

template<typename Container>
class ElementRange
{
public:
  using iterator = decltype(std::declval<Container>().begin());
  ElementRange() = default;

  ElementRange(Container& container, size_t beginIdx, size_t endIdx) :
    begin_(container.begin() + beginIdx), end_(container.begin() + endIdx)
  {}

  ElementRange(iterator begin, iterator end) :
    begin_(begin), end_(end)
  {}

  iterator begin() const
  {
    return begin_;
  }

  iterator end() const
  {
    return end_;
  }

  size_t size() const
  {
    return end_ - begin_;
  }

  typename iterator::value_type &operator[](size_t index) const
  {
    return begin_[index];
  }

private:
  iterator begin_;
  iterator end_;
};

template<typename Container>
ElementRange<Container> Range(Container& container, size_t beginIdx, size_t endIdx)
{
  return { container, beginIdx, endIdx };
}

template<typename Container>
ElementRange<Container> Range(typename Container::iterator begin, typename Container::iterator end)
{
  return { begin, end };
}

#define unless(procedure) if(!(procedure))
#define until(procedure) while(!(procedure))