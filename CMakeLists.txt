cmake_minimum_required(VERSION 3.22.1)

project(ScallopShell)

find_package(Ceasoning REQUIRED)

add_subdirectory(src)

## Refactor this out into a separate file?
## Lots of noise for just "install a CMake target"
install(TARGETS lexer
	EXPORT ScallopLangTargets
	DESTINATION lib)
install(EXPORT ScallopLangTargets
	FILE ScallopLangTargets.cmake
	DESTINATION lib/cmake/ScallopLang)
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/ScallopLangConfig.cmake"
	INSTALL_DESTINATION "lib/cmake/ScallopLang"
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/ScallopLangConfigVersion.cmake"
	VERSION "${ScallopLang_VERSION_MAJOR}.${ScallopLang_VERSION_MINOR}"
	COMPATIBILITY AnyNewerVersion)
install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/ScallopLangConfigVersion.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/ScallopLangConfig.cmake"
	DESTINATION lib/cmake/ScallopLang)
export(EXPORT ScallopLangTargets
	FILE "${CMAKE_CURRENT_BINARY_DIR}/ScallopLangTargets.cmake")


if(BUILD_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif(BUILD_TESTING)
