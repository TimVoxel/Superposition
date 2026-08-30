#include <ParticleSystemLink.hpp>

void ParticleSystemLink::start(float currentTime)
{
    onStart(currentTime);

    for (std::unique_ptr<SceneObject>& child : children_)
    {
        SceneObject* c = child.get();
        if (c != first_ && c != second_)
        {
            child->start(currentTime);
        }
    }
}

void ParticleSystemLink::update(float deltaTime, float currentTime)
{
    applyAnimation(currentTime);
    onUpdate(deltaTime, currentTime);

    for (std::unique_ptr<SceneObject>& child : children_)
    {
        SceneObject* c = child.get();
        if (c != first_ && c != second_)
        {
            child->update(deltaTime, currentTime);
        }
    }
}

void ParticleSystemLink::onStart(float currentTime)
{
    startTime_ = currentTime;
    isActive_ = true;
    spawnRatePS_ = 0.0f;
    spawnAccumulator_ = 1.0f;
    first_->start(currentTime);
    second_->start(currentTime);
}

void ParticleSystemLink::onUpdate(float deltaTime, float currentTime)
{
    if (!isActive_)
    {
        return;
    }

    const float elapsedTime = currentTime - startTime_;
    const ParticleSystemConfig& firstConfig = first_->config();

    waveFunction_->update(deltaTime);
    first_->applyAnimation(currentTime);
    second_->applyAnimation(currentTime);

    if (elapsedTime < firstConfig.durationSeconds)
    {
        spawnRatePS_ = std::min(
            spawnRatePS_ +
            (firstConfig.maxSpawnRatePS / firstConfig.durationSeconds) * deltaTime,
            firstConfig.maxSpawnRatePS
        );

        spawnAccumulator_ += deltaTime * spawnRatePS_;

        while (spawnAccumulator_ >= 1.0f)
        {
            auto [x, y] = waveFunction_->sample();
            first_->spawn(x, y);
            second_->spawn(-x, -y);
            spawnAccumulator_ -= 1.0f;
        }
    }

    first_->updateExisting(deltaTime);
    second_->updateExisting(deltaTime);
}

const ParticleSystem* ParticleSystemLink::first() const { return first_; }
const ParticleSystem* ParticleSystemLink::second() const { return second_; }

std::unique_ptr<ParticleSystemLink> ParticleSystemLink::fromJson(
    const nlohmann::json& json)
{
    auto firstObject = SceneObject::fromJson(json.at("first"));
    auto secondObject = SceneObject::fromJson(json.at("second"));

    auto* first = dynamic_cast<ParticleSystem*>(firstObject.get());
    auto* second = dynamic_cast<ParticleSystem*>(secondObject.get());

    if (!first || !second)
    {
        throw std::runtime_error("ParticleSystemLink children first and second must be ParticleSystems");
    }

    auto link = std::make_unique<ParticleSystemLink>(
        std::unique_ptr<ParticleSystem>(static_cast<ParticleSystem*>(firstObject.release())),
        std::unique_ptr<ParticleSystem>(static_cast<ParticleSystem*>(secondObject.release())),
        WaveFunction::fromJson(json.at("waveFunction")));

    return link;
}