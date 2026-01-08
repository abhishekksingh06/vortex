project(
  'vortex',
  'cpp',
  version: '0.1.0',
  meson_version: '>=1.3.0',
  default_options: [
    'cpp_std=c++23',
    'warning_level=3',
    'werror=false',  # Changed to false to avoid stopping on warnings
    'buildtype=debugoptimized',
    'optimization=3',
    'debug=true',
    'b_ndebug=false', 
  ],
)

llvm_dep = dependency('llvm', version: '>=21.0')

mlir_libs = [
  'MLIR',
  'MLIRAnalysis',
  'MLIRArith',
  'MLIRArithDialect',
  'MLIRBuiltinDialect',
  'MLIRFunc',
  'MLIRFuncDialect',
  'MLIRIR',
  'MLIRMemRef',
  'MLIRMemRefDialect',
  'MLIROpenMP',
  'MLIRParser',
  'MLIRPass',
  'MLIRSCF',
  'MLIRSCFDialect',
  'MLIRSupport',
  'MLIRTransforms',
  'MLIRTransformUtils',
]

mlir_deps = []
cpp = meson.get_compiler('cpp')

foreach lib : mlir_libs
  dep = cpp.find_library(lib, required: false)
  if dep.found()
    mlir_deps += dep
  endif
endforeach

# Combine all dependencies
all_deps = [llvm_dep] + mlir_deps

add_project_arguments(
  '-DVORTEX_VERSION="@0@"'.format(meson.project_version()),
  '-DVORTEX_BUILD_TYPE="@0@"'.format(get_option('buildtype')),
  language: 'cpp',
)

vortex_include = include_directories('include')

# subdir('mlir')  
subdir('lib') 
subdir('tools')     
# subdir('test')
# subdir('unittests') 

install_subdir('include/vortex', install_dir: 'include/vortex')

summary({
  'LLVM version'     : llvm_dep.version(),
  'C++ standard'     : 'c++23',
  'Build type'       : get_option('buildtype'),
  'Install prefix'   : get_option('prefix'),
  'Warnings as errors': false,
  'MLIR libraries'   : mlir_deps.length(),
}, section: 'Vortex Configuration')
