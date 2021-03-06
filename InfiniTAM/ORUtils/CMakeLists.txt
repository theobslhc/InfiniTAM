#############################
# Specify the project files #
#############################

##
SET(ORUTILS_HEADERS
Vector.h
Matrix.h
Cholesky.h
MathUtils.h
Image.h
CUDADefines.h
LexicalCast.h
MemoryBlock.h
MemoryBlockPersister.h
PlatformIndependence.h
)

#################################################################
# Collect the project files into common, CPU-only and CUDA-only #
#################################################################

set(ORUTILS_OBJECTS
Dummy.cpp
${ORUTILS_HEADERS}
)

#############################
# Specify the source groups #
#############################

SOURCE_GROUP("" FILES ${ORUTILS_HEADERS})

##############################################################
# Specify the include directories, target and link libraries #
##############################################################

add_library(ORUtils ${ORUTILS_OBJECTS})

#############################
# Install rules for ORUtils #
#############################

#install lib
install(TARGETS ORUtils
        DESTINATION lib/InfiniTAM/ORUtils)
#install headers
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/. DESTINATION include/InfiniTAM/ORUtils
        FILES_MATCHING REGEX "^.*\\.(h|hpp)$"
        ) #PATTERN "*/containers" EXCLUDE
