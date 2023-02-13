cmake_minimum_required(VERSION 3.0)

include(GNUInstallDirs)

project(RcuSimulator VERSION 0.1)

find_package(PkgConfig)
pkg_check_modules(RCU_SIMULATOR_DEPS REQUIRED 
                  tlf-common-utils 
			   libevdev)

file(GLOB_RECURSE SOURCE_FILES
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
			                     ${CMAKE_SOURCE_DIR}/include
                           ${RCU_SIMULATOR_DEPS_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
                      ${RCU_SIMULATOR_DEPS_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
