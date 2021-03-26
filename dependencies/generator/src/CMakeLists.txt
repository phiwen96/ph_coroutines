

file(GLOB_RECURSE HEADER_LIST "${CMAKE_CURRENT_LIST_DIR}/../include/*.hpp")
file(GLOB SOURCE_LIST ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

set (self ph_coroutines_generator)

add_library (${self}
	${SOURCE_LIST}  ${HEADER_LIST} 
)





ph_precompile (TARGET ${self} PRIVATE PCH ${std_pch} ${boost_pch})

add_library (ph::coroutines::generator ALIAS ${self})
target_link_libraries (${self} PRIVATE ${Boost_LIBRARIES})
target_include_directories (${self} PUBLIC ${PROJECT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories (${self} PRIVATE ${Boost_INCLUDE_DIRS})

target_compile_features (${self} PUBLIC cxx_std_20)



target_include_directories(${self} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include/${self}>
    $<INSTALL_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include/${self}>
)

target_include_directories(${self} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)