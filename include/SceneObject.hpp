#pragma once

class SceneObject
{
public:
    virtual ~SceneObject() = default;

    virtual void start(float currentTime) = 0;
    virtual void update(float deltaTime, float currentTime) = 0;
};