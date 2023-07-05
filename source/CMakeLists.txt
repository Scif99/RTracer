add_executable(${PROJECT} 
    main.cpp 
    sphere.cpp 
    triangle.cpp
    )

#Let the compiler know where the header files are located
target_include_directories(${PROJECT}
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/source/include/ 
)