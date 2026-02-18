namespace Application;

internal static class Program
{
    private delegate void Runner(string[] args);
    
    internal static void Main(string[] args)
    {
        var runners = new List<Runner>
        {
            Gmurman.Gmurman.Run,
        };
        
        while (true)
        {
            Console.Clear();
            Console.WriteLine("Эта программа моделирует вероятностные эксперименты.");
            Console.WriteLine("1. Задачи из пособия Гмурмана");
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