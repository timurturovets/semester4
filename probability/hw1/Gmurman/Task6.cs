namespace Gmurman;

internal class Task6
{
    internal static void Run()
    {
        const int size = 10;
        const int experiments = 1_000_000;

        var oneFace = 0;
        var twoFaces = 0;
        var threeFaces = 0;

        var rnd = new Random();

        for (var i = 0; i < experiments; i++)
        {
            var x = rnd.Next(size);
            var y = rnd.Next(size);
            var z = rnd.Next(size);

            var paintedFaces = CountPaintedFaces(x, y, z, size);

            switch (paintedFaces)
            {
                case 1:
                    oneFace++;
                    break;
                case 2:
                    twoFaces++;
                    break;
                case 3:
                    threeFaces++;
                    break;
            }
        }
        
        Console.WriteLine("Вычисленные вероятности: ");
        Console.WriteLine($"1 грань: {(double)oneFace / experiments:F10}");
        Console.WriteLine($"2 грани: {(double)twoFaces / experiments:F10}");
        Console.WriteLine($"3 грани: {(double)threeFaces / experiments:F10}");
    }

    private static int CountPaintedFaces(int x, int y, int z, int size)
    {
        var count = 0;

        if (x == 0 || x == size - 1) count++;
        if (y == 0 || y == size - 1) count++;
        if (z == 0 || z == size - 1) count++;

        return count;
    }
}