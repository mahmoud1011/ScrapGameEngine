using System.Runtime.InteropServices;

namespace Scrap;

/// <summary>
/// The managed entry surface the engine calls into.
/// </summary>
/// <remarks>
/// Every method here is <see cref="UnmanagedCallersOnlyAttribute"/>: the runtime hands
/// the engine a raw function pointer and the call costs no marshalling layer. That
/// constrains signatures to blittable types, which is the intended discipline — an
/// entity crosses the boundary as an integer handle, never as a managed object the
/// native side could hold past its lifetime.
/// </remarks>
public static class Bootstrap
{
    /// <summary>Native signature: <c>int (*)(int)</c>.</summary>
    [UnmanagedCallersOnly]
    public static int Initialize(int engineVersion)
    {
        var runtime = RuntimeInformation.FrameworkDescription;
        var arch = RuntimeInformation.ProcessArchitecture;

        Console.WriteLine($"[C#] ScrapScript online — {runtime} on {arch}");
        Console.WriteLine($"[C#] engine reports version {engineVersion}");

        // A quick exercise of newer language features, so the spike proves the modern
        // toolchain is genuinely in play and not just "some C# compiled".
        Component[] components =
        [
            new Transform(0f, 0f),
            new Sprite("player.png"),
            new Rigidbody(Mass: 1.5f),
        ];

        foreach (var component in components)
        {
            Console.WriteLine($"[C#]   {Describe(component)}");
        }

        return 0;
    }

    /// <summary>Native signature: <c>void (*)(float)</c>. Called once per frame.</summary>
    [UnmanagedCallersOnly]
    public static void Tick(float deltaTime)
    {
        Accumulated += deltaTime;
        Frames++;
    }

    /// <summary>Native signature: <c>float (*)()</c>.</summary>
    [UnmanagedCallersOnly]
    public static float AverageFrameTime()
        => Frames == 0 ? 0f : Accumulated / Frames;

    private static float Accumulated;
    private static int Frames;

    // Records and pattern matching over a closed hierarchy — C# 12/13/14 shape.
    private abstract record Component;
    private sealed record Transform(float X, float Y) : Component;
    private sealed record Sprite(string Texture) : Component;
    private sealed record Rigidbody(float Mass) : Component;

    private static string Describe(Component component) => component switch
    {
        Transform { X: 0, Y: 0 }      => "Transform at origin",
        Transform t                   => $"Transform at ({t.X}, {t.Y})",
        Sprite { Texture: var texture } => $"Sprite using {texture}",
        Rigidbody { Mass: > 1f } r    => $"Rigidbody, heavy ({r.Mass} kg)",
        Rigidbody r                   => $"Rigidbody ({r.Mass} kg)",
        _                             => "unknown component",
    };
}
