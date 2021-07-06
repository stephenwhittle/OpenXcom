include(modio-code-generation)
include(split-compilation)

if (NOT TARGET ${MODIO_TARGET_NAME})
add_library (${MODIO_TARGET_NAME} INTERFACE)
endif()

target_include_directories(${MODIO_TARGET_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/modio/cache)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/modio/compression)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/modio/core)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/modio/detail)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/modio/http)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/modio/impl)

#Populate INTERFACE_SOURCES so CMake view in VS shows the headers in projects using the SDK
target_sources(${MODIO_TARGET_NAME} INTERFACE 
		modio/ModioSDK.h
	)

add_public_header(${MODIO_TARGET_NAME} modio/ModioSDK.h)

add_modio_generated_header_to_target(
	${MODIO_TARGET_NAME}
	${MODIO_ROOT_DIR}/codegen/templates/ErrorCode.h.in 
	${MODIO_ROOT_DIR}/codegen/data/ModioErrorCodes.json
	${CMAKE_BINARY_DIR}/generated/modio/core/ModioErrorCode.h
	)

add_modio_generated_header_to_target(
	${MODIO_TARGET_NAME}
	${MODIO_ROOT_DIR}/codegen/templates/DefaultRequestParameters.h.in
	${SWAGGER_FILE_PATH}
	${CMAKE_BINARY_DIR}/generated/modio/core/ModioDefaultRequestParameters.h
)

add_modio_generated_header_to_target(
	${MODIO_TARGET_NAME}
	${MODIO_ROOT_DIR}/codegen/templates/ModioErrorConditionLibrary.h.in
	${MODIO_ROOT_DIR}/codegen/data/ModioErrorCodes.json
	${CMAKE_BINARY_DIR}/generated/modio/core/ModioErrorConditionLibrary.h
)

# Ensure that everythingin boost beast can be compiled as a header only library
target_compile_definitions(${MODIO_TARGET_NAME} INTERFACE 
    BOOST_BEAST_DECL=inline)

target_include_directories(${MODIO_TARGET_NAME} INTERFACE ${ModioGeneratedHeaderPath})

target_link_libraries(${MODIO_TARGET_NAME} INTERFACE asio ghc_filesystem fmt-header-only tl::optional platform nlohmann_json)

#target_compile_definitions (${MODIO_TARGET_NAME}Shared PRIVATE ${modio_base_defines})
#note the trailing slash
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/ DESTINATION modio/
	FILES_MATCHING 
		PATTERN "*.h"
		PATTERN "*.ipp"
		PATTERN "*.inl"
		PATTERN "*.hpp")

#only create the shared library target if it wasn't already created by a toolchain file in order to set toolchain-specific properties
if (NOT TARGET ${MODIO_TARGET_NAME}Shared)
	add_library(${MODIO_TARGET_NAME}Shared SHARED ${CMAKE_CURRENT_LIST_DIR}/modio/detail/ModioSDKSharedLibrary.cpp)
else()
	target_sources(${MODIO_TARGET_NAME}Shared PRIVATE ${CMAKE_CURRENT_LIST_DIR}/modio/detail/ModioSDKSharedLibrary.cpp)
endif()


set_target_properties(${MODIO_TARGET_NAME}Shared PROPERTIES LINKER_LANGUAGE CXX)

add_modio_implementation_to_target(${MODIO_TARGET_NAME}Shared)
#TODO: this is hacky and we need to fix it
target_include_directories(${MODIO_TARGET_NAME}Shared PUBLIC "${MODIO_ROOT_DIR}/ext/inja/third_party/include")
target_compile_definitions(${MODIO_TARGET_NAME}Shared PUBLIC -DMODIO_SEPARATE_COMPILATION=1 -DMODIO_NO_HEADER_ONLY)
target_link_libraries(${MODIO_TARGET_NAME}Shared PUBLIC ${MODIO_TARGET_NAME})
get_target_property(modio_headers ${MODIO_TARGET_NAME} PUBLIC_HEADER)
set_property(TARGET ${MODIO_TARGET_NAME}Shared PROPERTY PUBLIC_HEADER ${modio_headers})
#install (TARGETS ${MODIO_TARGET_NAME}Shared ARCHIVE RUNTIME PUBLIC_HEADER DESTINATION include)

if (MODIO_DYNAMIC_LIB_SUFFIX)
message(STATUS "Setting dynamic lib suffix")
set_target_properties(${MODIO_TARGET_NAME}Shared PROPERTIES SUFFIX ${MODIO_DYNAMIC_LIB_SUFFIX})
endif()

add_library(${MODIO_TARGET_NAME}Static STATIC)

set_target_properties(${MODIO_TARGET_NAME}Static PROPERTIES LINKER_LANGUAGE CXX)

add_modio_implementation_to_target(${MODIO_TARGET_NAME}Static)
target_link_libraries(${MODIO_TARGET_NAME}Static PUBLIC ${MODIO_TARGET_NAME})
#TODO: this is hacky and we need to fix it
target_include_directories(${MODIO_TARGET_NAME}Static PUBLIC "${MODIO_ROOT_DIR}/ext/inja/third_party/include")
target_compile_definitions(${MODIO_TARGET_NAME}Static PUBLIC -DMODIO_SEPARATE_COMPILATION=1 -DMODIO_NO_HEADER_ONLY)

if (MODIO_STATIC_LIB_SUFFIX)
set_target_properties(${MODIO_TARGET_NAME}Static PROPERTIES SUFFIX ${MODIO_STATIC_LIB_SUFFIX})
endif()


install(
	DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/
	DESTINATION ${CMAKE_INSTALL_PREFIX}/header_only/common
	COMPONENT header_only
	EXCLUDE_FROM_ALL
)

install(
	DIRECTORY ${ModioGeneratedHeaderPath}/
	DESTINATION ${CMAKE_INSTALL_PREFIX}/header_only/common
	COMPONENT header_only
	EXCLUDE_FROM_ALL
	PATTERN ModioTestConfig.h EXCLUDE
	
)