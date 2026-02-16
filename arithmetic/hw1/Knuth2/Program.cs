using Helpers;

namespace Knuth2;

public class Task11
{
    public static void Run(string[] args)
    {
        Polynomial polynomial;
        
        Console.Write("Введите через пробел коэффициенты многочлена от меньшей степени к большей: ");
        while (!Polynomial.TryParse(Console.ReadLine(), out polynomial!))
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }

        double x;
        Console.Write("Введите x: ");
        while(!double.TryParse(Console.ReadLine(), out x)) Console.Write("Некорректный ввод. Введите заново: ");

        var result = polynomial.EvaluateOdd(x);
        Console.WriteLine($"Нечётный многочлен в точке x: f({x}) = {result}");
    }
}