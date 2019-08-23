#pragma once

#include <agz/tracer/core/bsdf.h>
#include <agz/tracer/core/material.h>

AGZ_TRACER_BEGIN

class IdealBlack : public Material
{
    class IdealBlackBSDF : public BSDF
    {
    public:

        Spectrum eval(const Vec3&, const Vec3&, TransportMode) const noexcept override
        {
            return Spectrum();
        }

        BSDFSampleResult sample(const Vec3&, TransportMode mode, const Sample3&) const noexcept override
        {
            return BSDF_SAMPLE_RESULT_INVALID;
        }

        real pdf(const Vec3&, const Vec3&, TransportMode) const noexcept override
        {
            return 0;
        }

        Spectrum albedo() const noexcept override
        {
            return Spectrum();
        }

        bool is_delta() const noexcept override
        {
            return true;
        }

        bool is_black() const noexcept override
        {
            return true;
        }
    };

    IdealBlackBSDF bsdf_;

public:

    using Material::Material;

    static std::string description()
    {
        return R"___(ideal_black [Material])___";
    }

    void initialize(const Config&, obj::ObjectInitContext&) override
    {
        // do nothing
    }

    ShadingPoint shade(const EntityIntersection &inct, Arena&) const override
    {
        ShadingPoint shd;
        shd.bsdf = IDEAL_BLACK_BSDF_INSTANCE();
        return shd;
    }

    static const BSDF *IDEAL_BLACK_BSDF_INSTANCE()
    {
        static const IdealBlackBSDF bsdf;
        return &bsdf;
    }

    static const IdealBlack &IDEAL_BLACK_INSTANCE()
    {
        static IdealBlack ret;
        return ret;
    }
};

AGZ_TRACER_END