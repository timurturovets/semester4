namespace helpers;

public class Polynomial(IEnumerable<double> degrees)
{
    // ReSharper disable once InconsistentNaming
    public const double EPS = 1e-9;
    private readonly List<double> _coefficients = degrees.ToList();
    public int Degree => _coefficients.Count - 1;

    public static Polynomial operator +(Polynomial p1, Polynomial p2)
    {
        var coefficients =  new List<double>();
        
        var bigger = p1.Degree > p2.Degree ? p1 : p2;
        var minRank = int.Min(p1.Degree, p2.Degree);

        var i = 0;
        for (; i < minRank; i++)
        {
            coefficients.Add(p1[i] + p2[i]);
        }

        for (; i < bigger.Degree + 1; i++)
        {
            coefficients.Add(bigger[i]);
        }
        
        return new Polynomial(coefficients);
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
        for (var i = 0; i < Degree; i++)
        {
            this[i] *= scalar;
        }
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
            if (Math.Abs(this[i]) > EPS) return false;
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
}