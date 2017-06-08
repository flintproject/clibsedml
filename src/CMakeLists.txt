add_library(sedml
  document.c
  mathml.c
  reader.c
  writer.c
  xhtml.c
  )

target_link_libraries(sedml PRIVATE "${LIBXML2_LIBRARIES}")

install(TARGETS sedml
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  )

install(DIRECTORY sedml DESTINATION "${CMAKE_INSTALL_PREFIX}/include")
