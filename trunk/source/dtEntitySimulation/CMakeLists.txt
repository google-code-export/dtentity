
SET(TARGET_H ../../include/dtEntitySimulation/groundclampingcomponent.h
			 ../../include/dtEntitySimulation/manipulatorcomponent.h
			 ../../include/dtEntitySimulation/manipulators.h
			 ../../include/dtEntitySimulation/particlecomponent.h
			 ../../include/dtEntitySimulation/pathcomponent.h
			 ../../include/dtEntitySimulation/quattools.h
)			 
			 
SET(TARGET_SRC_IN groundclampingcomponent.cpp
			   manipulatorcomponent.cpp
			   manipulators.cpp
			   particlecomponent.cpp
			   pathcomponent.cpp
			   pluginfactory.cpp
)

REPLACE_SIDS_IN_SOURCE(TARGET_SRC_OUT ${TARGET_SRC_IN})
SET(TARGET_SRC ${TARGET_SRC_IN} ${TARGET_SRC_OUT})

SETUP_PLUGIN(dtEntitySimulation)
