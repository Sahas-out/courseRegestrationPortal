set(utils "utils/")
set(bin "bin/")
set(include "include/")
set(server "src/server/")
set(client "src/client/")
set(resource_dir "${CMAKE_BINARY_DIR}/resources/")
set(resource_files ${resource_dir}students  ${resource_dir}courses ${resource_dir}faculties ${resource_dir}student_courses) 
set(executable_files ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/server ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/client)
cmake_minimum_required(VERSION 3.28.3)
project(courseRegestrationPortal)
set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${bin})
include_directories(${CMAKE_SOURCE_DIR}/${include})
add_custom_command(
  OUTPUT ${resource_files}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${resource_dir}
  COMMAND ${CMAKE_COMMAND} -E touch ${resource_files}
)
add_custom_target(resource_files DEPENDS ${resource_files})
file(GLOB resources ${server}resource_api/*.c)
add_executable(server ${server}server.c ${server}client_routine.c ${resources})
add_executable(client ${client}client.c ${client}client_ui.c)
add_dependencies(server resource_files)
add_custom_target(clean_all
  COMMAND ${CMAKE_COMMAND} -E rm -f ${resource_files}
  COMMENT "Cleaning generated resource files"
  COMMAND ${CMAKE_COMMAND} -E rm -f ${executable_files}
  COMMENT "Cleaning generated executable files"
)


