/*
* @@name:	priv_reduction.2
* @@type:	C++
* @@operation:	run
* @@expect:	success
* @@version:	omp_6.0
*/
#include <stdio.h>
#include <omp.h>
#define N 100

void do_red(int n, int *v, int &sum_v)
{
  sum_v = 0;     // sum_v is private
  #pragma omp for reduction(original(private),+: sum_v)
  for (int i = 0; i < n; i++) {
    sum_v += v[i];
  }
}

int main(void)
{
  int v[N];
  for (int i = 0; i < N; i++)
    v[i] = i;

  #pragma omp parallel
  {
    int s_v;     // s_v is private
    do_red(N, v, s_v);
    printf("myid %d: sum of v = %d\n", omp_get_thread_num(), s_v);
  }
  return 0;
}
