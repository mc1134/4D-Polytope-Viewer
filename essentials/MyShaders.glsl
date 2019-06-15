// ***************************
// Shader programs to be used in conjunction with the
//  Phong lighting shaders of EduPhong.glsl
// Are first used with Project #6, Math 155A, Winter 2019
//
// Revision: Feb 23, 2019
// ***************************

// #beginglsl ...  #endglsl mark begining and end of code blocks.
// Syntax for #beginglsl is:
//
//   #beginglsl <shader-type> <shader-code-name>
//
// where <shader-type> is
//      vertexshader or fragmentshader or geometryshader or codeblock,
// and <shader-code-name> is used to compile/link the shader into a 
//      shader program.
// A codeblock is meant to be used as a portion of a larger shader.

// *****************************
// applyTextureMap - code block
//    applyTextureMap() is called after the Phong lighting is calculated.
//        - It returns a vec4 color value, which is used as the final pixel color.
//    Inputs: (all global variables)
//        - nonspecColor and specularColor (global variables, vec3 objects)
//        - theTexCoords (the texture coordinates, a vec2 object)
//    Returns a vec4:
//       - Will be used as the final fragment color
// *****************************
#beginglsl codeblock MyProcTexture
// vec3 nonspecColor;		// These items already declared 
// vec3 specularColor;
// vec2 theTexCoords;
float InTorusShape( vec2 pos );
float InTetrahedron( vec2 pos );
float InCube( vec2 pos );
float IsOctahedron( vec2 pos );
float InDodecahedron( vec2 pos );

float sq2 = sqrt(2);
float sq3 = sqrt(3);
float sq6 = sqrt(6);
float PI = 3.1415926535897932;
float phi = (sqrt(5)+1)/2;
float rval;
float tolerance = 0.01;

uniform int mode;
uniform float texTime;

vec4 applyTextureFunction() {
	vec2 wrappedTexCoords = fract(theTexCoords);	// Wrap s,t to [0,1].
	switch ( mode ) {
		case 0:
		case 2:
		case 5:
			rval = InTetrahedron(wrappedTexCoords);
			break;
		case 1:
			rval = InCube(wrappedTexCoords);
			break;
		case 3:
			rval = IsOctahedron(wrappedTexCoords);
			break;
		case 4:
			rval = InDodecahedron(wrappedTexCoords);
			break;
		default:
			rval = InTorusShape(wrappedTexCoords);
			break;
	}
	if ( rval != -1 ) {
		return vec4( cos(rval), sin(rval), cos(rval)+sin(rval), 1 );
	} else {
		vec3 combinedPhongColor = nonspecColor+specularColor;
		return vec4(combinedPhongColor, 1.0f) * texture(theTextureMap, theTexCoords);   // Use the Phong light colors
	}
}


// transformed coordinates from unit square to [-1,1]x[-1,1] and corresponding radius in unit circle
float x; float y; float r;
// variables for theta, cosine, and sine
float t; float c; float s;
// endpoints of the edge in question
float x1; float x2; float y1; float y2;
// projection of x,y onto the line (x1,y1)(x2,y2)
float xLine; float yLine;
// slope of the line (x1,y1)(x2,y2)
float m;
// indices of which point to use
int p1; int p2;


