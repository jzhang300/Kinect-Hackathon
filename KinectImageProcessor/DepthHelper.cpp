//------------------------------------------------------------------------------
// <copyright file="DepthHelper.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// DepthHelper.cpp
#include "pch.h"
#include "DepthHelper.h"
#include <robuffer.h>

using namespace KinectImageProcessor;
using namespace Platform;
using namespace Windows::Storage::Streams;

DepthHelper::DepthHelper()
{
}

bool DepthHelper::CreateImagePixelDataFromArray(
    _In_ const Platform::Array<UINT16>^ frameArray,
    _In_ Platform::WriteOnlyArray<byte>^ pixelArray)
{
    if (frameArray == nullptr || pixelArray == nullptr)
    {
        return false;
    }

    CopyDepthBytes(reinterpret_cast<byte*>(frameArray->Data), frameArray->Length, pixelArray->Data, pixelArray->Length);

    return true;
}

void DepthHelper::CopyDepthBytes(_In_reads_(cbSrcSize) byte* puiSource, int cbSrcSize, _Out_writes_all_(cbDestSize) byte* pbDest, int cbDestSize)
{
	const int MapDepthToByte = 512/256;
	const int MapDepthToByte2 = 8000 / 256;

    UINT16* pSrc = reinterpret_cast<UINT16*>(puiSource);
    RGBQUAD* pRGBX = reinterpret_cast<RGBQUAD*>(pbDest);

    // end pixel is start + width * height - 1
    const UINT16* pBufferEnd = pSrc + cbSrcSize;

    while (pSrc < pBufferEnd)
    {
        UINT16 depth = *pSrc;

		// To convert to a byte, we're mapping the depth value to the byte range.
		
		//Close STuff is BLUE
		if (depth < 1200 && depth >= 500){
			byte intensity = static_cast<byte>(depth / MapDepthToByte2);
			pRGBX->rgbRed = 255 - intensity;
			pRGBX->rgbGreen = 0;
			pRGBX->rgbBlue = 0;
			pRGBX->rgbReserved = 255;
		}
		// super close stuff is white
		else if (depth < 500) {
			byte intensity = static_cast<byte>(depth / MapDepthToByte2);
			pRGBX->rgbRed = 255;
			pRGBX->rgbGreen = 255;
			pRGBX->rgbBlue = 255;
			pRGBX->rgbReserved = 255;
		}
		//Medium stuff is red
		else if(depth > 1000 && depth < 3000){
			byte intensity = static_cast<byte>(depth / MapDepthToByte2);
			pRGBX->rgbRed = 0;
			pRGBX->rgbGreen = 0;
			pRGBX->rgbBlue = 255-intensity;
			pRGBX->rgbReserved = 255;

		}
		//All Else is white
		else{
			byte intensity = static_cast<byte>(depth / MapDepthToByte);
			pRGBX->rgbRed = 255;
			pRGBX->rgbGreen = 255;
			pRGBX->rgbBlue = 255;
			pRGBX->rgbReserved = 255;

		}

        // adjust pointers
        ++pRGBX;
        ++pSrc;
    }
}