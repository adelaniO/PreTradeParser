set(HEADER_LIST
"${ExchangeParser_SOURCE_DIR}/include/errorlog.hpp"
"${ExchangeParser_SOURCE_DIR}/include/scanner.hpp"
)

set(SOURCE_LIST
    "errorlog.cpp"
    "scanner.cpp"
 )

# Make an automatic library - will be static or dynamic based on user setting
add_library(parserlib ${SOURCE_LIST} ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(parserlib PUBLIC ../include)
target_compile_features(parserlib PUBLIC cxx_std_17)