#!/bin/sh
#
#SBATCH --account=apam
#SBATCH --job-name=TitanPerformanceAnalysis
#SBATCH --gres=gpu:1
#SBATCH -c 1
#SBATCH --time=1:00
#SBATCH --mem-per-cpu=1gb

module load cuda92/toolkit
./build/ANALYSIS >> output.txt

# End of script
