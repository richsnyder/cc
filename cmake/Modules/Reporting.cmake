FUNCTION(_FIND_FILE)

  CMAKE_PARSE_ARGUMENTS(A
    ""
    "FILE;OUT"
    ""
    ${ARGN}
  )

  IF(NOT DEFINED A_FILE)
    MESSAGE(FATAL_ERROR "A filename is required")
  ENDIF()

  IF(NOT DEFINED A_OUT)
    MESSAGE(FATAL_ERROR "An output variable is required")
  ENDIF()

  IF(EXISTS "${PROJECT_SOURCE_DIR}/${A_FILE}")
    SET(${A_OUT} "${PROJECT_SOURCE_DIR}/${A_FILE}" PARENT_SCOPE)
  ELSEIF(EXISTS "${PROJECT_BINARY_DIR}/${A_FILE}")
    SET(${A_OUT} "${PROJECT_BINARY_DIR}/${A_FILE}" PARENT_SCOPE)
  ELSE()
    MESSAGE(FATAL_ERROR "File '${A_FILE}' not found")
  ENDIF()

ENDFUNCTION()

FUNCTION(REPORT)

  # Parse arguments

  CMAKE_PARSE_ARGUMENTS(A
    ""
    "OUTPUT"
    "MARKDOWN;SOURCE"
    ${ARGN}
  )

  # Ensure that all of the required arguments are present.

  IF(NOT DEFINED A_OUTPUT)
    MESSAGE(FATAL_ERROR "An output filename is required")
  ENDIF()

  IF(NOT DEFINED A_SOURCE)
    MESSAGE(FATAL_ERROR "At least one source file is required")
  ENDIF()

  # Use pandoc to convert each Markdown file to LaTeX.

  FOREACH(source ${A_MARKDOWN})
    STRING(MAKE_C_IDENTIFIER "${source}" id)
    _FIND_FILE(OUT src FILE ${source})
    SET(dst ${CMAKE_CURRENT_BINARY_DIR}/${id}.tex)
    ADD_CUSTOM_COMMAND(
      OUTPUT ${dst}
      MAIN_DEPENDENCY ${src}
      COMMAND ${Pandoc_EXECUTABLE}
      ARGS -o ${dst} ${src}
    )
    LIST(APPEND latex ${dst})
    SET(inputs "${inputs}\n\\input\{${dst}\}\n")
  ENDFOREACH()

  # Pygmentize each of the source files.

  SET(inputs "${inputs}\n\\section\{SOURCE CODE\}\n")

  FOREACH(source ${A_SOURCE})
    STRING(MAKE_C_IDENTIFIER "${source}" id)
    _FIND_FILE(OUT src FILE ${source})
    SET(dst ${CMAKE_CURRENT_BINARY_DIR}/${id}.tex)
    SET(title "\\texttt\{${source}\}")
    STRING(REPLACE "_" "\\_" title "${title}")
    ADD_CUSTOM_COMMAND(
      OUTPUT ${dst}
      MAIN_DEPENDENCY ${src}
      COMMAND ${Pygmentize_EXECUTABLE}
      ARGS -f latex -O linenos -o ${dst} ${src}
    )
    LIST(APPEND latex ${dst})
    SET(inputs "${inputs}\n\\subsection*{${title}}\n\\input\{${dst}\}\n")
  ENDFOREACH()

  # Identify packages to add to the preamble.

  SET(preamble "\\usepackage\{lmodern\}")
  SET(preamble "${preamble}\n\\usepackage[T1]\{fontenc\}")
  SET(preamble "${preamble}\n\\usepackage[margin=1.0in]\{geometry\}")
  SET(preamble "${preamble}\n\\usepackage\{amssymb,amsmath\}")
  SET(preamble "${preamble}\n\\usepackage\{bookmark\}")
  SET(preamble "${preamble}\n\\usepackage\{textcomp\}")
  SET(preamble "${preamble}\n\\usepackage\{xcolor\}")

  # Create the LaTeX document.

  GET_FILENAME_COMPONENT(dir ${A_OUTPUT} DIRECTORY)
  GET_FILENAME_COMPONENT(name_we ${A_OUTPUT} NAME_WE)
  SET(dst ${CMAKE_CURRENT_BINARY_DIR}/${dir}/${name_we}.tex)
  EXECUTE_PROCESS(COMMAND bash -c "echo '' | ${Pygmentize_EXECUTABLE} -f latex -O full -o ${dst}")
  FILE(READ ${dst} content)
  SET(match "\\section\*\{\}\n\n")
  SET(match "${match}\\begin\{Verbatim\}[commandchars=\\\\\\\{\\\}]\n\n")
  SET(match "${match}\\end\{Verbatim\}")
  STRING(REPLACE "${match}" "${inputs}" content "${content}")
  SET(match "\\usepackage[utf8]\{inputenc\}")
  STRING(REPLACE "${match}" "${match}\n${preamble}" content "${content}")
  FILE(WRITE ${dst} ${content})

  # Create a documentation target.

  ADD_CUSTOM_COMMAND(
    OUTPUT ${A_OUTPUT}
    MAIN_DEPENDENCY ${dst}
    DEPENDS ${latex}
    COMMAND ${PDFLATEX_COMPILER}
    ARGS ${dst}
  )

  ADD_CUSTOM_TARGET(doc DEPENDS ${A_OUTPUT})

  # Mark the output PDF file for installation.

  INSTALL(FILES ${A_OUTPUT} DESTINATION doc)

ENDFUNCTION()
