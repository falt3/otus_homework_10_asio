cmake_minimum_required(VERSION 3.5)

set(PATCH_VERSION "0" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(BulkServer VERSION ${PROJECT_VERSION})

find_package(Boost COMPONENTS 
    program_options
    system
    # filesystem
    # regex
    # unit_test_framework 
    REQUIRED
)

set(SOURCES_APP
        main.cpp
        pool.cpp
        interpret.cpp)

set(nameApp bulk_server)
add_executable(${nameApp} ${SOURCES_APP})

target_link_libraries(${nameApp} ${Boost_LIBRARIES})

set_target_properties(
    ${nameApp} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_compile_options(
    ${nameApp} PRIVATE
    -Wall -Wextra -pedantic -Werror
)  


install(TARGETS ${nameApp} RUNTIME DESTINATION bin)


set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)