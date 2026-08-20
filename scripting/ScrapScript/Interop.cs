using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Scrap;

/// <summary>
/// The native function table the engine hands over at startup.
/// </summary>
/// <remarks>
/// Field order and types must match <c>ScrapInterop</c> in ScriptEngine.h exactly —
/// this is a raw memory layout, not a marshalled type. Adding an entry means adding
/// it in both places, in the same position.
/// </remarks>
[StructLayout(LayoutKind.Sequential)]
internal unsafe struct NativeFunctions
{
    public delegate* unmanaged<int, byte*, void> Log;
    public delegate* unmanaged<ulong, float*, byte> GetTranslation;
    public delegate* unmanaged<ulong, float*, byte> SetTranslation;
    public delegate* unmanaged<ulong, float*, byte> GetRotation;
    public delegate* unmanaged<ulong, float*, byte> SetRotation;
    public delegate* unmanaged<ulong, float*, byte> GetScale;
    public delegate* unmanaged<ulong, float*, byte> SetScale;
}

/// <summary>
/// Managed-side access to the engine.
/// </summary>
/// <remarks>
/// Every call goes through a function pointer over blittable arguments. Nothing
/// managed ever holds a native address, and the engine never holds a managed object —
/// an entity is a <see cref="ulong"/> UUID both ways, so neither a GC compaction nor
/// an assembly unload can leave a dangling reference.
/// </remarks>
internal static unsafe class Interop
{
    private static NativeFunctions _fn;
    private static bool _ready;

    internal static void Bind(NativeFunctions* table)
    {
        _fn = *table;
        _ready = true;
    }

    internal static void Log(LogLevel level, string message)
    {
        if (!_ready) { Console.WriteLine(message); return; }

        // Stack-allocate for short messages; the engine copies before returning, so
        // the buffer does not need to outlive the call.
        var byteCount = System.Text.Encoding.UTF8.GetByteCount(message) + 1;
        Span<byte> buffer = byteCount <= 512 ? stackalloc byte[byteCount] : new byte[byteCount];
        System.Text.Encoding.UTF8.GetBytes(message, buffer);
        buffer[^1] = 0;

        fixed (byte* p = buffer)
        {
            _fn.Log((int)level, p);
        }
    }

    internal static Vector3 GetTranslation(ulong id) => Read(id, _fn.GetTranslation);
    internal static void SetTranslation(ulong id, Vector3 v) => Write(id, v, _fn.SetTranslation);
    internal static Vector3 GetRotation(ulong id) => Read(id, _fn.GetRotation);
    internal static void SetRotation(ulong id, Vector3 v) => Write(id, v, _fn.SetRotation);
    internal static Vector3 GetScale(ulong id) => Read(id, _fn.GetScale);
    internal static void SetScale(ulong id, Vector3 v) => Write(id, v, _fn.SetScale);

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    private static Vector3 Read(ulong id, delegate* unmanaged<ulong, float*, byte> fn)
    {
        if (!_ready) return default;
        float* values = stackalloc float[3];
        return fn(id, values) != 0 ? new Vector3(values[0], values[1], values[2]) : default;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    private static void Write(ulong id, Vector3 v, delegate* unmanaged<ulong, float*, byte> fn)
    {
        if (!_ready) return;
        float* values = stackalloc float[3];
        values[0] = v.X; values[1] = v.Y; values[2] = v.Z;
        fn(id, values);
    }
}

internal enum LogLevel
{
    Info = 0,
    Warning = 1,
    Error = 2,
}
