#pragma once

#include <animation/Animation.hpp>
#include <Transform.hpp>
#include <memory>

class SceneObject
{
public:
    virtual ~SceneObject() = default;

    virtual void start(float currentTime);
    virtual void update(float deltaTime, float currentTime);

    const Transform& transform() const { return transform_; }
    Transform& transform() { return transform_; }
    const Transform worldTransform() const;

    const SceneObject* parent() const { return parent_; }
    void setParent(SceneObject* parent) { parent_ = parent; }
    const std::vector<std::unique_ptr<SceneObject>>& children() const
    {
        return children_;
    }

    void addChild(std::unique_ptr<SceneObject> child)
    {
        child->setParent(this);
        children_.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<Animation>>& animations() const
    {
        return animations_;
    }
    
    static std::unique_ptr<SceneObject> fromJson(const nlohmann::json& json);

    void addAnimation(std::unique_ptr<Animation> animation)
    {
        animations_.push_back(std::move(animation));
    }

    void applyAnimation(float currentTime);

protected:
    explicit SceneObject(
        Transform transform = { { 0.0f, 0.0f}, {1.0f, 1.0f} },
        SceneObject* parent = nullptr)
        : transform_(transform), parent_(parent)
    {
    }

    virtual void onStart(float currentTime) = 0;
    virtual void onUpdate(float deltaTime, float currentTime) = 0;

    Transform transform_;
    Transform animatedTransform_;
    SceneObject* parent_;
    std::vector<std::unique_ptr<SceneObject>> children_;
    std::vector<std::unique_ptr<Animation>> animations_;
};