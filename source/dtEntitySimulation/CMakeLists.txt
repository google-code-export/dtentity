
SET(TARGET_H ../../include/dtEntitySimulation/groundclampingcomponent.h
			 ../../include/dtEntitySimulation/manipulatorcomponent.h
			 ../../include/dtEntitySimulation/manipulators.h
			 ../../include/dtEntitySimulation/particlecomponent.h
			 ../../include/dtEntitySimulation/pathcomponent.h
			 ../../include/dtEntitySimulation/quattools.h
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

SETUP_PLUGIN(dtEntitySimulation)
