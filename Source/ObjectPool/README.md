# Object Pool System for Unreal Engine

A robust object pooling system for Unreal Engine that helps optimize performance by reusing Actors and ActorComponents instead of constantly creating and destroying them. This system is particularly useful for frequently spawned objects like projectiles, particles, enemies, or UI components.

## Features

- **Actor Pooling**: Pool any Actor class that implements the `Poolable` interface
- **Component Pooling**: Pool ActorComponents for efficient component management
- **Blueprint Integration**: Custom Blueprint nodes for easy pooling operations
- **Automatic Lifespan Management**: Objects can automatically return to pool after a specified time
- **Configuration System**: Data-driven pool configuration via DataAssets
- **World Subsystem**: Automatic lifecycle management tied to world context
- **Interface-Based**: Clean separation using the `IPoolable` interface

## Installation

1. Copy the `ObjectPool` folder into your project's `Source` directory
2. Add `"ObjectPool"` to your project's module dependencies in your `.Build.cs` file:
   ```cpp
   // Source/{ProjectName}/{ProjectName}.Build.cs
   PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "ObjectPool" });
   ```
3. Regenerate project files and compile

## Quick Start

### 1. Create a Pool Configuration

1. Create a new **Data Asset** based on `ObjectPoolConfig`
2. Configure your pools by adding entries to `ActorPools` and `ActorComponentPools`
3. Set pool limits and lifespans for each object type

### 2. Configure Project Settings

1. Go to **Edit → Project Settings → Object Pool Settings**
2. Assign your `ObjectPoolConfig` DataAsset

### 3. Implement the Poolable Interface

For any Actor or Component you want to pool:

**C++ Implementation:**
```cpp
UCLASS()
class YOURGAME_API APooledProjectile : public AActor, public IPoolable
{
    GENERATED_BODY()

public:
    // IPoolable interface
    virtual void OnSpawnFromPool_Implementation() override;
    virtual void OnReturnToPool_Implementation() override;
};

void APooledProjectile::OnSpawnFromPool_Implementation()
{
    // Reset object state when retrieved from pool
    SetActorTickEnabled(true);
    // Initialize your object here
}

void APooledProjectile::OnReturnToPool_Implementation() 
{
    // Clean up object state before returning to pool
    SetActorTickEnabled(false);
    // Reset variables, clear references, etc.
}
```

**Blueprint Implementation:**
1. Open your Actor/Component Blueprint
2. Go to **Class Settings → Interfaces**
3. Add the `Poolable` interface
4. Implement `OnSpawnFromPool` and `OnReturnToPool` events

## Usage

### Blueprint Nodes

The system provides several custom Blueprint nodes:

#### Spawn Pooled Actor from Class
- **Purpose**: Spawns an actor from pool or creates new one if pool is empty
- **Inputs**: Actor Class, Transform, Collision Handling, Owner
- **Output**: Spawned Actor reference

#### Return to Pool
- **Purpose**: Returns an actor to its pool for reuse
- **Input**: Actor reference
- **Note**: If actor doesn't implement Poolable, it will be destroyed normally

#### Add Pooled Component by Class
- **Purpose**: Adds a component from pool or creates new one
- **Inputs**: Owner Actor, Component Class, Transform, Manual Attachment
- **Output**: Component reference

#### Return Component to Pool
- **Purpose**: Returns a component to its pool
- **Input**: Component reference

### C++ Usage

```cpp
// Get the Object Pool subsystem
UObjectPoolWorldSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UObjectPoolWorldSubsystem>();

// Spawn a pooled actor
AActor* PooledActor = PoolSubsystem->BeginDeferredPooledActorSpawnFromClass(
    this, 
    APooledProjectile::StaticClass(), 
    SpawnTransform
);
PooledActor = PoolSubsystem->FinishSpawningPooledActor(PooledActor, SpawnTransform);

// Return actor to pool
PoolSubsystem->ReturnToPool(PooledActor);
```

## Configuration

### ObjectPoolConfig DataAsset

The `UObjectPoolConfig` DataAsset contains two main arrays:

#### ActorPools
```cpp
USTRUCT(BlueprintType)
struct FActorPoolInfo
{
    // Pool configuration parameters
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FObjectPoolParams PoolParams;

    // Actor class to pool (must implement IPoolable)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<AActor> ActorClass;
};
```

#### ActorComponentPools
```cpp
USTRUCT(BlueprintType)
struct FActorComponentPoolInfo
{
    // Pool configuration parameters
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FObjectPoolParams PoolParams;

    // Component class to pool (must implement IPoolable)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UActorComponent> ActorComponentClass;
};
```

#### Pool Parameters
```cpp
USTRUCT(BlueprintType)
struct FObjectPoolParams
{
    // Maximum number of objects in pool
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 PoolLimit = 50;

    // Auto-return to pool timer (0 = never auto-return)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Lifespan = 5.0f;
};
```

## Architecture

### Core Components

1. **UObjectPoolWorldSubsystem**: Main system managing all pools
2. **IPoolable Interface**: Contract for poolable objects
3. **FPoolState**: Internal pool state management
4. **Custom K2Nodes**: Blueprint integration nodes
5. **UObjectPoolConfig**: Configuration DataAsset

### Pool State Management

Each object type maintains its own `FPoolState`:
- Tracks current pool size vs. maximum capacity
- Manages lifespan timers for automatic cleanup
- Handles object retrieval and storage

### Automatic Lifespan Management

Objects can be configured with a lifespan timer:
- When spawned, a timer starts counting down
- When timer expires, object automatically returns to pool
- Set lifespan to 0 for manual-only pool returns

## Best Practices

### Performance Optimization
- Set appropriate pool limits based on your game's needs
- Use lifespans for objects that might be forgotten (like projectiles)
- Implement proper cleanup in `OnReturnToPool` to avoid memory leaks

### Object State Management
```cpp
void APooledProjectile::OnSpawnFromPool_Implementation()
{
    // Always reset object state
    Health = MaxHealth;
    Velocity = FVector::ZeroVector;
    bIsActive = true;
    
    // Re-enable necessary systems
    SetActorTickEnabled(true);
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
}

void APooledProjectile::OnReturnToPool_Implementation()
{
    // Clean up state and references
    TargetActor = nullptr;
    bIsActive = false;
    
    // Disable systems for performance
    SetActorTickEnabled(false);
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
}
```

### Memory Management
- Clear all object references in `OnReturnToPool`
- Reset any timers or delegates
- Ensure objects are in a clean state for reuse

## Limitations

- Objects must implement the `IPoolable` interface to be pooled
- Pool configuration must be set before world initialization
- Maximum pool sizes are enforced to prevent memory issues
- Objects in pool remain in memory until world destruction

## Troubleshooting

### Common Issues

**"Pool for [Class] at max capacity"**
- Increase the pool limit in your ObjectPoolConfig
- Check if objects are being returned to pool properly
- Verify lifespan settings are appropriate

**Objects not returning to pool automatically**
- Ensure lifespan is set to a value > 0
- Check that the object implements IPoolable correctly
- Verify the object hasn't been destroyed manually

**Blueprint nodes not appearing**
- Ensure the ObjectPool module is properly loaded
- Check that your Blueprint's parent class supports the required context
- Verify the object class implements the Poolable interface
