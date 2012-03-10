
SET(TARGET_H ../../include/dtEntitySimulation/groundclampingcomponent.h
			 ../../include/dtEntitySimulation/manipulatorcomponent.h
			 ../../include/dtEntitySimulation/manipulators.h
			 ../../include/dtEntitySimulation/particlecomponent.h
			 ../../include/dtEntitySimulation/pathcomponent.h
			 ../../include/dtEntitySimulation/quattools.h
)			 
			 
SET(TARGET_SRC groundclampingcomponent.cpp
			   manipulatorcomponent.cpp
			   manipulators.cpp
			   particlecomponent.cpp
			   pathcomponent.cpp
			   pluginfactory.cpp
)

SETUP_PLUGIN(dtEntitySimulation)
