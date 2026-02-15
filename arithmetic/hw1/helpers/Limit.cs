namespace Helpers;

public enum LimitType
{
    Finite,
    PositiveInfinity,
    NegativeInfinity,
    Indeterminate
}

public readonly struct Limit
{
    public LimitType Type { get; }
    public double? Value { get; }

    private Limit(LimitType type, double? value)
    {
        Type = type;
        Value = value;
    }
    
    public static Limit Finite(double value) => new (LimitType.Finite, value);
    public static Limit PositiveInfinity()   => new (LimitType.PositiveInfinity, null);
    public static Limit NegativeInfinity()   => new (LimitType.NegativeInfinity, null);
    public static Limit Indeterminate()      => new (LimitType.Indeterminate, null);

    public override string ToString()
    {
        var messages = new Dictionary<LimitType, string>
        {
            { LimitType.PositiveInfinity, "предел равен +бесконечности" },
            { LimitType.NegativeInfinity, "предел равен -бесконечности" },
            { LimitType.Indeterminate, "предел не определён" }
        };

        return Type == LimitType.Finite 
            ? $"предел конечен и равен {Value}" 
            : messages[Type];
    }
}