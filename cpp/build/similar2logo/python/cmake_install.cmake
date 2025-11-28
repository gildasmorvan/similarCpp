# Install script for directory: /Users/morvan/Antigravity/similar/cpp/similar2logo/python

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/morvan/Antigravity/similar/cpp/similar2logo/python/../../../python/similar2logo/_core.cpython-314-darwin.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/morvan/Antigravity/similar/cpp/similar2logo/python/../../../python/similar2logo" TYPE MODULE FILES "/Users/morvan/Antigravity/similar/cpp/build/similar2logo/python/_core.cpython-314-darwin.so")
  if(EXISTS "$ENV{DESTDIR}/Users/morvan/Antigravity/similar/cpp/similar2logo/python/../../../python/similar2logo/_core.cpython-314-darwin.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/morvan/Antigravity/similar/cpp/similar2logo/python/../../../python/similar2logo/_core.cpython-314-darwin.so")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/morvan/Antigravity/similar/cpp/similar2logo/python/../../microkernel/build"
      -delete_rpath "/Users/morvan/Antigravity/similar/cpp/similar2logo/python/../../extendedkernel/build"
      "$ENV{DESTDIR}/Users/morvan/Antigravity/similar/cpp/similar2logo/python/../../../python/similar2logo/_core.cpython-314-darwin.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" -x "$ENV{DESTDIR}/Users/morvan/Antigravity/similar/cpp/similar2logo/python/../../../python/similar2logo/_core.cpython-314-darwin.so")
    endif()
  endif()
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/morvan/Antigravity/similar/cpp/build/similar2logo/python/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