// display a slowly rotating tetrahedron (d4)
float InTetrahedron( vec2 pos ) {
	// vertices at
	// -0.5,	-sqrt(3)/6,	-sqrt(6)/12
	// 0.5,		-sqrt(3)/6,	-sqrt(6)/12
	// 0.0,		sqrt(3)/3,	-sqrt(6)/12
	// 0.0,		0.0,		sqrt(6)/4
	// edges at 
	// P1+(P2-P1)t; P1+(P3-P1)t; P1+(P4-P1)t;
	// P2+(P3-P2)t; P2+(P4-P2)t;
	// P3+(P4-P3)t;
	// rotate then return color
	float P[] = {	
		-0.5,	-sq3/6,	-sq6/12,
		0.5,	-sq3/6,	-sq6/12,
		0.0,	sq3/3,	-sq6/12,
		0.0,	0.0,	sq6/4,
	};
	int ordering[] = {
		1,2,	1,3,	1,4,	2,3,	2,4,	3,4,
	};
	x = 2 * pos.x - 1;
	y = 2 * pos.y - 1;
	t = 2 * PI * texTime;
	c = cos(t);
	s = sin(t);

	for(int i = 0; i < 6; i++) {
		p1 = ordering[2*i]-1;
		p2 = ordering[2*i+1]-1;
		x1 = P[3*p1]*c*c-P[3*p1+1]*s-P[3*p1+2]*c*s;
		y1 = P[3*p1]*c*s+P[3*p1+1]*c-P[3*p1+2]*s*s;
		x2 = P[3*p2]*c*c-P[3*p2+1]*s-P[3*p2+2]*c*s;
		y2 = P[3*p2]*c*s+P[3*p2+1]*c-P[3*p2+2]*s*s;
		m = (y2-y1)/(x2-x1);
		xLine = (x+m*y-m*y1+m*m*x1)/(m*m+1);
		yLine = m*(xLine-x1)+y1;
		// 3 cases: if x,y within circle of radius (tolerance) around x1,y1
		//			if x,y within circle of radius (tolerance) around x2,y2
		//			if x,y within (tolerance) of the line (x1,y1)(x2,y2)
		if (
			pow(x1-x,2)+pow(y1-y,2) <= pow(tolerance,2) || pow(x2-x,2)+pow(y2-y,2) <= pow(tolerance,2) ||
			(x1 < x2 && x1 <= xLine && xLine <= x2 || x1 >= x2 && x2 <= xLine && xLine <= x1) &&
			(y1 < y2 && y1 <= yLine && yLine <= y2 || y1 >= y2 && y2 <= yLine && yLine <= y1) &&
			pow(x-xLine,2)+pow(y-yLine,2) <= pow(tolerance,2)
		) {
			return pow(pow(x, 2) + pow(y, 2), 0.5);
		}
	}
	return -1;
}

// display a slowly rotating cube (d6)
float InCube( vec2 pos ) {
	// vertices at
	// 0.5,		0.5,	0.5
	// -0.5,	0.5,	0.5
	// -0.5,	-0.5,	0.5
	// 0.5,		-0.5,	0.5
	// 0.5,		-0.5,	-0.5
	// -0.5,	-0.5,	-0.5
	// -0.5,	0.5,	-0.5
	// 0.5,		0.5,	-0.5
	// edges at
	// P1+(P2-P1)t; P4+(P3-P4)t; P5+(P6-P5)t; P8+(P7-P8)t;
	// P1+(P4-P1)t; P2+(P3-P2)t; P7+(P6-P7)t; P8+(P5-P8)t;
	// P1+(P8-P1)t; P2+(P7-P2)t; P3+(P6-P3)t; P4+(P5-P4)t;
	float P[] = {	
		0.5,	0.5,	0.5,
		-0.5,	0.5,	0.5,
		-0.5,	-0.5,	0.5,
		0.5,	-0.5,	0.5,
		0.5,	-0.5,	-0.5,
		-0.5,	-0.5,	-0.5,
		-0.5,	0.5,	-0.5,
		0.5,	0.5,	-0.5,
	};
	int ordering[] = {
		1,2,	4,3,	5,6,	8,7,
		1,4,	2,3,	7,6,	8,5,
		1,8,	2,7,	3,6,	4,5,
	};
	x = 2 * pos.x - 1;
	y = 2 * pos.y - 1;
	t = 2 * PI * texTime;
	c = cos(t);
	s = sin(t);

	for(int i = 0; i < 12; i++) {
		p1 = ordering[2*i]-1;
		p2 = ordering[2*i+1]-1;
		// rotate around y axis, then z axis
		//x1 = P[3*p1]*c*c-P[3*p1+1]*s-P[3*p1+2]*c*s;
		//y1 = P[3*p1]*c*s+P[3*p1+1]*c-P[3*p1+2]*s*s;
		//x2 = P[3*p2]*c*c-P[3*p2+1]*s-P[3*p2+2]*c*s;
		//y2 = P[3*p2]*c*s+P[3*p2+1]*c-P[3*p2+2]*s*s;
		// rotate around vector (1/sq3,1/sq3,1/sq3)
		x1 = (P[3*p1]*(2*c+1)+P[3*p1+1]*(-c-sq3*s+1)+P[3*p1+2]*(-c+sq3*s+1))/3;
		y1 = (P[3*p1]*(-c+sq3*s+1)+P[3*p1+1]*(2*c+1)+P[3*p1+2]*(-c-sq3*s+1))/3;
		x2 = (P[3*p2]*(2*c+1)+P[3*p2+1]*(-c-sq3*s+1)+P[3*p2+2]*(-c+sq3*s+1))/3;
		y2 = (P[3*p2]*(-c+sq3*s+1)+P[3*p2+1]*(2*c+1)+P[3*p2+2]*(-c-sq3*s+1))/3;
		m = (y2-y1)/(x2-x1);
		xLine = (x+m*y-m*y1+m*m*x1)/(m*m+1);
		yLine = m*(xLine-x1)+y1;
		// 3 cases: if x,y within circle of radius (tolerance) around x1,y1
		//			if x,y within circle of radius (tolerance) around x2,y2
		//			if x,y within (tolerance) of the line (x1,y1)(x2,y2)
		if (
			pow(x1-x,2)+pow(y1-y,2) <= pow(tolerance,2) || pow(x2-x,2)+pow(y2-y,2) <= pow(tolerance,2) ||
			(x1 < x2 && x1 <= xLine && xLine <= x2 || x1 >= x2 && x2 <= xLine && xLine <= x1) &&
			(y1 < y2 && y1 <= yLine && yLine <= y2 || y1 >= y2 && y2 <= yLine && yLine <= y1) &&
			pow(x-xLine,2)+pow(y-yLine,2) <= pow(tolerance,2)
		) {
			return pow(pow(x, 2) + pow(y, 2), 0.5);
		}
	}
	return -1;
}

