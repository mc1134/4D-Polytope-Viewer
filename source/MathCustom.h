#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

// calculates the outer product of vec1 and vec2, which have n1 and n2 number of elements respectively
float * outerProduct(float * vec1, int n1, float * vec2, int n2);