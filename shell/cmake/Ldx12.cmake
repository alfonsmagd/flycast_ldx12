# This first integration builds and links the library without adding a renderer.
set(FLYCAST_ENABLE_LDX12_DEFAULT OFF)
if(WIN32 AND MSVC AND NOT WINDOWS_STORE AND ARCHITECTURE STREQUAL "x86_64")
	set(FLYCAST_ENABLE_LDX12_DEFAULT ON)
endif()
option(FLYCAST_ENABLE_LDX12 "Build and link the Ldx12 library (Windows MSVC x64)" ${FLYCAST_ENABLE_LDX12_DEFAULT})

if(FLYCAST_ENABLE_LDX12 AND POLICY CMP0131)
	# Set this before Flycast's target is created so LINK_ONLY isolates compile settings.
	cmake_policy(SET CMP0131 NEW)
endif()

function(flycast_add_ldx12 target)
	if(NOT FLYCAST_ENABLE_LDX12)
		return()
	endif()
	if(NOT WIN32 OR NOT MSVC OR WINDOWS_STORE OR NOT ARCHITECTURE STREQUAL "x86_64")
		message(FATAL_ERROR "Ldx12 integration requires a Windows desktop MSVC x64 build.")
	endif()
	if(CMAKE_VERSION VERSION_LESS 3.24)
		message(FATAL_ERROR "Ldx12 requires CMake 3.24 or newer.")
	endif()
	if(NOT EXISTS "${PROJECT_SOURCE_DIR}/core/deps/LightDX12/CMakeLists.txt")
		message(FATAL_ERROR "Ldx12 submodule is missing. Run: git submodule update --init --recursive")
	endif()

	# Only the core library is needed. Keep these settings local to this function.
	set(LDX12_BUILD_APP OFF)
	set(LDX12_BUILD_EXAMPLES OFF)
	set(LDX12_BUILD_TESTS OFF)
	set(LDX12_BUILD_UTILS OFF)
	set(LDX12_BUILD_THIRD_PARTY OFF)
	set(LDX12_BUILD_DESKTOP_RETRO_OVERLAY OFF)
	set(LDX12_INSTALL OFF)
	add_subdirectory(core/deps/LightDX12)
	set_target_properties(Ldx12 PROPERTIES FOLDER "Dependencies/Ldx12")

	# Link the library and expose its headers without imposing its UNICODE macros
	# or C++20 requirement on existing Flycast sources. Future Ldx12 consumers
	# must compile as C++20; the Ldx12 library itself already does so.
	target_link_libraries(${target} PRIVATE "$<LINK_ONLY:Ldx12::Ldx12>")
	target_compile_definitions(${target} PRIVATE USE_LDX12)
	target_include_directories(${target} PRIVATE "$<TARGET_PROPERTY:Ldx12,INTERFACE_INCLUDE_DIRECTORIES>")
endfunction()
