namespace Helpers;

// ReSharper disable file InconsistentNaming
public class Polynomial(IEnumerable<double> degrees)
{
    private readonly List<double> _coefficients = degrees.ToList();
    public int Degree => _coefficients.Count - 1;

    public static Polynomial operator +(Polynomial p1, Polynomial p2)
    {
        var max = Math.Max(p1.Degree, p2.Degree);
        var result = new double[max + 1];

        for (var i = 0; i <= max; i++)
        {
            var p1v = i <= p1.Degree ? p1[i] : 0;
            var p2v = i <= p2.Degree ? p2[i] : 0;

            OperationCounter.Add();
            result[i] = p1v + p2v;
        }
        
        return new Polynomial(result);
    }

    public static Polynomial operator -(Polynomial p1, Polynomial p2)
    {
        var max = Math.Max(p1.Degree, p2.Degree);
        var result = new double[max + 1];

        for (var i = 0; i <= max; i++)
        {
            var p1v = i <= p1.Degree ? p1[i] : 0;
            var p2v = i <= p2.Degree ? p2[i] : 0;

            OperationCounter.Add();
            result[i] = p1v - p2v;
        }
        
        return new Polynomial(result);
    }
    public static Polynomial operator *(Polynomial p, double scalar)
    {
        var coefficients = new List<double>();
        for (var i = 0; i < p.Degree + 1; i++)
        {
            coefficients.Add(p[i] * scalar);
        }
        return new Polynomial(coefficients);
    }

    public void operator += (Polynomial p)
    {
        var minRank = int.Min(Degree, p.Degree);
        
        var i = 0;
        for (; i < minRank; i++)
        {
            this[i] += p[i];
        }

        if (Degree >= p.Degree) return;
        
        for (; i < p.Degree; i++)
        {
            _coefficients.Add(p[i]);
        }
    }

    public void operator *= (double scalar)
    {
        for (var i = 0; i <= Degree; i++)
        {
            this[i] *= scalar;
        }
    }

    public static Polynomial operator <<(Polynomial p, int shift)
    {
        if (shift <= 0) return p;

        var coefficients = new double[p.Degree + 1 + shift];

        for (var i = 0; i <= p.Degree; i++)
        {
            coefficients[i + shift] = p[i];
        }
        
        return new Polynomial(coefficients);
    }
    public double this[int index]
    {
        get
        {
            if (index < 0 || index > Degree) throw new IndexOutOfRangeException();
            return _coefficients[index];
        }
        private set
        {
            if (index < 0 || index > Degree) throw new IndexOutOfRangeException();
            _coefficients[index] = value;
        }
    }

    public static bool TryParse(string? line, out Polynomial? polynomial)
    {
        polynomial = null;
        if (string.IsNullOrWhiteSpace(line)) return false;

        var coefficients = new List<double>();
        var coefficientsStrings = line.Split(' ', StringSplitOptions.RemoveEmptyEntries);

        foreach (var cs in coefficientsStrings)
        {
            if (!double.TryParse(cs, out var c)) return false;
            coefficients.Add(c);
        }
        
        polynomial = new Polynomial(coefficients);
        return true;
    }
    
    public Polynomial ScaleUp(int newDegree)
    {
        if (newDegree == Degree) return this;
        if (newDegree < Degree)
        {
            throw new InvalidOperationException("New rank must be bigger than current when scaling up");
        }
        
        var coefficients = new List<double>(_coefficients);

        coefficients.AddRange(Enumerable.Repeat(0.0, newDegree - Degree));
        
        return new Polynomial(coefficients);
    }

    public double Evaluate(double x)
    {
        var result = 0.0;
        
        for (var i = Degree; i >= 0; i--)
        {
            result = result * x + this[i];
        }

        return result;
    }
    public Polynomial Derivative()
    {
        if (Degree == 0) return new Polynomial([0.0]);

        var coefficients = new double[Degree];

        for (var i = 1; i <= Degree; i++)
        {
            coefficients[i - 1] = this[i] * i;
        }
        
        return new Polynomial(coefficients);
    }

