INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_DPD dpd)

FIND_PATH(
    DPD_INCLUDE_DIRS
    NAMES dpd/api.h
    HINTS $ENV{DPD_DIR}/include
        ${PC_DPD_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    DPD_LIBRARIES
    NAMES gnuradio-dpd
    HINTS $ENV{DPD_DIR}/lib
        ${PC_DPD_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/dpdTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DPD DEFAULT_MSG DPD_LIBRARIES DPD_INCLUDE_DIRS)
MARK_AS_ADVANCED(DPD_LIBRARIES DPD_INCLUDE_DIRS)
