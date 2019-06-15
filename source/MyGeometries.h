#pragma once

// 
// MyGeometries.h   ---  Header file for MyGeometries.cpp.
// 
//   Sets up and renders 
//     - the ground plane, and
//     - the surface of rotation
//   for the Math 155A project #4.
//
//

extern double shapeRadius;
extern double shapeMin;
extern double shapeMax;
extern double shapeScale;

//
// Function Prototypes
//
void MySetupSurfaces();                // Called once, before rendering begins.
void SetupForTextures();               // Loads textures, sets Phong material
void MyRemeshGeometries();             // Called when mesh changes, must update resolutions.

void MyRenderGeometries();            // Called to render the two surfaces



