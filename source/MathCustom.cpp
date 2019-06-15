#include "MathCustom.h"

// for vec1 = <a1, a2, ..., an> and vec2 = <b1, b2, ..., bn>, vec1\oplus vec2 is
// [ b1*vec1, b2*vec1, ..., bn*vec1 ]
float * outerProduct(float * vec1, int n1, float * vec2, int n2) {
	float * matrix = (float*)malloc(sizeof(float) * n1 * n2);
	if (matrix == NULL) {
		fprintf(stderr, "Error: cannot allocate %d bytes for the outer product matrix of the given vectors.\n", sizeof(float) * n1 * n2);
	}
	else {
		for (int i = 0; i < n1; i++) {
			for (int j = 0; j < n2; j++) {
				matrix[i*n1 + j] = vec1[i] * vec2[j];
			}
		}
	}
	return matrix;
}