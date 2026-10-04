target_sources(${PROJECT_NAME} PRIVATE
        ldx12context.cpp
        ldx12context.h
        ldx12context_lr.h
        ldx12context_lr.cpp)

# Ldx12 headers require C++20. Apply it only to this backend's implementation.
set_source_files_properties(ldx12context.cpp TARGET_DIRECTORY ${PROJECT_NAME} PROPERTIES
        COMPILE_OPTIONS "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/std:c++20>")
