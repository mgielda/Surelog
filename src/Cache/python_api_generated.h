// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_PYTHONAPI_SURELOG_PYTHONAPICACHE_H_
#define FLATBUFFERS_GENERATED_PYTHONAPI_SURELOG_PYTHONAPICACHE_H_

#include "flatbuffers/flatbuffers.h"

#include "header_generated.h"

namespace SURELOG {
namespace PYTHONAPICACHE {

struct PythonAPICache;

struct PythonAPICache FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_M_HEADER = 4,
    VT_M_PYTHON_SCRIPT_FILE = 6,
    VT_M_ERRORS = 8,
    VT_M_SYMBOLS = 10
  };
  const SURELOG::CACHE::Header *m_header() const {
    return GetPointer<const SURELOG::CACHE::Header *>(VT_M_HEADER);
  }
  const flatbuffers::String *m_python_script_file() const {
    return GetPointer<const flatbuffers::String *>(VT_M_PYTHON_SCRIPT_FILE);
  }
  const flatbuffers::Vector<flatbuffers::Offset<SURELOG::CACHE::Error>> *m_errors() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<SURELOG::CACHE::Error>> *>(VT_M_ERRORS);
  }
  const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *m_symbols() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *>(VT_M_SYMBOLS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_M_HEADER) &&
           verifier.VerifyTable(m_header()) &&
           VerifyOffset(verifier, VT_M_PYTHON_SCRIPT_FILE) &&
           verifier.VerifyString(m_python_script_file()) &&
           VerifyOffset(verifier, VT_M_ERRORS) &&
           verifier.VerifyVector(m_errors()) &&
           verifier.VerifyVectorOfTables(m_errors()) &&
           VerifyOffset(verifier, VT_M_SYMBOLS) &&
           verifier.VerifyVector(m_symbols()) &&
           verifier.VerifyVectorOfStrings(m_symbols()) &&
           verifier.EndTable();
  }
};

struct PythonAPICacheBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_m_header(flatbuffers::Offset<SURELOG::CACHE::Header> m_header) {
    fbb_.AddOffset(PythonAPICache::VT_M_HEADER, m_header);
  }
  void add_m_python_script_file(flatbuffers::Offset<flatbuffers::String> m_python_script_file) {
    fbb_.AddOffset(PythonAPICache::VT_M_PYTHON_SCRIPT_FILE, m_python_script_file);
  }
  void add_m_errors(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<SURELOG::CACHE::Error>>> m_errors) {
    fbb_.AddOffset(PythonAPICache::VT_M_ERRORS, m_errors);
  }
  void add_m_symbols(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> m_symbols) {
    fbb_.AddOffset(PythonAPICache::VT_M_SYMBOLS, m_symbols);
  }
  explicit PythonAPICacheBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  PythonAPICacheBuilder &operator=(const PythonAPICacheBuilder &);
  flatbuffers::Offset<PythonAPICache> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<PythonAPICache>(end);
    return o;
  }
};

inline flatbuffers::Offset<PythonAPICache> CreatePythonAPICache(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<SURELOG::CACHE::Header> m_header = 0,
    flatbuffers::Offset<flatbuffers::String> m_python_script_file = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<SURELOG::CACHE::Error>>> m_errors = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> m_symbols = 0) {
  PythonAPICacheBuilder builder_(_fbb);
  builder_.add_m_symbols(m_symbols);
  builder_.add_m_errors(m_errors);
  builder_.add_m_python_script_file(m_python_script_file);
  builder_.add_m_header(m_header);
  return builder_.Finish();
}

inline flatbuffers::Offset<PythonAPICache> CreatePythonAPICacheDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<SURELOG::CACHE::Header> m_header = 0,
    const char *m_python_script_file = nullptr,
    const std::vector<flatbuffers::Offset<SURELOG::CACHE::Error>> *m_errors = nullptr,
    const std::vector<flatbuffers::Offset<flatbuffers::String>> *m_symbols = nullptr) {
  auto m_python_script_file__ = m_python_script_file ? _fbb.CreateString(m_python_script_file) : 0;
  auto m_errors__ = m_errors ? _fbb.CreateVector<flatbuffers::Offset<SURELOG::CACHE::Error>>(*m_errors) : 0;
  auto m_symbols__ = m_symbols ? _fbb.CreateVector<flatbuffers::Offset<flatbuffers::String>>(*m_symbols) : 0;
  return SURELOG::PYTHONAPICACHE::CreatePythonAPICache(
      _fbb,
      m_header,
      m_python_script_file__,
      m_errors__,
      m_symbols__);
}

inline const SURELOG::PYTHONAPICACHE::PythonAPICache *GetPythonAPICache(const void *buf) {
  return flatbuffers::GetRoot<SURELOG::PYTHONAPICACHE::PythonAPICache>(buf);
}

inline const SURELOG::PYTHONAPICACHE::PythonAPICache *GetSizePrefixedPythonAPICache(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<SURELOG::PYTHONAPICACHE::PythonAPICache>(buf);
}

inline const char *PythonAPICacheIdentifier() {
  return "SLPY";
}

inline bool PythonAPICacheBufferHasIdentifier(const void *buf) {
  return flatbuffers::BufferHasIdentifier(
      buf, PythonAPICacheIdentifier());
}

inline bool VerifyPythonAPICacheBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<SURELOG::PYTHONAPICACHE::PythonAPICache>(PythonAPICacheIdentifier());
}

inline bool VerifySizePrefixedPythonAPICacheBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<SURELOG::PYTHONAPICACHE::PythonAPICache>(PythonAPICacheIdentifier());
}

inline const char *PythonAPICacheExtension() {
  return "slpy";
}

inline void FinishPythonAPICacheBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<SURELOG::PYTHONAPICACHE::PythonAPICache> root) {
  fbb.Finish(root, PythonAPICacheIdentifier());
}

inline void FinishSizePrefixedPythonAPICacheBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<SURELOG::PYTHONAPICACHE::PythonAPICache> root) {
  fbb.FinishSizePrefixed(root, PythonAPICacheIdentifier());
}

}  // namespace PYTHONAPICACHE
}  // namespace SURELOG

#endif  // FLATBUFFERS_GENERATED_PYTHONAPI_SURELOG_PYTHONAPICACHE_H_
