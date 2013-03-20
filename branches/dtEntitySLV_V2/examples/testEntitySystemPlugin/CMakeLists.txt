SET(TARGET_H testcomponent.h)
SET(TARGET_SRC testcomponent.cpp pluginfactory.cpp)

SET(TARGET_ADDED_LIBRARIES dtEntityOSG)
SETUP_PLUGIN(testEntitySystemPlugin)
