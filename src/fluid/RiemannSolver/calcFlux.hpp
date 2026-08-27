// ***********************************************************************************
// Idefix MHD astrophysical code
// Copyright(C) Geoffroy R. J. Lesur <geoffroy.lesur@univ-grenoble-alpes.fr>
// and other code contributors
// Licensed under CeCILL 2.1 License, see COPYING for more information
// ***********************************************************************************

#ifndef FLUID_RIEMANNSOLVER_CALCFLUX_HPP_
#define FLUID_RIEMANNSOLVER_CALCFLUX_HPP_

#if MHD == YES
#include "hlldMHD.hpp"
#include "hllMHD.hpp"
#include "roeMHD.hpp"
#include "tvdlfMHD.hpp"
#endif

#include "hllcHD.hpp"
#include "hllHD.hpp"
#include "tvdlfHD.hpp"
#include "roeHD.hpp"
#include "hllDust.hpp"

#include "shockFlattening.hpp"
#include "tw.hpp"
//#include <twin-checker/CheckerApi.h>

/*#define TWTOSTR(var) (#var)
#define TWCHECK(var) \
  do { \
    twin_register_site((55555+__LINE__), __FILE__, strlen(__FILE__)); \
    Kokkos::fence(); \
    auto tmp = Kokkos::create_mirror_view(var); \
    Kokkos::deep_copy(tmp, var); \
    Kokkos::fence(); \
    twin_check_double_array(tmp.data(), tmp.span(), TWTOSTR(var), strlen(TWTOSTR(var)), (55555+__LINE__), __LINE__); \
  } while(0)
*/

// Compute Riemann fluxes from states
template <typename Phys>
template <int dir>
void RiemannSolver<Phys>::CalcFlux(IdefixArray4D<real> &flux) {
  idfx::pushRegion("RiemannSolver::CalcFlux");

  TWCHECK(55555,flux);
  TWCHECK(55555,this->hydro->Uc);
  TWCHECK(55555,this->Vc);
  TWCHECK(55555,this->Vs);
  TWCHECK(55555,this->cMax);
  TWCHECK(55555,this->data->dx[IDIR]);
  TWCHECK(55555,this->data->dx[JDIR]);
  TWCHECK(55555,this->data->dx[KDIR]);

  if constexpr(dir == IDIR) {
    // enable shock flattening
    if(haveShockFlattening) shockFlattening->FindShock();
  }

  if constexpr(Phys::mhd) {
    switch (mySolver) {
      case TVDLF_MHD:
        TvdlfMHD<dir>(flux);
        break;
      case HLL_MHD:
        HllMHD<dir>(flux);
        break;
      case HLLD_MHD:
        HlldMHD<dir>(flux);
        break;
      case ROE_MHD:
        RoeMHD<dir>(flux);
        break;
      default:
        break;
    }
  } else {
    if constexpr(Phys::dust) {
      switch (mySolver) {
        case HLL_DUST:
          HllDust<dir>(flux);
          break;
        default: // do nothing
          IDEFIX_ERROR("Internal error: Unknown solver");
          break;
      }
    } else {
      // Default hydro solvers
      switch (mySolver) {
        case TVDLF:
          TvdlfHD<dir>(flux);
          break;
        case HLL:
          HllHD<dir>(flux);
          break;
        case HLLC:
          HllcHD<dir>(flux);
          break;
        case ROE:
          RoeHD<dir>(flux);
          break;
        default: // do nothing
          IDEFIX_ERROR("Internal error: Unknown solver");
          break;
      }
    }// Dust
  }

  TWCHECK(55555,this->data->dx[IDIR]);
  TWCHECK(55555,this->data->dx[JDIR]);
  TWCHECK(55555,this->data->dx[KDIR]);
  TWCHECK(55555,this->Vc);
  TWCHECK(55555,this->Vs);
  TWCHECK(55555,this->cMax);
  TWCHECK(55555,flux);

  idfx::popRegion();
}
#endif // FLUID_RIEMANNSOLVER_CALCFLUX_HPP_
