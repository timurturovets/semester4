namespace Gmurman;

public class Gmurman
{
    private delegate void Runner();
    public static void Run(string[] args)
    {
        var innerRunners = new Dictionary<int, Runner>()
        {
            { 6, Task6.Run },
            { 8, Task8.Run },
        };
        
        Console.WriteLine("Задачи из пособия Гмурмана");
        Console.WriteLine("6. Куб распилен на 1000 кубиков...");
        Console.WriteLine("8. В коробке 6 занумерованных кубиков...");
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