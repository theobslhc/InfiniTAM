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
)

#################################################################
# Collect the project files into common, CPU-only and CUDA-only #
#################################################################

set(ORUTILS_OBJECTS
${ORUTILS_HEADERS}
)

#############################
# Specify the source groups #
#############################

SOURCE_GROUP("" FILES ${ORUTILS_HEADERS})

##############################################################
# Specify the include directories, target and link libraries #
##############################################################

IF(WITH_CUDA)
#  include_directories(${CUDA_INCLUDE_DIRS})
#  cuda_add_library(ITMLib
#	${ITMLIB_CPU_OBJECTS}
#	${ITMLIB_CUDA_OBJECTS}
#	${ITMLIB_COMMON_OBJECTS}
#	OPTIONS -gencode arch=compute_11,code=compute_11 -gencode arch=compute_30,code=compute_30)
#  target_link_libraries(ITMLib ${CUDA_LIBRARY})
ELSE()
#  add_definitions(-DCOMPILE_WITHOUT_CUDA)
#  add_library(ITMLib ${ITMLIB_CPU_OBJECTS} ${ITMLIB_COMMON_OBJECTS})
ENDIF()

#target_link_libraries(ITMLib Utils)