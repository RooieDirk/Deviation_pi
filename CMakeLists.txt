##---------------------------------------------------------------------------
## Author:      nohal aka. Pavel Kalian
## Copyright:   
## License:     wxWidgets License
##---------------------------------------------------------------------------
 
# define minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

PROJECT(wmm_pi)

SET(PACKAGE_NAME deviation_pi)
SET(VERBOSE_NAME deviation)
SET(TITLE_NAME deviation)
SET(CPACK_PACKAGE_CONTACT "Dirk Smits")

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "1")

#SET(CMAKE_BUILD_TYPE Debug)

INCLUDE("cmake/PluginConfigure.cmake")

SET(SRC_WMM
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
            src/CompasDev1Main.h
            src/CompasDev1Main.cpp
            src/printing.h
            src/printing.cpp
            src/bearingdlg.h
            src/bearingdlg.cpp
            src/icons.h
            src/icons.cpp
            
	)

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

ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_WMM} ${SRC_JSON})

TARGET_LINK_LIBRARIES( ${PACKAGE_NAME} ${wxWidgets_LIBRARIES} )

IF (QT_ANDROID )
    TARGET_LINK_LIBRARIES( ${PACKAGE_NAME} ${OCPN_Core_LIBRARIES} )
    TARGET_LINK_LIBRARIES( ${PACKAGE_NAME} "-lgnustl_shared")
ENDIF(QT_ANDROID)

INCLUDE("cmake/PluginInstall.cmake")
INCLUDE("cmake/PluginLocalization.cmake")
INCLUDE("cmake/PluginPackage.cmake")
