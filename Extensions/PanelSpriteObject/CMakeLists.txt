cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0015 NEW)

project(PanelSpriteObject)

#Sanity checks and options
IF ("${CMAKE_BUILD_TYPE}" STREQUAL "")
  	message( "CMAKE_BUILD_TYPE is empty, assuming build type is Release" )
	set(CMAKE_BUILD_TYPE Release)
ENDIF()
if(CMAKE_COMPILER_IS_GNUCXX)
	IF ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    	set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
    ENDIF()
endif()

#Common directories:
###
set(GD_base_dir ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(GDCORE_include_dir ${GD_base_dir}/Core)
set(GDCORE_lib_dir ${GD_base_dir}/Binaries/Output/${CMAKE_BUILD_TYPE}_${CMAKE_SYSTEM_NAME})
set(GDCpp_include_dir ${GD_base_dir}/GDCpp)
set(GDCpp_lib_dir ${GD_base_dir}/Binaries/Output/${CMAKE_BUILD_TYPE}_${CMAKE_SYSTEM_NAME})
set(GDCpp_Runtime_lib_dir ${GD_base_dir}/Binaries/Output/${CMAKE_BUILD_TYPE}_${CMAKE_SYSTEM_NAME}/CppPlatform/Runtime)

IF(WIN32) #Special case for wxWidgets on Windows
	set(wxwidgets_include_dir ${GD_base_dir}/ExtLibs/wxwidgets/include)
	IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
		set(wxwidgets_include_dir ${wxwidgets_include_dir} ${GD_base_dir}/ExtLibs/wxwidgets/lib/gcc_dll/mswud)
	ELSE()
		set(wxwidgets_include_dir ${wxwidgets_include_dir} ${GD_base_dir}/ExtLibs/wxwidgets/lib/gcc_dll/mswu)
	ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)
	set(wxwidgets_lib_dir ${GD_base_dir}/ExtLibs/wxwidgets/lib/gcc_dll)
ENDIF(WIN32)

#Dependencies on external libraries:
###
include_directories(${boost_include_dir})
include_directories(${sfml_include_dir})
include_directories(${GDCORE_include_dir})
include_directories(${GDCpp_include_dir})
IF(WIN32) #Special case for wxWidgets on Windows
	include_directories(${wxwidgets_include_dir})
ELSE()
	find_package(wxWidgets COMPONENTS core base adv net ribbon stc aui propgrid richtext html xrc REQUIRED)
	include( "${wxWidgets_USE_FILE}" )
	include_directories(${GTK_INCLUDE_DIRS})
	link_directories(${GTK_LIBRARY_DIRS})
ENDIF(WIN32)
IF(NOT TARGET GDCore) #Ensure that the build will work even when GDCore is not in the target list.
link_directories(${GDCORE_lib_dir})
ENDIF()
IF(NOT TARGET GDCpp) #Ensure that the build will work even when GDC++ is not in the target list.
link_directories(${GDCpp_lib_dir})
ENDIF()
IF(NOT TARGET GDCpp_Runtime)
	add_library(GDCpp_Runtime SHARED IMPORTED)
	IF(WIN32)
		set_property(TARGET GDCpp_Runtime PROPERTY IMPORTED_LOCATION ${GDCpp_Runtime_lib_dir}/GDCpp.dll)
	 	set_property(TARGET GDCpp_Runtime PROPERTY IMPORTED_IMPLIB ${GDCpp_Runtime_lib_dir}/libGDCpp.dll.a)
	ELSE()
		set_property(TARGET GDCpp_Runtime PROPERTY IMPORTED_LOCATION ${GDCpp_Runtime_lib_dir}/libGDCpp.so)
	 	#set_property(TARGET GDCpp_Runtime PROPERTY IMPORTED_IMPLIB ${GDCpp_Runtime_lib_dir}/libGDCpp.so.a)
	ENDIF(WIN32)
ENDIF()

#Defines
###
set(PanelSpriteObject_extra_definitions "${PanelSpriteObject_extra_definitions} GD_IDE_ONLY=1;")
IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
	add_definitions( -DDEBUG )
	IF(WIN32)
		add_definitions( __WXDEBUG__ )
	ENDIF(WIN32)
