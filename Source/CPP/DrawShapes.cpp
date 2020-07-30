// Copyright Chris Sixsmith 2020.

#include "DrawShapes.h"
#include "Alchemist.h"

void DrawConnectorArrow(Alchemist* Instance, Point Start, Point End)
{
	// Draw our first line, the actual body of the arrow.
	SDL_RenderDrawLine(Instance->GetRenderer(), Start.X, Start.Y, End.X, End.Y);

	double Sigma = atan2(End.Y - Start.Y, End.X - Start.X);

	double S1 = Sigma + (135 * (3.14 / 180));
	double S2 = Sigma - (135 * (3.14 / 180));

	double r = 20;
	
	Point P1((int)(r * cos(S1)), (int)(r * sin(S1)));
	Point P2((int)(r * cos(S2)), (int)(r * sin(S2)));

	P1 += End;
	P2 += End;
	
	SDL_RenderDrawLine(Instance->GetRenderer(), P1.X, P1.Y, End.X, End.Y);
	SDL_RenderDrawLine(Instance->GetRenderer(), P2.X, P2.Y, End.X, End.Y);
}

void DrawThickRectangle(Alchemist* Instance, SDL_Rect Rect, int Thickness)
{
	for(int i = 0; i < Thickness; i++)
	{
		SDL_RenderDrawRect(Instance->GetRenderer(), &Rect);

		Rect.x += 1;
		Rect.y += 1;
		Rect.w -= 2;
		Rect.h -= 2;
	}
}
