// @HEADER
// ***********************************************************************
//
//              PyTrilinos: Python Interface to Trilinos
//                 Copyright (2005) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Bill Spotz (wfspotz@sandia.gov)
//
// ***********************************************************************
// @HEADER

#ifndef NUMPY_INCLUDE_H
#define NUMPY_INCLUDE_H

// This include file takes care of four of the five things necessary
// when including the numpy header file arrayobject.h.  First, the
// Python.h header file is included.  Second, the
// PY_ARRAY_UNIQUE_SYMBOL is defined.  Third, the numpy/arrayobject.h
// header file is included.  Finally, the numpy/noprefix.h header file
// is included if the NUMPY_NOPREFIX macro is defined, which the user
// should make sure is the case for numpy version 1.0 or greater.

// The user is responsible for defining the macro NO_IMPORT_ARRAY in
// those source files that do not call the numpy routine
// import_array().

#include <Python.h>
#define PY_ARRAY_UNIQUE_SYMBOL PyTrilinos_NumPy
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

#ifdef NUMPY_NOPREFIX
#include <numpy/noprefix.h>
#define PyArray_SBYTE PyArray_BYTE
#endif

#if NPY_API_VERSION < 0x00000007
#define NPY_ANYORDER      PyArray_ANYORDER
#define NPY_DOUBLE        PyArray_DOUBLE
#define NPY_INT           PyArray_INT
#define NPY_ARRAY_FARRAY  NPY_FARRAY
#define PyArray_NDIM(arr) (arr->nd)
#define PyArray_DIMS(arr) (arr->dimensions)
#define PyArray_DATA(arr) (arr->data)
#endif

#endif // NUMPY_INCLUDE_H
