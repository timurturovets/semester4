namespace task7;

using Helpers;

public class Task7
{
    public static void Run(string[] args)
    {
        double x;
        Console.Write("Введите x: ");
        while (!double.TryParse(Console.ReadLine(), out x))
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }
        
        var (asymptotic, approx) = PoweredRecurrence.AsymptoticT(x);
        Console.WriteLine("Ведущий порядок роста - член 2^x.");
        Console.WriteLine($"{asymptotic}, T({x}) = {approx}");
    }
}