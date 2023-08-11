# ---------------------------------------------------------------------------
#Author:      Pavel Kalin  Copyright: License:     wxWidgets License
# ---------------------------------------------------------------------------
## ----- When changing this file do NOT change the order in which sections occur        ----- ##
## ----- Changes should only be made between the section blocks that identify where     ----- ##
## ----- these changes should be. The whole configuration process relies on this        ----- ##
## ----- sequence to be successful                                                      ----- ##
##
##
## ----- Modify section below to include all the details for your plugin ----- ##
##
## ------------------------------------------------------------------------------
##  Frontend TP1.0.253.0 07/08/2023  Author Jon Gough 
## ------------------------------------------------------------------------------

set(CMLOC "CMakeLists: ")

# define minimum cmake version
cmake_minimum_required(VERSION 3.5.1)
if(COMMAND cmake_policy)
    if(POLICY CMP0043)
        cmake_policy(SET CMP0043 NEW)
    endif(POLICY CMP0043)
    cmake_policy(SET CMP0048 NEW)
# Need cmake 3.13 to implement this, but raspbian builds are at 3.7.2 and bionic at 3.10 and xenial builds at 3.5.1
#    cmake_policy(SET CMP0076 NEW)
    if(POLICY CMP0077)
        cmake_policy(SET CMP0077 NEW)
    endif(POLICY CMP0077)
endif(COMMAND cmake_policy)


# define plugin name, owner and versions
SET(VERBOSE_NAME "deviation")
SET(COMMON_NAME  "Deviation")   # Search "getcommonname"in <plugin_pi>.cpp
SET(TITLE_NAME "DEVIATION")
set(PACKAGE_CONTACT "Dirk Smits")
set(PACKAGE "deviation")
set(SHORT_DESCRIPTION "Compass deviation PlugIn for OpenCPN")
set(LONG_DESCRIPTION "Set deviation table to correct incoming nmea compass course to true course sentences, also activate wmm.")

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "1")
set(VERSION_PATCH "0")
set(VERSION_TWEAK "0")
set(VERSION_DATE "09/08/2023") # DD/MM/YYYY format
set(OCPN_MIN_VERSION "ov58")
set(OCPN_API_VERSION_MAJOR "1")
set(OCPN_API_VERSION_MINOR "16")
set(TP_COMMENT "  * Release for O5 using CI")

set(PARENT "opencpn")

# The next line allows for setting up your own local webserver with git on it for testing purposes, the default is github.com
#set(GIT_REPOSITORY_SERVER "github.com")

# Specifies cloudsmith repository sufficies
set(PROD "prod")    # Standard Repos
set(BETA "beta")    # Standard Repos
set(ALPHA "alpha")  # Standard Repos

# set this if your cloudsmith repository name does not match your git repository name
#set(CLOUDSMITH_BASE_REPOSITORY "testplugin_pi")
set(CLOUDSMITH_BASE_REPOSITORY "testplugin")
# Set the following to use OpenCPN cloudsmith repositories instead of own repositories
set(CLOUDSMITH_USER "opencpn")
#set(CLOUDSMITH_USER "rgleason")

set(XML_INFO_URL "https://opencpn-manuals.github.io/main/deviation/index.html")
set(XML_SUMMARY ${SHORT_DESCRIPTION})
set(XML_DESCRIPTION ${LONG_DESCRIPTION})

##
## ----- Modify section above to include all the details for your plugin ----- ##
##

##
## ----- Modify section below if there are special requirements for the plugin ----- ##
##

option(PLUGIN_USE_SVG "Use SVG graphics" OFF)

set(CMAKE_CXX_STANDARD 11)

# Use local version of GLU library
# requires libs/glu directory
set(USE_LOCAL_GLU FALSE)
# Prefer libGL.so to libOpenGL.so, see CMP0072
set(OpenGL_GL_PREFERENCE "LEGACY")

