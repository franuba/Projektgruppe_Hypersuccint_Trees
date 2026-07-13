set(PHT_UI_PY_SOURCES
    gui.py
)
list(TRANSFORM PHT_UI_PY_SOURCES PREPEND "${CMAKE_CURRENT_LIST_DIR}/")
add_custom_target(pht_ui ALL SOURCES ${PHT_UI_PY_SOURCES})
add_dependencies(pht_ui pht_lib)


set(PHT_UI_OTHER_SOURCES
    pht_hst.pyx
    ../src/bitvector_utils.cpp
    ../src/hst_node.cpp
    ../src/hst_output.cpp
    ../src/hypersuccinct_tree.cpp
    ../src/hypersuccinct_tree_queries.cpp
    ../src/logger.cpp
    ../src/timer.cpp
    ../src/xml_reader.cpp
    ../extern/irrxml/irrXML.cpp
    ../extern/succinct_bv/src/bit_vector.cc
)
list(TRANSFORM PHT_UI_OTHER_SOURCES PREPEND "\"")
list(TRANSFORM PHT_UI_OTHER_SOURCES APPEND "\", ")
list(JOIN PHT_UI_OTHER_SOURCES "" PHT_UI_OTHER_SOURCES)
set(PHT_UI_COMPILE_ARGS
    -std:c++17
)
list(TRANSFORM PHT_UI_COMPILE_ARGS PREPEND "\"")
list(TRANSFORM PHT_UI_COMPILE_ARGS APPEND "\", ")
list(JOIN PHT_UI_COMPILE_ARGS "" PHT_UI_COMPILE_ARGS)
set(PHT_UI_INCLUDE_DIRS
    ../include
    ../extern/irrxml
    ../extern/succinct_bv/include
    ../extern/thread-pool-2.0.0
)
list(TRANSFORM PHT_UI_INCLUDE_DIRS PREPEND "\"")
list(TRANSFORM PHT_UI_INCLUDE_DIRS APPEND "\", ")
list(JOIN PHT_UI_INCLUDE_DIRS "" PHT_UI_INCLUDE_DIRS)
configure_file("${CMAKE_CURRENT_LIST_DIR}/setup.py.in" ${CMAKE_CURRENT_LIST_DIR}/setup.py)
add_custom_command(TARGET pht_ui POST_BUILD WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR} COMMAND python ARGS setup.py build_ext --inplace)

#add_custom_command(TARGET pht_ui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/pht/$<CONFIG>/pht_lib.dll ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/pht_lib.dll)
#add_custom_command(TARGET pht_ui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PHT_UI_PY_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
