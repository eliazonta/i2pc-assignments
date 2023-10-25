# Introduction to Parallel Computing - Assignments

## Table of content
1. [Implicit parallelism techniques and performance models](#H1)

    1. [Array addition and vectorization](#H11)
    2. [Matrix copy via block reverse ordering](#H12)

2. [H2 - [WIP]]()
3. [H3 - [TBD]]()



# 1. Implicit parallelism techniques and performance models <a name="H1"></a>

## 1.1 Array addition and vectorization<a name="H11"></a>
[[LINK]](/H1/es1/README.md)

Run the following:
```bash
    cd H1/es1
```
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
    export OMP_NUM_THREADS=1; ./<target>.out
```
> 💡 For more exploration run:
```bash
gprof <target>.out gmon.out > analysis.txt
```
```bash
less analysis.txt
```
## 1.2 Matrix copy via block reverse ordering<a name="H12"></a>

[[LINK]](H1/es2/README.md)

Run the following:

```bash
    cd H1/es2
```
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
    export OMP_NUM_THREADS=1; ./<target>.out
```
> 💡 For more exploration run:
```bash
gprof <target>.out gmon.out > analysis.txt
```
```bash
less analysis.txt
```