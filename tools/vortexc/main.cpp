#include <mlir/IR/Builders.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/MLIRContext.h>
int main() {
  mlir::MLIRContext ctx;
  mlir::Builder builder(&ctx);
}
