using Helpers;

namespace Knuth4;

public class Task13
{
    public static void Run(string[] args)
    {
        Console.Write("Введите общую степень n: ");
        int n;
        while(!int.TryParse(Console.ReadLine(), out n) || n < 0) Console.Write("Некорректный ввод. Введите заново: ");

        var coefficients = new Polynomial[n + 1];
        
        for (var j = 0; j <= n; j++)
        {
            var maxI = n - j;
            Console.Write($"Введите коэффициенты для y^{j}, x^0 .. x^{maxI} через пробел: ");
            while(true)
            {
                var line = Console.ReadLine();
                if (string.IsNullOrWhiteSpace(line)) continue;

                var parts = line.Split(' ', StringSplitOptions.RemoveEmptyEntries);
                if (parts.Length != maxI + 1)
                {
                    Console.Write($"Нужно {maxI + 1} коэффициентов. Введите заново: ");
                    continue;
                }

                var coeffs = new double[maxI + 1];
                var ok = true;
                for (var i = 0; i <= maxI; i++)
                {
                    if (double.TryParse(parts[i], out coeffs[i])) continue;
                    ok = false;
                    break;
                }

                if (!ok)
                {
                    Console.Write("Некорректный ввод. Введите заново: ");
                    continue;
                }

                coefficients[j] = new Polynomial(coeffs);
                break;
            }
        }

        var polynomial2D = new Polynomial2D(coefficients);

        Console.Write("Введите x: ");
        double x;
        while (!double.TryParse(Console.ReadLine(), out x)) Console.Write("Некорректный ввод. Введите заново: ");

        Console.Write("Введите y: ");
        double y;
        while (!double.TryParse(Console.ReadLine(), out y)) Console.Write("Некорректный ввод. Введите заново: ");

        var value = polynomial2D.Evaluate(n, (int)x, (int)y);

        Console.WriteLine($"P({x},{y}) = {value}");
        Console.WriteLine($"Операций сложения: {OperationCounter.Additions}");
        Console.WriteLine($"Операций умножения: {OperationCounter.Multiplications}");
    }
}