#CMakeLists.txt

# 定义cmake最低版本
cmake_minimum_required(VERSION 3.10)

# 定义工程文件名
PROJECT (RayTracing)

# 第三方库
# 在计算机中寻找符合要求的第三方库，同时这些库也要求支持用cmake构建（现在一般c++库都是支持cmake构建的）
# 第一个参数是库的名字，后面的REQUIRED表示这个库是必须的，如果没有就会报错
# find_package(imgui REQUIRED)

# 因为这个项目由众多的C++源文件组成，所以这里用file glob命令通过通配符来匹配所有的C++源文件，并将它存放在变量 SRC_FILES 中
file(GLOB SRC_FILES
    "${PROJECT_SOURCE_DIR}/src/*.h"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.c"
    "${PROJECT_SOURCE_DIR}/src/*.cc"
)

# 构建可执行文件（工程文件名字，匹配的所有源文件）
ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${SRC_FILES})

# 假设用到了第三方库，那么需要再这里进行链接操作
# 
# TARGET_LINK_LIBRARIES(${CMAKE_PROJEXT_NAME} PRIVATE imgui::imgui)

# 如果用到了C++17的语法，那么需要通过TARGET_COMPILE_FEATURES打开对c++17的支持
TARGET_COMPILE_FEATURES(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_17)

# 拷贝编译构建后的静态文件
# 拷贝<项目根目录>/assets -> <项目根目录>/build/Debug/assets
# ADD_CUSTOM_COMMAND(
#     TARGET ${CMAKE_PROJECT_NAME}
#     POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#             "${PROJECT_SOURCE_DIR}/assets"
#             "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/assets"
# )

# ADD_CUSTOM_COMMAND(
#     TARGET ${CMAKE_PROJECT_NAME}
#     POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#             "${PROJECT_SOURCE_DIR}/shader"
#             "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/shader"
# )
