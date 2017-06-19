add_library(sedml
  document.c
  mathml.c
  reader.c
  writer.c
  xhtml.c
  )
if(MSVC)
  if(BUILD_SHARED_LIBS)
    target_compile_definitions(sedml PRIVATE SEDML_DLLEXPORT)
  else(BUILD_SHARED_LIBS)
    target_compile_definitions(sedml PRIVATE SEDML_STATIC)
  endif(BUILD_SHARED_LIBS)
endif(MSVC)

target_link_libraries(sedml PRIVATE "${LIBXML2_LIBRARIES}")

install(TARGETS sedml
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  )

install(DIRECTORY sedml DESTINATION "${CMAKE_INSTALL_PREFIX}/include")
