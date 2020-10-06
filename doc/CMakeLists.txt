FIND_PACKAGE(Pandoc)
FIND_PACKAGE(Pygmentize)
FIND_PACKAGE(LATEX)

IF(Pandoc_FOUND AND Pygmentize_FOUND AND LATEX_FOUND)

  INCLUDE(Reporting)

  SET(MARKDOWN
    README.md
    LICENSE.md
  )

  SET(SOURCE
    Dockerfile
    CMakeLists.txt

    cmake/Modules/Reporting.cmake

    src/CMakeLists.txt
    src/cc.h
    src/cc_list.h
    src/cc_list.c
    src/cc_map.h
    src/cc_map.c
    src/cc_memory.h
    src/cc_memory.c
    src/cc_string.h
    src/cc_string.c
    src/cc_vector.h
    src/cc_vector.c
    src/cc_version.h

    doc/CMakeLists.txt

    test/CMakeLists.txt
    test/main.cpp
    test/point.hpp
    test/point.cpp
    test/iarray.hpp
    test/iarray.cpp
    test/list.hpp
    test/list_atomic.cpp
    test/list_struct.cpp
    test/list_deep.cpp
    test/map.hpp
    test/map_atomic.cpp
    test/map_struct.cpp
    test/map_deep.cpp
    test/string.hpp
    test/string.cpp
    test/vector.hpp
    test/vector_atomic.cpp
    test/vector_struct.cpp
    test/vector_deep.cpp
    test/nested.cpp
  )

  REPORT(OUTPUT cc.pdf MARKDOWN ${MARKDOWN} SOURCE ${SOURCE})

ENDIF()
