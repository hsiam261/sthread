cmake_minimum_required(VERSION 3.12)
project(sthread VERSION 1.0.0)

# Set C standard to C11
set(CMAKE_C_STANDARD 11)

# Add the include directory
include_directories(include)

# Get a list of all .c files in the src directory
file(GLOB_RECURSE SRC_FILES src/*.c)

# Create the dynamic library
add_library(sthread SHARED ${SRC_FILES})

# Set the library version (if needed)
set_target_properties(sthread PROPERTIES VERSION ${PROJECT_VERSION})

# Install the library
install(TARGETS sthread
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)

# Install the header files
install(DIRECTORY include/ DESTINATION include)
