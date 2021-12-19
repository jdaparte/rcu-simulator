cmake_minimum_required(VERSION 3.9)

project(keyboard-sym VERSION 0.1)

option(BUILD_DOC "Build documentation" ON)
option(DUMMY_LIB "Build dummy library" OFF)
if(NOT PACKAGE_TYPE)
  set(PACKAGE_TYPE ZIP)
endif()

include(GNUInstallDirs)

# tgz generation
string(TIMESTAMP TIME "%Y%m%d")
set(CPACK_GENERATOR ${PACKAGE_TYPE})
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
if("${PACKAGE_TYPE}" STREQUAL "RPM")
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
else()
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${TIME}")
endif()
include(CPack)

#pkg-config
set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_BINDIR}/pkgconfig" CACHE PATH "Installation directory for pkgconfig (.pc) files")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/KeyboardSym.pc.cmakein ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${INSTALL_PKGCONFIG_DIR})

find_package(PkgConfig REQUIRED)

file(GLOB_RECURSE SOURCE_FILES
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

pkg_check_modules(KEYBOARD_SYM_DEPS REQUIRED 
                                    tlf-common-utils 
				    libevdev)

target_include_directories(${PROJECT_NAME} PUBLIC
			   ${CMAKE_SOURCE_DIR}/include
                           ${KEYBOARD_SYM_DEPS_INCLUDE_DIRS})


target_link_libraries(${PROJECT_NAME}
		      ${KEYBOARD_SYM_DEPS_LIBRARIES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

