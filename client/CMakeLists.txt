project(tvdbclient)

include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${libtvdb_SOURCE_DIR}
)

kde4_add_executable(tvdbclient tvdbclient.cpp)
target_link_libraries(tvdbclient
  tvdb
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
)

install(TARGETS tvdbclient ${INSTALL_TARGETS_DEFAULT_ARGS})
