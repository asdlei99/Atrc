#include <Atrc/Material/Metal.h>
#include <Atrc/Math/Math.h>

AGZ_NS_BEG(Atrc)

MetalBxDF::MetalBxDF(const Intersection &inct, const Spectrum &color, Real roughness)
    : nor_(inct.nor), color_(color), roughness_(roughness)
{

}

BxDFType MetalBxDF::GetType() const
{
    return BXDF_REFLECTION | BXDF_SPECULAR;
}

Spectrum MetalBxDF::Eval(const Vec3r &wi, const Vec3r &wo) const
{
    return SPECTRUM::BLACK;
}

Option<BxDFSample> MetalBxDF::Sample(const Vec3r &wi, BxDFType type) const
{
    if(type.Contains(BXDF_REFLECTION | BXDF_SPECULAR) &&
        Dot(wi, nor_) > 0.0)
    {
        Vec3r dir = 2.0 * Dot(nor_, wi) * nor_ - wi;
        if(roughness_)
            dir = (dir + roughness_ * CommonSampler::Uniform_InUnitSphere::Sample().sample).Normalize();
        Real cos = Dot(dir, nor_);
        if(cos <= 0.0)
            return None;
        return BxDFSample{ dir, color_ / SS(cos), 1.0 };
    }
    return None;
}

MetalMaterial::MetalMaterial(const Spectrum &color, Real roughness)
    : color_(color), roughness_(roughness)
{
    
}

RC<BxDF> MetalMaterial::GetBxDF(const Intersection &inct) const
{
    return NewRC<MetalBxDF>(inct, color_, roughness_);
}

AGZ_NS_END(Atrc)
