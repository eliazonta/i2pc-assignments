## 2.2 Parallel matrix transposition
Run the following:
<!-- ```bash
    cd H1/es2
``` -->
```bash
    make clean
```
```bash
    make 
```
To submit the job use (edit the script if needed):
```bash
    qsub script.pbs
```
Otherwise to run in an interactive session first log in a node with:
```bash
    qsub -I -q short_cpuQ
```
then:
```bash
    export OMP_NUM_THREADS=<num threads>; ./<target>.out
```
> 💡 For more exploration run:
```bash
gprof <target>.out gmon.out > analysis.txt
```
```bash
less analysis.txt
```