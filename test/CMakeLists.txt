#
# cc - C Containers library
#
# cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
# copyright protection in the United States.
#
# DISTRIBUTION STATEMENT A.  Approved for public release; distribution is
# unlimited.  Granted clearance per 88ABW-2020-3430.
#

# Enable the C++ language.

ENABLE_LANGUAGE(CXX)

# Identify directories that contain include files.

INCLUDE_DIRECTORIES(BEFORE ${PROJECT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(BEFORE ${PROJECT_BINARY_DIR}/src)

# List the source files.

SET(SOURCES
  main.cpp
  iarray.cpp
  point.cpp
  list_atomic.cpp
  list_struct.cpp
  list_deep.cpp
  map_atomic.cpp
  map_struct.cpp
  map_deep.cpp
  string.cpp
  vector_atomic.cpp
  vector_struct.cpp
  vector_deep.cpp
  nested.cpp
)

# Add the tests.

ADD_EXECUTABLE(test-cc ${SOURCES})
TARGET_LINK_LIBRARIES(test-cc cc)
SET_PROPERTY(TARGET test-cc PROPERTY CXX_STANDARD 11)
ADD_TEST(NAME cc COMMAND test-cc)
