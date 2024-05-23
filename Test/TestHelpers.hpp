// MIT License
//
// Copyright(c) 2024 Mihail Mladenov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#pragma once

#include "../MinTTF.hpp"

using namespace MTTF;

#include <fstream>
#include <string>

inline auto WritePGM(const GrayScaleSurface& surf)
{
	std::fstream ofs("TestOpenStans.pgm", std::ios::binary | std::ios::out | std::ios::trunc);
	ofs << "P5\n" << surf.width << ' ' << surf.height << "\n255\n";
	ofs.write((const C*)surf.data.data(), surf.width * surf.height);
}

inline auto WriteToSVG(const GlyphData& glyphData)
{
	std::fstream ofs("glyphData.svg", std::ios::binary | std::ios::out | std::ios::trunc);
	auto width = glyphData.boundingBoxDiagonal.endPoint.x - glyphData.boundingBoxDiagonal.startPoint.x;
	auto height = glyphData.boundingBoxDiagonal.endPoint.y - glyphData.boundingBoxDiagonal.startPoint.y;

	Str svg = "<svg xmlns = \"http://www.w3.org/2000/svg\" width =\"";
	svg += std::to_string(width);
	svg += "\" height=\"" + std::to_string(height) + "\"";
	svg += " viewBox=\" ";
	svg += std::to_string(glyphData.boundingBoxDiagonal.startPoint.x) + " ";
	svg += std::to_string(glyphData.boundingBoxDiagonal.startPoint.y) + " ";
	svg += std::to_string(glyphData.boundingBoxDiagonal.endPoint.x) + " ";
	svg += std::to_string(glyphData.boundingBoxDiagonal.endPoint.y) + " ";
	svg += "\">\n";

	for (auto& component : glyphData.components)
	{
		if (HoldsAlternative<QuadraticBezierCurve>(component))
		{

		}
		else
		{

		}
	}

	ofs.write(svg.data(), svg.size());
}