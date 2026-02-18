namespace Gmurman;

internal class Task26
{
    internal static void Run()
    {
        const double L = 20.0;
        const double l = 10.0;
        const int experiments = 1_000_000;

        var hits = 0;
        var rnd = new Random();

        for (var i = 0; i < experiments; i++)
        {
            var x = rnd.NextDouble() * L;
            if (x <= l) hits++;
        }
        
        Console.WriteLine($"Вычисленная вероятность: {(double) hits / experiments}");
    }
}