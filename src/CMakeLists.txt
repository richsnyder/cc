#
# cc - C Containers library
#
# cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
# copyright protection in the United States.
#

SET(SOURCES
  cc_list.c
  cc_map.c
  cc_memory.c
  cc_string.c
  cc_vector.c
  ../contrib/xxhash/xxhash.c
)

SET(HEADERS
  cc_cc.h
  cc_list.h
  cc_map.h
  cc_memory.h
  cc_string.h
  cc_vector.h
  ../contrib/xxhash/xxhash.h
)

SET(CONFIGURED_HEADERS
  cc_version.h
)

# Identify the directories that contain include files.

INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR})

# Configure header files.

FOREACH(file ${CONFIGURED_HEADERS})
  CONFIGURE_FILE(${file}.in ${file} @ONLY)
ENDFOREACH()

# Mark the header files for installation.

FOREACH(file ${HEADERS})
  INSTALL(FILES ${file} DESTINATION include/cc)
ENDFOREACH()
FOREACH(file ${CONFIGURED_HEADERS})
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${file} DESTINATION include/cc)
ENDFOREACH()

# Define the library target.

ADD_LIBRARY(cc SHARED ${SOURCES})
SET_PROPERTY(TARGET cc PROPERTY C_STANDARD 11)
INSTALL(TARGETS cc LIBRARY DESTINATION lib)
