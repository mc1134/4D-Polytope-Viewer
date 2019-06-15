/*
 * GlGeomCylinder.h - Version 0.4 - February 23, 2019
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

#ifndef GLGEOM_CYLINDER_H
#define GLGEOM_CYLINDER_H

#include <limits.h>

// GlGeomCylinder
//     Generates vertices, normals, and texture coodinates for a cylinder.
//     Cylinder formed of "slices" and "stacks" and "rings"
//     Cylinder has radius 1, height 2 and is centered at the origin.
//     The central axis is the y-axis. Texture coord (0.5,0.5) is on the z-axis.
// Supports either of the modes:
//    (1) Allocating and loading a VAO, VBO, and EBO, and doing the rendering.
//    (2) Loading an external VBO with vertex data, and an external EBO with
//        elements. This requires the calling program to allocate and load the
//        VAO, VBO and EBO; to set the attribute information in the VAO,
//        and to issue the glDrawElements(GL_TRIANGLES,...) commands.
// Mode (1) is easiest when rendering simple cylinders as independent objects.
// Mode (2) allows more sophisticated handling of triangle data.
// For both modes:
//          First call either the constructor or Remesh() to set the numbers 
//          of slices, stacks and rings.
// For Mode (1), then call the routines:
//          InitializeAttribLocations() - gives locations in the VBO buffer for the shader program
//          Render() - gives the glDrawElements commands for the cylindeer using the VAO, VBO and EBO.
// For Mode (2), then call the routines
//          CalcVboAndEbo()


class GlGeomCylinder
{
public:
    GlGeomCylinder() : GlGeomCylinder(3, 1, 1) {}
    GlGeomCylinder(int slices, int stacks=1, int rings=1);
    ~GlGeomCylinder();

	// Re-mesh to change the number slices and stacks and rings.
    // Mode (1): Can be called either before or after InitAttribLocations(), but it is
    //    more efficient if Remesh() is called first, or if the constructor sets the mesh resolution.
    // Mode (2): Call before CalcVboAndEbo (or use the constructor to specify mesh resolution).
    void Remesh(int slices, int stacks, int rings);

	// Allocate the VAO, VBO, and EBO.
	// Set up info about the Vertex Attribute Locations
	// This must be called before Render() is first called.
    // First parameter is the location for the vertex position vector in the shader program.
    // Second parameter is the location for the vertex normal vector in the shader program.
    // Third parameter is the location for the vertex 2D texture coordinates in the shader program.
    // The second and third parameters are optional.
    void InitializeAttribLocations(
		unsigned int pos_loc, unsigned int normal_loc = UINT_MAX, unsigned int texcoords_loc = UINT_MAX);

    void Render();          // Render: renders entire cylinder
    void RenderTop();
    void RenderBase();
    void RenderSide();

    // Mode (2) 
    // CalcVboAndEbo- return all VBO vertex information, and EBO elements for GL_TRIANGLES drawing.
    // Inputs:
    //   VBOdataBuffer, EBOdataBuffer are filled with the vertex info and elements for GL_TRIANGLES drawing
    //   vertPosOffset and stride control where the vertex positions are placed.
    //   vertNormalOffset and stride control where the vertex normals are placed.
    //   vertTexCoordsOffset and stride control where the texture coordinates are placed.
    //   Offset and stride values are **integers** (not bytes), measuring offsets in terms of floats.
    //   Use "-1" for the offset for any value which should be omitted.
    //   For the (unit) sphere, the normals are always exactly equal to the positions.
    // Output: 
    //   Data VBO and EBO data is calculated and loaded into the two buffers VBOdataBuffer and EBOdataBuffer.
    // Typical usages are:
    //   CalcVboAndEbo( vboPtr, eboPtr, 0, -1, -1, 3); // positions only, tightly packed
    //   CalcVboAndEbo( vboPtr, eboPtr, 0, -1, 3, 5); // positions, then (s,t) texture coords, tightly packed
    //   CalcVboAndEbo( vboPtr, eboPtr, 0, 3, 6, 8); // positions, normals, then (s,t) texture coords, tightly packed
    // Use GetNumElements() and GetNumVerticesTexCoords() and GetNumVerticesNoTexCoords()
    //    to determine the amount of data that will returned by CalcVboAndEbo.
    //    The calling program must preallocate this before the call to CalcVboAndEbo of course.
    //    Numbers are different since texture coordinates must be assigned differently
    //        to some vertices depending on which triangle they appear in.
    void CalcVboAndEbo(float* VBOdataBuffer, unsigned int* EBOdataBuffer,
        int vertPosOffset, int vertNormalOffset, int vertTexCoordsOffset,
        unsigned int stride);
    int GetNumElements() const { return 2*GetNumElementsDisk() + GetNumElementsSide(); }
    int GetNumVerticesTexCoords() const { return 2*GetNumVerticesDisk() + GetNumVerticesSideTexCoords(); }
    int GetNumVerticesNoTexCoords() const { return 2*GetNumVerticesDisk() + GetNumVerticesSideNoTexCoords(); }
    
    // "Disk" methods are for the bottom or top circular face.  "Side" for the cylinder's side
    int GetNumElementsDisk() const { return 3*(2*numRings - 1)*numSlices; }
    int GetNumVerticesDisk() const { return 1 + numRings*numSlices; }
    int GetNumElementsSide() const { return 6*numStacks*numSlices; }
    int GetNumVerticesSideTexCoords() const { return (numStacks + 1)*(numSlices + 1); }
    int GetNumVerticesSideNoTexCoords() const { return (numStacks + 1)*numSlices; }



    int GetVAO() const { return theVAO; }
    int GetVBO() const { return theVBO; }
    int GetEBO() const { return theEBO; }

    int GetNumSlices() const { return numSlices; }
    int GetNumStacks() const { return numStacks; }
    int GetNumRings() const { return numRings; }

	// Disable all copy and assignment operators.
	// A GlGeomCylinder can be allocated as a global or static variable, or with new.
	//     If you need to pass it to/from a function, use references or pointers
    //     and be sure that there are no implicit copy or assignment operations!
    GlGeomCylinder(const GlGeomCylinder&) = delete;
    GlGeomCylinder& operator=(const GlGeomCylinder&) = delete;
    GlGeomCylinder(GlGeomCylinder&&) = delete;
    GlGeomCylinder& operator=(GlGeomCylinder&&) = delete;

private:
    int numSlices;          // Number of radial slices (like cake slices
    int numStacks;          // Number of stacks between the two end faces
    int numRings;           // Number of concentric rings on two end faces


private: 
    unsigned int theVAO = 0;        // Vertex Array Object
    unsigned int theVBO = 0;        // Vertex Buffer Object
    unsigned int theEBO = 0;        // Element Buffer Object;
    bool VboEboLoaded = false;

    void PreRender();

    // Stride value, and offset values for the data in the VBO.
    //   These take into account whether normals and texture coordinates are used.
    bool UseNormals() const { return normalLoc != UINT_MAX; }
    bool UseTexCoords() const { return texcoordsLoc != UINT_MAX; }
    int StrideVal() const {
        return 3 + (UseNormals() ? 3 : 0) + (UseTexCoords() ? 2 : 0);
    }
    int NormalOffset() const { return 3; }
    int TexOffset() const { return 3 + (UseNormals() ? 3 : 0); }
    void SetDiscVerts(float x, float z, int i, int j, float* VBOdataBuffer,
        int vertPosOffset, int vertNormalOffset, int vertTexCoordsOffset, int stride);
 
	unsigned int posLoc;            // location of vertex position x,y,z data in the shader program
	unsigned int normalLoc;         // location of vertex normal data in the shader program
	unsigned int texcoordsLoc;      // location of s,t texture coordinates in the shader program.
};

inline GlGeomCylinder::GlGeomCylinder(int slices, int stacks, int rings)
{
	numSlices = slices;
	numStacks = stacks;
    numRings = rings;
}

#endif  // GLGEOM_CYLINDER_H
