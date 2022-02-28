
message("Importing Common project settings")

################################################################################
# Source groups
################################################################################
set(common_files
        "Scene.cpp"
        "Scene.h"
        "shader.cpp"
        "shader.h"
        "rgbe.cpp"
        )
source_group("" FILES ${no_group_source_files})

################################################################################
# SubDirectories
#################################################################################
add_subdirectory(GLFW)
add_subdirectory(glew)
add_subdirectory(assimp)

