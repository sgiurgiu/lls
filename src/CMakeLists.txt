
find_package(PkgConfig REQUIRED)

pkg_search_module(HIDAPI_USB REQUIRED hidapi-libusb)
pkg_search_module(HIDAPI_HIDRAW REQUIRED hidapi-hidraw)
pkg_search_module(SQLITE REQUIRED sqlite3)

find_library(V8_LIB v8 REQUIRED)
find_library(V8_BASE_LIB NAMES v8_libbase libv8_libbase.so.6  REQUIRED)
find_library(V8_PLATFORM_LIB NAMES  v8_libplatform libv8_libplatform.so.6  REQUIRED)

set(EXECUTABLE_NAME lls)

set (LLS_HEADERS globals.h utils.h keyboard.h jskeyboard.h)
set (LLS_SRCS main.cpp globals.cpp utils.cpp keyboard.cpp jskeyboard.cpp)

add_executable(${EXECUTABLE_NAME} ${LLS_SRCS} ${LLS_HEADERS})

message(STATUS "V8 library is ${SQLITE_LIBRARIES} ${HIDAPI_HIDRAW_LIBRARIES} ${HIDAPI_USB_LIBRARIES} ${V8_LIB}")

target_compile_options(${EXECUTABLE_NAME} PUBLIC ${HIDAPI_HIDRAW_CFLAGS_OTHER} ${HIDAPI_USB_CFLAGS_OTHER} ${SQLITE_CFLAGS_OTHER})
target_link_libraries(${EXECUTABLE_NAME} ${SQLITE_LIBRARIES} ${HIDAPI_HIDRAW_LIBRARIES} ${HIDAPI_USB_LIBRARIES})

target_link_libraries(${EXECUTABLE_NAME} "-Wl,--start-group" ${V8_LIB} ${V8_BASE_LIB} ${V8_PLATFORM_LIB} "-Wl,--end-group")

target_include_directories(${EXECUTABLE_NAME} PUBLIC ${SQLITE_INCLUDE_DIRS} ${HIDAPI_HIDRAW_INCLUDE_DIRS} ${HIDAPI_USB_INCLUDE_DIRS})

install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)
