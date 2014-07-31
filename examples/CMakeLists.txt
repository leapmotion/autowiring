add_executable(ContextExample ContextExample.cpp)
target_link_libraries(ContextExample Autowiring)
set_property(TARGET ContextExample PROPERTY FOLDER "Examples")

add_executable(ThreadExample ThreadExample.cpp)
target_link_libraries(ThreadExample Autowiring)
set_property(TARGET ThreadExample PROPERTY FOLDER "Examples")
