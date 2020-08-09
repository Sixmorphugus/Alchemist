// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"

// Save me from boilerplate
#define IMPLEMENT_POINT_ARITHMETIC(Op) \
Point operator Op(const Point& RHS) const \
{ \
	Point Out = *this; \
	Out.X Op ## = RHS.X; \
	Out.Y Op ## = RHS.Y; \
	return Out; \
} \
Point& operator Op ## =(const Point& RHS) \
{ \
	*this = *this Op RHS; \
	return *this; \
} \
Point operator Op(int RHS) const \
{ \
	Point Out = *this; \
	Out.X Op ## = RHS; \
	Out.Y Op ## = RHS; \
	return Out; \
} \
Point& operator Op ## =(int RHS) \
{ \
	*this = *this Op RHS; \
	return *this; \
}

struct Point
{
	int X, Y;
	
	Point()
		: X(0), Y(0)
	{}

	Point(int XIn, int YIn)
		: X(XIn), Y(YIn)
	{}

	Point(int In)
		: X(In), Y(In)
	{}

	bool operator==(const Point& Other) const
	{
		return X == Other.X && Y == Other.Y;
	}

	bool operator!=(const Point& Other) const
	{
		return X != Other.X || Y != Other.Y;
	}

	Point NormaliseToLength(double Length) const
	{
		Point Out;

		double Divisor = sqrt((double)((X * X) + (Y * Y)));
		
		Out.X = (int)(((double)X / Divisor) * Length);
		Out.Y = (int)(((double)Y / Divisor) * Length);

		return Out;
	}

	bool IsInRectangle(const SDL_Rect& Rect) const
	{
		return X >= Rect.x && Y >= Rect.y && X < Rect.x + Rect.w && Y < Rect.y + Rect.h;
	}
	
	IMPLEMENT_POINT_ARITHMETIC(+);
	IMPLEMENT_POINT_ARITHMETIC(-);
	IMPLEMENT_POINT_ARITHMETIC(*);
	IMPLEMENT_POINT_ARITHMETIC(/);
	IMPLEMENT_POINT_ARITHMETIC(%);
};

// custom specialization of std::hash injected in namespace std
namespace std
{
    template<> struct hash<Point>
    {
        std::size_t operator()(Point const& ToHash) const noexcept
        {
            std::size_t h1 = std::hash<int>{}(ToHash.X);
            std::size_t h2 = std::hash<int>{}(ToHash.Y);
            return h1 ^ (h2 << 1);
        }
    };
}

using Size = Point;