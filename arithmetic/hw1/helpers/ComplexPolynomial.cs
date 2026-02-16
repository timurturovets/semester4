using System.Numerics;
namespace Helpers;

public class ComplexPolynomial
{
    public static Complex Horner(Complex[] a, Complex z)
    {
        OperationCounter.Reset();

        var n = a.Length;
        var result = a[0];

        for (var k = 1; k < n; k++)
        {
            var temp = result * z;

            result = temp + a[k];
            OperationCounter.Multiply(4);
            OperationCounter.Add(2);
        }
        
        return result;
    }

    public static Complex Scheme3(Complex[] a, Complex z)
    {
        OperationCounter.Reset();

        var x = z.Real;
        var y = z.Imaginary;
        var u = a[0].Real;
        var v = a[0].Imaginary;

        var n = a.Length - 1;

        for (var k = 1; k <= n; k++)
        {
            var tempU = u;
            var tempV = v;

            u = x * tempU - y * tempV + a[k].Real;
            v = y * tempU + x * tempV + a[k].Imaginary;

            OperationCounter.Multiply(4);
            OperationCounter.Add(4);
        }

        return new Complex(u, v);
    }
}