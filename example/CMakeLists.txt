## Header only utilities shared by the examples, included as "common/<file>.hpp"
add_library(example_common INTERFACE)
target_include_directories(example_common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(example_common INTERFACE unitree_sdk2)

add_subdirectory(helloworld)
add_subdirectory(wireless_controller)
add_subdirectory(jsonize)
add_subdirectory(state_machine)


add_subdirectory(go2)
add_subdirectory(b2)
add_subdirectory(h1)
add_subdirectory(g1)
add_subdirectory(go2w)
add_subdirectory(b2w)
add_subdirectory(a2)
add_subdirectory(r1)
add_subdirectory(h2)
add_subdirectory(as2)