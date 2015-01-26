find_path(OPENHAPTICS_INCLUDE_DIR
  HL/hl.h HLU/hlu.h HD/hd.h HDU/hdu.h
  PATHS /usr/include)

find_library(OPENHAPTICS_hd_LIBRARY
  NAMES
    HD)
find_library(OPENHAPTICS_hdu_LIBRARY
  NAMES
    HDU)
find_library(OPENHAPTICS_hl_LIBRARY
  NAMES
    HL)
find_library(OPENHAPTICS_hlu_LIBRARY
  NAMES
    HLU)

set(OPENHAPTICS_INCLUDE_DIRS "${OPENHAPTICS_INCLUDE_DIR}")
set(OPENHAPTICS_LIBRARIES "${OPENHAPTICS_hd_LIBRARY}" "${OPENHAPTICS_hdu_LIBRARY}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OPENHAPTICS
  REQUIRED_VARS
    OPENHAPTICS_INCLUDE_DIR
    OPENHAPTICS_hd_LIBRARY
    OPENHAPTICS_hdu_LIBRARY
    OPENHAPTICS_hl_LIBRARY
    OPENHAPTICS_hlu_LIBRARY)

mark_as_advanced(
  OPENHAPTICS_INCLUDE_DIR
  OPENHAPTICS_hd_LIBRARY
  OPENHAPTICS_hdu_LIBRARY
  OPENHAPTICS_hl_LIBRARY
  OPENHAPTICS_hlu_LIBRARY)

if(OPENHAPTICS_FOUND)
  if(NOT TARGET OH::HD)
    add_library(OH::HD INTERFACE IMPORTED)
    set_target_properties(OH::HD PROPERTIES
      INTERFACE_LINK_LIBRARIES "${OPENHAPTICS_hd_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${OPENHAPTICS_INCLUDE_DIR}")
  endif()
  if(NOT TARGET OH::HDU)
    add_library(OH::HDU INTERFACE IMPORTED)
    set_target_properties(OH::HDU PROPERTIES
      INTERFACE_LINK_LIBRARIES "${OPENHAPTICS_hdu_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${OPENHAPTICS_INCLUDE_DIR}")
  endif()
  if(NOT TARGET OH::HL)
    add_library(OH::HL INTERFACE IMPORTED)
    set_target_properties(OH::HL PROPERTIES
      INTERFACE_LINK_LIBRARIES "${OPENHAPTICS_hl_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${OPENHAPTICS_INCLUDE_DIR}")
  endif()
  if(NOT TARGET OH::HLU)
    add_library(OH::HLU INTERFACE IMPORTED)
    set_target_properties(OH::HLU PROPERTIES
      INTERFACE_LINK_LIBRARIES "${OPENHAPTICS_hlu_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${OPENHAPTICS_INCLUDE_DIR}")
  endif()
endif()
