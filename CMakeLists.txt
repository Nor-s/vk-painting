# CMake 프로그램의 최소버전
cmake_minimum_required(VERSION 3.0.0)

# 프로젝트 정보
# project setting
project(vk-painting VERSION 0.1.0)

#set(CMAKE_BUILD_TYPE Release)
message("build type: " ${CMAKE_BUILD_TYPE})

# 소스코드
set(VKCPP_SRC_FILES
    #vkcpp device
    ${CMAKE_SOURCE_DIR}/src/vkcpp/device/window/main_window.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/device/device.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/device/instance.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/device/physical_device.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/device/queue.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/device/surface.cpp
    #vkcpp object
    ${CMAKE_SOURCE_DIR}/src/vkcpp/object/camera/camera.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/object/camera/main_camera.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/object/camera/sub_camera.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/object/model.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/object/object2d.cpp
    #vkcpp render
    ${CMAKE_SOURCE_DIR}/src/vkcpp/render/buffer/descriptor_sets.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/render/command/command_buffers.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/render/command/command_pool.cpp
    #vkcpp image
    ${CMAKE_SOURCE_DIR}/src/vkcpp/render/image/image_depth.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/render/image/image.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/render/image/image2d.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/render/image/offscreen.cpp
    #vkcpp pipeline
    ${CMAKE_SOURCE_DIR}/src/vkcpp/render/pipeline/graphics_pipeline.cpp
    #vkcpp swapchain
    ${CMAKE_SOURCE_DIR}/src/vkcpp/render/swapchain/framebuffers.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/render/swapchain/offscreens.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/render/swapchain/render_pass.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/render/swapchain/swapchain.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/render/render_stage.cpp
    #vkcpp utility
    ${CMAKE_SOURCE_DIR}/src/vkcpp/utility/create.cpp
    ${CMAKE_SOURCE_DIR}/src/vkcpp/utility/utility.cpp
    )
set(APP_SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/class/application.cpp
    ${CMAKE_SOURCE_DIR}/src/class/brush.cpp
    ${CMAKE_SOURCE_DIR}/src/class/picture.cpp
    ${CMAKE_SOURCE_DIR}/src/class/population.cpp
    ${CMAKE_SOURCE_DIR}/src/main.cpp
) 

add_executable(${CMAKE_PROJECT_NAME} ${APP_SRC_FILES} ${VKCPP_SRC_FILES})
# 컴파일 옵션 설정
target_compile_options(${CMAKE_PROJECT_NAME}  PUBLIC
   # -Wall    
   # -Werror
    -O2
    -std=c++17
    )

if(WIN32)
# include
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC 
    ${CMAKE_SOURCE_DIR}/external
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/vkcpp
    C:/VulkanSDK/1.2.189.2/Include
)
# lib
target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/lib
    C:/VulkanSDK/1.2.189.2/Lib
)
# l
target_link_libraries(${CMAKE_PROJECT_NAME} 
-lpthread
-lglfw3
-lvulkan-1
-lgdi32
-static
)
endif(WIN32)


if(APPLE)
# include
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC 
    /opt/homebrew/include
    /Users/soongunno/VulkanSDK/1.2.182.0/macOS/include
    ${CMAKE_SOURCE_DIR}/external/
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/vkcpp
)
# lib
target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC
    /opt/homebrew/lib
    /Users/soongunno/VulkanSDK/1.2.182.0/macOS/Lib
)
# l
target_link_libraries(${CMAKE_PROJECT_NAME} 
-lglfw
-lvulkan
 -ldl 
 -lpthread
)

endif(APPLE)

