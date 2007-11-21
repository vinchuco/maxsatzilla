#include "gslutils.hh"

#include <math.h>

/*
gsl_stats_correlation()
  Calculate Pearson correlation = cov(X, Y) / (sigma_X * sigma_Y)
This routine efficiently computes the correlation in one pass of the
data and makes use of the algorithm described in:

B. P. Welford, "Note on a Method for Calculating Corrected Sums of
Squares and Products", Technometrics, Vol 4, No 3, 1962.

This paper derives a numerically stable recurrence to compute a sum
of products

S = sum_{i=1..N} [ (x_i - mu_x) * (y_i - mu_y) ]

with the relation

S_n = S_{n-1} + ((n-1)/n) * (x_n - mu_x_{n-1}) * (y_n - mu_y_{n-1})
*/

double gsl_stats_correlation (const double data1[], const size_t stride1,
			      const double data2[], const size_t stride2,
			      const size_t n)
{
  size_t i;
  long double sum_xsq = 0.0;
  long double sum_ysq = 0.0;
  long double sum_cross = 0.0;
  long double ratio;
  long double delta_x, delta_y;
  long double mean_x, mean_y;
  long double r;

  /*
   * Compute:
   * sum_xsq = Sum [ (x_i - mu_x)^2 ],
   * sum_ysq = Sum [ (y_i - mu_y)^2 ] and
   * sum_cross = Sum [ (x_i - mu_x) * (y_i - mu_y) ]
   * using the above relation from Welford's paper
   */

  mean_x = data1[0 * stride1];
  mean_y = data2[0 * stride2];

  for (i = 1; i < n; ++i)
    {
      ratio = i / (i + 1.0);
      delta_x = data1[i * stride1] - mean_x;
      delta_y = data2[i * stride2] - mean_y;
      sum_xsq += delta_x * delta_x * ratio;
      sum_ysq += delta_y * delta_y * ratio;
      sum_cross += delta_x * delta_y * ratio;
      mean_x += delta_x / (i + 1.0);
      mean_y += delta_y / (i + 1.0);
    }

  r = sum_cross / (sqrt(sum_xsq) * sqrt(sum_ysq));

  return r;
}