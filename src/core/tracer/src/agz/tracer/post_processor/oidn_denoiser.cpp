#ifdef USE_OIDN

#include <agz/tracer/core/post_processor.h>
#include <agz/tracer/utility/logger.h>

#include <OpenImageDenoise/oidn.hpp>

AGZ_TRACER_BEGIN

class OIDNDenoiser : public PostProcessor
{
    bool clamp_color_ = false;

public:

    explicit OIDNDenoiser(CustomedFlag customed_flag = DEFAULT_CUSTOMED_FLAG)
    {
        object_customed_flag_ = customed_flag;
    }

    static std::string description()
    {
        return R"___(
oidn_denoiser [PostProcessor]
    clamp [0/1] (optional; defaultly set to 0) clamp color to [0, 1] before denoising
)___";
    }

    void initialize(const Config &params, obj::ObjectInitContext&) override
    {
        AGZ_HIERARCHY_TRY

        init_customed_flag(params);

        clamp_color_ = params.child_int_or("clamp", 0) != 0;

        AGZ_HIERARCHY_WRAP("in initializing oidn denoiser")
    }

    void initialize(bool clamp_color)
    {
        clamp_color_ = clamp_color;
    }

    void process(ImageBuffer &image, GBuffer &gbuffer) override
    {
        AGZ_LOG1("oidn denoising");

        oidn::DeviceRef device = oidn::newDevice();
        device.commit();

        ImageBuffer output(image.height(), image.width());

        oidn::FilterRef filter = device.newFilter("RT");

        auto clamped_data = image.get_data();
        if(clamp_color_)
            clamped_data = clamped_data.map([](const Spectrum &c) { return c.clamp(0, 1); });
        filter.setImage("color", clamped_data.raw_data(), oidn::Format::Float3, image.width(), image.height());

        AlbedoBuffer::data_t clamped_albedo;
        if(gbuffer.albedo)
        {
            clamped_albedo = gbuffer.albedo->get_data().map([](const Spectrum &a) { return a.clamp(0, 1); });
            filter.setImage("albedo", clamped_albedo.raw_data(), oidn::Format::Float3, image.width(), image.height());
        }

        NormalBuffer::data_t clamped_normal;
        if(gbuffer.normal)
        {
            clamped_normal = gbuffer.normal->get_data().map([](const Vec3 &n)
            {
                if(!n)
                    return Vec3(1, 0, 0);
                return n.normalize().clamp(-1, 1);
            });
            filter.setImage("normal", clamped_normal.raw_data(), oidn::Format::Float3, image.width(), image.height());
        }

        filter.setImage("output", output.get_data().raw_data(), oidn::Format::Float3, image.width(), image.height());
        filter.set("hdr", true);
        filter.commit();
        filter.execute();

        const char* err;
        if(device.getError(err) != oidn::Error::None)
            AGZ_LOG0("oidn_denoiser error: ", err);
        else
            image = std::move(output);
    }
};

PostProcessor *create_oidn_denoiser(
    bool clamp_color,
    obj::Object::CustomedFlag customed_flag,
    Arena &arena)
{
    auto ret = arena.create<OIDNDenoiser>(customed_flag);
    ret->initialize(clamp_color);
    return ret;
}

AGZT_IMPLEMENTATION(PostProcessor, OIDNDenoiser, "oidn_denoiser")

AGZ_TRACER_END

#endif // #ifdef USE_OIDN