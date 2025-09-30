file(GLOB SRC_LIST "${COMPONENT_DIR}/src/*.cpp")

idf_component_register(
    SRCS ${SRC_LIST}
    INCLUDE_DIRS
        "${COMPONENT_DIR}/src"
        "${COMPONENT_DIR}/include"
    REQUIRES M5Unified lvgl
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC LV_LVGL_H_INCLUDE_SIMPLE LV_CONF_INCLUDE_SIMPLE)
