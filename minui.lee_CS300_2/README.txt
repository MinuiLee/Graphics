/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: README.txt
Purpose: Providing information on project
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_2
Author: Minui Lee, minui.lee, 180002020
Creation date: Nov 1, 2020
End Header --------------------------------------------------------*/ 

a. How to use parts of your user interface that is NOT specified in the assignment description. 
	Once the program is running, user can interact with ImGui using mouse. 
	Clicking and dragging will interact with program's properties.
	Press W, A, S and D to move around the camera (only X and Z axis).
	Press ESC to exit the program.

b. Any assumption that you make on how to use the application that, if violated, might cause
the application to fail.

c. Which part of the assignment has been completed?
	I beilieve all the parts that are required have been completed.

d. Which part of the assignment has NOT been completed (not done, not working, etc.) and
explanation on why those parts are not completed?

e. Where the relevant source codes (both C++ and shaders) for the assignment are located.
Specify the file path (folder name), file name, function name (or line number).
	Source code directory: minui.lee_CS300_1\Graphics3D
	All codes are relevant.

f. Which machine did you test your application on.
	Windows10, GeForce GTX 1660 Ti, 4.6 NVIDIA 451.67

g. The number of hours you spent on the assignment, on a weekly basis
	2 weeks

h. Any other useful information pertaining to the application 


Texture mapping calculation comparison between in CPU and GPU:

Cylindrical: Looks about the same, but the texture mapped from CPU calculation has a wiggly line on the boundaries.
Spherical: Looks about the same, but the texture mapped from CPU calculation has a wiggly line on the boundaries.
Planar(6-sided): Intersections between side seems off when calculating UV values in CPU.

When interpolating UV values of points that are lying on the boundaries, one end has a value close to 1, and the other end has 0. The values are supposed to be 1 and close to 1, not 0, and close to 1. This causes the texture mapping to have the wiggly line on the boundaries