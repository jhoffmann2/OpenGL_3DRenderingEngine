set(PROJECT_NAME RenderingEngine)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
        "Rendering/GBuffer.cpp"
        "Rendering/GBuffer.h"
        "GLApplication.cpp"
        "MainScene.cpp"
        "MainScene.h"
        )
source_group("" FILES ${no_group_source_files})

set(Camera
        "camera/Camera.cpp"
        "camera/Camera.h"
        "camera/CameraTransform.cpp"
        )
source_group("camera" FILES ${Camera})

set(Common
        "../Common/Scene.cpp"
        "../Common/Scene.h"
        "../Common/shader.cpp"
        "../Common/shader.hpp"
        )
source_group("Common" FILES ${Common})

set(Common__Imgui
        "../Common/imgui/examples/imgui_impl_glfw.cpp"
        "../Common/imgui/examples/imgui_impl_glfw.h"
        "../Common/imgui/examples/imgui_impl_opengl3.cpp"
        "../Common/imgui/examples/imgui_impl_opengl3.h"
        "../Common/imgui/imconfig.h"
        "../Common/imgui/imgui.cpp"
        "../../Common/imgui/imgui.h"
        "../Common/imgui/imgui_demo.cpp"
        "../Common/imgui/imgui_draw.cpp"
        "../Common/imgui/imgui_internal.h"
        "../Common/imgui/imgui_widgets.cpp"
        "../Common/imgui/imstb_rectpack.h"
        "../Common/imgui/imstb_textedit.h"
        "../Common/imgui/imstb_truetype.h"
        )
source_group("Common\\Imgui" FILES ${Common__Imgui})

set(Common__soil
        "../Common/soil-master/src/image_dxt.c"
        "../Common/soil-master/src/image_helper.c"
        "../Common/soil-master/src/soil.c"
        "../Common/soil-master/src/stb_image_aug.c"
        )
source_group("Common\\soil" FILES ${Common__soil})

set(GameObject
        "GameObjects/GameObject.cpp"
        "GameObjects/GameObject.h"
        )
source_group("GameObject" FILES ${GameObject})

set(GameObject__Components
        "Lighting/LightComponent.cpp"
        "Lighting/LightComponent.h"
        "Materials/MaterialComponent.cpp"
        "Materials/MaterialComponent.h"
        "GameObjects/ObjectComponent.cpp"
        "GameObjects/ObjectComponent.h"
        "Hierarchy/ParentChildComponent.cpp"
        "Hierarchy/ParentChildComponent.h"
        "Rendering/RenderingComponent.cpp"
        "Rendering/RenderingComponent.h"
        "Transform/TransformComponent.cpp"
        "Transform/TransformComponent.h"
        "Volumes/BoxVolumeComponent.cpp"
        "Volumes/BoxVolumeComponent.h"
        )
source_group("GameObject\\Components" FILES ${GameObject__Components})

set(Lighting
        "Lighting/Light.h"
        "Lighting/LightHandle.cpp"
        "Lighting/LightHandle.h"
        "Lighting/LightSystem.cpp"
        "Lighting/LightSystem.h"
        )
source_group("Lighting" FILES ${Lighting})

set(Materials
        "Materials/material.cpp"
        "Materials/Material.h"
        "Materials/MaterialHandle.h"
        "Materials/MaterialSystem.cpp"
        "Materials/MaterialSystem.h"
        )
source_group("Materials" FILES ${Materials})

set(Mesh
        "Mesh/Mesh.cpp"
        "Mesh/Mesh.h"
        )
source_group("Mesh" FILES ${Mesh})

set(Rendering
        "Rendering/DebugDraw.cpp"
        "Rendering/DebugDraw.h"
        "Rendering/FaceNormalRender.cpp"
        "Rendering/FaceNormalRender.h"
        "Rendering/SolidRender.cpp"
        "Rendering/SolidRender.h"
        "Rendering/VertexNormalRender.cpp"
        "Rendering/VertexNormalRender.h"
        )
source_group("Rendering" FILES ${Rendering})

set(SpacialPartitioning
        "SpacialPartitioning/SpacialTree.cpp"
        "SpacialPartitioning/SpacialTree.h"
        )
source_group("SpacialPartitioning" FILES ${SpacialPartitioning})

set(SpacialPartitioning__BspTrees
        "SpacialPartitioning/BspTree.cpp"
        "SpacialPartitioning/BspTree.h"
        )
source_group("SpacialPartitioning\\BspTrees" FILES ${SpacialPartitioning__BspTrees})

