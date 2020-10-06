#
# cc - C Containers library
#
# cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
# copyright protection in the United States.
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.15.0 FATAL_ERROR)

PROJECT(cc LANGUAGES C VERSION 0.1.0)

# Add custom modules to the module search path.

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

# Load modules.

INCLUDE(CTest)

# Identify the directories that contain include files.

INCLUDE_DIRECTORIES(BEFORE ${PROJECT_SOURCE_DIR}/contrib)

# Add a valgrind leak check target.

ADD_CUSTOM_TARGET(valgrind
    COMMAND docker build --tag=cc .
    COMMAND docker system prune --filter "label=name=cc" --force
    COMMAND docker run --rm cc ./test-cc --leak-check=yes >& ${CMAKE_CURRENT_BINARY_DIR}/valgrind.txt
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# Add the project subdirectories.

ADD_SUBDIRECTORY(src)
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(test)
ENDIF()
ADD_SUBDIRECTORY(doc)
