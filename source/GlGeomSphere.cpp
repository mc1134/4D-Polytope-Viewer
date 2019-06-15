/*
* GlGeomSphere.cpp - Version 0.5 - February 9, 2019
*
* C++ class for rendering spheres in Modern OpenGL.
*   A GlGeomSphere object encapsulates a VAO, VBO, and VEO,
*   which can be used to render a sphere.
*   The number of slices and stacks can be varied.
*
* Author: Sam Buss
*
* Software accompanying POSSIBLE SECOND EDITION TO the book
*		3D Computer Graphics: A Mathematical Introduction with OpenGL,
*		by S. Buss, Cambridge University Press, 2003.
*
* Software is "as-is" and carries no warranty.  It may be used without
*   restriction, but if you modify it, please change the filenames to
*   prevent confusion between different versions.
* Bug reports: Sam Buss, sbuss@ucsd.edu.
* Web page: http://math.ucsd.edu/~sbuss/MathCG2
*/

// Set this true if you only want to get vertex data without using OpenGL here.
#define DONT_USE_OPENGL false

#if !DONT_USE_OPENGL
// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#endif  // DONT_USE_OPENGL

#include "LinearR3.h"
#include "MathMisc.h"
#include "assert.h"

#include "GlGeomSphere.h"

void GlGeomSphere::Remesh(int slices, int stacks)
{
    if (slices == numSlices && stacks == numStacks) {
        return;
    }

    numSlices = ClampRange(slices, 3, 255);
    numStacks = ClampRange(stacks, 3, 255);
}

void GlGeomSphere::CalcVboAndEbo(float* VBOdataBuffer, unsigned int* EBOdataBuffer,
    int vertPosOffset, int vertNormalOffset, int vertTexCoordsOffset, unsigned int stride)
{
    assert(vertPosOffset >= 0 && stride>0);
    bool calcNormals = (vertNormalOffset >= 0);       // Should normals be calculated?
    bool calcTexCoords = (vertTexCoordsOffset >= 0);  // Should texture coordinates be calculated?

     for (int i = 0; i <= numSlices; i++) {
        // Handle a slice of vertices.
        // theta measures from the (negative-z)-axis, going counterclockwise viewed from above.
        float theta = ((float)(i%numSlices))*(float)PI2 / (float)(numSlices);
        float sTexCd = ((float)i) / (float)numSlices;     // s texture coordinate
        float costheta = cos(theta);
        float sintheta = sinf(theta);
        for (int j = 0; j <= numStacks; j++) {
            unsigned int vertNumber;
            if (!GetVertexNumber(i, j, calcTexCoords, &vertNumber)) {
                continue;       // North or South pole -- duplicate not needed
            }
            // phi measures from the (postive-y)-axis
            float tTexCd = ((float)j) / (float)(numStacks); // t texture coordinate
            float phi = tTexCd * (float)PI;
            float cosphi = cosf(phi);
            float sinphi = (j < numStacks) ? sinf(phi) : 0.0f;
            float x = -sintheta*sinphi;       // Position, x coordinate            
            float y = -cosphi;                // Position, y coordinate
            float z = -costheta*sinphi;       // Position, z coordinate
            float* basePtr = VBOdataBuffer + stride*vertNumber;
            float* vPtr = basePtr + vertPosOffset;
            *vPtr = x;
            *(vPtr + 1) = y;
            *(vPtr + 2) = z;
            if (calcNormals) {
                float* nPtr = basePtr + vertNormalOffset;
                *nPtr = x;
                *(nPtr + 1) = y;
                *(nPtr + 2) = z;
            }
            if (calcTexCoords) {
                float* tcPtr = basePtr + vertTexCoordsOffset;
                *tcPtr = (j != 0 && j != numStacks) ? sTexCd : 0.5f;  // s=0.5 at the poles
                *(tcPtr + 1) = tTexCd;
            }
        }
     }
     
     // Calculate elements (vertex indices) suitable for putting into an EBO
     //      in GL_TRIANGLES mode.
     unsigned int* toEbo = EBOdataBuffer;
     for (int i = 0; i < numSlices; i++) {
         // Handle a slice of vertices.
         unsigned int leftIdxOld, rightIdxOld;
         GetVertexNumber(i, 0, calcTexCoords, &leftIdxOld);
         GetVertexNumber(i + 1, 1, calcTexCoords, &rightIdxOld);
         for (int j = 0; j < numStacks-1; j++) {
             unsigned int leftIdxNew, rightIdxNew;
             GetVertexNumber(i, j + 1, calcTexCoords, &leftIdxNew);
             GetVertexNumber(i + 1, j + 2, calcTexCoords, &rightIdxNew);
             *(toEbo++) = leftIdxOld;
             *(toEbo++) = rightIdxOld;
             *(toEbo++) = leftIdxNew;

             *(toEbo++) = leftIdxNew;
             *(toEbo++) = rightIdxOld;
             *(toEbo++) = rightIdxNew;

             leftIdxOld = leftIdxNew;
             rightIdxOld = rightIdxNew;
         }
     }
     assert(toEbo - EBOdataBuffer == GetNumElements());
}

