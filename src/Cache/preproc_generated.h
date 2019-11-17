// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_PREPROC_SURELOG_MACROCACHE_H_
#define FLATBUFFERS_GENERATED_PREPROC_SURELOG_MACROCACHE_H_

#include "flatbuffers/flatbuffers.h"

#include "header_generated.h"

namespace SURELOG {
namespace MACROCACHE {

struct Macro;

struct PPCache;

enum MacroType {
  MacroType_NO_ARGS = 0,
  MacroType_WITH_ARGS = 1,
  MacroType_MIN = MacroType_NO_ARGS,
  MacroType_MAX = MacroType_WITH_ARGS
};

inline const MacroType (&EnumValuesMacroType())[2] {
  static const MacroType values[] = {
    MacroType_NO_ARGS,
    MacroType_WITH_ARGS
  };
  return values;
}

inline const char * const *EnumNamesMacroType() {
  static const char * const names[3] = {
    "NO_ARGS",
    "WITH_ARGS",
    nullptr
  };
  return names;
}

inline const char *EnumNameMacroType(MacroType e) {
  if (e < MacroType_NO_ARGS || e > MacroType_WITH_ARGS) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesMacroType()[index];
}

struct Macro FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_M_NAME = 4,
    VT_M_TYPE = 6,
    VT_M_LINE = 8,
    VT_M_COLUMN = 10,
    VT_M_ARGUMENTS = 12,
    VT_M_TOKENS = 14
  };
  const flatbuffers::String *m_name() const {
    return GetPointer<const flatbuffers::String *>(VT_M_NAME);
  }
  SURELOG::MACROCACHE::MacroType m_type() const {
    return static_cast<SURELOG::MACROCACHE::MacroType>(GetField<int8_t>(VT_M_TYPE, 0));
  }
  uint32_t m_line() const {
    return GetField<uint32_t>(VT_M_LINE, 0);
  }
  uint16_t m_column() const {
    return GetField<uint16_t>(VT_M_COLUMN, 0);
  }
  const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *m_arguments() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *>(VT_M_ARGUMENTS);
  }
  const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *m_tokens() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *>(VT_M_TOKENS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_M_NAME) &&
           verifier.VerifyString(m_name()) &&
           VerifyField<int8_t>(verifier, VT_M_TYPE) &&
           VerifyField<uint32_t>(verifier, VT_M_LINE) &&
           VerifyField<uint16_t>(verifier, VT_M_COLUMN) &&
           VerifyOffset(verifier, VT_M_ARGUMENTS) &&
           verifier.VerifyVector(m_arguments()) &&
           verifier.VerifyVectorOfStrings(m_arguments()) &&
           VerifyOffset(verifier, VT_M_TOKENS) &&
           verifier.VerifyVector(m_tokens()) &&
           verifier.VerifyVectorOfStrings(m_tokens()) &&
           verifier.EndTable();
  }
};

struct MacroBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_m_name(flatbuffers::Offset<flatbuffers::String> m_name) {
    fbb_.AddOffset(Macro::VT_M_NAME, m_name);
  }
  void add_m_type(SURELOG::MACROCACHE::MacroType m_type) {
    fbb_.AddElement<int8_t>(Macro::VT_M_TYPE, static_cast<int8_t>(m_type), 0);
  }
  void add_m_line(uint32_t m_line) {
    fbb_.AddElement<uint32_t>(Macro::VT_M_LINE, m_line, 0);
  }
  void add_m_column(uint16_t m_column) {
    fbb_.AddElement<uint16_t>(Macro::VT_M_COLUMN, m_column, 0);
  }
  void add_m_arguments(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> m_arguments) {
    fbb_.AddOffset(Macro::VT_M_ARGUMENTS, m_arguments);
  }
  void add_m_tokens(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> m_tokens) {
    fbb_.AddOffset(Macro::VT_M_TOKENS, m_tokens);
  }
  explicit MacroBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  MacroBuilder &operator=(const MacroBuilder &);
  flatbuffers::Offset<Macro> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Macro>(end);
    return o;
  }
};

