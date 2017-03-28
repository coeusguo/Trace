//
// vecmath.cpp
//
// Method definitions to do 2D and 3D linear algebra.  Basically just
// the matrix inversion methods need to be defined out-of-line like this.
//
// Originally written by Jean-Francois DOUE, October 1993
// Modified by Craig Kaplan and Daniel Wood, April 1999

#include "vecmath.h"

mat3f mat3f::inverse() const	    // Gauss-Jordan elimination with partial pivoting
{
	mat3f a(*this);				// As a evolves from original mat into identity
	mat3f b; 					// b evolves from identity into inverse(a)
	int	 i, j, i1;

	// Loop over cols of a from left to right, eliminating above and below diag
	for (j=0; j<3; j++) {		// Find largest pivot in column j among rows j..2
		i1 = j;					// Row with largest pivot candidate
		for (i=j+1; i<3; i++)
			if (fabs(a.v[i].n[j]) > fabs(a.v[i1].n[j]))
				i1 = i;

		// Swap rows i1 and j in a and b to put pivot on diagonal
		swap(a.v[i1], a.v[j]);
		swap(b.v[i1], b.v[j]);

		// Scale row j to have a unit diagonal
		if (a.v[j].n[j]==0.)
			throw SingularMatrixException();

		b.v[j] /= a.v[j].n[j];
		a.v[j] /= a.v[j].n[j];

		// Eliminate off-diagonal elems in col j of a, doing identical ops to b
		for (i=0; i<3; i++)
			if (i!=j) {
				b.v[i] -= a.v[i].n[j]*b.v[j];
				a.v[i] -= a.v[i].n[j]*a.v[j];
			}
	}
	return b;
}

mat4f mat4f::inverse() const	    // Gauss-Jordan elimination with partial pivoting
{
	mat4f a(*this);				// As a evolves from original mat into identity
	mat4f b;   					// b evolves from identity into inverse(a)
	int i, j, i1;

	// Loop over cols of a from left to right, eliminating above and below diag
	for (j=0; j<4; j++) {		// Find largest pivot in column j among rows j..3
		i1 = j;				    // Row with largest pivot candidate
		for (i=j+1; i<4; i++)
			if (fabs(a.v[i].n[j]) > fabs(a.v[i1].n[j]))
				i1 = i;

		// Swap rows i1 and j in a and b to put pivot on diagonal
		swap(a.v[i1], a.v[j]);
		swap(b.v[i1], b.v[j]);

		// Scale row j to have a unit diagonal
		if (a.v[j].n[j]==0.)
			throw SingularMatrixException();
		
		b.v[j] /= a.v[j].n[j];
		a.v[j] /= a.v[j].n[j];

		// Eliminate off-diagonal elems in col j of a, doing identical ops to b
		for (i=0; i<4; i++)
			if (i!=j) {
				b.v[i] -= a.v[i].n[j]*b.v[j];
				a.v[i] -= a.v[i].n[j]*a.v[j];
			}
	}
	return b;
}
