// ***********************************************************************************
// Idefix MHD astrophysical code
//
// Source file src/utils/testLookupTable.hpp
//
// Last modified : 10/2026
//
// Copyright(C) by :
// - Sébastien Valat <sebastien.valat@univ-grenoble-alpes.fr> (IPAG/UGA/CNRS - 2026)
// and other code contributors
//
// Licensed under CeCILL 2.1 License, see COPYING for more information
// ***********************************************************************************

// headers
#include "gtest-idfx.hpp"
#include "lookupTable.hpp"

// Expected bracketing neighbours of the 2D CSV table (toto.csv) around (x=2.1, y=3.5): x brackets
// [2,3], y brackets [3,4], and the data on the four surrounding vertices is 5,6,6,7. This is
// checked from several angles across the file below (Get vs GetNeighbours vs GetNeighboursIndx,
// host vs device, cached vs uncached search), so it is defined once here instead of being
// retyped as magic numbers in every block.
const real kXN2D[4]      = {2.0, 3.0, 3.0, 4.0};
const real kDataN2D[4]   = {5.0, 6.0, 6.0, 7.0};
const int  kIdx2D[2]     = {0, 1};
const int  kDataIdx2D[4] = {1, 4, 2, 5};
const real kValue2D      = 5.6;   // csv.Get({2.1, 3.5})

//Testing 2D CSV file on device
TEST(lookupTable, 2d_csv_file_on_device) {
  IdefixArray1D<real> arr = IdefixArray1D<real>("Test",1);
  IdefixArray1D<real>::host_mirror_type arrHost = Kokkos::create_mirror_view(arr);

  LookupTable<2> csv(DATA_FILE("toto.csv"),',');

  idefix_for("loop",0, 1, KOKKOS_LAMBDA (int i) {
    real x[2];
    x[0] = 2.1;
    x[1] = 3.5;
    arr(i) = csv.Get(x);
  });

  Kokkos::deep_copy(arrHost , arr);

  ASSERT_NEAR(arrHost(0), kValue2D, 1e-13) <<  "2D CSV, device";
}

//Testing 2D CSV file on Host
TEST(lookupTable, 2d_csv_file_on_host) {
    LookupTable<2> csv(DATA_FILE("toto.csv"),',');
    real x[2];
    x[0] = 2.1;
    x[1] = 3.5;
    real result = csv.GetHost(x);
    ASSERT_NEAR(result, kValue2D, 1e-13) << "2D CSV, host";
}

//Testing 1D CSV file on device.
TEST(lookupTable, 1d_csv_file_on_device) {
  IdefixArray1D<real> arr = IdefixArray1D<real>("Test",1);
  IdefixArray1D<real>::host_mirror_type arrHost = Kokkos::create_mirror_view(arr);

  // Read 1D CSV File
  LookupTable<1> csv1D(DATA_FILE("toto1D.csv"),',');

  idefix_for("loop",0, 1, KOKKOS_LAMBDA (int i) {
    real x[2];
    x[0] = 2.1;
    x[1] = 3.5;
    arr(i) = csv1D.Get(x);
  });

  Kokkos::deep_copy(arrHost , arr);
  ASSERT_EQ(arrHost(0), real(4.2)) << "1D CSV, device";
}

//Testing 1D CSV file on host.
TEST(lookupTable, 1d_csv_file_on_host) {
    LookupTable<1> csv1D(DATA_FILE("toto1D.csv"),',');
    real x[2];
    x[0] = 2.1;
    x[1] = 3.5;
    real result = csv1D.GetHost(x);
    ASSERT_EQ(result, real(4.2)) << "1D CSV, host";
}
