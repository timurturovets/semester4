namespace Helpers;

public static class Recurrence
{
    public static (string asymptotic, double approximate) AsymptoticT(double x, double alpha, double beta)
    {
        if (x <= 1) return ("T(x) = 0 (x <= 1)", 0);
        var r = alpha / beta;
        
        string asymptotic;
        double approx;
        if (r < 1)
        {
            asymptotic = $"T(x) ~ x / (1 - {alpha}/{beta})";
            approx = x / (1 - r);
        }

        else if (Math.Abs(r - 1) < Auxiliary.EPS)
        {
            asymptotic = $"T(x) ~ x * log_{beta}(x)";
            approx = x * Math.Log(x) / Math.Log(beta);
        }
        else
        {
            var exp = 1 + Math.Log(r) / Math.Log(beta);

            asymptotic = $"T(x) ~ x^{exp}";
            approx = Math.Pow(x, exp);
        }
        
        return (asymptotic, approx);
    }
}

public static class PoweredRecurrence
{
    public static (string asymptotic, double approximate) AsymptoticT(double x, double alpha, double beta)
    {
        var r = alpha / beta;
        string asymptotic;
        double approx;
        if (r < 1)
        {
            asymptotic = $"T(x) ~ x / (1 - {alpha}/{beta})";
            approx = x / (1 - r);
        }
        else if (Math.Abs(r - 1) < Auxiliary.EPS)
        {
            asymptotic = $"T(x) ~ x * log_{beta}(x)";
            approx = x * Math.Log(x) / Math.Log(beta);
        }
        else
        {
            var exp = 1 + Math.Log(r) / Math.Log(beta);
            asymptotic = $"T(x) ~ x^{exp}";
            approx = Math.Pow(x, exp);
        }

        return (asymptotic, approx);
    }
}