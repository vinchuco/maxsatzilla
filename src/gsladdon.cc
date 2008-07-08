#include "gsladdon.hh"

#include <iostream>
#include <sstream>

#include <cassert>

using std::ios;
using std::stringstream;

gsl_matrix *GslAddon::matrix_mult(const gsl_matrix *m1, const gsl_matrix *m2) {
  
  assert(m1->size2 == m2->size1);

  gsl_matrix *prod = gsl_matrix_alloc(m1->size1, m2->size2);
  
  for(uint r = 0; r < m1->size1; r++) {
    gsl_vector_const_view rowm1 = gsl_matrix_const_row(m1, r);

    for(uint c = 0; c < m2->size2; c++) {
      gsl_vector_const_view colm2 = gsl_matrix_const_column(m2, c);
      assert((&rowm1.vector)->size == (&colm2.vector)->size);
      double vprod = 0.0;
      for(uint i = 0; i < (&rowm1.vector)->size; i++)
	vprod += gsl_vector_get((&rowm1.vector), i) * gsl_vector_get((&colm2.vector), i); 

      gsl_matrix_set(prod, r, c, vprod);
    }
  }

  return prod;
}

string GslAddon::matrix_toString(const gsl_matrix *m, const string& name) {

  stringstream mstr;

  mstr.precision(9);
  mstr.setf(ios::fixed,ios::floatfield);

  mstr << "[Matrix " << name << ", " << m->size1 << "x" << m->size2 << "]\n";
  
  for(uint r = 0; r < m->size1; ++r) {
    mstr << "[";
    for(uint c = 0; c < m->size2; ++c) {
      mstr << gsl_matrix_get(m, r, c);
      if(c != m->size2 - 1)
	mstr << " ";
    }
    mstr << "]\n";
  }
  
  return mstr.str();
}

string GslAddon::vector_toString(const gsl_vector *v, const string& name) {

  stringstream vstr;

  vstr.precision(9);
  vstr.setf(ios::fixed, ios::floatfield);

  vstr << "[Vector " << name << ", " << v->size << "]\n";
  vstr << "[";
  for(uint i = 0; i < v->size; ++v) {
    vstr << gsl_vector_get(v, i);
    if(i != v->size - 1)
      vstr << " ";
  }
  vstr << "]\n";

  return vstr.str();
  
}
