FIND_PACKAGE(V8 REQUIRED)

SET(TARGET_ADDED_LIBRARIES osgLibRocket dtEntityWrappers ${V8_LIBRARIES})

INCLUDE_DIRECTORIES( 
  ${LIBROCKET_INCLUDE_DIR}
  ${V8_INCLUDE_DIR}
  ../osgLibRocket/source/osgLibRocket/include
)

SET(TARGET_H
   contextwrapper.h
   elementdocumentwrapper.h
   elementwrapper.h
   eventinstancer.h
   eventlistener.h
   eventlistenerinstancer.h
   eventlistenerwrapper.h
   hudcomponent.h
   messages.h
   rocketcomponent.h
   rocketsystemwrapper.h
)

SET(TARGET_SRC_DFTL
         contextwrapper.cpp
         elementdocumentwrapper.cpp
         elementwrapper.cpp
         eventinstancer.cpp
         eventlistener.cpp
			   eventlistenerinstancer.cpp
			   eventlistenerwrapper.cpp
			   messages.cpp
			   pluginfactory.cpp
			   rocketsystemwrapper.cpp	 
)
SET(TARGET_SRC_REPLACE
         hudcomponent.cpp
         rocketcomponent.cpp
)


REPLACE_SIDS_IN_SOURCE(TARGET_SRC_OUT ${TARGET_SRC_REPLACE})
SET(TARGET_SRC ${TARGET_SRC_DFTL} ${TARGET_SRC_OUT} ${TARGET_SRC_REPLACE})
SETUP_PLUGIN(dtEntityRocket)
