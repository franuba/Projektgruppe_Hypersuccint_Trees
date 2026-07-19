file(GLOB irrxml_src CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/extern/irrxml/*.h" "${CMAKE_SOURCE_DIR}/extern/irrxml/*.hpp" "${CMAKE_SOURCE_DIR}/extern/irrxml/*.c" "${CMAKE_SOURCE_DIR}/extern/irrxml/*.cpp" "${CMAKE_SOURCE_DIR}/extern/irrxml/*.cc" "*.h" "*.hpp" "*.c" "*.cpp" "*.cc")
add_library(irrxml STATIC ${irrxml_src})
set_target_properties(irrxml PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(irrxml PUBLIC "${CMAKE_SOURCE_DIR}/extern/irrxml")
target_compile_features(irrxml PUBLIC cxx_std_17)
