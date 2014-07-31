add_executable(ContextExample ContextExample.cpp)
target_link_libraries(ContextExample Autowiring)
set_property(TARGET ContextExample PROPERTY FOLDER "Examples")

add_executable(ThreadExample ThreadExample.cpp)
target_link_libraries(ThreadExample Autowiring)
set_property(TARGET ThreadExample PROPERTY FOLDER "Examples")

add_executable(EventExample EventExample.cpp)
target_link_libraries(EventExample Autowiring)
set_property(TARGET EventExample PROPERTY FOLDER "Examples")

add_executable(AutoFilterExample AutoFilterExample.cpp)
target_link_libraries(AutoFilterExample Autowiring)
set_property(TARGET AutoFilterExample PROPERTY FOLDER "Examples")
