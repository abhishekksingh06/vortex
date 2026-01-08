frontend_sources = [
  'Frontend/Lexer.cpp',
  'Frontend/Diag.cpp',
]

core_sources = frontend_sources 

vortex_core = static_library(
  'vortex_core',
  core_sources,
  include_directories: [vortex_include],  
  dependencies: all_deps,                 
  install: false,
  pic: true,
)

vortex_core_dep = declare_dependency(
  link_with: vortex_core,
  include_directories: vortex_include,
)
