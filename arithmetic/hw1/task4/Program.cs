using Helpers;

namespace task4;

public class Task4
{
    public static void Run(string[] args)
    {
        Polynomial f;
        Console.Write("Введите коэффициенты многочлена-числителя f(x) от меньшей степени к большей через пробел: ");
        while (!Polynomial.TryParse(Console.ReadLine(), out f!))
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }
        
        Polynomial g;
        Console.Write("Введите коэффициенты многочлена-знаменателя g(x) от меньшей степени к большей через пробел: ");
        while (!Polynomial.TryParse(Console.ReadLine(), out g!))
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }

        // ReSharper disable once InconsistentNaming
        double A;
        Console.Write("Введите число A: ");
        while (!double.TryParse(Console.ReadLine(), out A))
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }

        var rf = new RationalFunction(f, g);

        var resultAtPoint = rf.LimitAtPoint(A);
        var resultAtPositiveInfinity = rf.LimitAtInfinity(true);
        var resultAtNegativeInfinity = rf.LimitAtInfinity(false);
        
        Console.WriteLine($"При стремлении к точке {A}, {resultAtPoint.ToString()}");
        Console.WriteLine($"При стремлении к +бесконечности, {resultAtPositiveInfinity.ToString()}");
        Console.WriteLine($"При стремлении к -бесконечности, {resultAtNegativeInfinity.ToString()}");
    }
}