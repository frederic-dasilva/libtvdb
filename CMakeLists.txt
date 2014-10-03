project(libtvdb)

cmake_minimum_required(VERSION 2.6)

# search required packages
# ===============================================================================================
set(KDE_MIN_VERSION "4.3.80")
find_package(KDE4 4.3.80 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)

find_package(QuaZip)

# set the version to be used by LibTvdbConfig.cmake
# ===============================================================================================
set(CMAKE_LIBTVDB_VERSION_MAJOR 0)
set(CMAKE_LIBTVDB_VERSION_MINOR 4)
set(CMAKE_LIBTVDB_VERSION_RELEASE 0)
set(CMAKE_LIBTVDB_VERSION ${CMAKE_LIBTVDB_VERSION_MAJOR}.${CMAKE_LIBTVDB_VERSION_MINOR}.${CMAKE_LIBTVDB_VERSION_RELEASE})


# generate LibTvdbConfig.cmake for easy utilisation of the package by other cmake build systems
# ===============================================================================================
configure_file(LibTvdbConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/LibTvdbConfig.cmake @ONLY)
configure_file(LibTvdbConfigVersion.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/LibTvdbConfigVersion.cmake @ONLY)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/LibTvdbConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/LibTvdbConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/cmake/LibTvdb)


# build the lib itself
# ===============================================================================================
include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${QUAZIP_INCLUDE_DIRS}
)

configure_file(version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)

set( corelib_SRCS
  series.cpp
  season.cpp
  episode.cpp
  banner.cpp
  client.cpp
  version.cpp
  url.cpp
  filedownloader.cpp
)

kde4_add_library(tvdb SHARED ${corelib_SRCS})

target_link_libraries(tvdb
  ${QT_QTGUI_LIBRARY}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
${QUAZIP_LIBRARIES}
)

set_target_properties(tvdb PROPERTIES VERSION ${CMAKE_LIBTVDB_VERSION} SOVERSION ${CMAKE_LIBTVDB_VERSION_MAJOR})

install(TARGETS tvdb DESTINATION ${LIB_INSTALL_DIR})
install(FILES
  tvdbfiledownloader.h
  series.h
  season.h
  episode.h
  banner.h
  client.h
  ${CMAKE_CURRENT_BINARY_DIR}/version.h
  tvdb_export.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/tvdb)


# generate documentation
# ===============================================================================================
find_package(Doxygen)

if(DOXYGEN_EXECUTABLE)
  set(LIBTVDB_DOCS_OUTPUT "docs")
  configure_file(${libtvdb_SOURCE_DIR}/Doxyfile.cmake ${libtvdb_BINARY_DIR}/Doxyfile)

  set(LIBTVDB_DOCS_OUTPUT "sfpage")
  set(LIBTVDB_DOXYGEN_FOOTER "${CMAKE_CURRENT_SOURCE_DIR}/footer.html")
  configure_file(${libtvdb_SOURCE_DIR}/Doxyfile.cmake ${libtvdb_BINARY_DIR}/Doxyfile-sf)

  set(QTDOCS_ONLINE "http://doc.qt.nokia.com/latest/")

  if(EXISTS ${QT_DOC_DIR}/html)
    set(QTDOCS "${QT_DOC_DIR}/html")
  else(EXISTS ${QT_DOC_DIR}/html)
    set(QTDOCS ${QTDOCS_ONLINE})
  endif(EXISTS ${QT_DOC_DIR}/html)

  add_custom_target(
    apidox
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
    COMMAND docs/html/installdox -q -l qt4.tag@${QTDOCS} docs/html/*.html
    COMMENT "Building libtvdb API docs...")
  add_custom_target(
    sfpage
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile-sf
    COMMAND sfpage/html/installdox -q -l qt4.tag@${QTDOCS_ONLINE} sfpage/html/*.html
    COMMENT "Building libtvdb Sourceforge webpage...")
  add_custom_target(
    uploadsfpage
    COMMAND rsync -zrtq sfpage/html/ $ENV{USER},libtvdb@web.sf.net:/home/groups/l/li/libtvdb/htdocs/
    DEPENDS sfpage
    COMMENT "Uploading documentation to Sourceforge.net...")
endif(DOXYGEN_EXECUTABLE)

add_subdirectory(client)