inline flatbuffers::Offset<Macro> CreateMacro(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> m_name = 0,
    SURELOG::MACROCACHE::MacroType m_type = SURELOG::MACROCACHE::MacroType_NO_ARGS,
    uint32_t m_line = 0,
    uint16_t m_column = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> m_arguments = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> m_tokens = 0) {
  MacroBuilder builder_(_fbb);
  builder_.add_m_tokens(m_tokens);
  builder_.add_m_arguments(m_arguments);
  builder_.add_m_line(m_line);
  builder_.add_m_name(m_name);
  builder_.add_m_column(m_column);
  builder_.add_m_type(m_type);
  return builder_.Finish();
}

inline flatbuffers::Offset<Macro> CreateMacroDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *m_name = nullptr,
    SURELOG::MACROCACHE::MacroType m_type = SURELOG::MACROCACHE::MacroType_NO_ARGS,
    uint32_t m_line = 0,
    uint16_t m_column = 0,
    const std::vector<flatbuffers::Offset<flatbuffers::String>> *m_arguments = nullptr,
    const std::vector<flatbuffers::Offset<flatbuffers::String>> *m_tokens = nullptr) {
  auto m_name__ = m_name ? _fbb.CreateString(m_name) : 0;
  auto m_arguments__ = m_arguments ? _fbb.CreateVector<flatbuffers::Offset<flatbuffers::String>>(*m_arguments) : 0;
  auto m_tokens__ = m_tokens ? _fbb.CreateVector<flatbuffers::Offset<flatbuffers::String>>(*m_tokens) : 0;
  return SURELOG::MACROCACHE::CreateMacro(
      _fbb,
      m_name__,
      m_type,
      m_line,
      m_column,
      m_arguments__,
      m_tokens__);
}

struct PPCache FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_M_HEADER = 4,
    VT_M_MACROS = 6,
    VT_M_INCLUDES = 8,
    VT_M_BODY = 10,
    VT_M_ERRORS = 12,
    VT_M_SYMBOLS = 14,
    VT_M_CMD_INCLUDE_PATHS = 16,
    VT_M_CMD_DEFINE_OPTIONS = 18,
    VT_M_TIMEINFO = 20
  };
  const SURELOG::CACHE::Header *m_header() const {
    return GetPointer<const SURELOG::CACHE::Header *>(VT_M_HEADER);
  }
  const flatbuffers::Vector<flatbuffers::Offset<SURELOG::MACROCACHE::Macro>> *m_macros() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<SURELOG::MACROCACHE::Macro>> *>(VT_M_MACROS);
  }
  const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *m_includes() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *>(VT_M_INCLUDES);
  }
  const flatbuffers::String *m_body() const {
    return GetPointer<const flatbuffers::String *>(VT_M_BODY);
  }
  const flatbuffers::Vector<flatbuffers::Offset<SURELOG::CACHE::Error>> *m_errors() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<SURELOG::CACHE::Error>> *>(VT_M_ERRORS);
  }
  const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *m_symbols() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *>(VT_M_SYMBOLS);
  }
  const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *m_cmd_include_paths() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *>(VT_M_CMD_INCLUDE_PATHS);
  }
  const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *m_cmd_define_options() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *>(VT_M_CMD_DEFINE_OPTIONS);
  }
  const flatbuffers::Vector<flatbuffers::Offset<SURELOG::CACHE::TimeInfo>> *m_timeInfo() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<SURELOG::CACHE::TimeInfo>> *>(VT_M_TIMEINFO);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_M_HEADER) &&
           verifier.VerifyTable(m_header()) &&
           VerifyOffset(verifier, VT_M_MACROS) &&
           verifier.VerifyVector(m_macros()) &&
           verifier.VerifyVectorOfTables(m_macros()) &&
           VerifyOffset(verifier, VT_M_INCLUDES) &&
           verifier.VerifyVector(m_includes()) &&
           verifier.VerifyVectorOfStrings(m_includes()) &&
           VerifyOffset(verifier, VT_M_BODY) &&
           verifier.VerifyString(m_body()) &&
           VerifyOffset(verifier, VT_M_ERRORS) &&
           verifier.VerifyVector(m_errors()) &&
           verifier.VerifyVectorOfTables(m_errors()) &&
           VerifyOffset(verifier, VT_M_SYMBOLS) &&
           verifier.VerifyVector(m_symbols()) &&
           verifier.VerifyVectorOfStrings(m_symbols()) &&
           VerifyOffset(verifier, VT_M_CMD_INCLUDE_PATHS) &&
           verifier.VerifyVector(m_cmd_include_paths()) &&
           verifier.VerifyVectorOfStrings(m_cmd_include_paths()) &&
           VerifyOffset(verifier, VT_M_CMD_DEFINE_OPTIONS) &&
           verifier.VerifyVector(m_cmd_define_options()) &&
           verifier.VerifyVectorOfStrings(m_cmd_define_options()) &&
           VerifyOffset(verifier, VT_M_TIMEINFO) &&
           verifier.VerifyVector(m_timeInfo()) &&
           verifier.VerifyVectorOfTables(m_timeInfo()) &&
           verifier.EndTable();
  }
};

