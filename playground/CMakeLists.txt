SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_LIST_DIR})

FOREACH(subdir ${SUBDIRS})
    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/${subdir}/CMakeLists.txt)
        if(subdir STREQUAL "openGLPlayground")
            set(TARGETNAME OpenGLPlayground)
        else()
            set(TARGETNAME ${subdir})
        endif()
        add_subdirectory(${subdir})
        set_target_properties(
            ${TARGETNAME} PROPERTIES
            # on Visual Studio, sets the debugger working directory to the source directory,
            # to avoid manually copying resource files to the build directory
            # BROKEN ON VS22!
            VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${subdir})
    endif()
ENDFOREACH()
