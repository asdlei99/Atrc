﻿#include <optional>

#include <agz/tracer/core/bsdf.h>
#include <agz/tracer/core/fresnel.h>
#include <agz/tracer/core/material.h>
#include <agz/tracer/core/texture.h>
#include <agz/utility/misc.h>

AGZ_TRACER_BEGIN

namespace
{
    class GlassBSDF : public LocalBSDF
    {
        const FresnelPoint *fresnel_point_;
        Spectrum color_reflection_;
        Spectrum color_refraction_;

        // 计算折射方向向量，发生全反射时返回std::nullopt
        static std::optional<Vec3> refr_dir(const Vec3 &nwo, const Vec3 &nor, real eta)
        {
            real cos_theta_i = std::abs(nwo.z);
            real sin_theta_i_2 = (std::max)(real(0), 1 - cos_theta_i * cos_theta_i);
            real sin_theta_t_2 = eta * eta * sin_theta_i_2;
            if(sin_theta_t_2 >= 1)
                return std::nullopt;
            real cosThetaT = std::sqrt(1 - sin_theta_t_2);
            return (eta * cos_theta_i - cosThetaT) * nor - eta * nwo;
        }

    public:

        GlassBSDF(const Coord &geometry_coord, const Coord &shading_coord,
                  const FresnelPoint *fresnel_point,
                  const Spectrum &color_reflection, const Spectrum &color_refraction) noexcept
            : LocalBSDF(geometry_coord, shading_coord), fresnel_point_(fresnel_point),
              color_reflection_(color_reflection), color_refraction_(color_refraction)
        {

        }

        Spectrum eval(const Vec3&, const Vec3&, TransportMode) const noexcept override
        {
            return Spectrum();
        }

        BSDFSampleResult sample(const Vec3 &out_dir, TransportMode transport_mode, const Sample3 &sam) const noexcept override
        {
            Vec3 nwo = shading_coord_.global_to_local(out_dir).normalize();
            Vec3 nor = nwo.z > 0 ? Vec3(0, 0, 1) : Vec3(0, 0, -1);

            auto fr = fresnel_point_->eval(nwo.z);
            if(sam.u < fr.r)
            {
                BSDFSampleResult ret;
                Vec3 local_in = Vec3(-nwo.x, -nwo.y, nwo.z);
                ret.dir      = shading_coord_.local_to_global(local_in);
                ret.f        = color_reflection_ * fr / std::abs(local_in.z);
                ret.pdf      = fr.r;
                ret.mode     = transport_mode;
                ret.is_delta = true;

                ret.f *= local_angle::normal_corr_factor(geometry_coord_, shading_coord_, ret.dir);
                if(has_inf(ret.f))
                    return BSDF_SAMPLE_RESULT_INVALID;

                return ret;
            }

            real eta_i = nwo.z > 0 ? fresnel_point_->eta_o() : fresnel_point_->eta_i();
            real eta_t = nwo.z > 0 ? fresnel_point_->eta_i() : fresnel_point_->eta_o();
            real eta = eta_i / eta_t;

            auto opt_wi = refr_dir(nwo, nor, eta);
            if(!opt_wi)
                return BSDF_SAMPLE_RESULT_INVALID;
            Vec3 nwi = opt_wi->normalize();

            real corr_factor = transport_mode == TM_Radiance ? eta * eta : real(1);

            BSDFSampleResult ret;
            ret.dir      = shading_coord_.local_to_global(nwi);
            ret.f        = corr_factor * color_refraction_ * (1 - fr.r) / std::abs(nwi.z);
            ret.pdf      = 1 - fr.r;
            ret.is_delta = true;
            ret.mode     = transport_mode;

            ret.f *= local_angle::normal_corr_factor(geometry_coord_, shading_coord_, ret.dir);
            if(has_inf(ret.f))
                return BSDF_SAMPLE_RESULT_INVALID;

            return ret;
        }

        real pdf(const Vec3&, const Vec3&, TransportMode) const noexcept override
        {
            return 0;
        }

        Spectrum albedo() const noexcept override
        {
            return real(0.5) * (color_reflection_ + color_refraction_);;
        }
    };
}

class Glass : public Material
{
    std::shared_ptr<const Fresnel> fresnel_;
    std::shared_ptr<const Texture> color_reflection_map_;
    std::shared_ptr<const Texture> color_refraction_map_;

public:

    void initialize(
        std::shared_ptr<const Texture> color_reflection_map,
        std::shared_ptr<const Texture> color_refraction_map,
        std::shared_ptr<const Fresnel> fresnel)
    {
        color_reflection_map_ = color_reflection_map;
        color_refraction_map_ = color_refraction_map;
        fresnel_   = fresnel;
    }

    ShadingPoint shade(const EntityIntersection &inct, Arena &arena) const override
    {
        ShadingPoint ret;

        auto fresnel_point = fresnel_->get_point(inct.uv, arena);
        auto color_reflection = color_reflection_map_->sample_spectrum(inct.uv);
        auto color_refraction = color_refraction_map_->sample_spectrum(inct.uv);
        ret.bsdf = arena.create<GlassBSDF>(
            inct.geometry_coord, inct.user_coord, fresnel_point,
            color_reflection, color_refraction);
        ret.shading_normal = inct.user_coord.z;

        return ret;
    }
};

std::shared_ptr<Material> create_glass(
    std::shared_ptr<const Texture> color_reflection_map,
    std::shared_ptr<const Texture> color_refraction_map,
    std::shared_ptr<const Fresnel> fresnel)
{
    auto ret = std::make_shared<Glass>();
    ret->initialize(color_reflection_map, color_refraction_map, fresnel);
    return ret;
}

AGZ_TRACER_END
