namespace Helpers;

// ReSharper disable file InconsistentNaming
public class Polynomial2D(Polynomial[] coefficients)
{
    public Polynomial[] Coefficients = coefficients;

    public int Degree => Coefficients.Length - 1;

    public static Polynomial2D operator +(Polynomial2D p1, Polynomial2D p2)
    {
        var max = Math.Max(p1.Degree, p2.Degree);
        var result = new Polynomial[max + 1];

        for (var i = 0; i <= max; i++)
        {
            var p1v = i <= p1.Degree ? p1.Coefficients[i] : new Polynomial([0.0]);
            var p2v = i <= p2.Degree ? p2.Coefficients[i] : new Polynomial([0.0]);
            
            result[i] = p1v + p2v;
        }
        
        return new Polynomial2D(result);
    }

    public static Polynomial2D operator -(Polynomial2D p1, Polynomial2D p2)
    {
        var max = Math.Max(p1.Degree, p2.Degree);
        var result = new Polynomial[max + 1];

        for (var i = 0; i <= max; i++)
        {
            var p1v = i <= p1.Degree ? p1.Coefficients[i] : new Polynomial([0.0]);
            var p2v = i <= p2.Degree ? p2.Coefficients[i] : new Polynomial([0.0]);
            
            result[i] = p1v - p2v;
        }
        
        return new Polynomial2D(result);
    }
    public static Polynomial2D operator <<(Polynomial2D p, int shift)
    {
        if (shift <= 0) return p;

        var result = new Polynomial[p.Degree + 1 + shift];

        for (var i = 0; i < shift; i++)
        {
            result[i] = new Polynomial([0.0]);
        }

        for (var i = 0; i <= p.Degree; i++)
        {
            result[i + shift] = p.Coefficients[i];
        }
        
        return new Polynomial2D(result);
    }
    
    public Polynomial2D Slice(int from, int to)
    {
        if (from < 0) from = 0;
        if (to > Degree + 1) to = Degree + 1;
        if (from >= to) return new Polynomial2D([new Polynomial([0.0])]);

        var length = to - from;
        var result = new Polynomial[length];

        for (var i = 0; i < length; i++)
        {
            result[i] = Coefficients[from + i];
        }
        
        return new Polynomial2D(result);
    }
    
    public static Polynomial2D MultiplyClassic2D(Polynomial2D p1, Polynomial2D p2)
    {
        var deg = p1.Degree + p2.Degree;
        var result = new Polynomial[deg + 1];

        for (var i = 0; i <= p1.Degree; i++)
        {
            for (var j = 0; j <= p2.Degree; j++)
            {
                var prod = Polynomial.MultiplyClassic(p1.Coefficients[i], p2.Coefficients[j]);
                result[i + j] = prod;
            }
        }
        
        return new Polynomial2D(result);
    }

    public static Polynomial2D MultiplyKaratsuba2D(Polynomial2D p1, Polynomial2D p2)
    {
        var n = Math.Max(p1.Degree, p2.Degree) + 1;

        if (n <= 2) return MultiplyClassic2D(p1, p2);

        var m = n / 2;

        var p1Low = p1.Slice(0, m);
        var p1High = p1.Slice(m, n);
        var p2Low = p2.Slice(0, m);
        var p2High = p2.Slice(m, n);

        var z0 = MultiplyKaratsuba2D(p1Low, p2Low);
        var z2 = MultiplyKaratsuba2D(p1High, p2High);
        var z1 = MultiplyKaratsuba2D(p1Low + p1High, p2Low + p2High);

        z1 = z1 - z0 - z2;

        return z0 + (z1 << m) + (z2 << (2 * m));
    }

    public double Evaluate(int n, int x, int y)
    {
        OperationCounter.Reset();

        var Q = new double[n + 1];
        for (var j = 0; j <= n; j++)
        {
            var q = 0.0;

            var polynomial = Coefficients[j];

            for (var i = polynomial.Degree; i >= 0; i--)
            {
                q *= x;
                q += polynomial[i];
                OperationCounter.Multiply();
                OperationCounter.Add();
            }

            Q[j] = q;
        }

        var result = 0.0;
        for (var j = n; j >= 0; j--)
        {
            result *= y;
            result += Q[j];
            OperationCounter.Multiply();
            OperationCounter.Add();
        }

        return result;
    }
}