/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: main.cpp
Purpose: Main class cpp file that has the main while loop for running the project.
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_2
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/


#include "Game.h"

int main()
{
	Game game("GRAPHICS 3D");

	//main loop
	while (!game.GetWindowShouldClose())
	{
		//Update Input
		game.Update();
		game.Render();
	}

	return 0;
}