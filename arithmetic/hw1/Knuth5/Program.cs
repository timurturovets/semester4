using System.Globalization;
using System.Numerics;

using Helpers;
namespace Knuth5;

public class Task14
{
    public static void Run(string[] args)
    {
        Console.Write("Введите степень n: ");
        int n;
        while (!int.TryParse(Console.ReadLine(), out n) || n < 0)
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }

        var coefficients = new Complex[n + 1];
        for (var k = 0; k <= n; k++)
        {
            Console.Write($"Введите коэффициент a_{k} (в формате Re Im через пробел, пример: 1 2 для 1+2i): ");
            while (true)
            {
                var parts = Console.ReadLine()?.Split(' ', StringSplitOptions.RemoveEmptyEntries);
                if (parts is not { Length: 2 })
                {
                    Console.Write("Нужно 2 числа (Re и Im). Введите заново: ");
                    continue;
                }

                if (double.TryParse(parts[0], NumberStyles.Any, CultureInfo.InvariantCulture, out var re) 
                    && double.TryParse(parts[1], NumberStyles.Any, CultureInfo.InvariantCulture, out var im))
                {
                    coefficients[k] = new Complex(re, im);
                    break;
                }

                Console.Write("Некорректный ввод. Введите заново: ");
            }
        }

        Console.Write("Введите точку z (Re Im через пробел): ");
        Complex z;
        while (true)
        {
            var parts = Console.ReadLine()?.Split(' ', StringSplitOptions.RemoveEmptyEntries);
            if (parts is not { Length: 2 })
            {
                Console.Write("Нужно 2 числа (Re и Im). Введите заново: ");
                continue;
            }

            if (double.TryParse(parts[0], NumberStyles.Any, CultureInfo.InvariantCulture, out var x) 
                && double.TryParse(parts[1], NumberStyles.Any, CultureInfo.InvariantCulture, out var y))
            {
                z = new Complex(x, y);
                break;
            }

            Console.Write("Некорректный ввод. Попробуйте ещё раз: ");
        }

        Console.WriteLine();
        
        var resHorner = ComplexPolynomial.Horner(coefficients, z);
        Console.WriteLine($"\nРезультат метода Горнера: {resHorner}");
        Console.WriteLine($"Количество операций: {OperationCounter.Total()}");

        Console.WriteLine();
        
        var resScheme3 = ComplexPolynomial.Scheme3(coefficients, z);
        Console.WriteLine($"Результат схемы (3): {resScheme3}");
        Console.WriteLine($"Количество операций: {OperationCounter.Total()}");
    }
}