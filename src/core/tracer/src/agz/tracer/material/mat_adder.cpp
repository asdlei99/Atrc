#include <agz/tracer/core/material.h>
#include <agz/tracer/core/texture.h>
#include "./bsdf_adder.h"

AGZ_TRACER_BEGIN

class MatAdder : public Material
{
    std::vector<const Material*> mats_;

public:

    explicit MatAdder(CustomedFlag customed_flag = DEFAULT_CUSTOMED_FLAG)
    {
        object_customed_flag_ = customed_flag;
    }

    static std::string description()
    {
        return R"___(
add [Material]
    mats [Material] mixtured materials
)___";
    }

    void initialize(const Config &params, obj::ObjectInitContext &init_ctx) override
    {
        AGZ_HIERARCHY_TRY

        init_customed_flag(params);

        auto &mats = params.child_array("mats");
        if(!mats.size())
            throw ObjectConstructionException("empty material list");

        for(size_t i = 0; i < mats.size(); ++i)
        {
            auto mat = MaterialFactory.create(mats.at_group(i), init_ctx);
            mats_.push_back(mat);
        }

        AGZ_HIERARCHY_WRAP("in initializing linear mixtured material")
    }

    void initialize(std::vector<const Material*> &&mats)
    {
        mats_ = std::move(mats);
    }

    ShadingPoint shade(const EntityIntersection &inct, Arena &arena) const override
    {
        auto bsdf = arena.create<BSDFAdder>();
        for(auto mat : mats_)
        {
            auto subbsdf = mat->shade(inct, arena);
            bsdf->add(subbsdf.bsdf);
        }
        return { bsdf };
    }
};

Material *create_mat_adder(
    std::vector<const Material*> &&mats,
    obj::Object::CustomedFlag customed_flag,
    Arena &arena)
{
    auto ret = arena.create<MatAdder>(customed_flag);
    ret->initialize(std::move(mats));
    return ret;
}

AGZT_IMPLEMENTATION(Material, MatAdder, "add")

AGZ_TRACER_END