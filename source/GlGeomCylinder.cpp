/*
* GlGeomCylinder.cpp - Version 0.4 - January 28, 2019
*
* C++ class for rendering cylinders in Modern OpenGL.
*   A GlGeomCylinder object encapsulates a VAO, a VBO, and an EBO,
*   which can be used to render a cylinder.
*   The number of slices and stacks and rings can be varied.
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

#include "GlGeomCylinder.h"
#include "MathMisc.h"
#include "assert.h"


void GlGeomCylinder::Remesh(int slices, int stacks, int rings)
{
    if (slices == numSlices && stacks == numStacks && rings == numRings) {
        return;
    }
    numSlices = ClampRange(slices, 3, 255);
    numStacks = ClampRange(stacks, 1, 255);
    numRings = ClampRange(rings, 1, 255);

    VboEboLoaded = false;
}


void GlGeomCylinder::CalcVboAndEbo(float* VBOdataBuffer, unsigned int* EBOdataBuffer,
    int vertPosOffset, int vertNormalOffset, int vertTexCoordsOffset, unsigned int stride)
{
    assert(vertPosOffset >= 0 && stride > 0);
    bool calcNormals = (vertNormalOffset >= 0);       // Should normals be calculated?
    bool calcTexCoords = (vertTexCoordsOffset >= 0);  // Should texture coordinates be calculated?

    // VBO Data is laid out: top face vertices, then bottom face vertices, then side vertices

    // Set top and bottom center vertices
    SetDiscVerts(0.0, 0.0, 0, 0, VBOdataBuffer, vertPosOffset, vertNormalOffset, vertTexCoordsOffset, stride);
    int stopSlices = calcTexCoords ? numSlices : numSlices - 1;
    for (int i = 0; i <= stopSlices; i++) {
        // Handle a slice of vertices.
        // theta measures from the negative z-axis, counterclockwise viewed from above.
        float theta = ((float)(i%numSlices))*(float)PI2 / (float)(numSlices);
        float c = -cosf(theta);      // Negated values (start at negative z-axis)
        float s = -sinf(theta);
        if (i < numSlices) {
            // Top & bottom face vertices, positions and normals and texture coordinates
            for (int j = 1; j <= numRings; j++) {
                float radius = (float)j / (float)numRings;
                SetDiscVerts(s * radius, c * radius, i, j, VBOdataBuffer, vertPosOffset, vertNormalOffset, vertTexCoordsOffset, stride);
            }
        }
        float* basePtr = VBOdataBuffer + (2*GetNumVerticesDisk()+ i*(numStacks + 1))*stride;
        float sCoord = ((float)i) / (float)(numSlices);
        // Side vertices, positions and normals and texture coordinates
        for (int j = 0; j <= numStacks; j++, basePtr+=stride) {
            float* vPtr = basePtr + vertPosOffset;
            float tCoord = (float)j / (float)numStacks;
            *(vPtr++) = s;
            *(vPtr++) = -1.0f + 2.0f*tCoord;
            *vPtr = c;
            if (calcNormals) {
                float* nPtr = basePtr + vertNormalOffset;
                *(nPtr++) = s;
                *(nPtr++) = 0.0f;
                *nPtr = c;
            }
            if (calcTexCoords) {
                float* tcPtr = basePtr + vertTexCoordsOffset;
                *(tcPtr++) = sCoord;
                *tcPtr = tCoord;
            }
        }
    }

    // EBO data is also laid out as base, the top, then sides
    unsigned int* eboPtr = EBOdataBuffer;
    // Bottom 
    for (int i = 0; i < numSlices; i++) {
        int r = i*numRings + 1;
        int rightR = ((i+1)%numSlices)*numRings + 1;
        *(eboPtr++) = 0;
        *(eboPtr++) = rightR;
        *(eboPtr++) = r;
        for (int j = 0; j < numRings-1; j++) {
            *(eboPtr++) = r + j;
            *(eboPtr++) = rightR + j;
            *(eboPtr++) = rightR + j + 1;
 
            *(eboPtr++) = r + j;
            *(eboPtr++) = rightR + j + 1;
            *(eboPtr++) = r + j + 1;
        }
    }
    // Top 
    int delta = GetNumVerticesDisk();
    for (int i = 0; i < numSlices; i++) {
        int r = delta + i*numRings + 1;
        int leftR = delta + ((i + 1) % numSlices)*numRings + 1;
        *(eboPtr++) = delta;
        *(eboPtr++) = r;
        *(eboPtr++) = leftR;
        for (int j = 0; j < numRings-1; j++) {
            *(eboPtr++) = leftR + j;
            *(eboPtr++) = r + j;
            *(eboPtr++) = r + j + 1;

            *(eboPtr++) = leftR + j;
            *(eboPtr++) = r + j + 1;
            *(eboPtr++) = leftR + j + 1;
        }
    }
    // Side
    for (int i = 0; i < numSlices; i++) {
        int r = i*(numStacks + 1) + 2*delta;
        int ii = calcTexCoords ? (i + 1) : (i + 1) % numSlices;
        int rightR = ii*(numStacks + 1) + 2*delta;
        for (int j = 0; j < numStacks; j++) {
            *(eboPtr++) = rightR + j;
            *(eboPtr++) = r + j + 1;
            *(eboPtr++) = r + j;

            *(eboPtr++) = rightR + j;
            *(eboPtr++) = rightR + j + 1;
            *(eboPtr++) = r + j + 1;
        }
    }
}

void GlGeomCylinder::SetDiscVerts(float x, float z, int i, int j, float* VBOdataBuffer,
    int vertPosOffset, int vertNormalOffset, int vertTexCoordsOffset, int stride)
{
    // i is the slice number, j is the ring number.
    // j==0 means the center point.  In this case, i must equal 0. (Not checked)
    float* basePtrBottom = VBOdataBuffer + stride*(i*numRings + j);
    int delta = GetNumVerticesDisk()*stride;
    float* vPtrBottom = basePtrBottom + vertPosOffset;
    float* vPtrTop = vPtrBottom + delta;
    *(vPtrBottom++) = x;
    *(vPtrBottom++) = -1.0;
    *vPtrBottom = z;
    *(vPtrTop++) = x;
    *(vPtrTop++) = 1.0;
    *vPtrTop = z;
    if (vertNormalOffset>=0) {
        float* nPtrBottom = basePtrBottom + vertNormalOffset;
        float* nPtrTop = nPtrBottom + delta;
        *(nPtrBottom++) = 0.0;
        *(nPtrBottom++) = -1.0;
        *nPtrBottom = 0.0;
        *(nPtrTop++) = 0.0;
        *(nPtrTop++) = 1.0;
        *nPtrTop = 0.0;
    }
    if (vertTexCoordsOffset>=0) {
        float sCoord = 0.5f*(x + 1.0f);
        float tCoord = 0.5f*(-z + 1.0f);
        float* tcPtrBottom = basePtrBottom + vertTexCoordsOffset;
        float* tcPtrTop = tcPtrBottom + delta;
        *(tcPtrBottom++) = 1.0f - sCoord;
        *tcPtrBottom = tCoord;
        *(tcPtrTop++) = sCoord;
        *tcPtrTop = tCoord;
    }
}


#if !DONT_USE_OPENGL
void GlGeomCylinder::InitializeAttribLocations(
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
    VboEboLoaded = true;

    // Good practice to unbind things: helps with debugging if nothing else
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


// **********************************************
// This routine does the rendering.
// If the cylinder's VAO, VBO, EBO need to be loaded, it does this first.
// **********************************************

void GlGeomCylinder::Render()
{
    PreRender();

    glBindVertexArray(theVAO);
    glDrawElements(GL_TRIANGLES, GetNumElements() , GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);           // Good practice to unbind: helps with debugging if nothing else
}

void GlGeomCylinder::RenderTop()
{
    PreRender();

    glBindVertexArray(theVAO);
    int n = GetNumElementsDisk();
    glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);           // Good practice to unbind: helps with debugging if nothing else
}

void GlGeomCylinder::RenderBase()
{
    PreRender();

    glBindVertexArray(theVAO);
    int n = GetNumElementsDisk();
    glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, (void*)(n * sizeof(unsigned int)));
    glBindVertexArray(0);           // Good practice to unbind: helps with debugging if nothing else
}

void GlGeomCylinder::RenderSide()
{
    PreRender();

    glBindVertexArray(theVAO);
    int n = GetNumElementsDisk();
    glDrawElements(GL_TRIANGLES, GetNumElementsSide(), GL_UNSIGNED_INT, (void*)(2 * n * sizeof(unsigned int)));
    glBindVertexArray(0);           // Good practice to unbind: helps with debugging if nothing else
}

void GlGeomCylinder::PreRender()
{
    if (theVAO == 0) {
        assert(false && "GlGeomCylinder::InitializeAttribLocations must be called before rendering!");
    }
    if (!VboEboLoaded) {
        InitializeAttribLocations(posLoc, normalLoc, texcoordsLoc);
    }
}

#endif


GlGeomCylinder::~GlGeomCylinder()
{
#if !DONT_USE_OPENGL
    glDeleteBuffers(3, &theVAO);  // The three buffer id's are contigous in memory!
#endif  // !DONT_USE_OPENGL
}


