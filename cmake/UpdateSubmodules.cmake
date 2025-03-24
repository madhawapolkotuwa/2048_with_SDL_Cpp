# find_package(Git QUIET)
# if(GIT_FOUND)
#     option(UPDATE_SUBMODULES "Check submodules during build" ON)
#     if(NOT UPDATE_SUBMODULES)
#         return()
#     endif()
#     execute_process(COMMAND ${GIT_EXECUTABLE} submodule
#                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#                     OUTPUT_VARIABLE EXISTING_SUBMODULES
#                     RESULT_VARIABLE RETURN_CODE
#                     OUTPUT_STRIP_TRAILING_WHITESPACE)
#     message(STATUS "Updating git submodules:\n${EXISTING_SUBMODULES}")
#     execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
#                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#                     RESULT_VARIABLE RETURN_CODE)
#     if(NOT RETURN_CODE EQUAL "0")
#         message(WARNING "Cannot update submodules. Git command failed with ${RETURN_CODE}")
#         return()
#     endif()
#     message(STATUS "Git submodules updated successfully")
# endif()

# UpdateSubmodules.cmake
find_package(Git QUIET)
if(GIT_FOUND)
    option(UPDATE_SUBMODULES "Check submodules during build" ON)
    if(NOT UPDATE_SUBMODULES)
        return()
    endif()

    # Check if submodules exist
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule status --recursive
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE EXISTING_SUBMODULES
        RESULT_VARIABLE RETURN_CODE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(NOT RETURN_CODE EQUAL "0")
        message(WARNING "Cannot check submodules. Git command failed with ${RETURN_CODE}")
        return()
    endif()

    message(STATUS "Updating git submodules (recursive):\n${EXISTING_SUBMODULES}")

    # Update submodules recursively
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE RETURN_CODE
    )

    if(NOT RETURN_CODE EQUAL "0")
        message(WARNING "Cannot update submodules. Git command failed with ${RETURN_CODE}")
        return()
    endif()

    message(STATUS "Git submodules updated successfully (recursive)")
else()
    message(WARNING "Git not found. Submodules will not be updated.")
endif()