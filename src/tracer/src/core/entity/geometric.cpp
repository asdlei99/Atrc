#include <agz/tracer/core/entity.h>
#include <agz/tracer/core/geometry.h>
#include <agz/tracer/core/material.h>
#include <agz/tracer/core/medium.h>

AGZ_TRACER_BEGIN

class GeometricEntity : public Entity
{
    std::shared_ptr<const Geometry> geometry_;
    std::shared_ptr<const Material> material_;
    MediumInterface medium_interface_;

public:

    GeometricEntity(
        std::shared_ptr<const Geometry> geometry,
        std::shared_ptr<const Material> material,
        const MediumInterface &med,
        bool no_denoise)
    {
        geometry_ = std::move(geometry);
        material_ = std::move(material);
        medium_interface_ = med;
        
        set_no_denoise_flag(no_denoise);
    }

    bool has_intersection(const Ray &r) const noexcept override
    {
        return geometry_->has_intersection(r);
    }

    bool closest_intersection(const Ray &r, EntityIntersection *inct) const noexcept override
    {
        if(!geometry_->closest_intersection(r, inct))
            return false;
        inct->entity     = this;
        inct->material   = material_.get();

        inct->medium_in  = medium_interface_.in.get();
        inct->medium_out = medium_interface_.out.get();

        return true;
    }

    AABB world_bound() const noexcept override
    {
        return geometry_->world_bound();
    }

    const AreaLight *as_light() const noexcept override
    {
        return nullptr;
    }

    AreaLight *as_light() noexcept override
    {
        return nullptr;
    }
};

std::shared_ptr<Entity> create_geometric(
    std::shared_ptr<const Geometry> geometry,
    std::shared_ptr<const Material> material,
    const MediumInterface &med,
    bool no_denoise)
{
    return std::make_shared<GeometricEntity>(
        std::move(geometry), std::move(material), med, no_denoise);
}

AGZ_TRACER_END