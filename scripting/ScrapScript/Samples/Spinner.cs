namespace Game;

using Scrap;

/// <summary>
/// Rotates its entity, and drifts up and down.
/// </summary>
/// <remarks>
/// The sample script: attach by putting <c>Game.Spinner</c> into a ScriptComponent's
/// Type field, then press Play.
/// </remarks>
public sealed class Spinner : ScriptableEntity
{
    private float _elapsed;
    private Vector3 _origin;

    public float RadiansPerSecond { get; set; } = 1.4f;
    public float BobHeight { get; set; } = 0.35f;
    public float BobSpeed { get; set; } = 2.0f;

    public override void OnCreate()
    {
        _origin = Entity.Position;
        Log.Info($"Spinner attached to {Entity} at {_origin}");
    }

    public override void OnUpdate(float deltaTime)
    {
        _elapsed += deltaTime;

        var rotation = Entity.Rotation;
        Entity.Rotation = rotation with { Z = rotation.Z + RadiansPerSecond * deltaTime };

        Entity.Position = _origin with
        {
            Y = _origin.Y + MathF.Sin(_elapsed * BobSpeed) * BobHeight,
        };
    }

    public override void OnDestroy() => Log.Info($"Spinner detached after {_elapsed:0.00}s");
}

/// <summary>Pulses its entity's scale, to show two scripts running side by side.</summary>
public sealed class Pulser : ScriptableEntity
{
    private float _elapsed;
    private Vector3 _baseScale;

    public override void OnCreate() => _baseScale = Entity.Scale;

    public override void OnUpdate(float deltaTime)
    {
        _elapsed += deltaTime;
        var factor = 1f + MathF.Sin(_elapsed * 3f) * 0.18f;
        Entity.Scale = new Vector3(_baseScale.X * factor, _baseScale.Y * factor, _baseScale.Z);
    }
}
