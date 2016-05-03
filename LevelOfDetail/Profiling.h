#pragma once
#include "stdafx.h"
#include <iostream>
#include <fstream>

struct Profiler
{
	ID3D11Query* queryDisjoint = nullptr;
	ID3D11Query* queryBeginFrame = nullptr;
	ID3D11Query* queryEndFrame = nullptr;
	ID3D11Query* queryRenderObject = nullptr;
	ID3D11Query* queryPipelineStatistics = nullptr;

	Profiler()
	{
	}

	~Profiler()
	{
		SAFE_RELEASE(queryDisjoint);
		SAFE_RELEASE(queryBeginFrame);
		SAFE_RELEASE(queryEndFrame);
		SAFE_RELEASE(queryRenderObject);
		SAFE_RELEASE(queryPipelineStatistics);
	}

	void Init(ID3D11Device* device)
	{
		HRESULT hr;
		D3D11_QUERY_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		//Create disjoint query
		desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
		hr = device->CreateQuery(&desc, &queryDisjoint);
		if (FAILED(hr))
			throw std::runtime_error("Profiler::Init: Failed to create queryDisjoint. " + GetErrorMessageFromHRESULT(hr));

		//Create timestamp begin query
		desc.Query = D3D11_QUERY_TIMESTAMP;
		hr = device->CreateQuery(&desc, &queryBeginFrame);
		if (FAILED(hr))
			throw std::runtime_error("Profiler::Init: Failed to create queryBeginFrame. " + GetErrorMessageFromHRESULT(hr));

		//Create timestamp end query
		hr = device->CreateQuery(&desc, &queryEndFrame);
		if (FAILED(hr))
			throw std::runtime_error("Profiler::Init: Failed to create queryEndFrame. " + GetErrorMessageFromHRESULT(hr));

		//Create timestamp query for the rendering of the object
		hr = device->CreateQuery(&desc, &queryRenderObject);
		if (FAILED(hr))
			throw std::runtime_error("Profiler::Init: Failed to create queryRenderObject. " + GetErrorMessageFromHRESULT(hr));

		//Create pipeline staticstics query
		desc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
		hr = device->CreateQuery(&desc, &queryPipelineStatistics);
		if (FAILED(hr))
			throw std::runtime_error("Profiler::Init: Failed to create queryPipelineStatistics. " + GetErrorMessageFromHRESULT(hr));
	}

	void CollectData(ID3D11DeviceContext* deviceContext, string filename)
	{
		//Wait for data to be available
		while (deviceContext->GetData(queryDisjoint, NULL, 0, 0) == S_FALSE)
		{
			Sleep(1); 
		}

		//Check whether timestamps were disjoint during the last frame
		D3D11_QUERY_DATA_TIMESTAMP_DISJOINT tsDisjoint;
		deviceContext->GetData(queryDisjoint, &tsDisjoint, sizeof(tsDisjoint), 0);
		if (tsDisjoint.Disjoint)
		{
			return;
		}

		//Get all the timestamps
		UINT64 tsBeginFrame, tsEndFrame, tsRenderObject;
		deviceContext->GetData(queryBeginFrame, &tsBeginFrame, sizeof(UINT64), 0);
		deviceContext->GetData(queryEndFrame, &tsEndFrame, sizeof(UINT64), 0);
		deviceContext->GetData(queryRenderObject, &tsRenderObject, sizeof(UINT64), 0);

		//Convert to real time
		float msRenderObject = float(tsRenderObject - tsBeginFrame) / float(tsDisjoint.Frequency) * 1000.0f;
		float msFrameTime = float(tsEndFrame - tsBeginFrame) / float(tsDisjoint.Frequency) * 1000.0f;

		//printf(string(to_string(msRenderObject) + "\n").c_str());

		//Get pipeline statistics
		D3D11_QUERY_DATA_PIPELINE_STATISTICS pipelineStatistics;
		deviceContext->GetData(queryPipelineStatistics, &pipelineStatistics, sizeof(pipelineStatistics), 0);

		printf(string(to_string(pipelineStatistics.CPrimitives) + "\n").c_str());

		//std::ofstream outputFile;
		//outputFile.open(filename, ios::out | ios::app);

		//outputFile << msRenderObject << endl;

		//outputFile.close();

	}
};