// display a slowly rotating octahedron (d8)
float IsOctahedron( vec2 pos ) {
	// vertices at
	// 0.5,		0.0,	0.5
	// 0.5,		0.0,	-0.5
	// -0.5,	0.0,	-0.5
	// -0.5,	0.0,	0.5
	// 0.0,		1/sq2,	0.0
	// 0.0,		-1/sq2,	0.0
	// edges at
	// P1+(P2-P1)t; P2+(P3-P2)t; P3+(P4-P3)t; P4+(P1-P4)t;
	// P1+(P5-P1)t; P2+(P5-P2)t; P3+(P5-P3)t; P4+(P5-P4)t;
	// P1+(P6-P1)t; P2+(P6-P2)t; P3+(P6-P3)t; P4+(P6-P4)t;
	float P[] = {
		0.5,		0.0,	0.5,
		0.5,		0.0,	-0.5,
		-0.5,		0.0,	-0.5,
		-0.5,		0.0,	0.5,
		0.0,		1/sq2,	0.0,
		0.0,		-1/sq2,	0.0,
	};
	int ordering[] = {
		1,2,	2,3,	3,4,	4,1,
		1,5,	2,5,	3,5,	4,5,
		1,6,	2,6,	3,6,	4,6,
	};
	x = 2 * pos.x - 1;
	y = 2 * pos.y - 1;
	t = 2 * PI * texTime;
	c = cos(t);
	s = sin(t);

	for(int i = 0; i < 12; i++) {
		p1 = ordering[2*i]-1;
		p2 = ordering[2*i+1]-1;
		// rotate around y axis, then z axis
		//x1 = P[3*p1]*c*c-P[3*p1+1]*s-P[3*p1+2]*c*s;
		//y1 = P[3*p1]*c*s+P[3*p1+1]*c-P[3*p1+2]*s*s;
		//x2 = P[3*p2]*c*c-P[3*p2+1]*s-P[3*p2+2]*c*s;
		//y2 = P[3*p2]*c*s+P[3*p2+1]*c-P[3*p2+2]*s*s;
		// rotate around vector (1/sq3,1/sq3,1/sq3)
		x1 = (P[3*p1]*(2*c+1)+P[3*p1+1]*(-c-sq3*s+1)+P[3*p1+2]*(-c+sq3*s+1))/3;
		y1 = (P[3*p1]*(-c+sq3*s+1)+P[3*p1+1]*(2*c+1)+P[3*p1+2]*(-c-sq3*s+1))/3;
		x2 = (P[3*p2]*(2*c+1)+P[3*p2+1]*(-c-sq3*s+1)+P[3*p2+2]*(-c+sq3*s+1))/3;
		y2 = (P[3*p2]*(-c+sq3*s+1)+P[3*p2+1]*(2*c+1)+P[3*p2+2]*(-c-sq3*s+1))/3;
		m = (y2-y1)/(x2-x1);
		xLine = (x+m*y-m*y1+m*m*x1)/(m*m+1);
		yLine = m*(xLine-x1)+y1;
		// 3 cases: if x,y within circle of radius (tolerance) around x1,y1
		//			if x,y within circle of radius (tolerance) around x2,y2
		//			if x,y within (tolerance) of the line (x1,y1)(x2,y2)
		if (
			pow(x1-x,2)+pow(y1-y,2) <= pow(tolerance,2) || pow(x2-x,2)+pow(y2-y,2) <= pow(tolerance,2) ||
			(x1 < x2 && x1 <= xLine && xLine <= x2 || x1 >= x2 && x2 <= xLine && xLine <= x1) &&
			(y1 < y2 && y1 <= yLine && yLine <= y2 || y1 >= y2 && y2 <= yLine && yLine <= y1) &&
			pow(x-xLine,2)+pow(y-yLine,2) <= pow(tolerance,2)
		) {
			return pow(pow(x, 2) + pow(y, 2), 0.5);
		}
	}
	return -1;
}