struct PPCacheBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_m_header(flatbuffers::Offset<SURELOG::CACHE::Header> m_header) {
    fbb_.AddOffset(PPCache::VT_M_HEADER, m_header);
  }
  void add_m_macros(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<SURELOG::MACROCACHE::Macro>>> m_macros) {
    fbb_.AddOffset(PPCache::VT_M_MACROS, m_macros);
  }
  void add_m_includes(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> m_includes) {
    fbb_.AddOffset(PPCache::VT_M_INCLUDES, m_includes);
  }
  void add_m_body(flatbuffers::Offset<flatbuffers::String> m_body) {
    fbb_.AddOffset(PPCache::VT_M_BODY, m_body);
  }
  void add_m_errors(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<SURELOG::CACHE::Error>>> m_errors) {
    fbb_.AddOffset(PPCache::VT_M_ERRORS, m_errors);
  }
  void add_m_symbols(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> m_symbols) {
    fbb_.AddOffset(PPCache::VT_M_SYMBOLS, m_symbols);
  }
  void add_m_cmd_include_paths(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> m_cmd_include_paths) {
    fbb_.AddOffset(PPCache::VT_M_CMD_INCLUDE_PATHS, m_cmd_include_paths);
  }
  void add_m_cmd_define_options(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> m_cmd_define_options) {
    fbb_.AddOffset(PPCache::VT_M_CMD_DEFINE_OPTIONS, m_cmd_define_options);
  }
  void add_m_timeInfo(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<SURELOG::CACHE::TimeInfo>>> m_timeInfo) {
    fbb_.AddOffset(PPCache::VT_M_TIMEINFO, m_timeInfo);
  }
  explicit PPCacheBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  PPCacheBuilder &operator=(const PPCacheBuilder &);
  flatbuffers::Offset<PPCache> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<PPCache>(end);
    return o;
  }
};

inline flatbuffers::Offset<PPCache> CreatePPCache(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<SURELOG::CACHE::Header> m_header = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<SURELOG::MACROCACHE::Macro>>> m_macros = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> m_includes = 0,
    flatbuffers::Offset<flatbuffers::String> m_body = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<SURELOG::CACHE::Error>>> m_errors = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> m_symbols = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> m_cmd_include_paths = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> m_cmd_define_options = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<SURELOG::CACHE::TimeInfo>>> m_timeInfo = 0) {
  PPCacheBuilder builder_(_fbb);
  builder_.add_m_timeInfo(m_timeInfo);
  builder_.add_m_cmd_define_options(m_cmd_define_options);
  builder_.add_m_cmd_include_paths(m_cmd_include_paths);
  builder_.add_m_symbols(m_symbols);
  builder_.add_m_errors(m_errors);
  builder_.add_m_body(m_body);
  builder_.add_m_includes(m_includes);
  builder_.add_m_macros(m_macros);
  builder_.add_m_header(m_header);
  return builder_.Finish();
}

