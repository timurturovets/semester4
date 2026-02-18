namespace Gmurman;

internal class Task19
{
    internal static void Run()
    {
        const int total = 15;
        const int lvov = 10;
        const int taken = 5;
        const int experiments = 1_000_000;

        var success = 0;
        var rnd = new Random();

        for (var i = 0; i < experiments; i++)
        {
            var tubes = new bool[total];
            for (var j = 0; j < lvov; j++) tubes[j] = true;

            var count = 0;
            var selected = new bool[total];
            while (count < taken)
            {
                var index = rnd.Next(total);
                if (selected[index]) continue;
                
                selected[index] = true;
                count++;
            }

            var lvovCount = 0;
            for (var j = 0; j < total; j++)
            {
                if (selected[j] && tubes[j]) lvovCount++;
            }

            if (lvovCount == 3) success++;
        }
        
        Console.WriteLine($"Вычисленная вероятность: {(double) success / experiments}");
    }
}