// Student procedural texture, but with color
float InTorusShape( vec2 pos ) {
	float RB = 1;
	float RS = 0.25;
	float THETA = PI/3;
	x = 2 * pos.x - 1;
	y = 2 * pos.y - 1;
	r = pow(pow(x, 2) + pow(y, 2), 0.5);
	float theta = 0.0;
	if ( x != 0 ) {
		theta = atan(y/x);
	}
	if ( x < 0 ) {
		theta += PI;
	}
	if ( x > 0 && y < 0 ) {
		theta += 2 * PI;
	}

	float rOfTheta = pow(pow(0.5*(RB-RS)*cos(0.5*theta)+0.5*(RB+RS)*cos(theta), 2) + pow(0.5*(RS-RB)*sin(0.5*theta)+0.5*(RB+RS)*sin(theta), 2), 0.5);
	float rOfTheta2 = pow(pow(0.5*(RB-RS)*cos(0.5*theta+PI)+0.5*(RB+RS)*cos(theta+2*PI), 2) + pow(0.5*(RS-RB)*sin(0.5*theta+PI)+0.5*(RB+RS)*sin(theta+2*PI), 2), 0.5);
	if ( (theta >= THETA && theta < PI || theta >= 2*PI-THETA && theta < 2*PI) && rOfTheta <= r && r <= rOfTheta2
	  || (theta >= 0 && theta < THETA || theta >= PI && theta < 2*PI-THETA) && rOfTheta2 <= r && r <= rOfTheta ) {
		return r;
	} else {
		return -1;
	}
}

