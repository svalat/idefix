// ***********************************************************************************
// Idefix MHD astrophysical code
// Copyright(C) Geoffroy R. J. Lesur <geoffroy.lesur@univ-grenoble-alpes.fr>
// and other code contributors
// Licensed under CeCILL 2.1 License, see COPYING for more information
// ***********************************************************************************

#ifndef TW_DUMMY_HPP_
#define TW_DUMMY_HPP_

#include <string>
#include <Kokkos_Core.hpp>
#include "idefix.hpp"
#include "arrays.hpp"

#define TWCHECK_KOKKOS_ARRAY(var) do {} while(0)
#define TWCHECK(var) do {} while(0)
#define TWREGVAR(varname, var) do {} while(0)
#define TWCHECKALL(key) do {} while(0)

template <class T>
IdefixArray1D<T> IdefixCheckedArray1D(const std::string & name, int64_t si) {
  auto res = IdefixArray1D<T>(name, si);
  return res;
}

template <class T>
IdefixArray2D<T> IdefixCheckedArray2D(const std::string & name, int64_t sj, int64_t si) {
  auto res = IdefixArray2D<T>(name, sj, si);
  return res;
}

template <class T>
IdefixArray3D<T> IdefixCheckedArray3D(const std::string & name, int64_t sk, int64_t sj,
  int64_t si) {
  auto res = IdefixArray3D<T>(name, sk, sj, si);
  return res;
}

template <class T>
IdefixArray4D<T> IdefixCheckedArray4D(const std::string & name, int64_t sl, int64_t sk,
  int64_t sj, int64_t si) {
  auto res = IdefixArray4D<T>(name, sl, sk, sj, si);
  return res;
}

#endif // TW_DUMMY_HPP_
