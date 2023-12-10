file(GLOB_RECURSE UTILS_SOURCES "src/*.cpp")

add_library(utils SHARED)
target_sources(utils PUBLIC ${UTILS_SOURCES})
target_include_directories(utils PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include/mylib
)

target_compile_features(utils PUBLIC cxx_std_23)
add_library(utils::utils ALIAS utils)