set(SpacialPartitioning__Octrees
        "SpacialPartitioning/Octree.cpp"
        "SpacialPartitioning/Octree.h"
        )
source_group("SpacialPartitioning\\Octrees" FILES ${SpacialPartitioning__Octrees})

set(Texturing
        "Materials/Texture.cpp"
        "Materials/Texture.h"
        )
source_group("Texturing" FILES ${Texturing})

set(Utilities
        "Utilities/GLHelper.h"
        "Utilities/Utilities.h"
        )
source_group("Utilities" FILES ${Utilities})

set(Utilities__ntg-math
        "ntg/bounds.h"
        "ntg/bounds.inl"
        "ntg/bounds_vertex_iterator.inl"
        "ntg/bounds_vertex_itrerator.h"
        "ntg/collision.h"
        "ntg/collision.inl"
        "ntg/hit.h"
        "ntg/hit.inl"
        "ntg/hyperplane.h"
        "ntg/hyperplane.inl"
        "ntg/radial.h"
        "ntg/radial.inl"
        "ntg/ray.h"
        "ntg/ray.inl"
        "ntg/simplex.h"
        "ntg/simplex.inl"
        "ntg/type_mat1x1.h"
        "ntg/type_mat1x1.inl"
        )
source_group("Utilities\\ntg-math" FILES ${Utilities__ntg-math})

set(VertexGlobals
        "Rendering/ShaderGlobalSystem.cpp"
        "Rendering/ShaderGlobalSystem.h"
        )
source_group("VertexGlobals" FILES ${VertexGlobals})

set(ALL_FILES
        ${no_group_source_files}
        ${Camera}
        ${Common}
        ${Common__Imgui}
        ${Common__soil}
        ${GameObject}
        ${GameObject__Components}
        ${Lighting}
        ${Materials}
        ${Mesh}
        ${Rendering}
        ${SpacialPartitioning}
        ${SpacialPartitioning__BspTrees}
        ${SpacialPartitioning__Octrees}
        ${Texturing}
        ${Utilities}
        ${Utilities__ntg-math}
        ${VertexGlobals}
        )

include_directories(".")

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")

set(ROOT_NAMESPACE Assignment2)

################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/"
        )
set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
        )
################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
        "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
        "NDEBUG"
        ">"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
        )
set_source_files_properties(../Common/soil-master/src/image_dxt.c PROPERTIES
        COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS"
        )
set_source_files_properties(../Common/soil-master/src/image_helper.c PROPERTIES
        COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS"
        )
set_source_files_properties(../Common/soil-master/src/soil.c PROPERTIES
        COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS"
        )
set_source_files_properties(../Common/soil-master/src/stb_image_aug.c PROPERTIES
        COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS"
        )
################################################################################
# Compile and link options
################################################################################

target_link_libraries(${PROJECT_NAME}
        glfw
        libglew_static
        opengl32
        assimp::assimp
        )

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
            /Oi;
            /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
            )
    string(CONCAT FILE_CL_OPTIONS
            "/w"
            )
    source_file_compile_options(../Common/soil-master/src/image_dxt.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
            "/w"
            )
    source_file_compile_options(../Common/soil-master/src/image_helper.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
            "/w"
            )
    source_file_compile_options(../Common/soil-master/src/soil.c ${FILE_CL_OPTIONS})
    string(CONCAT FILE_CL_OPTIONS
            "/w"
            )
    source_file_compile_options(../Common/soil-master/src/stb_image_aug.c ${FILE_CL_OPTIONS})
    target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
            /INCREMENTAL
            >
            $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF;
            /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
            )

endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    message("Copying Release DLLs Enabled")
    add_custom_command(TARGET RenderingEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_BINARY_DIR}/Common/GLFW/src/glfw3.dll"
            $<TARGET_FILE_DIR:RenderingEngine>)

    add_custom_command(TARGET RenderingEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_BINARY_DIR}/Common/assimp/code/assimp-vc142-mt.dll"
            $<TARGET_FILE_DIR:RenderingEngine>)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message("Copying Debug DLLs Enabled")
    add_custom_command(TARGET RenderingEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_BINARY_DIR}/Common/GLFW/src/glfw3d.dll"
            $<TARGET_FILE_DIR:RenderingEngine>)

    add_custom_command(TARGET RenderingEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_BINARY_DIR}/Common/assimp/code/assimp-vc142-mtd.dll"
            $<TARGET_FILE_DIR:RenderingEngine>)
endif()


