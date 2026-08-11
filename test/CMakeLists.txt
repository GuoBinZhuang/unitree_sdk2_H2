## Unit tests, built with a dependency free test framework (test/ut_test.hpp)
set(UNITREE_SDK_TESTS
        test_any
        test_block_queue
        test_json
        test_string_tool
        test_time_tool
        test_unitree_joystick)

foreach (test_name ${UNITREE_SDK_TESTS})
    add_executable(${test_name} ${test_name}.cpp)
    target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${test_name} unitree_sdk2)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach ()
