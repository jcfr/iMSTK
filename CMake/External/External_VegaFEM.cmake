
include(imstkAddExternalProject)

if(CMAKE_PROJECT_NAME STREQUAL "iMSTK")
  set(VegaFEM_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
endif()

#-----------------------------------------------------------------------------
# Add External Project
#-----------------------------------------------------------------------------
set(VegaFEM_DEPENDENCIES "")
if(WIN32)
  list(APPEND VegaFEM_DEPENDENCIES PThreads)
endif(WIN32)

set(EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS)
# After updating the CMake minimum required version to >= 3.15, package
# export is disabled by default (see policy CMP0090) and explicitly setting
# these options will not be needed.
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.15")
  list(APPEND EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS
    -DCMAKE_EXPORT_PACKAGE_REGISTRY:BOOL=OFF
    )
else()
  list(APPEND EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS
    -DCMAKE_EXPORT_NO_PACKAGE_REGISTRY:BOOL=ON
    )
endif()

# Download options
if(NOT DEFINED iMSTK_VegaFEM_GIT_SHA)
  set(iMSTK_VegaFEM_GIT_SHA "32b766921e9e4ff5284b7a596ad8d4c58470c5d3") # build_model_reduction
endif()
if(NOT DEFINED iMSTK_VegaFEM_GIT_REPOSITORY)
  set(EXTERNAL_PROJECT_DOWNLOAD_OPTIONS
    URL https://gitlab.kitware.com/jcfr/vegafemv4.0/-/archive/${iMSTK_VegaFEM_GIT_SHA}/vegafemv4.0-${iMSTK_VegaFEM_GIT_SHA}.zip
    URL_HASH MD5=80984e88a272ee981b11402231e9c4bf
    )
else()
  set(EXTERNAL_PROJECT_DOWNLOAD_OPTIONS
    GIT_REPOSITORY ${iMSTK_VegaFEM_GIT_REPOSITORY}
    GIT_TAG ${iMSTK_VegaFEM_GIT_SHA}
    )
endif()

imstk_add_external_project( VegaFEM
  ${EXTERNAL_PROJECT_DOWNLOAD_OPTIONS}
  CMAKE_CACHE_ARGS
    -DVegaFEM_ENABLE_PTHREADS_SUPPORT:BOOL=ON
    -DVegaFEM_ENABLE_OpenGL_SUPPORT:BOOL=OFF
    -DVegaFEM_BUILD_MODEL_REDUCTION:BOOL=${${PROJECT_NAME}_USE_MODEL_REDUCTION}
    -DVegaFEM_BUILD_UTILITIES:BOOL=OFF
    ${EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS}
  DEPENDENCIES ${VegaFEM_DEPENDENCIES}
  RELATIVE_INCLUDE_PATH ""
  #VERBOSE
)

if(NOT USE_SYSTEM_VegaFEM)
  set(VegaFEM_DIR ${VegaFEM_INSTALL_DIR}/lib/cmake/VegaFEM)
  #message(STATUS "VegaFEM_DIR : ${VegaFEM_DIR}")
endif()

