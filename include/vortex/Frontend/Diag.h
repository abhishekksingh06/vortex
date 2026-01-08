#pragma once

#include "llvm/Support/SourceMgr.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mlir/IR/Diagnostics.h>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace vortex::Frontend {

enum class DiagKind : std::int8_t {
  Error,
  Warning,
  Note,
};

struct Span {
  std::int32_t start;
  std::int32_t end;

  constexpr Span(std::int32_t start, std::int32_t end)
      : start(start), end(end) {}
};

struct Diag {
  std::string filename;
  Span span;
  DiagKind kind;
  std::string msg;
  std::vector<std::string> notes;
  std::optional<std::string> help;

  static Diag error(std::string_view filename, Span span,
                    std::string_view msg) {
    return Diag(filename, span, DiagKind::Error, msg);
  }

  static Diag warning(std::string_view filename, Span span,
                      std::string_view msg) {
    return Diag(filename, span, DiagKind::Warning, msg);
  }

  static Diag note(std::string_view filename, Span span, std::string_view msg) {
    return Diag(filename, span, DiagKind::Note, msg);
  }

  Diag &add_note(std::string note) {
    notes.emplace_back(std::move(note));
    return *this;
  }

  Diag &with_help(std::string help_msg) {
    help = std::move(help_msg);
    return *this;
  }

private:
  Diag(std::string_view filename, Span span, DiagKind kind,
       std::string_view msg)
      : filename(filename), span(span), kind(kind), msg(msg) {}
};

class DiagRenderer {
public:
  virtual ~DiagRenderer() = default;
  virtual void render(const Diag &diag) = 0;
};

class DiagEngine {
public:
  void emit(Diag &diag);
  void flush();

  bool has_error() const;
  std::size_t error_count() const;

  void set_renderer(std::unique_ptr<DiagRenderer> renderer);

private:
  std::vector<Diag> diags;
  std::size_t errors;
  std::unique_ptr<DiagRenderer> renderer;
};

class LLVMTextRenderer : public DiagRenderer {
public:
  explicit LLVMTextRenderer(llvm::SourceMgr &sm) : sourceMgr(sm) {}

  void render(const Diag &diag) override;

private:
  llvm::SourceMgr &sourceMgr;

  static llvm::SourceMgr::DiagKind toLLVMKind(DiagKind k);
  const char *getPtr(std::int32_t offset);
};

} // namespace vortex::Frontend
