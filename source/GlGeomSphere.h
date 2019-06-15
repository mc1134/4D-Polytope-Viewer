/*
* GlGeomSphere.h - Version 0.5 - February 9, 2019
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

#pragma once
#ifndef GLGEOM_SPHERE_H
#define GLGEOM_SPHERE_H

// GlGeomSphere
//     Generates vertices, normals, and texture coordinates for a sphere.
//     Sphere formed of "slices" and "stacks"
//     "Slices" means the number of vertical wedges.
//     "Stacks" means the number of horizontal pieces.
// Supports either of the modes:
//    (1) Allocating and loading a VAO, VBO, and EBO, and doing the rendering.
//    (2) Loading an external VBO with vertex data, and an external EBO with
//        elements. This requires the external program to allocate and load the
//        VAO, VBO and EBO; to set the attribute information in the VAO,
//        and to issue the glDrawElements(GL_TRIANGLES,...) commands.
// Mode (1) is easiest when rendering simple spheres as independent objects.
// Mode (2) allows more sophisticated handling of triangle data.
// For both modes:
//          First call either the constructor GlGeomSphere() or Remesh() 
//             to set the numbers of slices and stacks.
// For Mode (1), then call the routines:
//          InitializeAttribLocations() - gives locations in the VBO buffer for the shader program
//          Render() - gives the glDrawElements commands for the sphere using the VAO, VBO and EBO.
// For Mode (2), then call the routines
//          CalcVboAndEbo()

class GlGeomSphere
{
public:
    GlGeomSphere() : GlGeomSphere(6, 6) {}
    GlGeomSphere(int slices, int stacks);
    ~GlGeomSphere();

	// Remesh: re-mesh to change the number slices and stacks.
	// Mode (1): Can be called either before or after InitAttribLocations(), but it is
	//    more efficient if Remesh() is called first, or if the constructor sets the mesh resolution.
    // Mode (2): Call before CalcVboAndEbo (or use the constructor to specify slices and stacks).
	void Remesh(int slices, int stacks);

	// Allocate the VAO, VBO, and EBO.
	// Set up info about the Vertex Attribute Locations
	// This must be called before render is first called.
    // First parameter is the location for the vertex position vector in the shader program.
    // Second parameter is the location for the vertex normal vector in the shader program.
    // Third parameter is the location for the vertex 2D texture coordinates in the shader program.
    // The second and third parameters are optional.
    void InitializeAttribLocations(
		unsigned int pos_loc, unsigned int normal_loc = UINT_MAX, unsigned int texcoords_loc = UINT_MAX);

	void Render();

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
    //    This must be preallocated before the call to CalcVboAndEbo of course.
    //    Numbers are different since texture coordinates must be assigned differently
    //        to some vertices depending on which triangle they appear in.
    void CalcVboAndEbo( float* VBOdataBuffer, unsigned int* EBOdataBuffer,
                        int vertPosOffset, int vertNormalOffset, int vertTexCoordsOffset, 
                        unsigned int stride);
    int GetNumElements() const { return 6*numSlices*(numStacks - 1); }
    int GetNumVerticesTexCoords() const { return (numSlices + 1)*(numStacks - 1) + 2; }
    int GetNumVerticesNoTexCoords() const { return numSlices*(numStacks - 1) + 2; }

    // Mode (1) buffers.
    unsigned int GetVAO() const { return theVAO; }
    unsigned int GetVBO() const { return theVBO; }
    unsigned int GetEBO() const { return theEBO; }

    int GetNumSlices() const { return numSlices; }
    int GetNumStacks() const { return numStacks; }

    // Some specialized render routines for rendering portions of the sphere
    // Selectively render a slice or a stack or a north pole triangle fan
    // Slice numbers i rangle from 0 to numSlices-1.
    // Stack numbers j are allowed to range from 1 to numStacks-2.
    void RenderSlice(int i);    // Renders the i-th slice as a triangle strip
    void RenderStack(int j);    // Renders the j-th stack as a triangle strip
    void RenderNorthPoleFan();  // Renders the north pole stack as a triangle fan.
    int GetNumElementsInSlice() const { return 6*(numStacks - 1); }

	// Disable all copy and assignment operators.
	// A GlGeomSphere can be allocated as a global or static variable, or with new.
    //     If you need to pass it to/from a function, use references or pointers
    //     and be sure that there are no implicit copy or assignment operations!
    GlGeomSphere(const GlGeomSphere&) = delete;
	GlGeomSphere& operator=(const GlGeomSphere&) = delete;
	GlGeomSphere(GlGeomSphere&&) = delete;
	GlGeomSphere& operator=(GlGeomSphere&&) = delete;

private:
    int numSlices;              // Number of radial slices
    int numStacks;              // Number of levels separating the north pole from the south pole.

    // Variables and methods below are for Mode (1) only.
    unsigned int theVAO = 0;        // Vertex Array Object
    unsigned int theVBO = 0;        // Vertex Buffer Object
    unsigned int theEBO = 0;        // Element Buffer Object;
    int loadedSlices = 0;           // Number of slices stored in VBO/EBO
    int loadedStacks = 0;           // Number of stacks stored in VBO/EBO

    // Stride value, and offset values for the data in the VBO (Mode (1) only.)
    // These take into account whether normals and texture coordinates are used.
    bool UseNormals() const { return normalLoc != UINT_MAX; }
    bool UseTexCoords() const { return texcoordsLoc != UINT_MAX; }
    int StrideVal() const {
        return 3 + (UseNormals() ? 3 : 0) + (UseTexCoords() ? 2 : 0);
    }
    int NormalOffset() const { return 3; }
    int TexOffset() const { return 3 + (UseNormals() ? 3 : 0); }

    unsigned int posLoc;            // location of vertex position x,y,z data in the shader program
    unsigned int normalLoc;         // location of vertex normal data in the shader program
    unsigned int texcoordsLoc;      // location of s,t texture coordinates in the shader program.

private:
    bool GetVertexNumber(int i, int j, bool calcTexCoords, unsigned int* retVertNum);
    bool Prerender();
};

inline GlGeomSphere::GlGeomSphere(int slices, int stacks)
{
	numSlices = slices;
	numStacks = stacks;
    loadedSlices = 0;
    loadedStacks = 0;
}

#endif  // GLGEOM_SPHERE_H