option(USE_GL "Enable OpenGL support" OFF)
message(STATUS "${CMLOC}USE_GL: ${USE_GL}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "" "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
message(STATUS "${CMLOC}Build type: ${CMAKE_BUILD_TYPE}")

##
## ----- The statements below are used to setup standard variables that are required by the CMAKE process - do not remove ----- ##
##

project(${PACKAGE})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include("PluginSetup")

##
## ----- The above is used to setup standard variables that are required by the CMAKE process - do not remove ----- ##
##

##
## ----- Modify section below if there are special requirements for the plugin ----- ##
##

set(CMAKE_VERBOSE_MAKEFILE "Activate verbose mode for make files" ON)

option(Plugin_CXX11 "Use c++11" OFF)

##
## ----- Modify section above if there are special requirements for the plugin ----- ##
##

##
## ----- do not change next section - needed to configure build process ----- ##
##

include("PluginConfigure")

##
## ----- do not change section above - needed to configure build process ----- ##
##

#
# ----- Change below to match project requirements for source, headers, etc. ----- ##
#

add_definitions(-DUSE_S57)
# add_definitions(-DPLUGIN_USE_SVG)


SET(SRCS
            src/deviation_pi.h
            src/deviation_pi.cpp
            src/spa.h
            src/spa.cpp
            src/SharedStuff.h
            src/SharedStuff.cpp
            src/readwritexml.h
            src/readwritexml.cpp
            src/preferencedlg.h
            src/preferencedlg.cpp
            src/compass.h
            src/compass.cpp
            src/printing.h
            src/printing.cpp
            src/bearingdlg.h
            src/bearingdlg.cpp
            src/icons.h
            src/icons.cpp
)

SET(SRC_NMEA0183
		libs/nmea0183/LatLong.hpp
		libs/nmea0183/latlong.cpp
		libs/nmea0183/long.cpp
		libs/nmea0183/nmea0183.cpp
		libs/nmea0183/nmea0183.hpp
		libs/nmea0183/Response.hpp
		libs/nmea0183/response.cpp
		libs/nmea0183/Sentence.hpp
		libs/nmea0183/sentence.cpp
		libs/nmea0183/talkerid.cpp
		libs/nmea0183/hexvalue.cpp
		libs/nmea0183/lat.cpp
		libs/nmea0183/expid.cpp
		libs/nmea0183/hdt.hpp
		libs/nmea0183/hdt.cpp
		libs/nmea0183/hdg.hpp
		libs/nmea0183/hdg.cpp
		libs/nmea0183/hdm.hpp
		libs/nmea0183/hdm.cpp
		libs/nmea0183/gll.hpp
		libs/nmea0183/gll.cpp
		libs/nmea0183/gga.hpp
		libs/nmea0183/gga.cpp
)

# IF(WIN32)
# SET(SRC_JSON
# 	    src/json_defs.h
# 	    src/jsonreader.h
# 	    src/jsonval.h
# 	    src/jsonwriter.h
# 	    src/jsonreader.cpp
# 	    src/jsonval.cpp
# 	    src/jsonwriter.cpp
#         )
# ENDIF(WIN32)


#
# ----- If using JSON validation in plugin section below is needed ----- ##
#

# Json not used

#
# ----- If using JSON validation in plugin section above is needed ----- ##
#

# INCLUDE("cmake/PluginNMEA.cmake")
# INCLUDE("cmake/PluginJSON.cmake")
# INCLUDE_DIRECTORIES(nmea0183)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/ocpn-api)
# include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/wxJSON/include)

#
# ----- Change below to match project requirements for android build ----- ##
#

if(QT_ANDROID)
    include_directories( ${PROJECT_SOURCE_DIR}/extinclude/GLES2)
endif(QT_ANDROID)

##
## ----- Change above to match project requirements for android build ----- ##
##


target_sources(${PACKAGE_NAME} PUBLIC ${SRCS} ${HDRS} ${OCPNSRC} ${EXTSRC} ${LIBSSRC} ${SRC_NMEA0183})
## ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_DEV} ${SRC_NMEA0183} ${SRC_JSON} ${SRC_LTINYXML})
## ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_DEV} ${SRC_NMEA0183} ${SRC_JSON} ${SRC_LTINYXML})

if(NOT OCPN_FLATPAK_CONFIG)
    # Build environment not available when flatpak is being configured so following statements will not work
    message(STATUS "${CMLOC}Adding target link libraries to ${PACKAGE_NAME}")

    add_subdirectory(libs/tinyxml)
    target_link_libraries(${PACKAGE_NAME} ocpn::tinyxml)
endif(NOT OCPN_FLATPAK_CONFIG)

add_definitions(-DTIXML_USE_STL)

#
# ----- If using JSON validation in plugin section below is needed ----- ##
#

#  JSON not used

#
# ----- If using JSON validation in plugin section above is needed ----- ##
#

##
## ----- Change above to match project requirements ----- ##
##

##
## ----- do not change next section - needed to configure build process ----- ##
##

# Needed for android builds
if(QT_ANDROID)
    include_directories(BEFORE ${qt_android_include})
endif(QT_ANDROID)

# Needed for all builds
include("PluginInstall")
include("PluginLocalization")
include("PluginPackage")

##
## ----- do not change section above - needed to configure build process ----- ##
##
message(STATUS "${CMLOC}include directories: ")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "${CMLOC}dir='${dir}'")
endforeach()



## PROJECT(deviation_pi)
##  SET(PACKAGE_NAME deviation_pi)
## ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_DEV} ${SRC_NMEA0183} ${SRC_JSON} ${SRC_LTINYXML})

