#
# cc - C Containers library
#
# cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
# copyright protection in the United States.
#
# DISTRIBUTION STATEMENT A.  Approved for public release; distribution is
# unlimited.  Granted clearance per 88ABW-2020-3430.
#

#[=======================================================================[.rst:
FindPygmentize
--------------

Locate the Pygmentize source highlighting utility.

Imported targets
^^^^^^^^^^^^^^^^

This module defines the following :prop_tgt:`IMPORTED` targets:

``Pygmentize``
  The Pygmentize executable, if found

Result variables
^^^^^^^^^^^^^^^^

This module defines the following variables:

``Pygmentize_FOUND``
  True if the Pygmentize utility was found
``Pygmentize_EXECUTABLE``
  The path to the Pygmentize executable

#]=======================================================================]

FIND_PROGRAM(Pygmentize_EXECUTABLE NAMES pygmentize)

IF(Pygmentize_EXECUTABLE)
  EXECUTE_PROCESS(
      COMMAND "${Pygmentize_EXECUTABLE}" -V
      OUTPUT_VARIABLE Pygmentize_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE
      RESULT_VARIABLE _)
  IF(_)
    MESSAGE(WARNING "Unable to determine pandoc version: ${_}")
  ELSE()
    STRING(REGEX MATCH
        "[0-9]+\.[0-9]+\.[0-9]+"
        Pygmentize_VERSION
        "${Pygmentize_VERSION}")
  ENDIF()
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Pygmentize
    REQUIRED_VARS Pygmentize_EXECUTABLE
    VERSION_VAR Pygmentize_VERSION)

IF(Pygmentize_FOUND)
  IF(NOT TARGET Pygmentize)
    ADD_EXECUTABLE(Pygmentize IMPORTED GLOBAL)
    SET_TARGET_PROPERTIES(Pygmentize PROPERTIES
    IMPORTED_LOCATION "${Pygmentize_EXECUTABLE}")
  ENDIF()
ENDIF()
