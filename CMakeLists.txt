cmake_minimum_required(VERSION 4.1)
project(3D_render)

include_directories(src)

include_directories(libraries/SDL/include/SDL3)
add_subdirectory(libraries/SDL EXCLUDE_FROM_ALL)

include_directories(libraries/SDL_image/include/SDL3_image)
add_subdirectory(libraries/SDL_image EXCLUDE_FROM_ALL)

include_directories(libraries/imgui)
add_subdirectory(libraries/imgui EXCLUDE_FROM_ALL)

include_directories(libraries/gltf/include)
add_subdirectory(libraries/gltf EXCLUDE_FROM_ALL)

# --- Détection du système d'exploitation ---
if(WIN32)
    # Configuration spécifique à Windows (ex: liens statiques/dynamiques)
    message(STATUS "Building for Windows")
    # ... (votre code existant pour Windows)
elseif(UNIX AND NOT APPLE)
    # Configuration spécifique à Linux (Ubuntu/Debian, etc.)
    message(STATUS "Building for Linux")
    # Ajoutez ici les dépendances spécifiques à Linux (ex: libGL, libX11)
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIRS} ${X11_INCLUDE_DIRS})
    link_directories(${OPENGL_LIBRARY_DIRS} ${X11_LIBRARY_DIRS})
endif()

# --- Configuration commune ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Optimisations pour les builds Release ---
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Enabling Release optimizations")
    add_compile_options(-O3 -march=native)  # Optimisations agressives (Linux)
    add_link_options(-Wl,--as-needed)       # Réduction de la taille binaire (Linux)
    # Pour Windows, utilisez plutôt:
    # add_compile_options(/O2 /arch:AVX2)
endif()

# --- Création de l'exécutable ---
add_executable(3D_render
    src/main.cpp
    src/camera.cpp
    src/material.cpp
    src/object.cpp
    src/vertice.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    SDL3::SDL3
    SDL3_image::SDL3_image
    imgui
    fastgltf::fastgltf
    ${OPENGL_LIBRARIES}  # Ajout pour Linux (OpenGL)
    ${X11_LIBRARIES}     # Ajout pour Linux (X11)
)

install(TARGETS 3D_render RUNTIME DESTINATION ${PROJECT_BINARY_DIR})
install(FILES "${PROJECT_BINARY_DIR}libraries/SDL/SDL3${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION ${PROJECT_BINARY_DIR})
install(FILES "${PROJECT_BINARY_DIR}libraries/SDL_image/SDL3_image${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION ${PROJECT_BINARY_DIR})
install(FILES "${PROJECT_BINARY_DIR}libraries/gltf/libfastgltf${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION ${PROJECT_BINARY_DIR})

include(CPack)