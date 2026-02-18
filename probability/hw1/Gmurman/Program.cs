namespace Gmurman;

public class Gmurman
{
    private delegate void Runner();
    public static void Run(string[] args)
    {
        var innerRunners = new Dictionary<int, Runner>
        {
            { 0, null! },
            { 6, Task6.Run },
            { 8, Task8.Run },
            { 13, Task13.Run },
            { 15, Task15.Run },
            { 19, Task19.Run },
            { 20, Task20.Run },
            { 21, Task21.Run },
            { 22, Task22.Run },
            { 26, Task26.Run },
        };
        
        Console.WriteLine("Задачи из пособия Гмурмана");
        Console.WriteLine("6. Куб распилен на 1000 кубиков...");
        Console.WriteLine("8. В коробке 6 занумерованных кубиков...");
        Console.WriteLine("13. В конверте среди 100 фотокарточек...");
        Console.WriteLine("15. Устройство состоит из пяти элементов...");
        Console.WriteLine("19. На складе имеется 15 кинескопов...");
        Console.WriteLine("20. В группе 12 студентов...");
        Console.WriteLine("21. В коробке пять одинаковых изделий...");
        Console.WriteLine("22. В \"секретном\" замке на общей оси четыре диска...");
        Console.WriteLine("26. На отрезке L длины 20 см...");
        Console.WriteLine("0. Выход");
        Console.WriteLine();
        
        int choice;
        do Console.Write("Выберите задачу: "); 
        while (!int.TryParse(Console.ReadLine(), out choice) || choice < 0 || !innerRunners.ContainsKey(choice));

        if (choice == 0) return;

        Console.WriteLine();
            
        innerRunners[choice]();
            
        Console.WriteLine();
    }
}