// Calculate the vertex number for the vertex on slice i and stack j.
// Returns false if this is a duplicate of the south or north pole.
bool GlGeomSphere::GetVertexNumber(int i, int j, bool calcTexCoords, unsigned int* retVertNum)
{
    if (j == 0) {
        *retVertNum = 0;    // South pole
        return (i == 0);
    }
    if (j == numStacks) {
        *retVertNum = 1;    // North pole
        return (i == 0);
    }
    int ii = calcTexCoords ? i : (i%numSlices);
    *retVertNum = (numStacks - 1)*ii + j + 1;
    return true;
}


#if !DONT_USE_OPENGL
void GlGeomSphere::InitializeAttribLocations(
	unsigned int pos_loc, unsigned int normal_loc, unsigned int texcoords_loc)
{
	posLoc = pos_loc;
	normalLoc = normal_loc;
	texcoordsLoc = texcoords_loc;

 	// Generate Vertex Array Object and Buffer Objects, not already done.
	if (theVAO == 0) {
		glGenVertexArrays(1, &theVAO);
		glGenBuffers(1, &theVBO);
		glGenBuffers(1, &theEBO);
	}

	// Link the VBO and EBO to the VAO, and request OpenGL to
	//   allocate memory for them.
	glBindVertexArray(theVAO);
	glBindBuffer(GL_ARRAY_BUFFER, theVBO);
    int numVertices = UseTexCoords() ? GetNumVerticesTexCoords() : GetNumVerticesNoTexCoords();
	glBufferData(GL_ARRAY_BUFFER, StrideVal() * numVertices * sizeof(float), 0, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetNumElements() * sizeof(unsigned int), 0, GL_STATIC_DRAW);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, StrideVal() * sizeof(float), (void*)0);
	glEnableVertexAttribArray(posLoc);
    int normalOffset = -1;
	if (UseNormals()) {
        normalOffset = NormalOffset();
		glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, StrideVal() * sizeof(float), 
                              (void*)(normalOffset * sizeof(float)));
		glEnableVertexAttribArray(normalLoc);
	}
    int tcOffset = -1;
	if (UseTexCoords()) {
        tcOffset = TexOffset();
		glVertexAttribPointer(texcoordsLoc, 2, GL_FLOAT, GL_FALSE, StrideVal() * sizeof(float), 
                              (void*)(tcOffset * sizeof(float)));
		glEnableVertexAttribArray(texcoordsLoc);
	}

	// Calculate the buffer data - map and the unmap the two buffers.
    float* VBOdata = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    unsigned int* EBOdata = (unsigned int*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    CalcVboAndEbo(VBOdata, EBOdata, 0, normalOffset, tcOffset, StrideVal());
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    loadedSlices = numSlices;
    loadedStacks = numStacks;

    // Good practice to unbind things: helps with debugging if nothing else
    glBindVertexArray(0); 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool GlGeomSphere::Prerender() {
    if (theVAO == 0) {
        assert(false && "GlGeomSphere::InitializeAttribLocations must be called before rendering!");
        return false;
    }
    if (loadedSlices != numSlices || loadedStacks != numStacks) {
        InitializeAttribLocations(posLoc, normalLoc, texcoordsLoc);
    }
    return true;
}

// **********************************************
// This routine does the rendering.
// If the sphere's VBO and EBO data need to be calculated, it does this first.
// **********************************************
void GlGeomSphere::Render()
{
    Prerender();
    glBindVertexArray(theVAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)GetNumElements(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);           // Good practice to unbind: helps with debugging if nothing else
}

// **********************************************
// This routine renders the i-th slice.
// If the sphere's VBO and EBO data need to be calculated, it does this first.
// **********************************************
void GlGeomSphere::RenderSlice(int i)
{
    assert(i >= 0 && i < numSlices);
    Prerender();

    glBindVertexArray(theVAO);
    GLsizei sliceLen = GetNumElementsInSlice();
    glDrawElements(GL_TRIANGLES, sliceLen, GL_UNSIGNED_INT, (void*)(i*sliceLen*sizeof(unsigned int)));
    glBindVertexArray(0);           // Good practice to unbind: helps with debugging if nothing else
}

// **********************************************
// This routine renders a single horizontal stack as a triangle strip.
// If the sphere's VBO and EBO data need to be calculated, it does this first.
//   Not efficient for reuse: Recalculates the EBO data every time.
//   Generates a new EBO everytime
//  j can range from 0 to numStacks. At the two extremes the bottom and top
//     fans are rendered as triangle strips with degenerate triangles.
// **********************************************
void GlGeomSphere::RenderStack(int j)
{
    assert(j >= 0 && j < numStacks);
    Prerender();

    unsigned int* stackElts = new unsigned int[(numSlices + 1) * 2];
    unsigned int* toElt = stackElts;
    for (int i = 0; i <= numSlices; i++) {
        GetVertexNumber(i, j+1, UseTexCoords(), toElt++);
        GetVertexNumber(i, j, UseTexCoords(), toElt++);
    }

    unsigned int tempEBO;
    glGenBuffers(1, &tempEBO);
    glBindVertexArray(theVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (numSlices + 1) * 2 * sizeof(unsigned int), stackElts, GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLE_STRIP, (numSlices + 1) * 2, GL_UNSIGNED_INT, 0);

    delete[] stackElts;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theEBO);  // Restore the main EBO (The VAO maintains its knowledge of this)
    glDeleteBuffers(1, &tempEBO);
    glBindVertexArray(0);

}