    public bool IsZero()
    {
        for (var i = 0; i <= Degree; i++)
        {
            if (Math.Abs(this[i]) > Auxiliary.EPS) return false;
        }

        return true;
    }

    public Limit GetLimitAtInfinity(bool positive)
    {
        var lead = this[Degree];

        if (Degree == 0) return Limit.Finite(this[0]);

        var sign = lead;
        if (!positive && Degree % 2 == 1) sign = -sign;

        return sign > 0 ? Limit.PositiveInfinity() : Limit.NegativeInfinity();
    }

    public Polynomial Slice(int from, int to)
    {
        if (from < 0) from = 0;
        if (to > Degree + 1) to = Degree + 1;
        if (from >= to) return new Polynomial([0.0]);

        var length = to - from;
        var coeffs = new double[length];

        for (var i = 0; i < length; i++)
        {
            coeffs[i] = this[from + i];
        }
        
        return new Polynomial(coeffs);
    }
    
    public static Polynomial MultiplyClassic(Polynomial p1, Polynomial p2)
    {
        var deg = p1.Degree + p2.Degree;
        var result = new double[deg + 1];

        for (var i = 0; i <= p1.Degree; i++)
        {
            for (var j = 0; j <= p2.Degree; j++)
            {
                OperationCounter.Add();
                OperationCounter.Multiply();
                result[i + j] += p1[i] * p2[j];
            }
        }
        
        return new Polynomial(result);
    }

    public static Polynomial MultiplyKaratsuba(Polynomial p1, Polynomial p2)
    {
        var n = Math.Max(p1.Degree, p2.Degree) + 1;

        if (n <= 2) return MultiplyClassic(p1, p2);

        var m = n / 2;

        var p1Low = p1.Slice(0, m);
        var p1High = p1.Slice(m, n);
        var p2Low = p2.Slice(0, m);
        var p2High = p2.Slice(m, n);

        var z0 = MultiplyKaratsuba(p1Low, p2Low);
        var z2 = MultiplyKaratsuba(p1High, p2High);
        var z1 = MultiplyKaratsuba(p1Low + p1High, p2Low + p2High);

        z1 = z1 - z0 - z2;

        return z0 + (z1 << m) + (z2 << (2 * m));
    }

    private static Polynomial EvaluateAt(Polynomial f0, Polynomial f1, Polynomial f2, double x)
    {
        return f0 + (f1 * x) + (f2 * (x * x));
    }

    public static Polynomial MultiplyToom3(Polynomial p1, Polynomial p2)
    {
        var n = Math.Max(p1.Degree, p2.Degree) + 1;

        if (n <= 2) return Polynomial.MultiplyClassic(p1, p2);

        var k = (n + 2) / 3;

        var p1_0 = p1.Slice(0, k);
        var p1_1 = p1.Slice(k, 2 * k);
        var p1_2 = p2.Slice(2 * k, 3 * k);

        var p2_0 = p2.Slice(0, k);
        var p2_1 = p2.Slice(k, 2 * k);
        var p2_2 = p2.Slice(2 * k, 3 * k);

        var prod0 = MultiplyToom3(p1_0, p2_0);

        var prod1 = MultiplyToom3(p1_0 + p1_1 + p1_2, p2_0 + p2_1 + p2_2);

        var prod2 = MultiplyToom3(p1_0 - p1_1 + p1_2, p2_0 - p2_1 + p2_2);

        var prod3 = MultiplyToom3(p1_0 + (p1_1 * 2) + (p1_2 * 4), p2_0 + (p2_1 * 2) + (p2_2 * 4));

        var prod4 = MultiplyToom3(p1_2, p2_2);

        var H0 = prod0;
        var H4 = prod4;

        var H2 = (prod2 + prod1) * 0.5 - H0 - H4;

        var H3 = (prod3 - prod1) * (1.0 / 3.0) - H2 * 2 - H4 * 2;

        var H1 = prod1 - H0 - H2 - H3 - H4;

        return H0
               + (H1 << k)
               + (H2 << (2 * k))
               + (H3 << (3 * k))
               + (H4 << (4 * k));
    }
}