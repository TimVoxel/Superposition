#include <Particle.hpp>
#include <vector>
#include <random> 
#include <WaveFunction.hpp>

constexpr float amplitude = 0.0005f;
constexpr float frequency = 50.0f;

class ParticleSystem
{
public:
    ParticleSystem(float simulationDuration, float maxSpawnRatePS);
    void spawn();
    void update(float deltaTime, bool shouldSpawn);
    const std::vector<Particle>& particles() const;

private:
    std::vector<Particle> particles_;
    std::mt19937 generator_{std::random_device{}()};
    std::uniform_real_distribution<float> phaseDistribution_{0, 1};
    WaveFunction waveFunction_{0.0f, 0.0f, 0.25f};
    float spawnAccumulator_ = 0.0f;
    float spawnRatePS_ = 0.0f;
    float spawnRateGrowthPS_ = 3.0f;
    float maxSpawnRatePS_ = 300.0f;
};