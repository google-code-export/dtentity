FIND_PACKAGE(V8 REQUIRED)

SET(TARGET_ADDED_LIBRARIES osgLibRocket dtEntityWrappers ${V8_LIBRARIES})

INCLUDE_DIRECTORIES( 
  ${LIBROCKET_INCLUDE_DIR}
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

SET(TARGET_SRC contextwrapper.cpp
	           elementdocumentwrapper.cpp
	           elementwrapper.cpp
	           eventinstancer.cpp
	           eventlistener.cpp
			   eventlistenerinstancer.cpp
			   eventlistenerwrapper.cpp
			   hudcomponent.cpp
			   messages.cpp
			   pluginfactory.cpp
			   rocketcomponent.cpp	 
			   rocketsystemwrapper.cpp	 
)

SETUP_PLUGIN(dtEntityRocket)
