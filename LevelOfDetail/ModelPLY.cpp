//#include "ModelPLY.h"
//
//ModelPLY::ModelPLY()
//{
//}
//
//
//ModelPLY::~ModelPLY()
//{
//	delete[] faces;
//	delete[] vertexBuffer;
//	delete[] normals;
//}
//
//float* ModelPLY::CalculateNormal(float *coord1, float *coord2, float *coord3)
//{
//	//Calculate Vector1 and Vector2
//	float va[3], vb[3], vr[3], val;
//	va[0] = coord1[0] - coord2[0];
//	va[1] = coord1[1] - coord2[1];
//	va[2] = coord1[2] - coord2[2];
//
//	vb[0] = coord1[0] - coord3[0];
//	vb[1] = coord1[1] - coord3[1];
//	vb[2] = coord1[2] - coord3[2];
//
//	//Cross product
//	vr[0] = va[1] * vb[2] - vb[1] * va[2];
//	vr[1] = vb[0] * va[2] - va[0] * vb[2];
//	vr[2] = va[0] * vb[1] - vb[0] * va[1];
//
//	//Normalization factor
//	val = sqrt(vr[0] * vr[0] + vr[1] * vr[1] + vr[2] * vr[2]);
//
//	float norm[3];
//	norm[0] = vr[0] / val;
//	norm[1] = vr[1] / val;
//	norm[2] = vr[2] / val;
//
//
//	return norm;
//}
//
//int ModelPLY::Load(char* filename)
//{
//	this->totalConnectedTriangles = 0;
//	this->totalConnectedPoints = 0;
//
//	char* pch = strstr(filename, ".ply");
//
//	if (pch != NULL)
//	{
//		FILE* file = fopen(filename, "r");
//
//		fseek(file, 0, SEEK_END);
//		long fileSize = ftell(file);
//
//		try
//		{
//			vertexBuffer = (float*)malloc(ftell(file));
//		}
//		catch (char*)
//		{
//			return -1;
//		}
//		if (vertexBuffer == NULL) return -1;
//		fseek(file, 0, SEEK_SET);
//
//		faces = (float*)malloc(fileSize * sizeof(float));
//		normals = (float*)malloc(fileSize * sizeof(float));
//
//		if (file)
//		{
//			int i = 0;
//			int temp = 0;
//			int quads_index = 0;
//			int triangle_index = 0;
//			int normal_index = 0;
//			char buffer[1000];
//
//			fgets(buffer, 300, file);
//
//
//			// READ HEADER
//			// -----------------
//
//			// Find number of vertices
//			while (strncmp("element vertex", buffer, strlen("element vertex")) != 0)
//			{
//				fgets(buffer, 300, file);
//			}
//			strcpy(buffer, buffer + strlen("element vertex"));
//			sscanf(buffer, "%i", &this->totalConnectedPoints);
//
//
//			//Find number of faces
//			fseek(file, 0, SEEK_SET);
//			while (strncmp("element face", buffer, strlen("element face")) != 0)
//			{
//				fgets(buffer, 300, file);
//			}
//			strcpy(buffer, buffer + strlen("element face"));
//			sscanf(buffer, "%i", &this->totalFaces);
//
//
//			//Go to end_header
//			while (strncmp("end_header", buffer, strlen("end_header")) != 0)
//			{
//				fgets(buffer, 300, file);
//			}
//
//
//			//Read vertices
//			i = 0;
//			for (int iterator = 0; iterator < this->totalConnectedPoints; iterator++)
//			{
//				fgets(buffer, 300, file);
//
//				sscanf(buffer, "%f %f %f", &vertexBuffer[i], &vertexBuffer[i + 1], &vertexBuffer[i + 2]);
//				i += 3;
//			}
//
//			//Read faces
//			i = 0;
//			for (int iterator = 0; iterator < this->totalFaces; iterator++)
//			{
//				fgets(buffer, 300, file);
//
//				if (buffer[0] == '3')
//				{
//
//					int vertex1 = 0, vertex2 = 0, vertex3 = 0;
//					buffer[0] = ' ';
//					sscanf(buffer, "%i%i%i", &vertex1, &vertex2, &vertex3);
//					printf("%f %f %f ", vertexBuffer[3 * vertex1], vertexBuffer[3 * vertex1 + 1], vertexBuffer[3 * vertex1 + 2]);
//
//					faces[triangle_index] = vertexBuffer[3 * vertex1];
//					faces[triangle_index + 1] = vertexBuffer[3 * vertex1 + 1];
//					faces[triangle_index + 2] = vertexBuffer[3 * vertex1 + 2];
//					faces[triangle_index + 3] = vertexBuffer[3 * vertex2];
//					faces[triangle_index + 4] = vertexBuffer[3 * vertex2 + 1];
//					faces[triangle_index + 5] = vertexBuffer[3 * vertex2 + 2];
//					faces[triangle_index + 6] = vertexBuffer[3 * vertex3];
//					faces[triangle_index + 7] = vertexBuffer[3 * vertex3 + 1];
//					faces[triangle_index + 8] = vertexBuffer[3 * vertex3 + 2];
//
//					float coord1[3] = { faces[triangle_index], faces[triangle_index + 1],faces[triangle_index + 2] };
//					float coord2[3] = { faces[triangle_index + 3],faces[triangle_index + 4],faces[triangle_index + 5] };
//					float coord3[3] = { faces[triangle_index + 6],faces[triangle_index + 7],faces[triangle_index + 8] };
//					float *norm = CalculateNormal(coord1, coord2, coord3);
//
//					normals[normal_index] = norm[0];
//					normals[normal_index + 1] = norm[1];
//					normals[normal_index + 2] = norm[2];
//					normals[normal_index + 3] = norm[0];
//					normals[normal_index + 4] = norm[1];
//					normals[normal_index + 5] = norm[2];
//					normals[normal_index + 6] = norm[0];
//					normals[normal_index + 7] = norm[1];
//					normals[normal_index + 8] = norm[2];
//
//					normal_index += 9;
//
//					triangle_index += 9;
//					totalConnectedTriangles += 3;
//				}
//
//
//				i += 3;
//			}
//
//
//			fclose(file);
//		}
//
//		else { printf("File cannot be opened."); }
//	}
//	else
//	{
//		printf("File does not have a .PLY extension. ");
//	}
//	return 0;
//}