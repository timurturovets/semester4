namespace helpers;

// ReSharper disable file InconsistentNaming
public class PoweredRationalFunction
{
    private Polynomial F1 { get; }
    private Polynomial S1 { get; }
    private int K { get; }

    private Polynomial F2 { get; }
    private Polynomial S2 { get; }
    private int L { get; }

    public PoweredRationalFunction(Polynomial f1, Polynomial s1, int k, Polynomial f2, Polynomial s2, int l)
    {
        if (k <= 0 || l <= 0) throw new ArgumentException("Powers must be positive");
        F1 = f1;
        S1 = s1;
        K = k;
        F2 = f2;
        S2 = s2;
        L = l;
    }

    public Limit LimitAtPoint(double A)
    {
        var inner1 = S1.Evaluate(A);
        var inner2 = S2.Evaluate(A);

        var f1Val = Math.Pow(F1.Evaluate(inner1), K);
        var f2Val = Math.Pow(F2.Evaluate(inner2), L);

        if (Math.Abs(f2Val) > Polynomial.EPS) return Limit.Finite(f1Val / f2Val);
        
        if (Math.Abs(f1Val) >  Polynomial.EPS) return f1Val > 0 
            ? Limit.PositiveInfinity()
            : Limit.NegativeInfinity();

        return Limit.Indeterminate();
    }

    public Limit LimitAtInfinity(bool positive)
    {
        var sign = positive ? 1 : -1;

        var leadF1 = F1[F1.Degree];
        var leadF2 = F2[F2.Degree];
        
        var leadS1 = S1[S1.Degree];
        var leadS2 = S2[S2.Degree];

        var effectiveDegreeF1 = F1.Degree * K * S1.Degree;
        var effectiveDegreeF2 = F2.Degree * L * S2.Degree;

        var coefficient1 = Math.Pow(leadF1 * Math.Pow(leadS1 * sign, F1.Degree), K);
        var coefficient2 = Math.Pow(leadF2 * Math.Pow(leadS2 * sign, F2.Degree), L);
        
        if (effectiveDegreeF1 > effectiveDegreeF2)
            return coefficient1 / coefficient2 > 0
                ? Limit.PositiveInfinity()
                : Limit.NegativeInfinity();

        return effectiveDegreeF1 < effectiveDegreeF2 
            ? Limit.Finite(0) 
            : Limit.Finite(coefficient1 / coefficient2);
    }
}