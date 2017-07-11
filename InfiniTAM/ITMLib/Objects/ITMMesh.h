// Copyright 2014-2015 Isis Innovation Limited and the authors of InfiniTAM

#pragma once

#include "../Utils/ITMLibDefines.h"
#include "../../ORUtils/Image.h"

#include <stdlib.h>

namespace ITMLib
{
	namespace Objects
	{
		class ITMMesh
		{
		public:
			struct Triangle { Vector3f p0, p1, p2; };

			MemoryDeviceType memoryType;

			uint noTotalTriangles;
			static const uint noMaxTriangles = SDF_LOCAL_BLOCK_NUM * 32;

			ORUtils::MemoryBlock<Triangle> *triangles;
			ORUtils::MemoryBlock<Triangle> *triangleColorMap;

			explicit ITMMesh(MemoryDeviceType memoryType)
			{
				this->memoryType = memoryType;
				this->noTotalTriangles = 0;

				triangles = new ORUtils::MemoryBlock<Triangle>(noMaxTriangles, memoryType);
				triangleColorMap = new ORUtils::MemoryBlock<Triangle>(noMaxTriangles, memoryType);
			}

			void WriteOBJ(const char *fileName)
			{
				ORUtils::MemoryBlock<Triangle> *cpu_triangles; bool shoulDelete = false;
				if (memoryType == MEMORYDEVICE_CUDA)
				{
					cpu_triangles = new ORUtils::MemoryBlock<Triangle>(noMaxTriangles, MEMORYDEVICE_CPU);
					cpu_triangles->SetFrom(triangles, ORUtils::MemoryBlock<Triangle>::CUDA_TO_CPU);
					shoulDelete = true;
				}
				else cpu_triangles = triangles;

				ORUtils::MemoryBlock<Triangle> *cpu_trianglesColor; shoulDelete = false;
				if (memoryType == MEMORYDEVICE_CUDA)
				{
					cpu_trianglesColor = new ORUtils::MemoryBlock<Triangle>(noMaxTriangles, MEMORYDEVICE_CPU);
					cpu_trianglesColor->SetFrom(triangleColorMap, ORUtils::MemoryBlock<Triangle>::CUDA_TO_CPU);
					shoulDelete = true;
				}
				else cpu_trianglesColor = triangleColorMap;

				Triangle *triangleArray = cpu_triangles->GetData(MEMORYDEVICE_CPU);
				Triangle *triangleColorMapArray = cpu_trianglesColor->GetData(MEMORYDEVICE_CPU);

				FILE *f = fopen(fileName, "w+");
				if (f != NULL)
				{
					for (uint i = 0; i < noTotalTriangles; i++)
					{
						float R,G,B;
						R = (triangleColorMapArray[i].p0.x + triangleColorMapArray[i].p1.x + triangleColorMapArray[i].p2.x)/3.0;
						G = (triangleColorMapArray[i].p0.y + triangleColorMapArray[i].p1.y + triangleColorMapArray[i].p2.y)/3.0;
						B = (triangleColorMapArray[i].p0.z + triangleColorMapArray[i].p1.z + triangleColorMapArray[i].p2.z)/3.0;

						fprintf(f, "v %.4f %.4f %.4f %.4f %.4f %.4f\n", triangleArray[i].p0.x, triangleArray[i].p0.y, triangleArray[i].p0.z,R,G,B);
						fprintf(f, "v %.4f %.4f %.4f %.4f %.4f %.4f\n", triangleArray[i].p1.x, triangleArray[i].p1.y, triangleArray[i].p1.z,R,G,B);
						fprintf(f, "v %.4f %.4f %.4f %.4f %.4f %.4f\n", triangleArray[i].p2.x, triangleArray[i].p2.y, triangleArray[i].p2.z,R,G,B);
						fprintf(f, "f %u %u %u\n", i * 3 + 3, i * 3 + 2, i * 3 + 1);
					}

					fclose(f);
				}

				if (shoulDelete) 
				{
					delete cpu_triangles;
					delete cpu_trianglesColor;
				}
			}

			void WriteSTL(const char *fileName)
			{
				ORUtils::MemoryBlock<Triangle> *cpu_triangles; bool shoulDelete = false;
				if (memoryType == MEMORYDEVICE_CUDA)
				{
					cpu_triangles = new ORUtils::MemoryBlock<Triangle>(noMaxTriangles, MEMORYDEVICE_CPU);
					cpu_triangles->SetFrom(triangles, ORUtils::MemoryBlock<Triangle>::CUDA_TO_CPU);
					shoulDelete = true;
				}
				else cpu_triangles = triangles;

				Triangle *triangleArray = cpu_triangles->GetData(MEMORYDEVICE_CPU);

				FILE *f = fopen(fileName, "wb+");

				if (f != NULL) {
					for (int i = 0; i < 80; i++) fwrite(" ", sizeof(char), 1, f);

					fwrite(&noTotalTriangles, sizeof(int), 1, f);

					float zero = 0.0f; short attribute = 0;
					for (uint i = 0; i < noTotalTriangles; i++)
					{
						fwrite(&zero, sizeof(float), 1, f); fwrite(&zero, sizeof(float), 1, f); fwrite(&zero, sizeof(float), 1, f);

						fwrite(&triangleArray[i].p2.x, sizeof(float), 1, f);
						fwrite(&triangleArray[i].p2.y, sizeof(float), 1, f);
						fwrite(&triangleArray[i].p2.z, sizeof(float), 1, f);

						fwrite(&triangleArray[i].p1.x, sizeof(float), 1, f);
						fwrite(&triangleArray[i].p1.y, sizeof(float), 1, f);
						fwrite(&triangleArray[i].p1.z, sizeof(float), 1, f);

						fwrite(&triangleArray[i].p0.x, sizeof(float), 1, f);
						fwrite(&triangleArray[i].p0.y, sizeof(float), 1, f);
						fwrite(&triangleArray[i].p0.z, sizeof(float), 1, f);

						fwrite(&attribute, sizeof(short), 1, f);

					}
					fclose(f);
				}

				if (shoulDelete) delete cpu_triangles;
			}

			~ITMMesh()
			{
				delete triangles;
			}

			// Suppress the default copy constructor and assignment operator
			ITMMesh(const ITMMesh&);
			ITMMesh& operator=(const ITMMesh&);
		};
	}
}
