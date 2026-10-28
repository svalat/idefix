// ***********************************************************************************
// Idefix MHD astrophysical code
// Copyright(C) Geoffroy R. J. Lesur <geoffroy.lesur@univ-grenoble-alpes.fr>
// and other code contributors
// Licensed under CeCILL 2.1 License, see COPYING for more information
// ***********************************************************************************

#ifndef TW_HPP_
#define TW_HPP_

#ifdef WITH_TWIN_CHECKER
  #include "tw_impl.hpp"
#else
  #include "tw_dummy.hpp"
#endif //WITH_TWIN_CHECKER

#endif // TW_HPP_