// **********************************************
// This routine renders the triangle fan around the North Pole.
// If the sphere's VBO and EBO data need to be calculated, it does this first.
//   Not efficient for reuse: Recalculates the EBO data every time.
//   Generates a new EBO everytime
// **********************************************
void GlGeomSphere::RenderNorthPoleFan() {
    Prerender();

    unsigned int* poleElts = new unsigned int[numSlices + 2];
    unsigned int* toElt = poleElts;
    GetVertexNumber( 0, numStacks, UseTexCoords(), toElt++ ); // North pole is the center of the triangle fan
    for (int i = 0; i <= numSlices; i++) {
        GetVertexNumber(i, numStacks - 1, UseTexCoords(), toElt++);
    }

    unsigned int tempEBO;
    glGenBuffers(1, &tempEBO);
    glBindVertexArray(theVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (numSlices+2) * sizeof(unsigned int), poleElts, GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLE_FAN, numSlices+2, GL_UNSIGNED_INT, 0);

    delete[] poleElts;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theEBO);  // Restore the main EBO (The VAO maintains its knowledge of this)
    glDeleteBuffers(1, &tempEBO);
    glBindVertexArray(0);
}


#endif  // !DONT_USE_OPENGL

GlGeomSphere::~GlGeomSphere()
{
#if !DONT_USE_OPENGL
    glDeleteBuffers(3, &theVAO);  // The three buffer id's are contigous in memory!
#endif  // !DONT_USE_OPENGL
}


