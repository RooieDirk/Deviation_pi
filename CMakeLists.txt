##---------------------------------------------------------------------------
## Author:      nohal aka. Pavel Kalian
## Copyright:   
## License:     wxWidgets License
##---------------------------------------------------------------------------
 
# define minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

SET(VERBOSE_NAME deviation)
SET(TITLE_NAME deviation)
PROJECT(deviation_pi_pi)
SET(PACKAGE_NAME deviation_pi)
SET(VERBOSE_NAME deviation)
SET(TITLE_NAME DEVIATION)
SET(CPACK_PACKAGE_CONTACT "Dirk Smits")

INCLUDE( VERSION.cmake )

SET(PLUGIN_VERSION "${PLUGIN_VERSION_MAJOR}.${PLUGIN_VERSION_MINOR}.${PLUGIN_VERSION_PATCH}" )

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "1")
#SET(CMAKE_BUILD_TYPE Debug)

INCLUDE("cmake/PluginConfigure.cmake")

SET(SRC_DEV
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
		src/nmea0183/LatLong.hpp
		src/nmea0183/latlong.cpp
		src/nmea0183/long.cpp
		src/nmea0183/nmea0183.cpp
		src/nmea0183/nmea0183.hpp
		src/nmea0183/Response.hpp
		src/nmea0183/response.cpp
		src/nmea0183/Sentence.hpp
		src/nmea0183/sentence.cpp
		src/nmea0183/talkerid.cpp
		src/nmea0183/hexvalue.cpp
		src/nmea0183/lat.cpp
		src/nmea0183/expid.cpp
		src/nmea0183/hdt.hpp
		src/nmea0183/hdt.cpp
		src/nmea0183/hdg.hpp
		src/nmea0183/hdg.cpp
		src/nmea0183/hdm.hpp
		src/nmea0183/hdm.cpp
		src/nmea0183/gll.hpp
		src/nmea0183/gll.cpp
		src/nmea0183/gga.hpp
		src/nmea0183/gga.cpp
)
# INCLUDE("cmake/PluginNMEA.cmake")
# INCLUDE("cmake/PluginJSON.cmake")
INCLUDE_DIRECTORIES(nmea0183)
IF(WIN32)
SET(SRC_JSON
	    src/json_defs.h
	    src/jsonreader.h
	    src/jsonval.h
	    src/jsonwriter.h
	    src/jsonreader.cpp
	    src/jsonval.cpp
	    src/jsonwriter.cpp
        )
ENDIF(WIN32)

IF(UNIX)
 INCLUDE("src/cmake/FindTinyXML.cmake")
 FIND_PACKAGE(TinyXML QUIET)
ENDIF(UNIX)

IF(TINYXML_FOUND)
 message (STATUS "Deviation_pi building with system tinyxml")
 INCLUDE_DIRECTORIES(${TINYXML_INCLUDE_DIR})
 ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_DEV} ${SRC_NMEA0183} ${SRC_JSON})
 TARGET_LINK_LIBRARIES(${PACKAGE_NAME} ${TINYXML_LIBRARIES})
ELSE(TINYXML_FOUND)
 message (STATUS "Deviation_pi building with embedded tinyxml")
 INCLUDE_DIRECTORIES(src/tinyxml/)
 SET(SRC_LTINYXML
            src/tinyxml/tinyxml.cpp
            src/tinyxml/tinyxmlparser.cpp
            src/tinyxml/tinyxmlerror.cpp
 )
ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_DEV} ${SRC_NMEA0183} ${SRC_JSON} ${SRC_LTINYXML})
ENDIF(TINYXML_FOUND)
ADD_DEFINITIONS(-DTIXML_USE_STL)

#ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_DEV} ${SRC_NMEA0183} ${SRC_JSON} ${SRC_LTINYXML})

TARGET_LINK_LIBRARIES( ${PACKAGE_NAME} ${wxWidgets_LIBRARIES} )

IF (QT_ANDROID )
    TARGET_LINK_LIBRARIES( ${PACKAGE_NAME} ${OCPN_Core_LIBRARIES} )
    TARGET_LINK_LIBRARIES( ${PACKAGE_NAME} "-lgnustl_shared")
ENDIF(QT_ANDROID)

INCLUDE("cmake/PluginInstall.cmake")
INCLUDE("cmake/PluginLocalization.cmake")
INCLUDE("cmake/PluginPackage.cmake")
