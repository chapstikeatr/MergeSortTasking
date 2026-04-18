#!/bin/bash
#SBATCH --job-name=mergesort_bench
#SBATCH --output=mergesort_%j.out
#SBATCH --error=mergesort_%j.err
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --time=05:00:00
#SBATCH --partition=Centaurus
#SBATCH --mem=30G

$HOME/MergeSortTasking/para/mergesort_para 10000000 1
$HOME/MergeSortTasking/para/mergesort_para 10000000 2
$HOME/MergeSortTasking/para/mergesort_para 10000000 4
$HOME/MergeSortTasking/para/mergesort_para 10000000 8
$HOME/MergeSortTasking/para/mergesort_para 100000000 1
$HOME/MergeSortTasking/para/mergesort_para 100000000 2
$HOME/MergeSortTasking/para/mergesort_para 100000000 4
$HOME/MergeSortTasking/para/mergesort_para 100000000 8
$HOME/MergeSortTasking/para/mergesort_para 1000000000 1
$HOME/MergeSortTasking/para/mergesort_para 1000000000 2
$HOME/MergeSortTasking/para/mergesort_para 1000000000 4
$HOME/MergeSortTasking/para/mergesort_para 1000000000 8


