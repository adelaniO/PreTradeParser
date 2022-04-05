FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.6)
FetchContent_MakeAvailable(catch)

add_executable(testParser testmain.cpp)
target_compile_features(testParser PRIVATE cxx_std_17)
target_link_libraries(testParser PRIVATE parserlib Catch2::Catch2)
add_test(NAME testparserlib COMMAND testParser WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})