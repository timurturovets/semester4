using task1;
using task2;
using task3;
using task4;
using task5;
using task6;
using task7;

namespace Application;

internal static class Program
{
    private delegate void Runner(string[] args);
    
    internal static void Main(string[] args)
    {
        var runners = new List<Runner>
        {
            Task1.Run,
            Task2.Run,
            Task3.Run,
            Task4.Run,
            Task5.Run,
            Task6.Run,
            Task7.Run,
        };
        
        while (true)
        {
            Console.Clear();
            Console.WriteLine("1. Определение принадлежности многочлена линейной оболочке");
            Console.WriteLine("2. Представление многочлена в виде линейной комбинации степеней");
            Console.WriteLine("3. Представление многочлена по степеням (x - B)");
            Console.WriteLine("4. Нахождение пределов рациональной функции R(x) = f(x)/g(x)");
            Console.WriteLine("5. Нахождение пределов рациональной функции T(x) = f1^k(s1(x))/f2^k(s2(x))");
            Console.WriteLine("6. Нахождение асимптотического решения рекуррентного соотношения T(x) = αT(x/β) + x");
            Console.WriteLine("7. Нахождение асимптотического решения рекуррентного соотношения T(x) = αT(x/β) + 2^x");
            Console.WriteLine("0. Выход");

            int choice;
            do Console.Write("Выберите задачу: ");
            while (!int.TryParse(Console.ReadLine(), out choice) || choice < 0 || choice > runners.Count);

            if (choice == 0) return;
            
            runners[choice - 1](args);
            
            Console.WriteLine();
            Console.Write("Нажмите любую кнопку, чтобы продолжить...");
            Console.ReadKey();
        }
    }
}