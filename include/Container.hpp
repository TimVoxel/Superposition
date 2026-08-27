#pragma once

#include <SceneObject.hpp>

class Container : public SceneObject
{
public:
    Container(Transform transform = {{0.0f, 0.0f}, { 1.0f, 1.0f}}, SceneObject* parent = nullptr)
        : SceneObject(transform, parent) {}
protected:
    void onStart(float currentTime) override {}
    void onUpdate(float deltaTime, float currentTime) override {}
};