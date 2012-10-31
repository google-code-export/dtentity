
SET(HEADER_DIR ../../include/dtEntitySimulation)

SET(TARGET_H
  ${HEADER_DIR}/groundclampingcomponent.h
  ${HEADER_DIR}/manipulatorcomponent.h
  ${HEADER_DIR}/manipulators.h
  ${HEADER_DIR}/particlecomponent.h
  ${HEADER_DIR}/pathcomponent.h
)			 
			 
SET(TARGET_SRC_ADD
         manipulators.cpp
         pluginfactory.cpp
)

SET(TARGET_SRC_REPLACE
         groundclampingcomponent.cpp
         manipulatorcomponent.cpp
         pathcomponent.cpp
         particlecomponent.cpp
)

REPLACE_SIDS_IN_SOURCE(TARGET_SRC_OUT ${TARGET_SRC_REPLACE})
SET(TARGET_SRC ${TARGET_SRC_ADD} ${TARGET_SRC_OUT} ${TARGET_SRC_REPLACE})

SET(TARGET_ADDED_LIBRARIES dtEntityOSG)
SETUP_PLUGIN(dtEntitySimulation)
