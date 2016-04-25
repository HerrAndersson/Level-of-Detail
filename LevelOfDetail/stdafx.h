//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently.

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>

#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <d3d11_3.h>
#include <DirectXMath.h>
#include <pix.h>
#include <d2d1_3.h>
#include <dwrite.h>

#include <wrl.h>
#include <vector>
#include <shellapi.h>
#include <stdexcept>
#include <string>

#include "Math.h"

using Microsoft::WRL::ComPtr;
using std::string;
using std::wstring;

static const int SCREEN_WIDTH = 1600;
static const int SCREEN_HEIGHT = 900;
static const string ASSET_PATH = "Assets/";
static const string MODEL_PATH = ASSET_PATH + "Models/";
static const string TEXTURE_PATH = ASSET_PATH + "Textures/";
static const float F_EPSILON = std::numeric_limits<float>::epsilon();
static const float F_MAX = (std::numeric_limits<float>::max)();
static const int LOD_LEVELS[] = { 5, 15, 25, 35, 45 };
static const float BLEND_TIME = 1.0f; //Seconds

#define SAFE_RELEASE(x) if( x ) { (x)->Release(); (x) = nullptr; }
#define SAFE_DELETE(x) if( x ) { delete x; (x) = nullptr; }