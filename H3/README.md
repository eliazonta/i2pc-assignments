## 3 Parallel matrix transposition
Run the following:
<!-- ```bash
    cd H1/es2
``` -->
cd in the right directory:
```bash
    cd i2pc-assignments/H3/
```
Load required modules with :
```bash
    module load gcc91
    module load mpich-3.2.1--gcc-9.1.0
```
To compile all executables run
```bash
    make 
```
To submit the job for sequential use (edit the script if needed):
```bash
    qsub script.pbs
```
To submit the job for parallel use (edit the script if needed):
```bash
    qsub mpi.pbs
```
To submit the job for parallel with blocks use (edit the script if needed):
```bash
    qsub mpi-blocks.pbs
```

Otherwise to run in an interactive session first log in a node with:
```bash
    qsub -I -q short_cpuQ
```
> 💡 For more exploration run:
```bash
gprof <target>.out gmon.out > analysis.txt
```
```bash
less analysis.txt
```