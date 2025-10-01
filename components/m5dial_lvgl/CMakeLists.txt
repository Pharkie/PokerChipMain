file(GLOB SRC_LIST "${COMPONENT_DIR}/src/*.cpp")

idf_component_register(
    SRCS ${SRC_LIST}
    INCLUDE_DIRS
        "${COMPONENT_DIR}/src"
        "${COMPONENT_DIR}/include"
    REQUIRES M5Unified lvgl__lvgl
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC LV_LVGL_H_INCLUDE_SIMPLE LV_CONF_INCLUDE_SIMPLE)

# Make lv_conf.h visible to LVGL
idf_build_get_property(build_components BUILD_COMPONENTS)
if("lvgl__lvgl" IN_LIST build_components)
    idf_component_get_property(lvgl_lib lvgl__lvgl COMPONENT_LIB)
    target_include_directories(${lvgl_lib} BEFORE PUBLIC "${COMPONENT_DIR}/include")
    target_compile_definitions(${lvgl_lib} PUBLIC LV_LVGL_H_INCLUDE_SIMPLE LV_CONF_INCLUDE_SIMPLE)
endif()
