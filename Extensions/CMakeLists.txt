#This is the CMake file used to build the extensions.
#For more information, see the Readme.md file.

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 NEW)

project(GD-Extensions)

#Sanity checks
IF ("${CMAKE_BUILD_TYPE}" STREQUAL "")
  message( "CMAKE_BUILD_TYPE is empty, assuming build type is Release" )
	set(CMAKE_BUILD_TYPE Release)
ENDIF()

IF (NOT EMSCRIPTEN) #Only add some extensions when compiling with emscripten.
	ADD_SUBDIRECTORY(AdvancedXML)
	ADD_SUBDIRECTORY(AES)
	ADD_SUBDIRECTORY(Box3DObject)
	ADD_SUBDIRECTORY(CommonDialogs)
ENDIF()
ADD_SUBDIRECTORY(DestroyOutsideAutomatism)
ADD_SUBDIRECTORY(DraggableAutomatism)
IF (NOT EMSCRIPTEN)
	ADD_SUBDIRECTORY(Function)
	ADD_SUBDIRECTORY(Light)
	ADD_SUBDIRECTORY(LinkedObjects)
	ADD_SUBDIRECTORY(Network)
	ADD_SUBDIRECTORY(ParticleSystem)
	ADD_SUBDIRECTORY(PanelSpriteObject)
	ADD_SUBDIRECTORY(PathAutomatism)
	ADD_SUBDIRECTORY(PathfindingAutomatism)
	ADD_SUBDIRECTORY(PhysicsAutomatism)
ENDIF()
ADD_SUBDIRECTORY(PlatformAutomatism)
IF (NOT EMSCRIPTEN)
	ADD_SUBDIRECTORY(PrimitiveDrawing)
	ADD_SUBDIRECTORY(SoundObject)
	ADD_SUBDIRECTORY(TextEntryObject)
ENDIF()
	ADD_SUBDIRECTORY(TextObject)
IF (NOT EMSCRIPTEN)
    ADD_SUBDIRECTORY(TileMapObject)
ENDIF()
ADD_SUBDIRECTORY(TiledSpriteObject)
IF (NOT EMSCRIPTEN)
	ADD_SUBDIRECTORY(TimedEvent)
ENDIF()
ADD_SUBDIRECTORY(TopDownMovementAutomatism)
