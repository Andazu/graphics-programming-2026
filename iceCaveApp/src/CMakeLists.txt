
set(libraries itugl glad glfw ${APPLE_LIBRARIES})

file(GLOB_RECURSE target_inc CONFIGURE_DEPENDS "*.h")
file(GLOB_RECURSE target_src CONFIGURE_DEPENDS "*.cpp")

add_executable(${TARGETNAME} ${target_inc} ${target_src})
target_link_libraries(${TARGETNAME} ${libraries})
