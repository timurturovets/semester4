using Helpers;

namespace task2;

public static class Task2
{
    public static void Run(string[] args)
    {
        Polynomial polynomial;
        
        Console.Write("Введите через пробел коэффициенты многочлена от меньшей степени к большей: ");
        while (!Polynomial.TryParse(Console.ReadLine(), out polynomial!))
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }

        double a;
        
        Console.Write("Введите рациональное число a: ");
        while(!double.TryParse(Console.ReadLine(), out a)) Console.Write("Некорректный ввод. Введите заново: ");

        var coefficients = TaylorCoefficients(polynomial, a);
        
        Console.Write("f(x) = ");

        var first = true;

        for (var k = 0; k < coefficients.Length; k++)
        {
            if (Math.Abs(coefficients[k]) < Auxiliary.EPS) continue;
            
            if (!first) Console.Write(" + ");
                    
            if (k == 0)      Console.Write($"{coefficients[k]}");
            else if (k == 1) Console.Write($"{coefficients[k]}*(x-{a})");
            else             Console.Write($"{coefficients[k]}*(x-{a})^{k}");

            first = false;
        }
        
        if (first) Console.Write("0");
    }

    private static double[] TaylorCoefficients(Polynomial f, double a)
    {
        var n = f.Degree;
        var result = new double[n + 1];

        var current = f;

        for (var k = 0; k <= n; k++)
        {
            result[k] = current.Evaluate(a) / Auxiliary.Factorial(k);
            current = current.Derivative();
        }

        return result;
    }
}