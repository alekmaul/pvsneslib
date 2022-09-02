cmake_minimum_required(VERSION 3.5)

project(tmxDocumentation NONE)

# -- CONFIGURE --

option(DOC_HTML "Build html help with Sphinx" Off)
option(DOC_SINGLEHTML "Build html single page help with Sphinx" Off)
find_program(SPHINX_EXECUTABLE NAMES sphinx-build DOC "Sphinx Documentation Builder (sphinx-doc.org)")

set(DOC_HTML_THEME "default" CACHE STRING "HTML Theme to use eg: sphinx_rtd_theme")
mark_as_advanced(DOC_HTML_THEME)
set(DOC_CODESTYLE "sphinx" CACHE STRING "The name of the Pygments (syntax highlighting) style to use")
mark_as_advanced(DOC_CODESTYLE)

if(NOT DOC_HTML AND NOT DOC_SINGLEHTML)
  return()
elseif(NOT SPHINX_EXECUTABLE)
  message(FATAL_ERROR "SPHINX_EXECUTABLE (sphinx-build) is not found!")
endif()

if(tmx_VERSION)
  set(doc_version "${tmx_VERSION_MAJOR}.${tmx_VERSION_MINOR}")
  set(doc_release "${tmx_VERSION}")
endif()
string(TIMESTAMP doc_copyright "2019-%Y, libTMX documentation authors" UTC)

configure_file(conf.py.in conf.py @ONLY)

# --   BUILD   --

set(chain_dependency "")
set(enabled_doc_formats "")
if(DOC_HTML)
  add_custom_command(
    OUTPUT doc_format_html
    COMMAND ${SPHINX_EXECUTABLE}
            -c ${CMAKE_CURRENT_BINARY_DIR}
            -d ${CMAKE_CURRENT_BINARY_DIR}/doctrees
            -b html
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_BINARY_DIR}/html
            > ${CMAKE_CURRENT_BINARY_DIR}/sphinx-html.log
    DEPENDS ${chain_dependency}
    COMMENT "sphinx-build html: see ${CMAKE_CURRENT_BINARY_DIR}/sphinx-html.log"
    VERBATIM)
    set_property(SOURCE doc_format_html PROPERTY SYMBOLIC True)
    set(chain_dependency "doc_format_html")
    list(APPEND enabled_doc_formats doc_format_html)
endif()

if(DOC_SINGLEHTML)
  add_custom_command(
    OUTPUT doc_format_singlehtml
    COMMAND ${SPHINX_EXECUTABLE}
            -c ${CMAKE_CURRENT_BINARY_DIR}
            -d ${CMAKE_CURRENT_BINARY_DIR}/doctrees
            -b singlehtml
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_BINARY_DIR}/singlehtml
            > ${CMAKE_CURRENT_BINARY_DIR}/sphinx-singlehtml.log
    DEPENDS ${chain_dependency}
    COMMENT "sphinx-build singlehtml: see ${CMAKE_CURRENT_BINARY_DIR}/sphinx-singlehtml.log"
    VERBATIM)
    set_property(SOURCE doc_format_singlehtml PROPERTY SYMBOLIC True)
    set(chain_dependency "doc_format_singlehtml")
    list(APPEND enabled_doc_formats doc_format_singlehtml)
endif()

add_custom_target(documentation ALL DEPENDS ${enabled_doc_formats})
