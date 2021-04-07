# configure_file(<input> <output> COPYONLY)
# set(CMAKE_INCLUDE_CURRENT_DIR ON)


file(GLOB_RECURSE HEADER_LIST "${PROJECT_SOURCE_DIR}/include/*.hpp")
file(GLOB SOURCE_LIST ${PROJECT_SOURCE_DIR}/src/*.cpp)
foreach (header_file ${HEADER_LIST})
    # get_filename_component(name ${header_file} NAME)
    # configure_file (${header_file} ${name} @ONLY)
endforeach ()


set (self ph_coroutines)

add_library (${self}
	SHARED ${SOURCE_LIST}  ${HEADER_LIST} 
)





ph_precompile (TARGET ${self} PRIVATE PCH ${std_pch} ${boost_pch})
target_precompile_headers (${self} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/common.hpp)

target_link_libraries (${self} PRIVATE ph::debug ph::color ph::concepts)
add_library (ph::coroutines ALIAS ph_coroutines)
target_link_libraries (${self} INTERFACE ph::debug ph::concepts)
# target_include_directories (${self} INTERFACE ph::coroutines::generator)

target_link_libraries (${self} PRIVATE ${Boost_LIBRARIES})
target_include_directories (${self} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories (${self} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories (${self} PRIVATE ${PROJECT_BINARY_DIR})
target_include_directories (${self} PRIVATE ${Boost_INCLUDE_DIRS})
set (use_namespaces "using namespace std\; using_namespace experimental\; using namespace chrono_literals\;")
# configure_file (/../include/ph_coroutines/i_am_co_awaited.hpp ${CMAKE_CURRENT_BINARY_DIR}/.hpp)
set (kuk "fitta")
configure_file (../include/ph_coroutines/test.hpp test.out.hpp @ONLY)

# target_compile_definitions (${self} PRIVATE USE_NAMESPACES )
target_compile_features (${self} PUBLIC cxx_std_20)
set (namespace_std "using namespace std\;")
set (namespace_experimental "using namespace experimental")

# message (FATAL_ERROR ${kiss})
# set (CMAKE_CXX_FLAGS "-NAMESPACES=\"using namespace std; using namespace experimental; using namespace chrono_literals;\"")
# add_definitions("NAMESPACES=\"using namespace std; using namespace experimental; using namespace chrono_literals;\"")
# set (CMAKE_CXX_FLAGS "-DNAMESPACES=\"using namespace std; using namespace experimental; using namespace chrono_literals;\"")
target_compile_definitions (${self} PUBLIC NAMESPACE_STD=${namespace_std})


target_include_directories(${self} PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/${self}>
    $<INSTALL_INTERFACE:include/${self}>
)

target_include_directories(${self} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)