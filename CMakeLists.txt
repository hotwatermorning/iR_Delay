cmake_minimum_required(VERSION 3.14.0)

set(vst3sdk_SOURCE_DIR C:/VST_SDK/VST3_SDK)
if(NOT vst3sdk_SOURCE_DIR)
    message(FATAL_ERROR "Path to VST3 SDK is empty!")
endif()

project(iR_Delay)
  
set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

smtg_add_vst3plugin(iR_Delay     
    source/version.h
    source/iR_Delay_cids.h
    source/iR_Delay_processor.h
    source/iR_Delay_processor.cpp
    source/iR_Delay_controller.h
    source/iR_Delay_controller.cpp
    source/iR_Delay_entry.cpp
    source/iR_Delay_paramids.h
    #if(SMTG_ADD_VSTGUI)
        #source/iR_GUIBase.cpp
    	#source/iR_GUIBase.h
    	#source/iR_Delay_guieditor.cpp
		#source/iR_Delay_guieditor.h
    #endif()
)

#- VSTGUI Wanted ----
if(SMTG_ADD_VSTGUI)
	target_sources(iR_Delay
		PRIVATE
			resource/iR_Delay_resource.rc
            source/iR_GUIBase.cpp
    	    source/iR_GUIBase.h
    	    source/iR_Delay_guieditor.cpp
		    source/iR_Delay_guieditor.h
	)
    target_link_libraries(iR_Delay
        PRIVATE
            	vstgui_support
    )
endif()
# -------------------

smtg_add_vst3_snapshot(iR_Delay resource/4E5634A5198F54A6A8D3E543748B39EF_snapshot.png)
smtg_add_vst3_snapshot(iR_Delay resource/4E5634A5198F54A6A8D3E543748B39EF_snapshot_2.0x.png)

target_link_libraries(iR_Delay
    PRIVATE
        sdk
)

if(SMTG_MAC)
    smtg_set_bundle(iR_Delay INFOPLIST 
        "${CMAKE_CURRENT_LIST_DIR}/resource/Info.plist" 
        PREPROCESS
    )
elseif(SMTG_WIN)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT iR_Delay)
	
	set_target_properties(iR_Delay PROPERTIES
		VS_DEBUGGER_COMMAND
			"$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
		VS_DEBUGGER_COMMAND_ARGUMENTS
			" --pluginfolder \"$(OutDir)\" "
	)
endif()


