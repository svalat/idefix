// ***********************************************************************************
// Idefix MHD astrophysical code
//
// Source file src/utils/gtest-idfx.hpp
//
// Last modified : 10/2026
//
// Copyright(C) by :
// - Sébastien Valat <sebastien.valat@univ-grenoble-alpes.fr> (IPAG/UGA/CNRS - 2026)
// and other code contributors
//
// Licensed under CeCILL 2.1 License, see COPYING for more information
// ***********************************************************************************

#ifndef UTILS_TESTS_GTEST_IDFX_HPP_
#define UTILS_TESTS_GTEST_IDFX_HPP_

// includes
#include <string>
#if __has_include(<filesystem>)
  #include <filesystem> // NOLINT [build/c++17]
  namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
  #include <experimental/filesystem>
  namespace fs = std::experimental::filesystem;
#else
  error "Missing the <filesystem> header."
#endif
#include <gtest/gtest.h>
#ifdef WITH_MPI
  #include <mpi.h>
#endif //WITH_MPI
#include "idefix.hpp"

#define DATA_FILE(fname) (get_data_file_path(__FILE__, fname))

std::string get_data_file_path(const std::string & sourceFile, const std::string & fname) {
  fs::path p(sourceFile);
  return p.parent_path() / "data" / fname;
}

// configure MPI environnement to setup/tear down
class MPIEnvironment : public ::testing::Environment {
 public:
  ~MPIEnvironment() override {}

  // Override this to define how to set up the environment.
  void SetUp() override {
    char** argv;
    int argc = 0;
    #ifdef WITH_MPI
      int mpiError = MPI_Init(&argc, &argv);
      ASSERT_EQ(mpiError, 0);
    #endif
  }

  // Override this to define how to tear down the environment.
  void TearDown() override {
    #ifdef WITH_MPI
      int mpiError = MPI_Finalize();
      ASSERT_EQ(mpiError, 0);
    #endif
  }
};

// configure MPI environnement to setup/tear down
class KokkosEnvironment : public ::testing::Environment {
 public:
  ~KokkosEnvironment() override {}

  // Override this to define how to set up the environment.
  void SetUp() override {
    char** argv;
    int argc = 0;
    Kokkos::initialize(argc, argv);
  }

  // Override this to define how to tear down the environment.
  void TearDown() override {
    Kokkos::finalize();
  }
};

// configure MPI environnement to setup/tear down
class IdefixEnvironment : public ::testing::Environment {
 public:
  ~IdefixEnvironment() override {}

  // Override this to define how to set up the environment.
  void SetUp() override {
    int status = idfx::initialize();
    ASSERT_EQ(status, 0);
  }

  // Override this to define how to tear down the environment.
  void TearDown() override {
  }
};

// setup advanded main for tests using MPI / Kokkos
int main(int argc, char* argv[]) {
  bool initKokkosBeforeMPI = false;
  // When running on GPUS with Omnipath network,
  // Kokkos needs to be initialised *before* the MPI layer
  #ifdef KOKKOS_ENABLE_CUDA
    if(std::getenv("PSM2_CUDA") != NULL) {
      initKokkosBeforeMPI = true;
    }
  #endif

  //ini gtest
  ::testing::InitGoogleTest(&argc, argv);

  //init MPI & Kokkos with right order
  if (initKokkosBeforeMPI) {
    ::testing::AddGlobalTestEnvironment(new MPIEnvironment);
    ::testing::AddGlobalTestEnvironment(new KokkosEnvironment);
  } else {
    ::testing::AddGlobalTestEnvironment(new KokkosEnvironment);
    ::testing::AddGlobalTestEnvironment(new MPIEnvironment);
  }

  //init idefix
  ::testing::AddGlobalTestEnvironment(new IdefixEnvironment);

  //run tests
  return RUN_ALL_TESTS();
}

#endif //UTILS_TESTS_GTEST_IDFX_HPP_
