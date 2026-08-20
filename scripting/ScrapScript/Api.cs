namespace Scrap;

/// <summary>A three-component vector.</summary>
public readonly record struct Vector3(float X, float Y, float Z)
{
    public static readonly Vector3 Zero = new(0f, 0f, 0f);
    public static readonly Vector3 One = new(1f, 1f, 1f);

    public static Vector3 operator +(Vector3 a, Vector3 b) => new(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
    public static Vector3 operator -(Vector3 a, Vector3 b) => new(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
    public static Vector3 operator *(Vector3 v, float s) => new(v.X * s, v.Y * s, v.Z * s);
    public static Vector3 operator *(float s, Vector3 v) => v * s;

    public override string ToString() => $"({X:0.###}, {Y:0.###}, {Z:0.###})";
}

/// <summary>
/// A handle to an engine entity.
/// </summary>
/// <remarks>
/// A UUID and nothing else. Reads and writes go straight through to the engine's
/// component storage rather than caching, so a script and the inspector never
/// disagree about where something is.
/// </remarks>
public readonly struct Entity(ulong id)
{
    public ulong Id { get; } = id;

    public Vector3 Position
    {
        get => Interop.GetTranslation(Id);
        set => Interop.SetTranslation(Id, value);
    }

    /// <summary>Euler angles in radians, matching the engine's storage.</summary>
    public Vector3 Rotation
    {
        get => Interop.GetRotation(Id);
        set => Interop.SetRotation(Id, value);
    }

    public Vector3 Scale
    {
        get => Interop.GetScale(Id);
        set => Interop.SetScale(Id, value);
    }

    public override string ToString() => $"Entity({Id})";
}

/// <summary>Engine-side logging, routed to the editor console.</summary>
public static class Log
{
    public static void Info(string message) => Interop.Log(LogLevel.Info, message);
    public static void Warn(string message) => Interop.Log(LogLevel.Warning, message);
    public static void Error(string message) => Interop.Log(LogLevel.Error, message);
}

/// <summary>
/// Base class for a script attached to an entity via ScriptComponent.
/// </summary>
/// <remarks>
/// Deliberately shaped like the lifecycle people already know: OnCreate once when play
/// starts, OnUpdate every frame, OnDestroy on stop.
/// </remarks>
public abstract class ScriptableEntity
{
    /// <summary>
    /// The entity this script is attached to.
    /// </summary>
    /// <remarks>
    /// A field rather than a property on purpose. Entity is a readonly struct, so
    /// behind a property `Entity.Position = v` would assign to the temporary the
    /// getter returned and silently do nothing - the compiler rejects it, and the
    /// natural syntax is the one worth keeping.
    /// </remarks>
    public Entity Entity;

    internal void Bind(Entity entity) => Entity = entity;

    public virtual void OnCreate() { }
    public virtual void OnUpdate(float deltaTime) { }
    public virtual void OnDestroy() { }
}
