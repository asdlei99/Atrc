#pragma once

#include <Atrc/Common.h>
#include <Atrc/Integrator/Integrator.h>

AGZ_NS_BEG(Atrc)

class PathTracer
    : ATRC_IMPLEMENTS Integrator,
      ATRC_PROPERTY AGZ::Uncopiable
{
    uint32_t minDepth_;
    Real stopProb_;
    float stopCoef_;

    Spectrum Trace(const Scene &scene, const Ray &r, uint32_t depth) const;

public:

    explicit PathTracer(uint32_t minDepth, Real stopProb);

    Spectrum GetRadiance(const Scene &scene, const Ray &r) const override;
};

AGZ_NS_END(Atrc)
