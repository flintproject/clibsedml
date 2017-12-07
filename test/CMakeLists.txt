set(tests
  check_document
  check_mathml
  check_reader
  check_reader1
  check_reader2
  check_reader_l1v3_ikappab
  check_reader_l1v3_leloup
  check_reader_l1v3_lorenz_cellml
  check_reader_l1v3_lorenz_sbml
  check_reader_l1v3_oscli
  check_rw
  check_writer
  )

foreach(test ${tests})
  add_executable(${test} "${test}.c")
  target_link_libraries(${test} sedml)
  add_test(NAME ${test} COMMAND ${test})
endforeach(test ${tests})
