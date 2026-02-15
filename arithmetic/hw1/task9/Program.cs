using Helpers;
namespace task9;

public class Task9
{
    public static void Run(string[] args)
    {
        var n = ReadPowerOfThree();

        var f = GenerateRandomPolynomial(n);
        var g = GenerateRandomPolynomial(n);

        Console.WriteLine($"Степень многочленов: {n - 1}");
        Console.WriteLine($"Размер n = {n}");

        OperationCounter.Reset();
        var result = Polynomial.MultiplyToom3(f, g);
        var ops = OperationCounter.Total();
        
        Console.WriteLine($"Количество арифметических операций: {ops}");

        var exp = Math.Log(5, 3);
        
        Console.WriteLine("Рекуррентное соотношение:");
        Console.WriteLine("T(n) = 5T(n/3) + O(n)");
        Console.WriteLine($"log_3(5) = {exp:F3}");
        Console.WriteLine($"T(n) = O(n^{exp:F3})");
        
    }

    private static int ReadPowerOfThree()
    {
        while (true)
        {
            Console.Write("Введите n (должно быть степенью 3): ");

            if (!int.TryParse(Console.ReadLine(), out int n) || n <= 0)
            {
                Console.WriteLine("Некорректный ввод.");
                continue;
            }

            if (IsPowerOfThree(n)) return n;
            
        }
    }

    private static bool IsPowerOfThree(int n)
    {
        while (n % 3 == 0) n /= 3;
        return n == 1;
    }

    private static Polynomial GenerateRandomPolynomial(int size)
    {
        var rnd = new Random();
        var coefficients = new double[size];

        for (var i = 0; i < size; i++)
            coefficients[i] = rnd.Next(-5, 6);

        return new Polynomial(coefficients);
    }
}