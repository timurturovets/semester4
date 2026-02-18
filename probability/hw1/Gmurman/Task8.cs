namespace Gmurman;

internal class Task8
{
    internal static void Run()
    {
        const int n = 6;
        const int experiments = 1_000_000;

        var success = 0;
        var rnd = new Random();
        
        for (var i = 0; i < experiments; i++)
        {
            var cubes = Enumerable.Range(1, n).ToArray();
            Shuffle(cubes, rnd);

            if (IsStrictlyIncreasing(cubes)) success++;
        }
        
        Console.WriteLine($"Вычисленная вероятность: {(double) success / experiments}");
    }

    private static void Shuffle(int[] cubes, Random rnd)
    {
        for (var i = cubes.Length - 1; i > 0; i--)
        {
            var j = rnd.Next(i + 1);
            (cubes[i], cubes[j]) = (cubes[j], cubes[i]);
        }
    }

    private static bool IsStrictlyIncreasing(int[] array)
    {
        for (var i = 1; i < array.Length; i++)
        {
            if (array[i] <= array[i - 1]) return false;
        }

        return true;
    }
}