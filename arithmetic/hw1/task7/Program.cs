namespace task7;

using Helpers;

public class Task7
{
    public static void Run(string[] args)
    {
        double alpha;
        Console.Write("Введите число альфа: ");
        while (!double.TryParse(Console.ReadLine(), out alpha))
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }
        
        double beta;
        Console.Write("Введите число бета: ");
        while (!double.TryParse(Console.ReadLine(), out beta))
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }
        
        double x;
        Console.Write("Введите x: ");
        while (!double.TryParse(Console.ReadLine(), out x))
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }
        
        var (asymptotic, approx) = PoweredRecurrence.AsymptoticT(x, alpha, beta);
        Console.WriteLine($"{asymptotic}, T({x}) = {approx}");
    }
}