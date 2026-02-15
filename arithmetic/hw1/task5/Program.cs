namespace task5;
using helpers;
public class Task5
{
    public static void Run(string[] args)
    {
        Polynomial f1;
        Console.Write("Введите коэффициенты многочлена-числителя f1(x) от меньшей степени к большей через пробел: ");
        while (!Polynomial.TryParse(Console.ReadLine(), out f1!))
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }
        
        Polynomial s1;
        Console.Write("Введите коэффициенты многочлена s1(x) от меньшей степени к большей через пробел: ");
        while (!Polynomial.TryParse(Console.ReadLine(), out s1!))
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }
        
        int k;
        Console.Write("Введите степень числителя k: ");
        while (!int.TryParse(Console.ReadLine(), out k) || k <= 0)
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }
        
        Polynomial f2;
        Console.Write("Введите коэффициенты многочлена-знаменателя f2(x) от меньшей степени к большей через пробел: ");
        while (!Polynomial.TryParse(Console.ReadLine(), out f2!))
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }
        
        Polynomial s2;
        Console.Write("Введите коэффициенты многочлена s2(x) от меньшей степени к большей через пробел: ");
        while (!Polynomial.TryParse(Console.ReadLine(), out s2!))
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }
        
        int l;
        Console.Write("Введите степень числителя l: ");
        while (!int.TryParse(Console.ReadLine(), out l) || l <= 0)
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }
        
        double A;
        Console.Write("Введите число A: ");
        while (!double.TryParse(Console.ReadLine(), out A))
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }
        
        var T = new PoweredRationalFunction(f1, s1, k, f2, s2, l);
        
        var resultAtPoint = T.LimitAtPoint(A);
        var resultAtPositiveInfinity = T.LimitAtInfinity(true);
        var resultAtNegativeInfinity = T.LimitAtInfinity(false);
        
        Console.WriteLine($"При стремлении к точке {A}, {resultAtPoint.ToString()}");
        Console.WriteLine($"При стремлении к +бесконечности, {resultAtPositiveInfinity.ToString()}");
        Console.WriteLine($"При стремлении к -бесконечности, {resultAtNegativeInfinity.ToString()}");
    }
}