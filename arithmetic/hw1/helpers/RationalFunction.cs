namespace Helpers;

// ReSharper disable file InconsistentNaming
public class RationalFunction
{
    public Polynomial Numerator { get; }
    public Polynomial Denominator { get; }

    public RationalFunction(Polynomial numerator, Polynomial denominator)
    {
        if (denominator.IsZero()) throw new ArgumentException("Denominator cannot be zero");
        
        Numerator = numerator;
        Denominator = denominator;
    }

    public Limit LimitAtPoint(double A)
    {
        var fA = Numerator.Evaluate(A);
        var gA = Denominator.Evaluate(A);

        if (Math.Abs(gA) > Auxiliary.EPS) return Limit.Finite(fA / gA);

        var fDer = Numerator.Derivative();
        var gDer = Denominator.Derivative();

        if (fDer.IsZero() && gDer.IsZero()) return Limit.Indeterminate();
        
        // Лопиталь
        return new RationalFunction(fDer, gDer).LimitAtPoint(A);
    }

    public Limit LimitAtInfinity(bool positiveInfinity)
    {
        var degF = Numerator.Degree;
        var degG = Denominator.Degree;

        if (degF < degG) return Limit.Finite(0);

        var leadF = Numerator[degF];
        var leadG = Denominator[degG];

        if (degF == degG) return Limit.Finite(leadF / leadG);

        var sign = leadF / leadG;
        var diff = degF - degG;

        if (!positiveInfinity && diff % 2 == 1) sign = -sign;
        
        return sign > 0 ? Limit.PositiveInfinity() : Limit.NegativeInfinity();
    }
}