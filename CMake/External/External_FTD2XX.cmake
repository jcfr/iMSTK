if(NOT WIN32)
  message(WARNING "System not supported. Only Windows is supported for External_FTD2XX.cmake.")
  message(WARNING "FTD2XX should not be needed on your system.")
  return()
endif()

include(imstkAddExternalProject)

# Set FTD2XX_SOURCE_DIR
imstk_define_external_dirs( FTD2XX )

# Directories and filenames specific to the OpenVR archive layout
set(FTD2XX_DLL_DIR "i386")
set(FTD2XX_DLL_NAME "ftd2xx.dll")
set(FTD2XX_LIB_DIR "i386")
set(FTD2XX_LIB_NAME "ftd2xx.lib")
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
  set(FTD2XX_DLL_DIR "amd64")
  set(FTD2XX_DLL_NAME "ftd2xx64.dll")
  set(FTD2XX_LIB_DIR "amd64")
  set(FTD2XX_LIB_NAME "ftd2xx.lib")
endif()

#-----------------------------------------------------------------------------
# Set install commands
#-----------------------------------------------------------------------------
if(CMAKE_PROJECT_NAME STREQUAL "iMSTK")
  set(FTD2XX_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})

  set(copy_ftd2xx_headers_command
    ${CMAKE_COMMAND} -E copy
    ${FTD2XX_SOURCE_DIR}/ftd2xx.h
    ${FTD2XX_INSTALL_DIR}/include/ftd2xx/ftd2xx.h
    )
  set(copy_ftd2xx_lib_command
    ${CMAKE_COMMAND} -E copy
    ${FTD2XX_SOURCE_DIR}/${FTD2XX_LIB_DIR}/${FTD2XX_LIB_NAME}
    ${FTD2XX_INSTALL_DIR}/lib/${FTD2XX_LIB_NAME}
    )
  set(copy_ftd2xx_dll_command
    ${CMAKE_COMMAND} -E copy
    ${FTD2XX_SOURCE_DIR}/${FTD2XX_DLL_DIR}/${FTD2XX_DLL_NAME}
    ${FTD2XX_INSTALL_DIR}/bin/${FTD2XX_DLL_NAME}
    )

  set(FTD2XX_INSTALL_COMMAND
    INSTALL_COMMAND
      COMMAND ${copy_ftd2xx_headers_command}
      COMMAND ${copy_ftd2xx_lib_command}
      COMMAND ${copy_ftd2xx_dll_command}
    )

  if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
    # HS 2021-aug-08 Some dependencies expect the dll name to
    # be ftd2xx.dll rather than ftd2xx64.dll we're copying here
    # to the renamed version as the whole chain is built via 64 bit builds
    set(copy_ftd2xx_dll_command_rename
      ${CMAKE_COMMAND} -E copy
      ${FTD2XX_SOURCE_DIR}/${FTD2XX_LIB_DIR}/ftd2xx64.dll
      ${FTD2XX_INSTALL_DIR}/bin/ftd2xx.dll
      )

    list(APPEND FTD2XX_INSTALL_COMMAND
      COMMAND ${copy_ftd2xx_dll_command_rename}
      )
  endif()
else()
  set(FTD2XX_INSTALL_COMMAND
    INSTALL_COMMAND
      COMMAND ${SKIP_STEP_COMMAND}
    )
endif()


#-----------------------------------------------------------------------------
# Add External Project
#-----------------------------------------------------------------------------
imstk_add_external_project( FTD2XX
  URL http://www.ftdichip.com/Drivers/CDM/CDM%20v2.12.18%20WHQL%20Certified.zip
  URL_MD5 e1a194765d7bcc58968d523484b74140
  DOWNLOAD_DIR ${FTD2XX_PREFIX}
  SOURCE_DIR ${FTD2XX_SOURCE_DIR}
  UPDATE_COMMAND ${SKIP_STEP_COMMAND}
  CONFIGURE_COMMAND ${SKIP_STEP_COMMAND}
  BUILD_COMMAND ${SKIP_STEP_COMMAND}
  ${FTD2XX_INSTALL_COMMAND}
  RELATIVE_INCLUDE_PATH "/"
  #VERBOSE
)
