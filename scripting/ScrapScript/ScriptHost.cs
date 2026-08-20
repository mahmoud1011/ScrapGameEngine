using System.Reflection;
using System.Runtime.InteropServices;

namespace Scrap;

/// <summary>
/// The managed entry surface the engine drives scripts through.
/// </summary>
/// <remarks>
/// Every entry point is <see cref="UnmanagedCallersOnlyAttribute"/>, so the runtime
/// hands the engine a raw function pointer with no marshalling layer. That restricts
/// arguments to blittable types, which is the intended discipline rather than a
/// limitation: an entity crosses as a <see cref="ulong"/>, a type name as a
/// null-terminated UTF-8 pointer.
/// </remarks>
public static unsafe class ScriptHost
{
    private static readonly Dictionary<ulong, ScriptableEntity> Instances = [];
    private static readonly Dictionary<string, Type?> TypeCache = [];

    /// <summary>Native signature: <c>int (*)(const ScriptInterop*)</c>.</summary>
    /// <remarks>
    /// Takes <c>void*</c> rather than the layout struct so the struct can stay
    /// internal - it is an implementation detail, and the ABI is identical either way.
    /// </remarks>
    [UnmanagedCallersOnly]
    public static int Initialize(void* functions)
    {
        try
        {
            Interop.Bind((NativeFunctions*)functions);
            Log.Info($"ScriptHost ready on {RuntimeInformation.FrameworkDescription}");
            return 0;
        }
        catch (Exception e)
        {
            Console.Error.WriteLine($"[C#] ScriptHost.Initialize failed: {e}");
            return 1;
        }
    }

    /// <summary>
    /// Native signature: <c>int (*)(ulong, const char*)</c>. Returns 0 on success.
    /// </summary>
    [UnmanagedCallersOnly]
    public static int CreateInstance(ulong entityId, byte* typeNameUtf8)
    {
        try
        {
            var typeName = Marshal.PtrToStringUTF8((IntPtr)typeNameUtf8);
            if (string.IsNullOrWhiteSpace(typeName)) return 1;

            var type = ResolveType(typeName);
            if (type is null)
            {
                Log.Error($"script type not found: {typeName}");
                return 1;
            }

            if (Activator.CreateInstance(type) is not ScriptableEntity instance)
            {
                Log.Error($"{typeName} does not derive from ScriptableEntity");
                return 1;
            }

            instance.Bind(new Entity(entityId));
            Instances[entityId] = instance;
            instance.OnCreate();
            return 0;
        }
        catch (Exception e)
        {
            // A throwing script must not take the engine down with it.
            Log.Error($"CreateInstance threw: {e.Message}");
            return 1;
        }
    }

    /// <summary>Native signature: <c>void (*)(ulong, float)</c>.</summary>
    [UnmanagedCallersOnly]
    public static void UpdateInstance(ulong entityId, float deltaTime)
    {
        if (!Instances.TryGetValue(entityId, out var instance)) return;

        try
        {
            instance.OnUpdate(deltaTime);
        }
        catch (Exception e)
        {
            // Drop the instance rather than throwing the same exception every frame.
            Log.Error($"{instance.GetType().Name}.OnUpdate threw, script disabled: {e.Message}");
            Instances.Remove(entityId);
        }
    }

    /// <summary>Native signature: <c>void (*)()</c>. Called when play mode ends.</summary>
    [UnmanagedCallersOnly]
    public static void DestroyAll()
    {
        foreach (var instance in Instances.Values)
        {
            try { instance.OnDestroy(); }
            catch (Exception e) { Log.Error($"OnDestroy threw: {e.Message}"); }
        }
        Instances.Clear();
    }

    /// <summary>
    /// Resolves an assembly-qualified or bare type name, caching both hits and misses.
    /// </summary>
    private static Type? ResolveType(string typeName)
    {
        if (TypeCache.TryGetValue(typeName, out var cached)) return cached;

        var type = Type.GetType(typeName);
        if (type is null)
        {
            // Fall back to a scan so a bare "Game.Spinner" works without the assembly
            // suffix, which is what people actually type into the inspector.
            foreach (var assembly in AppDomain.CurrentDomain.GetAssemblies())
            {
                type = assembly.GetType(typeName);
                if (type is not null) break;
            }
        }

        TypeCache[typeName] = type;
        return type;
    }
}
