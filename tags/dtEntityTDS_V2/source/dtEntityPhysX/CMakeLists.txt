FIND_PACKAGE(PhysX REQUIRED)

SET(TARGET_ADDED_LIBRARIES ${PHYSX_LOADER_LIBRARY} ${PHYSX_COOKING_LIBRARY} ${PHYSX_CHARACTER_LIBRARY})

INCLUDE_DIRECTORIES( 
  ${PHYSX_CHARACTER_INCLUDE_DIR}
  ${PHYSX_COOKING_INCLUDE_DIR}
  ${PHYSX_FOUNDATION_INCLUDE_DIR}
  ${PHYSX_LOADER_INCLUDE_DIR}
  ${PHYSX_PHYSICS_INCLUDE_DIR}
)

SET(TARGET_H
   ../../include/dtEntityPhysX/errorstream.h
   ../../include/dtEntityPhysX/pagedterraincullvisitor.h
   ../../include/dtEntityPhysX/physxcomponent.h
   ../../include/dtEntityPhysX/physxgroundclampingcomponent.h
   ../../include/dtEntityPhysX/physxpagedterraincomponent.h
   ../../include/dtEntityPhysX/stream.h
   ../../include/dtEntityPhysX/utils.h
)

SET(TARGET_SRC pagedterraincullvisitor.cpp
	           physxcomponent.cpp
	           physxgroundclampingcomponent.cpp
	           physxpagedterraincomponent.cpp
	           pluginfactory.cpp
	           stream.cpp
	           utils.cpp
)

SETUP_PLUGIN(dtEntityPhysX)

