IF(WITH_CUDA)
  include_directories(${CUDA_INCLUDE_DIRS})
ELSE()
  add_definitions(-DCOMPILE_WITHOUT_CUDA)
ENDIF()

IF(PNG_FOUND)
  include_directories(${PNG_INCLUDE_DIRS})
  add_definitions(${PNG_DEFINITIONS})
  add_definitions(-DUSE_LIBPNG)
ENDIF()

add_library(Utils
FileUtils.cpp
FileUtils.h
NVTimer.h
)

IF(PNG_FOUND)
  target_link_libraries(Utils ${PNG_LIBRARIES})
ENDIF()

###########################
# Install rules for Utils #
###########################

#install lib
install(TARGETS Utils
        DESTINATION lib/InfiniTAM/Utils)
#install headers
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/. DESTINATION include/InfiniTAM/Utils
        FILES_MATCHING REGEX "^.*\\.(h|hpp)$"
        ) #PATTERN "*/containers" EXCLUDE
