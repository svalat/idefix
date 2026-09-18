#!/bin/bash
#SBATCH --account=c1916888
#SBATCH --job-name="novae_no_damping_beta1e3_rad_redc01"
#SBATCH --constraint=MI250
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=00:10:00
#SBATCH --output=novae_like_3D_no_damping_beta1e3_rad_redc01_%j.out    # name of output file
#SBATCH --error=novae_like_3D_no_damping_beta1e3_rad_redc01_%j.err     # name of error file (here, in common with the output file)
#SBATCH --mail-type=ALL
#SBATCH --mail-user=nicolas.scepi@univ-grenoble-alpes.fr

module purge
source /opt/cray/pe/cpe/25.09/restore_lmod_system_defaults.sh
module load cpe/25.09
module load craype-accel-amd-gfx90a craype-x86-trento
module load PrgEnv-amd
module load rocm/6.4.3
module load cray-python/3.11.7
module load cmake

export MPICH_GPU_SUPPORT_ENABLED=1
export IDEFIX_FLAGS="-DCMAKE_CXX_COMPILER=hipcc -DCMAKE_C_COMPILER=hipcc -DIdefix_MPI=ON -DKokkos_ENABLE_HIP=ON -DKokkos_ENABLE_HIP_MULTIPLE_KERNEL_INSTANTIATIONS=ON -DKokkos_ARCH_AMD_GFX90A=ON"

export HIPCC_COMPILE_FLAGS_APPEND="-isystem ${CRAY_MPICH_PREFIX}/include"
export HIPCC_LINK_FLAGS_APPEND="-L${CRAY_MPICH_PREFIX}/lib -lmpi ${PE_MPICH_GTL_DIR_amd_gfx90a} ${PE_MPICH_GTL_LIBS_amd_gfx90a} -lstdc++fs"
export CXX=hipcc
export CC=hipcc

# # With    HyperThreading (SMT), 192 cores and 384 hardware threads.
# srun --ntasks-per-node=24 --cpus-per-task=16 --threads-per-core=2 -- ./hello_world
# Without HyperThreading (SMT), 192 cores and 192 hardware threads.
srun --ntasks-per-node=1 --cpus-per-task=2 --threads-per-core=1 --gpu-bind=closest -- idefix -dec 4 1 1 -restart 0


