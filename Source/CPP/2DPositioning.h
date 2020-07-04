// Copyright Chris Sixsmith 2020.

#pragma once

struct Point
{
	int X, Y;
	
	Point()
		: X(0), Y(0)
	{}

	Point(int XIn, int YIn)
		: X(XIn), Y(YIn)
	{}
};

using Size = Point;