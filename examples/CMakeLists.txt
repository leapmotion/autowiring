add_executable(ContextExample ContextExample.cpp)
target_link_libraries(ContextExample Autowiring)
set_property(TARGET ContextExample PROPERTY FOLDER "Examples")