namespace task3;
using helpers;
public static class Task3
{
    public static void Run(string[] args)
    {
        Polynomial polynomial;
        Console.Write("Введите коэффициенты многочлена f(x) в базисе (x-a)^k через пробел: ");
        while (!Polynomial.TryParse(Console.ReadLine(), out polynomial!))
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }

        Console.Write("Введите текущий базис a: ");
        var a = 0.0;
        while(!double.TryParse(Console.ReadLine(), out a)) Console.Write("Некорректный ввод. Введите заново: ");
        
        Console.Write("Введите новый центр базиса B: ");
        var B = 0.0;
        while(!double.TryParse(Console.ReadLine(), out B)) Console.Write("Некорректный ввод. Введите заново: ");
        
        var shiftedPolynomial = ShiftBasis(polynomial, a, B);
        
        Console.Write("f(x) = ");

        var first = true;

        for (var k = 0; k <= polynomial.Degree; k++)
        {
            var c = shiftedPolynomial[k];

            if (Math.Abs(c) < Polynomial.EPS) continue;

            if (!first) Console.Write(c >= 0 ? " + " : " - ");
            else if (c < 0) Console.Write("-");

            var absC = Math.Abs(c);

            if (!(Math.Abs(absC - 1) < Polynomial.EPS && k != 0))
            {
                Console.Write($"{absC}");
            }

            if (k > 0)
            {
                Console.Write($"(x - {B})");
                if (k > 1) Console.Write($"^{k}");
            }

            first = false;
        }
        
    }
    
    // ReSharper disable once InconsistentNaming
    private static Polynomial ShiftBasis(Polynomial f, double a, double B)
    {
        var delta = B - a;

        var newCoefficients = new double[f.Degree + 1];

        for (var j = 0; j <= f.Degree; j++)
        {
            for (var i = 0; i <= j; i++)
            {
                newCoefficients[i] += f[j] * Functions.CombinationsCount(j, i) * Math.Pow(delta, j - i);
            }
        }
        
        return new Polynomial(newCoefficients);
    }
}