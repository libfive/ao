set(SRCS
    libfive/__init__.py
    libfive/runner.py
    libfive/shape.py

    libfive/stdlib/__init__.py
    libfive/stdlib/csg.py
    libfive/stdlib/shapes.py
    libfive/stdlib/transforms.py
    libfive/stdlib/text.py
)

set(FFI
    libfive/ffi.py
)

set(OUTS "")
foreach(SRC ${SRCS})
    set(FULL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/${SRC})
    list(APPEND OUTS ${SRC})

    # We copy each file from the source directory to the build dir
    add_custom_command(OUTPUT ${SRC}
        COMMAND ${CMAKE_COMMAND} -E copy ${FULL_SRC} ${CMAKE_CURRENT_BINARY_DIR}/${SRC}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS ${FULL_SRC})
endforeach(SRC)

add_custom_target(libfive-python ALL DEPENDS ${OUTS})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${FFI}  "${CMAKE_CURRENT_BINARY_DIR}/${FFI}")

################################################################################

if(UNIX)
    # Find the site-packages directory
    execute_process(
        COMMAND ${Python3_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(), end='')"
        OUTPUT_VARIABLE PYTHON_SITE_PACKAGES_DIR)

    # Install all Python files into that directory
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libfive
            DESTINATION ${PYTHON_SITE_PACKAGES_DIR}
            FILES_MATCHING PATTERN "*.py")
endif(UNIX)
