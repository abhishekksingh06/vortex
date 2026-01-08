#include "vortex/Frontend/Diag.h"

#include <algorithm>
#include <cstddef>
#include <llvm/Support/SMLoc.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>

namespace vortex::Frontend {

void DiagEngine::emit(Diag &diag) {
  if (diag.kind == DiagKind::Error)
    ++errors;
  diags.emplace_back(std::move(diag));
}

void DiagEngine::flush() {
  if (!renderer)
    return;
  std::sort(diags.begin(), diags.end(), [](const Diag &a, const Diag &b) {
    if (a.filename != b.filename)
      return a.filename < b.filename;
    return a.span.start < b.span.start;
  });
  for (const auto &d : diags)
    renderer->render(d);
  diags.clear();
}

bool DiagEngine::has_error() const { return errors > 0; }
std::size_t DiagEngine::error_count() const { return errors; }

void DiagEngine::set_renderer(std::unique_ptr<DiagRenderer> renderer) {
  this->renderer = std::move(renderer);
}

llvm::SourceMgr::DiagKind LLVMTextRenderer::toLLVMKind(DiagKind k) {
  using DK = llvm::SourceMgr::DiagKind;
  switch (k) {
  case DiagKind::Error:
    return DK::DK_Error;
  case DiagKind::Note:
    return DK::DK_Note;
  case DiagKind::Warning:
    return DK::DK_Warning;
  }
}

const char *LLVMTextRenderer::getPtr(std::int32_t offset) {
  auto *buf = sourceMgr.getMemoryBuffer(sourceMgr.getMainFileID());
  auto size = buf->getBufferSize();
  if (offset < 0 || static_cast<size_t>(offset) > size)
    return buf->getBufferEnd();
  return buf->getBufferStart() + offset;
}

void LLVMTextRenderer::render(const vortex::Frontend::Diag &diag) {
  llvm::SourceMgr::DiagKind kind = toLLVMKind(diag.kind);

  const char *startPtr = getPtr(diag.span.start);
  const char *endPtr = getPtr(diag.span.end);

  auto start = llvm::SMLoc::getFromPointer(startPtr);
  auto end = llvm::SMLoc::getFromPointer(endPtr);

  llvm::SmallVector<llvm::SMRange, 1> ranges;
  ranges.emplace_back(start, end);

  sourceMgr.PrintMessage(llvm::errs(), start, kind, diag.msg, ranges, {});

  for (const auto &note : diag.notes) {
    sourceMgr.PrintMessage(llvm::errs(), start, llvm::SourceMgr::DK_Note, note,
                           ranges, {});
  }

  if (diag.help) {
    llvm::errs() << "help: " << *diag.help << "\n";
  }
}

} // namespace vortex::Frontend
