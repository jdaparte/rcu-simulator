cmake_minimum_required(VERSION 3.0)

include(GNUInstallDirs)

project(keyboard-sym VERSION 0.1)

find_package(PkgConfig)
pkg_check_modules(KEYBOARD_SYM_DEPS REQUIRED 
                  tlf-common-utils 
				          libevdev)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/KeyboardSym.pc.cmakein ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)

file(GLOB_RECURSE SOURCE_FILES
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_BINDIR}/pkgconfig" CACHE PATH "Installation directory for pkgconfig (.pc) files")

target_include_directories(${PROJECT_NAME} PUBLIC
			                     ${CMAKE_SOURCE_DIR}/include
                           ${KEYBOARD_SYM_DEPS_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
                      ${KEYBOARD_SYM_DEPS_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

