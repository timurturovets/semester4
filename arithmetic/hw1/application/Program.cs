using task1;
using task2;
using task3;
using task4;
using task5;
using task6;
using task7;
using task8;
using task9;
using Knuth1;
using Knuth2;
using Knuth3;
using Knuth4;

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
            Task8.Run,
            Task9.Run,
            Task10.Run,
            Task11.Run,
            Task12.Run,
            Task13.Run,
            
        };
        
        while (true)
        {
            Console.Clear();
            Console.WriteLine("Эта программа решает задачи с применением многочленов.");
            Console.WriteLine("1. Определение принадлежности многочлена линейной оболочке");
            Console.WriteLine("2. Представление многочлена в виде линейной комбинации степеней");
            Console.WriteLine("3. Представление многочлена по степеням (x - B)");
            Console.WriteLine("4. Нахождение пределов рациональной функции R(x) = f(x)/g(x)");
            Console.WriteLine("5. Нахождение пределов рациональной функции T(x) = f1^k(s1(x))/f2^k(s2(x))");
            Console.WriteLine("6. Нахождение асимптотического решения рекуррентного соотношения T(x) = aT(x/b) + x");
            Console.WriteLine("7. Нахождение асимптотического решения рекуррентного соотношения T(x) = aT(x/b) + 2^x");
            Console.WriteLine("8. (фон цур Гатен) Сравнение классического умножения и умножения методом Карацубы");
            Console.WriteLine("9. (фон цур Гатен) Умножение рекурсивным методом на подобии метода Карацубы");
            Console.WriteLine("10.(Кнут) Деление неотрицательного n-разрядного целого числа");
            Console.WriteLine("11.(Кнут) Вычисление нечётного многочлена");
            Console.WriteLine("12.(Кнут) Применение правила Горнера при умножении и сложении многочленов");
            Console.WriteLine("13.(Кнут) Аналог метода Горнера для вычисления многочлена от двух переменных");
            Console.WriteLine("0. Выход");
            Console.WriteLine();
            
            int choice;
            do Console.Write("Выберите задачу: ");
            while (!int.TryParse(Console.ReadLine(), out choice) || choice < 0 || choice > runners.Count);

            if (choice == 0) return;

            Console.WriteLine();
            
            runners[choice - 1](args);
            
            Console.WriteLine();
            Console.Write("Нажмите любую кнопку, чтобы продолжить...");
            Console.ReadKey();
        }
    }
}