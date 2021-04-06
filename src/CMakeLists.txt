

file(GLOB_RECURSE HEADER_LIST "${PROJECT_SOURCE_DIR}/include/*.hpp")
file(GLOB SOURCE_LIST ${PROJECT_SOURCE_DIR}/src/*.cpp)

set (self ph_coroutines)

add_library (${self}
	SHARED ${SOURCE_LIST}  ${HEADER_LIST} 
)





ph_precompile (TARGET ${self} PRIVATE PCH ${std_pch} ${boost_pch})
target_link_libraries (${self} PRIVATE ph::debug ph::color ph::concepts)
add_library (ph::coroutines ALIAS ph_coroutines)
target_link_libraries (${self} INTERFACE ph::debug ph::concepts)
# target_include_directories (${self} INTERFACE ph::coroutines::generator)

target_link_libraries (${self} PRIVATE ${Boost_LIBRARIES})
target_include_directories (${self} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories (${self} PRIVATE ${Boost_INCLUDE_DIRS})

target_compile_features (${self} PUBLIC cxx_std_20)


target_include_directories(${self} PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/${self}>
    $<INSTALL_INTERFACE:include/${self}>
)

target_include_directories(${self} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)