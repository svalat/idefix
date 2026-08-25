// ***********************************************************************************
// Idefix MHD astrophysical code
// Copyright(C) Geoffroy R. J. Lesur <geoffroy.lesur@univ-grenoble-alpes.fr>
// and other code contributors
// Licensed under CeCILL 2.1 License, see COPYING for more information
// ***********************************************************************************

#ifndef FLUID_RIEMANNSOLVER_HDSOLVERS_TVDLFHD_HPP_
#define FLUID_RIEMANNSOLVER_HDSOLVERS_TVDLFHD_HPP_

#include "../idefix.hpp"
#include "fluid.hpp"
#include "extrapolateToFaces.hpp"
#include "flux.hpp"
#include "convertConsToPrim.hpp"
#include "tw.hpp"

// Compute Riemann fluxes from states using TVDLF solver
template <typename Phys>
template<const int DIR>
void RiemannSolver<Phys>::TvdlfHD(IdefixArray4D<real> &Flux) {
  idfx::pushRegion("RiemannSolver::TVDLF_Solver");

  constexpr int ioffset = (DIR==IDIR) ? 1 : 0;
  constexpr int joffset = (DIR==JDIR) ? 1 : 0;
  constexpr int koffset = (DIR==KDIR) ? 1 : 0;

  IdefixArray4D<real> Vc = this->Vc;
  IdefixArray4D<real> Vs = this->Vs;
  IdefixArray3D<real> cMax = this->cMax;
  EquationOfState eos = *(hydro->eos.get());

  // Required for high order interpolations
  IdefixArray1D<real> dx = this->data->dx[DIR];

  ExtrapolateToFaces<Phys,DIR> extrapol = *this->GetExtrapolator<DIR>();

  //TWINCHECK
  IdefixArray4D<real> twFluxL("twFluxL",data->np_tot[KDIR], data->np_tot[JDIR], data->np_tot[IDIR], Phys::nvar);
  IdefixArray4D<real> twFluxR("twFluxR",data->np_tot[KDIR], data->np_tot[JDIR], data->np_tot[IDIR], Phys::nvar);
  IdefixArray4D<real> twuL("twuL",data->np_tot[KDIR], data->np_tot[JDIR], data->np_tot[IDIR], Phys::nvar);
  IdefixArray4D<real> twuR("twuR",data->np_tot[KDIR], data->np_tot[JDIR], data->np_tot[IDIR], Phys::nvar);
  IdefixArray4D<real> twvL("twvuL",data->np_tot[KDIR], data->np_tot[JDIR], data->np_tot[IDIR], Phys::nvar);
  IdefixArray4D<real> twvR("twvuR",data->np_tot[KDIR], data->np_tot[JDIR], data->np_tot[IDIR], Phys::nvar);
  IdefixArray4D<real> tw1("tw1",data->np_tot[KDIR], data->np_tot[JDIR], data->np_tot[IDIR], Phys::nvar);
  IdefixArray4D<real> tw2("tw2",data->np_tot[KDIR], data->np_tot[JDIR], data->np_tot[IDIR], Phys::nvar);
  IdefixArray4D<real> tw3("tw3",data->np_tot[KDIR], data->np_tot[JDIR], data->np_tot[IDIR], Phys::nvar);
  IdefixArray4D<real> tw4("tw3",data->np_tot[KDIR], data->np_tot[JDIR], data->np_tot[IDIR], Phys::nvar);

  idefix_for("TVDLF_Kernel",
             data->beg[KDIR],data->end[KDIR]+koffset,
             data->beg[JDIR],data->end[JDIR]+joffset,
             data->beg[IDIR],data->end[IDIR]+ioffset,
    KOKKOS_LAMBDA (int k, int j, int i) {
      // Init the directions (should be in the kernel for proper optimisation by the compilers)
      constexpr int Xn = DIR+MX1;

      // Primitive variables
      real vL[Phys::nvar];
      real vR[Phys::nvar];
      real vRL[Phys::nvar];

      // Conservative variables
      real uL[Phys::nvar];
      real uR[Phys::nvar];

      // Flux (left and right)
      real fluxL[Phys::nvar];
      real fluxR[Phys::nvar];

      // Signal speeds
      real cRL, cmax;

      // 1-- Read primitive variables
      extrapol.ExtrapolatePrimVar(i, j, k, vL, vR);

#pragma unroll
      for(int nv = 0 ; nv < Phys::nvar; nv++) {
        vRL[nv] = HALF_F*(vL[nv]+vR[nv]);
      }

      // 2-- Get the wave speed
#if HAVE_ENERGY
      cRL = std::sqrt(eos.GetGamma(vRL[PRS],vRL[RHO])*(vRL[PRS]/vRL[RHO]));
#else
      cRL = HALF_F*(eos.GetWaveSpeed(k,j,i)
                   +eos.GetWaveSpeed(k-koffset,j-joffset,i-ioffset));
#endif
      cmax = FMAX(FABS(vRL[Xn]+cRL),FABS(vRL[Xn]-cRL));


      // 3-- Compute the conservative variables
      K_PrimToCons<Phys>(uL, vL, &eos);
      K_PrimToCons<Phys>(uR, vR, &eos);

      // 4-- Compute the left and right fluxes
      K_Flux<Phys,DIR>(fluxL, vL, uL, cRL*cRL);
      K_Flux<Phys,DIR>(fluxR, vR, uR, cRL*cRL);

      //tw
      for(int nv = 0 ; nv < Phys::nvar; nv++) {
        twuL(k,j,i,nv) = uL[nv];
        twuR(k,j,i,nv) = uR[nv];
        twvL(k,j,i,nv) = vL[nv];
        twvR(k,j,i,nv) = vR[nv];
        twFluxL(k,j,i,nv) = fluxL[nv];
        tw1(k,j,i,nv) = fluxL[nv]+fluxR[nv];
        tw2(k,j,i,nv) = cmax*(uR[nv]-uL[nv]);
        tw3(k,j,i,nv) = (fluxL[nv]+fluxR[nv] - cmax*(uR[nv]-uL[nv]));
        tw4(k,j,i,nv) = tw1(k,j,i,nv) - tw2(k,j,i,nv);
      }

      // 5-- Compute the flux from the left and right states
#pragma unroll
      for(int nv = 0 ; nv < Phys::nvar; nv++) {
        //Flux(nv,k,j,i) = (fluxL[nv]+fluxR[nv] - (uR[nv]-uL[nv]));
        Flux(nv,k,j,i) = HALF_F*(fluxL[nv]+fluxR[nv] - cmax*(uR[nv]-uL[nv]));
      }

      //6-- Compute maximum wave speed for this sweep
      cMax(k,j,i) = cmax;
    }
  );

  TWCHECK(66666, twuL);
  TWCHECK(66666, twuR);
  TWCHECK(66666, twvL);
  TWCHECK(66666, twvR);
  TWCHECK(66666, twFluxL);
  TWCHECK(66666, twFluxR);
  TWCHECK(66666, cMax);
  TWCHECK(66666, tw1);
  TWCHECK(66666, tw2);
  TWCHECK(66666, tw4);
  TWCHECK(66666, tw3);
  TWCHECK(66666, Flux);


  idfx::popRegion();
}

#endif // FLUID_RIEMANNSOLVER_HDSOLVERS_TVDLFHD_HPP_
