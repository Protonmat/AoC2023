project(utils)

add_library(${PROJECT_NAME} SHARED utils.cpp)

target_include_directories(utils INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
) 

# define the symbol stating we are using the declspec(dllexport) when
# building on windows
target_compile_definitions(utils PRIVATE "USING_UTILS")


set(installable_libs utils)

install(TARGETS ${installable_libs} DESTINATION lib)
