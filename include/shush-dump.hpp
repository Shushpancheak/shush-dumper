#pragma once
#include <exception>
#include <string>
#include <xmemory>
#include <iostream>
#include <fstream>
#include <shush-format.hpp>
#include <utility>

namespace shush {
namespace dump {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// - - - - - - - - - - - - - - MACROS- - - - - - - - - - - - - - - - - - - - - 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Message Assert.
#ifndef NDEBUG
#define MASSERT(x, errc) (!(x) && (throw shush::dump::Dump(#x, __FILE__, __LINE__, this->GetDumpMessage(errc)), 0))
#else
#define MASSERT(x, errc) ;
#endif
#ifndef NDEBUG
#define ASSERTED \
   && ({throw shush::dump::Dump(#x, __FILE__, __LINE__, this->GetDumpMessage());})
#else
#define ASSERTED ;
#endif

template <class T>
struct OkOnConstructOnDestructClass {
  OkOnConstructOnDestructClass(T this_ptr)
      : this_ptr(this_ptr) {
    this_ptr->Ok();
  }

  ~OkOnConstructOnDestructClass() {
    this_ptr->Ok();
  }

  T this_ptr;
};

// Verify method.
#ifndef NDEBUG
#define VERIFIED shush::dump::OkOnConstructOnDestructClass<decltype(this)> v(this);
#else
#define VERIFIED ;
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// - - - - - - - - - - - - - - -EXCEPTION CLASS- - - - - - - - - - - - - - - - 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

inline const uint8_t  EXC_SIZE_EXPRESSION  = 128;
inline const uint8_t  EXC_SIZE_FILE_NAME   = 255;
inline const uint32_t EXC_SIZE_MESSAGE     = 10000;
inline const uint8_t  DUMP_EXCEPTION_COUNT = 16;

inline const char    DEFAULT_DUMP_NAME[26] = "crash-report-{date}.dump";
inline const uint8_t FILE_NAME_SIZE        = 50;

/**
 * @brief A class used in Dump as an element of a dump stack.
 */
class DumpElement {
public:
  /**
   * @note Mind the size limit for strings.
   */
  DumpElement(const char* const expression, const char* const file_name,
              uint32_t line_number, const char* const msg);
  DumpElement() = default;
  ~DumpElement() = default;

  DumpElement(DumpElement&);
  DumpElement& operator=(const DumpElement&);
  DumpElement(DumpElement&&) noexcept;
  DumpElement& operator=(DumpElement&&) noexcept;

  void LoadVars(const char* const expression, const char* const file_name,
                uint32_t line_number, const std::string& msg);

  char     expression [EXC_SIZE_EXPRESSION]{};
  char     file_name  [EXC_SIZE_FILE_NAME ]{};
  uint32_t line_number                     {};
  std::string msg; //TODO make static version of dump
};

class Dump {
public:
  Dump(const char* const expression, const char* const file_name,
       uint32_t line_number, const char* const msg);
  Dump(const char* const expression, const char* const file_name,
       uint32_t line_number, const std::string& msg);
  Dump();
  ~Dump() = default;

  Dump(Dump&) = delete;
  Dump(Dump&&) = delete;
  Dump& operator=(const Dump&) = delete;
  Dump& operator=(Dump&&) = delete;

  void PushToStack(DumpElement&& exc);
  void EmplaceToStack(const char* const expression, const char* const file_name,
                      uint32_t line_number, const char* const msg);
  DumpElement PopFromStack();
  bool Empty();
  size_t Size() const;

  DumpElement dump_stack[DUMP_EXCEPTION_COUNT];

private:
  size_t cur_size_;
};

extern void HandleFinalDump(Dump& dump);
extern std::string GetBadGoodStr(bool exr);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// - - - - - - - - - - - - - - - -DUMPING- - - - - - - - - - - - - - - - - - - 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

}
}