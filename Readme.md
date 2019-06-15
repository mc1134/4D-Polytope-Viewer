# 4D Polytope Viewer

## (continuation of Final Project for MATH 155A WI19)

Many thanks to the aforementioned class and its instructors (Prof. Sam Buss and TAs Jonathan Conder and Nicholas Sieger) for providing much of the source code as well as teaching the class. Here is a [link](https://www.math.ucsd.edu/~sbuss/CourseWeb/Math155A_2019Winter/) to the class website.

## What the program does

This program renders the six regular convex polytopes of the fourth dimension. The five regular convex <em>polyhedra</em> of the <em>third dimension</em> are known as the platonic solids: tetrahedron, cube/hexahedron, octahedron, dodecahedron, and icosahedron. For those of you who are familiar with 4-, 6-, 8-, 12-, and 20-sided dice, these are indeed the 3D shapes that have 4, 6, 8, 12, and 20 faces. The six regular convex polytopes are constructed out of multiple regular convex polyhedra. Here is a table of the six 4D polytopes:

| Common name (other names)               | 3D cell      | Vertices | Edges | Faces | Cells | Number of symmetries |
| --------------------------------------- | ------------ | -------- | ----- | ----- | ----- | -------------------- |
| Simplex (5-cell, pentachoron)           | Tetrahedron  | 5        | 10    | 10    | 5     | 120                  |
| Tesseract (8-cell, octachoron)          | Cube         | 16       | 32    | 24    | 8     | 384                  |
| Orthoplex (16-cell, hexadecachoron)     | Tetrahedron  | 8        | 24    | 32    | 16    | 384                  |
| Octaplex (24-cell, polyoctahedron)      | Octahedron   | 24       | 96    | 96    | 24    | 1152                 |
| Dodecaplex (120-cell, polydodecahedron) | Dodecahedron | 600      | 1200  | 720   | 120   | 14400                |
| Tetraplex (600-cell, polytetrahedron)   | Tetrahedron  | 120      | 720   | 1200  | 600   | 14400                |

This table can also be found on the Wikipedia page for [Regular 4-polytopes](https://en.wikipedia.org/wiki/Regular_4-polytope).

The program supports rotation of the camera about the center point but not translation in the viewing space. There are six rotations, each of which is a rotation along two axes while the other two axes remain fixed. In order, they are the xy/xz/xw/yz/yw/zw planes, and all six rotations are independent of each other. Realistically, there are only three degrees of rotational freedom (just as how in spherical coordinates on the 2-sphere in 3 dimensions, there are only two degrees of rotational freedom because all possible points on the unit 2-sphere can be expressed using only 2 angles). The rotations are computed in reverse: first, the points are rotated in the zw plane, then the yw plane, etc.

## How to run the program

This can be done in two ways:

#1: Just run the file "Final Project.exe":
<ol>
	<li>Download the folder "essentials". This should contain the following files:
	<ul>
		<li>Final Project.exe</li>
		<li>bg.png</li>
		<li>gnd.png</li>
		<li>surf2.png</li>
		<li>EduPhong.glsl</li>
		<li>MyShaders.glsl</li>
	</ul></li>
	<li>Make sure the above files are in the same directory.</li>
	<li>The program should be 892 KB.</li>
</ol>

#2: Alternatively you could "build from source", aka compile the program using Microsoft Visual Studio:
1. Download the folder "source".
2. Open Visual Studio and under the "File" menu choose "New > Project..." (Windows shortcut is Ctrl+Shift+N).
3. Under the "Visual C++" dropdown list on the left, select "General". Then in the middle, select "Empty Project".
4. For the "Name", write anything you want (mine is "Final Project"); remember this for later. Also remember the path shown in "Location". "Solution" should already be "Create new solution". Finally, deselect "Create directory for solution" if it is selected. Then press OK.
5. Under the "Project" menu choose "Add Existing Item..." (Windows shortcut is Shift+Alt+A). Add all the files in the downloaded folder ("source"). For better bookkeeping, move all the files in "source" to the folder specified by the path in step 4, then add the files to the project as specified before.
6. Under the "Build" menu choose "Build Solution" (Windows shortcut is Ctrl+Shift+B). If there are any build errors, do your best to resolve them (there shouldn't be any). Once the solution is successfully built, under the "Debug" menu choose "Start Debugging" (Windows shortcut is F5). Alternatively, to build and run (if the build is successful), press the "Local Windows Debugger" button or simply just press F5, which will build the program for you if it has not been built already.

Some source files are not currently in use; they may be used in the future.