// Dodecahedron shape
float InDodecahedron( vec2 pos ) {
	// vertices at
	// x = 1/sq3,-1/sq3,1/sq3,-1/sq3,1/sq3,-1/sq3,1/sq3,-1/sq3,0,0,0,0,1/(phi*sq3),-1/(phi*sq3),1/(phi*sq3),-1/(phi*sq3),phi/sq3,-phi/sq3,phi/sq3,-phi/sq3
	// y = 1/sq3,1/sq3,-1/sq3,-1/sq3,1/sq3,1/sq3,-1/sq3,-1/sq3,1/(phi*sq3),-1/(phi*sq3),1/(phi*sq3),-1/(phi*sq3),phi/sq3,phi/sq3,-phi/sq3,-phi/sq3,0,0,0,0
	// z = 1/sq3,1/sq3,1/sq3,1/sq3,-1/sq3,-1/sq3,-1/sq3,-1/sq3,phi/sq3,phi/sq3,-phi/sq3,-phi/sq3,0,0,0,0,1/(phi*sq3),1/(phi*sq3),-1/(phi*sq3),-1/(phi*sq3)
	// edges as ordering of vertices
	// 1,9,		1,13,	1,17,	2,9,	2,14,	2,18,
	// 3,10,	3,15,	3,17,	4,10,	4,16,	4,18,
	// 5,11,	5,13,	5,19,	6,11,	6,14,	6,20,
	// 7,12,	7,15,	7,19,	8,12,	8,16,	8,20,
	// 9,10,	11,12,	13,14,	15,16,	17,19,	18,20,
	float P[] = {
		1/sq3,1/sq3,1/sq3,			-1/sq3,1/sq3,1/sq3,			1/sq3,-1/sq3,1/sq3,			-1/sq3,-1/sq3,1/sq3,
		1/sq3,1/sq3,-1/sq3,			-1/sq3,1/sq3,-1/sq3,		1/sq3,-1/sq3,-1/sq3,		-1/sq3,-1/sq3,-1/sq3,
		0,1/(phi*sq3),phi/sq3,		0,-1/(phi*sq3),phi/sq3,		0,1/(phi*sq3),-phi/sq3,		0,-1/(phi*sq3),-phi/sq3,
		1/(phi*sq3),phi/sq3,0,		-1/(phi*sq3),phi/sq3,0,		1/(phi*sq3),-phi/sq3,0,		-1/(phi*sq3),-phi/sq3,0,
		phi/sq3,0,1/(phi*sq3),		-phi/sq3,0,1/(phi*sq3),		phi/sq3,0,-1/(phi*sq3),		-phi/sq3,0,-1/(phi*sq3),
	};
	int ordering[] = {
		1,9,	1,13,	1,17,	2,9,	2,14,	2,18,
		3,10,	3,15,	3,17,	4,10,	4,16,	4,18,
		5,11,	5,13,	5,19,	6,11,	6,14,	6,20,
		7,12,	7,15,	7,19,	8,12,	8,16,	8,20,
		9,10,	11,12,	13,14,	15,16,	17,19,	18,20,
	};
	x = 2 * pos.x - 1;
	y = 2 * pos.y - 1;
	t = 2 * PI * texTime;
	c = cos(t);
	s = sin(t);

	for(int i = 0; i < 30; i++) {
		p1 = ordering[2*i]-1;
		p2 = ordering[2*i+1]-1;
		// rotate around y axis, then z axis
		//x1 = P[3*p1]*c*c-P[3*p1+1]*s-P[3*p1+2]*c*s;
		//y1 = P[3*p1]*c*s+P[3*p1+1]*c-P[3*p1+2]*s*s;
		//x2 = P[3*p2]*c*c-P[3*p2+1]*s-P[3*p2+2]*c*s;
		//y2 = P[3*p2]*c*s+P[3*p2+1]*c-P[3*p2+2]*s*s;
		// rotate around vector (1/sq3,1/sq3,1/sq3)
		x1 = (P[3*p1]*(2*c+1)+P[3*p1+1]*(-c-sq3*s+1)+P[3*p1+2]*(-c+sq3*s+1))/3;
		y1 = (P[3*p1]*(-c+sq3*s+1)+P[3*p1+1]*(2*c+1)+P[3*p1+2]*(-c-sq3*s+1))/3;
		x2 = (P[3*p2]*(2*c+1)+P[3*p2+1]*(-c-sq3*s+1)+P[3*p2+2]*(-c+sq3*s+1))/3;
		y2 = (P[3*p2]*(-c+sq3*s+1)+P[3*p2+1]*(2*c+1)+P[3*p2+2]*(-c-sq3*s+1))/3;
		m = (y2-y1)/(x2-x1);
		xLine = (x+m*y-m*y1+m*m*x1)/(m*m+1);
		yLine = m*(xLine-x1)+y1;
		// 3 cases: if x,y within circle of radius (tolerance) around x1,y1
		//			if x,y within circle of radius (tolerance) around x2,y2
		//			if x,y within (tolerance) of the line (x1,y1)(x2,y2)
		if (
			pow(x1-x,2)+pow(y1-y,2) <= pow(tolerance,2) || pow(x2-x,2)+pow(y2-y,2) <= pow(tolerance,2) ||
			(x1 < x2 && x1 <= xLine && xLine <= x2 || x1 >= x2 && x2 <= xLine && xLine <= x1) &&
			(y1 < y2 && y1 <= yLine && yLine <= y2 || y1 >= y2 && y2 <= yLine && yLine <= y1) &&
			pow(x-xLine,2)+pow(y-yLine,2) <= pow(tolerance,2)
		) {
			return pow(pow(x, 2) + pow(y, 2), 0.5);
		}
	}
	return -1;
}

#endglsl
