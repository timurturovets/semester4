using Helpers;

namespace task1;

// ReSharper disable file InconsistentNaming
public static class Task1
{
    public static void Run(string[] args)
    {
        Polynomial polynomial;
        
        Console.Write("Введите через пробел коэффициенты многочлена от меньшей степени к большей: ");
        while (!Polynomial.TryParse(Console.ReadLine(), out polynomial!))
        {
            Console.Write("Некорректный ввод. Введите заново: ");
        }
        
        Console.Write("Введите количество многочленов g(x): ");
        uint k;
        while (!uint.TryParse(Console.ReadLine(), out k)) Console.Write("Некорректный ввод. Введите заново: ");

        var gs = new List<Polynomial>((int)k);
        for (var i = 0; i < k; i++)
        {
            Polynomial? g;
            Console.Write($"Введите через пробел коэффициенты {i+1}-го многочлена от меньшей степени к большей: ");
            while (!Polynomial.TryParse(Console.ReadLine(), out g))
            {
                Console.Write("Некорректный ввод. Введите заново: ");
            }
            gs.Add(g!);
        }

        var (ok, coefficients) = IsInSpan(polynomial, gs);
        
        if (!ok) Console.Write("Многочлен не принадлежит линейной оболочке.");
        else
        {
            Console.WriteLine("Многочлен принадлежит линейной оболочке. Представление:");
            Console.Write("f(x) = ");

            var first = true;
            for (var l = 0; l < coefficients!.Length; l++)
            {
                if (Math.Abs(coefficients[l]) < Auxiliary.EPS) continue;
                
                if (!first) Console.Write(" + ");
                
                Console.Write($"{coefficients[l]} * g{l+1}(x)");
                first = false;
            }
        }
    }
    private static (bool solvable, double[]? solution) Solve(double[,] A, double[] b)
    {
        var n = A.GetLength(0);
        var m = A.GetLength(1);
        
        if (b.Length != n) throw new ArgumentException("Size of b must be equal to rows count in A");

        // расширенная матрица, последний столбец это коэффициенты f
        var aug = new double[n, m + 1];
        
        for (var i = 0; i < n; i++)
        {
            for (var j = 0; j < m; j++)
            {
                aug[i, j] = A[i, j];
            }

            aug[i, m] = b[i];
        }

        var rank = 0;
        
        for (var col = 0; col < m && rank < n; col++)
        {
            var pivot = -1;
            for (var i = rank; i < n; i++)
            {
                if (!(Math.Abs(aug[i, col]) > Auxiliary.EPS)) continue;
                pivot = i;
                break;
            }

            if (pivot == -1) continue;

            for (var j = 0; j <= m; j++)
            {
                (aug[rank, j], aug[pivot, j]) = (aug[pivot, j], aug[rank, j]);
            }

            var divider = aug[rank, col];
            for (var j = col; j <= m; j++)
            {
                aug[rank, j] /= divider;
            }

            for (var i = 0; i < n; i++)
            {
                if (i == rank) continue;

                var factor = aug[i, col];
                for (var j = col; j <= m; j++)
                {
                    aug[i, j] -= factor * aug[rank, j];
                }
            }

            rank++;
        }

        for (var i = rank; i < n; i++)
        {
            if (Math.Abs(aug[i, m]) > Auxiliary.EPS) return (false, null);
        }

        var solution = new double[m];

        for (var i = 0; i < rank; i++)
        {
            for (var j = 0; j < m; j++)
            {
                if (!(Math.Abs(aug[i, j] - 1) < Auxiliary.EPS)) continue;
                solution[j] = aug[i, m];
                break;
            }
        }

        return (true, solution);
    }

    private static (bool belongs, double[]? coefficients) IsInSpan(Polynomial f, List<Polynomial> gs)
    {
        if (gs.Count == 0) return (false, null);
        
        var maxDegree = Math.Max(f.Degree, gs.Max(g => g.Degree));

        var fNorm = f.ScaleUp(maxDegree);
        var gNorm = gs.Select(g => g.ScaleUp(maxDegree)).ToList();

        var rows = maxDegree + 1;
        var cols = gNorm.Count;

        var A = new double[rows, cols];
        var b = new double[rows];

        for (var i = 0; i < rows; i++)
        {
            b[i] = fNorm[i];

            for (var j = 0; j < cols; j++)
            {
                A[i, j] = gNorm[j][i];
            }
        }

        var (solvable, solution) = Solve(A, b);

        return solvable ? (true, solution) : (false, null);
    }
}