ELSEIF ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	add_definitions( -DRELEASE )
	add_definitions( -DBOOST_DISABLE_ASSERTS )
ELSE()
	add_definitions( -DDEV )
	add_definitions( -DBOOST_DISABLE_ASSERTS )
ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)

IF(WIN32)
	add_definitions( -DWINDOWS )
	add_definitions( "-DGD_CORE_API=__declspec(dllimport)" )
	add_definitions( "-DGD_API=__declspec(dllimport)" )
	add_definitions( "-DGD_EXTENSION_API=__declspec(dllexport)" )

	add_definitions( -D__GNUWIN32__ )
	add_definitions( -D__WXMSW__ )
	add_definitions( -DwxUSE_UNICODE=1 )
	add_definitions( -DWXUSINGDLL )
ELSE()
	add_definitions( -DLINUX )
	add_definitions( -DGD_API= )
	add_definitions( -DGD_CORE_API= )
	add_definitions( -DGD_EXTENSION_API= )
	add_definitions(${GTK_CFLAGS_OTHER})
ENDIF(WIN32)


#The targets
###
include_directories(.)
file(GLOB source_files *)

add_library(PanelSpriteObject SHARED ${source_files})
add_library(PanelSpriteObject_Runtime SHARED ${source_files})
set_target_properties(PanelSpriteObject PROPERTIES COMPILE_DEFINITIONS "${PanelSpriteObject_extra_definitions}")
set_target_properties(PanelSpriteObject_Runtime PROPERTIES COMPILE_DEFINITIONS "${PanelSpriteObject_Runtime_extra_definitions}")
set_target_properties(PanelSpriteObject PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${GD_base_dir}/Binaries/Output/${CMAKE_BUILD_TYPE}_${CMAKE_SYSTEM_NAME}/CppPlatform/Extensions")
set_target_properties(PanelSpriteObject PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${GD_base_dir}/Binaries/Output/${CMAKE_BUILD_TYPE}_${CMAKE_SYSTEM_NAME}/CppPlatform/Extensions")
set_target_properties(PanelSpriteObject PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${GD_base_dir}/Binaries/Output/${CMAKE_BUILD_TYPE}_${CMAKE_SYSTEM_NAME}/CppPlatform/Extensions")
set_target_properties(PanelSpriteObject_Runtime PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${GD_base_dir}/Binaries/Output/${CMAKE_BUILD_TYPE}_${CMAKE_SYSTEM_NAME}/CppPlatform/Extensions/Runtime")
set_target_properties(PanelSpriteObject_Runtime PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${GD_base_dir}/Binaries/Output/${CMAKE_BUILD_TYPE}_${CMAKE_SYSTEM_NAME}/CppPlatform/Extensions/Runtime")
set_target_properties(PanelSpriteObject_Runtime PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${GD_base_dir}/Binaries/Output/${CMAKE_BUILD_TYPE}_${CMAKE_SYSTEM_NAME}/CppPlatform/Extensions/Runtime")
set_target_properties(PanelSpriteObject_Runtime PROPERTIES RUNTIME_OUTPUT_NAME "PanelSpriteObject")
set_target_properties(PanelSpriteObject_Runtime PROPERTIES LIBRARY_OUTPUT_NAME "PanelSpriteObject")
set_target_properties(PanelSpriteObject_Runtime PROPERTIES ARCHIVE_OUTPUT_NAME "PanelSpriteObject")
set_target_properties(PanelSpriteObject PROPERTIES PREFIX "")
set_target_properties(PanelSpriteObject_Runtime PROPERTIES PREFIX "")
IF(WIN32)
	set_target_properties(PanelSpriteObject PROPERTIES SUFFIX ".xgdwe")
	set_target_properties(PanelSpriteObject_Runtime PROPERTIES SUFFIX ".xgdw")
ELSE()
	set_target_properties(PanelSpriteObject PROPERTIES SUFFIX ".xgdle")
	set_target_properties(PanelSpriteObject_Runtime PROPERTIES SUFFIX ".xgdl")
ENDIF(WIN32)
set_target_properties(PanelSpriteObject_Runtime PROPERTIES RUNTIME_OUTPUT_NAME "PanelSpriteObject")

#Linker files for the IDE extension
###
target_link_libraries(PanelSpriteObject GDCore)
target_link_libraries(PanelSpriteObject GDCpp)
IF(WIN32)
	target_link_libraries(PanelSpriteObject sfml-audio sfml-graphics sfml-window sfml-network sfml-system)
	IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
		target_link_libraries(PanelSpriteObject ${wxwidgets_lib_dir}/libwxmsw29ud_xrc.a ${wxwidgets_lib_dir}/libwxmsw29ud_stc.a ${wxwidgets_lib_dir}/libwxmsw29ud_richtext.a ${wxwidgets_lib_dir}/libwxmsw29ud_propgrid.a ${wxwidgets_lib_dir}/libwxmsw29ud_ribbon.a ${wxwidgets_lib_dir}/libwxmsw29ud_aui.a ${wxwidgets_lib_dir}/libwxmsw29ud_adv.a ${wxwidgets_lib_dir}/libwxmsw29ud_html.a ${wxwidgets_lib_dir}/libwxmsw29ud_core.a ${wxwidgets_lib_dir}/libwxbase29ud_xml.a ${wxwidgets_lib_dir}/libwxbase29ud_net.a ${wxwidgets_lib_dir}/libwxexpatd.a ${wxwidgets_lib_dir}/libwxbase29ud.a ${wxwidgets_lib_dir}/libwxpngd.a ${wxwidgets_lib_dir}/libwxjpegd.a ${wxwidgets_lib_dir}/libwxzlibd.a ${wxwidgets_lib_dir}/libwxtiffd.a)
	ELSE()
		target_link_libraries(PanelSpriteObject ${wxwidgets_lib_dir}/libwxmsw29u_xrc.a ${wxwidgets_lib_dir}/libwxmsw29u_stc.a ${wxwidgets_lib_dir}/libwxmsw29u_richtext.a ${wxwidgets_lib_dir}/libwxmsw29u_propgrid.a ${wxwidgets_lib_dir}/libwxmsw29u_ribbon.a ${wxwidgets_lib_dir}/libwxmsw29u_aui.a ${wxwidgets_lib_dir}/libwxmsw29u_adv.a ${wxwidgets_lib_dir}/libwxmsw29u_html.a ${wxwidgets_lib_dir}/libwxmsw29u_core.a ${wxwidgets_lib_dir}/libwxbase29u_xml.a ${wxwidgets_lib_dir}/libwxbase29u_net.a ${wxwidgets_lib_dir}/libwxexpat.a ${wxwidgets_lib_dir}/libwxbase29u.a ${wxwidgets_lib_dir}/libwxpng.a ${wxwidgets_lib_dir}/libwxjpeg.a ${wxwidgets_lib_dir}/libwxzlib.a ${wxwidgets_lib_dir}/libwxtiff.a)
	ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)

	target_link_libraries(PanelSpriteObject ws2_32 user32 opengl32 glu32)
ELSE()
	target_link_libraries(PanelSpriteObject sfml-audio sfml-graphics sfml-window sfml-network sfml-system)
	target_link_libraries(PanelSpriteObject ${wxWidgets_LIBRARIES})
	target_link_libraries(PanelSpriteObject ${GTK_LIBRARIES})
ENDIF(WIN32)


#Linker files for the GD C++ Runtime extension
###
target_link_libraries(PanelSpriteObject_Runtime GDCpp_Runtime)
IF(WIN32)
	target_link_libraries(PanelSpriteObject_Runtime sfml-audio sfml-graphics sfml-window sfml-network sfml-system)
	target_link_libraries(PanelSpriteObject_Runtime ws2_32 user32 opengl32 glu32)
ELSE()
	target_link_libraries(PanelSpriteObject_Runtime sfml-audio sfml-graphics sfml-window sfml-network sfml-system)
ENDIF(WIN32)
