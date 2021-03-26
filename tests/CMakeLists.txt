# Testing library
ph_fetch (catch2)
file(GLOB SOURCE_LIST *.cpp)
add_executable (testlib ${SOURCE_LIST})
target_link_libraries (testlib PRIVATE ${PROJECT_NAME} Catch2::Catch2)
ph_precompile (TARGET testlib PRIVATE PCH ${std_pch})
add_test (NAME testlibtest COMMAND testlib) 
 






