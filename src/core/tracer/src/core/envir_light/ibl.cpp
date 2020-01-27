﻿#include <agz/tracer/core/light.h>
#include <agz/tracer/core/texture2d.h>
#include <agz/utility/misc.h>
#include <agz/utility/texture.h>

#include "./env_sampler.h"

AGZ_TRACER_BEGIN

class IBL : public EnvirLight
{
    std::shared_ptr<const Texture2D> tex_;
    Vec3 up_ = Vec3(0, 0, 1);

    Spectrum avg_rad_;

    std::unique_ptr<EnvironmentLightSampler> sampler_;

public:

    IBL(std::shared_ptr<const Texture2D> tex, const Vec3 &up)
    {
        tex_     = tex;
        up_      = up;
        sampler_ = std::make_unique<EnvironmentLightSampler>(tex_);

        const int tex_width = tex_->width(), tex_height = tex_->height();
        for(int y = 0; y < tex_height; ++y)
        {
            const real v0 = real(y) / tex_height;
            const real v1 = real(y + 1) / tex_height;

            for(int x = 0; x < tex_width; ++x)
            {
                const real u0 = real(x) / tex_width;
                const real u1 = real(x + 1) / tex_width;

                const real delta_area = std::abs(2 * PI_r * (u1 - u0) * (std::cos(PI_r * v1) - std::cos(PI_r * v0)));
                const real u = (u0 + u1) / 2, v = (v0 + v1) / 2;
                avg_rad_ += PI_r * delta_area * tex_->sample_spectrum({ u, v });
            }
        }
    }

    LightSampleResult sample(const Vec3 &ref, const Sample5 &sam) const noexcept override
    {
        const auto [dir, pdf] = sampler_->sample({ sam.u, sam.v, sam.w });

        LightSampleResult ret;
        ret.ref      = ref;
        ret.pos      = emit_pos(ref, dir).pos;
        ret.nor      = -dir;
        ret.radiance = radiance(ref, dir);
        ret.pdf      = pdf;

        return ret;
    }

    real pdf(const Vec3 &ref, const Vec3 &ref_to_light) const noexcept override
    {
        return sampler_->pdf(ref_to_light);
    }

    LightEmitResult emit(const Sample5 &sam) const noexcept override
    {
        auto [dir, pdf_dir] = sampler_->sample({ sam.u, sam.v, sam.w });
        dir = -dir;

        const Vec2 disk_sam = math::distribution::uniform_on_unit_disk(sam.r, sam.s);
        const Coord dir_coord = Coord::from_z(dir);
        const Vec3 pos = world_centre_ + (disk_sam.x * dir_coord.x + disk_sam.y * dir_coord.y - dir) * world_radius_;

        LightEmitResult ret;
        ret.pos       = pos;
        ret.dir       = dir;
        ret.nor       = dir.normalize();
        ret.radiance  = radiance(world_centre_, -dir);
        ret.pdf_pos   = 1 / (PI_r * world_radius_ * world_radius_);
        ret.pdf_dir   = pdf_dir;

        return ret;
    }

    LightEmitPDFResult emit_pdf(const Vec3 &position, const Vec3 &direction, const Vec3 &normal) const noexcept override
    {
        const real pdf_pos = 1 / (PI_r * world_radius_ * world_radius_);
        const real pdf_dir = sampler_->pdf(-direction);
        return { pdf_pos, pdf_dir };
    }

    LightEmitPosResult emit_pos(const Vec3 &ref, const Vec3 &ref_to_light) const noexcept override
    {
        // o: world_center
        // r: world_radius
        // x: ref
        // d: ref_to_light.normalize()
        // o + r * (u * ex + v * ey + d) = x + d * t
        // solve [u, v, t] and ans = ref + ref_to_light * t

        // => [a b c][u v t]^T = m
        // where a = r * ex
        //       b = r * ey
        //       c = -d
        //       m = x - o + r * d

        const auto [ex, ey, d] = Coord::from_z(ref_to_light);

        const Vec3 a = world_radius_ * ex;
        const Vec3 b = world_radius_ * ey;
        const Vec3 c = -d;
        const Vec3 m = ref - world_centre_ - world_radius_ * d;

        const real det  = Mat3::from_cols(a, b, c).det();
        const real tdet = Mat3::from_cols(a, b, m).det();

        if(std::abs(det) < EPS)
            return { world_centre_, -ref_to_light };

        const real t = tdet / det;
        const Vec3 pos = ref + t * d;

        return { pos, c };
    }

    Spectrum power() const noexcept override
    {
        const real radius = world_radius_;
        return avg_rad_ * radius * radius;
    }

    Spectrum radiance(const Vec3 &ref, const Vec3 &ref_to_light) const noexcept override
    {
        const Vec3 dir   = Coord::from_z(up_).global_to_local(ref_to_light).normalize();
        const real phi   = local_angle::phi(dir);
        const real theta = local_angle::theta(dir);
        const real u     = math::clamp<real>(phi / (2 * PI_r), 0, 1);
        const real v     = math::clamp<real>(theta / PI_r, 0, 1);
        return tex_->sample_spectrum({ u, v });
    }
};

std::shared_ptr<EnvirLight> create_ibl_light(
    std::shared_ptr<const Texture2D> tex,
    const Vec3 &up)
{
    return std::make_shared<IBL>(tex, up);
}

AGZ_TRACER_END