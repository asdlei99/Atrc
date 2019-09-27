#include <agz/tracer/core/bsdf.h>
#include <agz/tracer/core/bssrdf.h>
#include <agz/tracer/core/entity.h>
#include <agz/tracer/core/light.h>
#include <agz/tracer/core/intersection.h>
#include <agz/tracer/core/material.h>
#include <agz/tracer/core/path_tracing_integrator.h>
#include <agz/tracer/core/sampler.h>
#include <agz/tracer/core/scattering.h>
#include <agz/tracer/core/scene.h>

AGZ_TRACER_BEGIN

class NativeVolumetricPathTracingIntegrator : public PathTracingIntegrator
{
    int min_depth_ = 5;
    int max_depth_ = 20;
    real cont_prob_ = real(0.5);

    Spectrum background_rad_ = Spectrum(-1);
    bool has_background_ = false;

public:

    static std::string description()
    {
        return R"___(
native [PathTracingIntegrator]
    min_depth [int]         (optional; defaultly set to 5)   minimum path length before applying Russian Roulette
    max_depth [int]         (optional; defaultly set to 10)  maximal path length
    cont_prob [real]        (optional; defaultly set to 0.9) continuing probability in Russian Roulette
    background_rad [Spectrum]   (optional; defaultly set to -1)  background radiance; disable when set to negative value
)___";
    }

    void initialize(const Config &params, obj::ObjectInitContext&) override
    {
        AGZ_HIERARCHY_TRY

        init_customed_flag(params);

        min_depth_ = params.child_int_or("min_depth", 5);
        if(min_depth_ < 1)
            throw ObjectConstructionException("invalid min depth value: " + std::to_string(min_depth_));

        max_depth_ = params.child_int_or("max_depth", 10);
        if(max_depth_ < min_depth_)
            throw ObjectConstructionException("invalid max depth value: " + std::to_string(max_depth_));

        cont_prob_ = params.child_real_or("cont_prob", real(0.9));
        if(cont_prob_ < 0 || cont_prob_ > 1)
            throw ObjectConstructionException("invalid continue prob value: " + std::to_string(cont_prob_));

        if(params.find_child("background_rad"))
            background_rad_ = params.child_spectrum("background_rad");
        has_background_ = background_rad_.r >= 0 && background_rad_.g >= 0 && background_rad_.b >= 0;

        AGZ_HIERARCHY_WRAP("in initializing volumetric path tracing integrator")
    }

    Spectrum eval(GBufferPixel *gpixel, const Scene &scene, const Ray &ray, Sampler &sampler, Arena &arena) const override
    {
        auto env = scene.env();
        Spectrum ret, coef(1);
        Ray r = ray;

        for(int depth = 1; depth <= max_depth_; ++depth)
        {
            if(depth > min_depth_)
            {
                if(sampler.sample1().u > cont_prob_)
                    return ret;
                coef /= cont_prob_;
            }

            bool has_ent_inct; EntityIntersection ent_inct;
            SampleScatteringResult scattering_sample;
            scattering_sample.p_has_inct = &has_ent_inct;
            scattering_sample.p_inct     = &ent_inct;
            if(!scene.next_scattering_point(r, &scattering_sample, sampler.sample1(), arena))
            {
                if(has_background_ && depth == 1)
                    return background_rad_;
                ret += coef * env->radiance(r.o, r.d);
                return ret;
            }
            auto &pnt = scattering_sample.pnt;

            auto medium = pnt.medium(pnt.wr());
            Spectrum tr = medium->tr(r.o, pnt.pos());
            coef *= tr / scattering_sample.pdf;

            if(depth == 1 && has_ent_inct)
            {
                if(pnt.is_on_surface())
                    gpixel->albedo = pnt.bsdf()->albedo();
                else
                    gpixel->albedo = ent_inct.material->shade(ent_inct, arena).bsdf->albedo();
                gpixel->position = ent_inct.pos;
                gpixel->normal   = ent_inct.user_coord.z;
                gpixel->depth    = r.d.length() * ent_inct.t;
                gpixel->binary   = 1;
            }

            if(pnt.is_on_surface())
            {
                auto inct = pnt.as_entity_inct();
                if(auto light = inct.entity->as_light())
                    ret += coef * light->radiance(inct, inct.wr);
            }

            BSDFSampleResult bsdf_sample = pnt.sample(pnt.wr(), sampler.sample3(), TM_Radiance);
            if(!bsdf_sample.f || bsdf_sample.pdf < EPS)
                break;

            r = Ray(pnt.pos(), bsdf_sample.dir.normalize(), EPS);
            coef *= bsdf_sample.f * pnt.proj_wi_factor(bsdf_sample.dir) / bsdf_sample.pdf;
            if((std::max)({ coef.r, coef.g, coef.b }) < real(0.001))
                break;

            bool use_bssrdf = false;
            if(pnt.is_on_surface() && pnt.bssrdf())
            {
                auto &inct = pnt.as_entity_inct();
                bool wi_up = inct.geometry_coord.in_positive_z_hemisphere(bsdf_sample.dir);
                bool wo_up = inct.geometry_coord.in_positive_z_hemisphere(inct.wr);
                if(wo_up && !wi_up)
                    use_bssrdf = true;
            }

            if(use_bssrdf)
            {
                auto bssrdf = pnt.bssrdf();
                auto bssrdf_sample = bssrdf->sample(TM_Radiance, sampler.sample4(), arena);
                if(!bssrdf_sample.valid() || !bssrdf_sample.f)
                    break;
                coef *= bssrdf_sample.f / bssrdf_sample.pdf;

                auto new_pnt = ScatteringPoint(bssrdf_sample.inct, bssrdf_sample.bsdf);
                BSDFSampleResult new_bsdf_sample = new_pnt.sample(new_pnt.wr(), sampler.sample3(), TM_Radiance);
                if(!new_bsdf_sample.f || new_bsdf_sample.pdf < EPS)
                    break;

                r = Ray(new_pnt.pos(), new_bsdf_sample.dir.normalize(), EPS);
                coef *= new_bsdf_sample.f * new_pnt.proj_wi_factor(new_bsdf_sample.dir) / new_bsdf_sample.pdf;
                if((std::max)({ coef.r, coef.g, coef.b }) < real(0.001))
                    break;
            }
        }

        return ret;
    }
};

AGZT_IMPLEMENTATION(PathTracingIntegrator, NativeVolumetricPathTracingIntegrator, "native");

AGZ_TRACER_END