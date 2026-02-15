namespace Helpers;

public static class Auxiliary
{
    // ReSharper disable once InconsistentNaming
    public const double EPS = 1e-9;
    public static int Factorial(int n)
    {
        if (n == 0) return 1;
        return n * Factorial(n - 1);
    }

    public static double CombinationsCount(int n, int k)
    {
        if (k < 0 || k > n) return 0;
        if (k == 0 || k == n) return 1;

        double result = 1;
        for (var i = 1; i <= k; i++)
        {
            result *= (n - i + 1) / (double)i;
        }

        return result;
    }
}