inline flatbuffers::Offset<PPCache> CreatePPCacheDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<SURELOG::CACHE::Header> m_header = 0,
    const std::vector<flatbuffers::Offset<SURELOG::MACROCACHE::Macro>> *m_macros = nullptr,
    const std::vector<flatbuffers::Offset<flatbuffers::String>> *m_includes = nullptr,
    const char *m_body = nullptr,
    const std::vector<flatbuffers::Offset<SURELOG::CACHE::Error>> *m_errors = nullptr,
    const std::vector<flatbuffers::Offset<flatbuffers::String>> *m_symbols = nullptr,
    const std::vector<flatbuffers::Offset<flatbuffers::String>> *m_cmd_include_paths = nullptr,
    const std::vector<flatbuffers::Offset<flatbuffers::String>> *m_cmd_define_options = nullptr,
    const std::vector<flatbuffers::Offset<SURELOG::CACHE::TimeInfo>> *m_timeInfo = nullptr) {
  auto m_macros__ = m_macros ? _fbb.CreateVector<flatbuffers::Offset<SURELOG::MACROCACHE::Macro>>(*m_macros) : 0;
  auto m_includes__ = m_includes ? _fbb.CreateVector<flatbuffers::Offset<flatbuffers::String>>(*m_includes) : 0;
  auto m_body__ = m_body ? _fbb.CreateString(m_body) : 0;
  auto m_errors__ = m_errors ? _fbb.CreateVector<flatbuffers::Offset<SURELOG::CACHE::Error>>(*m_errors) : 0;
  auto m_symbols__ = m_symbols ? _fbb.CreateVector<flatbuffers::Offset<flatbuffers::String>>(*m_symbols) : 0;
  auto m_cmd_include_paths__ = m_cmd_include_paths ? _fbb.CreateVector<flatbuffers::Offset<flatbuffers::String>>(*m_cmd_include_paths) : 0;
  auto m_cmd_define_options__ = m_cmd_define_options ? _fbb.CreateVector<flatbuffers::Offset<flatbuffers::String>>(*m_cmd_define_options) : 0;
  auto m_timeInfo__ = m_timeInfo ? _fbb.CreateVector<flatbuffers::Offset<SURELOG::CACHE::TimeInfo>>(*m_timeInfo) : 0;
  return SURELOG::MACROCACHE::CreatePPCache(
      _fbb,
      m_header,
      m_macros__,
      m_includes__,
      m_body__,
      m_errors__,
      m_symbols__,
      m_cmd_include_paths__,
      m_cmd_define_options__,
      m_timeInfo__);
}

inline const SURELOG::MACROCACHE::PPCache *GetPPCache(const void *buf) {
  return flatbuffers::GetRoot<SURELOG::MACROCACHE::PPCache>(buf);
}

inline const SURELOG::MACROCACHE::PPCache *GetSizePrefixedPPCache(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<SURELOG::MACROCACHE::PPCache>(buf);
}

inline const char *PPCacheIdentifier() {
  return "SLPP";
}

inline bool PPCacheBufferHasIdentifier(const void *buf) {
  return flatbuffers::BufferHasIdentifier(
      buf, PPCacheIdentifier());
}

inline bool VerifyPPCacheBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<SURELOG::MACROCACHE::PPCache>(PPCacheIdentifier());
}

inline bool VerifySizePrefixedPPCacheBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<SURELOG::MACROCACHE::PPCache>(PPCacheIdentifier());
}

inline const char *PPCacheExtension() {
  return "slpp";
}

inline void FinishPPCacheBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<SURELOG::MACROCACHE::PPCache> root) {
  fbb.Finish(root, PPCacheIdentifier());
}

inline void FinishSizePrefixedPPCacheBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<SURELOG::MACROCACHE::PPCache> root) {
  fbb.FinishSizePrefixed(root, PPCacheIdentifier());
}

}  // namespace MACROCACHE
}  // namespace SURELOG

#endif  // FLATBUFFERS_GENERATED_PREPROC_SURELOG_MACROCACHE_H_
