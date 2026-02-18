namespace Gmurman;

internal class Task13
{
    internal static void Run()
    {
        const int total = 100;
        const int taken = 10;
        const int experiments = 1_000_000;

        var success = 0;
        var rnd = new Random();

        for (var i = 0; i < experiments; i++)
        {
            var target = rnd.Next(total);

            var selected = new bool[total];
            var count = 0;

            while (count < taken)
            {
                var index = rnd.Next(total);
                if (selected[index]) continue;
                
                selected[index] = true;
                count++;
            }

            if (selected[target]) success++;
        }
        
        Console.WriteLine($"Вычисленная вероятность: {(double) success / experiments}");
        
        var cppApproachAnalogue = new Func<Random, bool>(r => 
            Enumerable
                .Range(1, total)
                .OrderBy(_ => r.Next())
                .Take(taken)
                .Contains(1));

        success = 0;
        
        for (var i = 0; i < experiments; i++)
        {
            if (cppApproachAnalogue(rnd)) success++;
        }
        
        Console.WriteLine($"Вычисленная вероятность 2: {(double) success / experiments}");
    }
}