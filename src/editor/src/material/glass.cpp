#include <agz/editor/material/glass.h>
#include <agz/editor/material/material_thumbnail.h>
#include <agz/editor/texture2d/range.h>
#include <agz/editor/ui/utility/collapsible.h>
#include <agz/tracer/create/material.h>

AGZ_EDITOR_BEGIN

GlassWidget::GlassWidget(const InitData &init_data, ObjectContext &obj_ctx)
    : obj_ctx_(obj_ctx)
{
    color_      = init_data.color;
    ior_        = init_data.ior;
    color_refr_ = init_data.color_refr;

    if(!color_)
        color_ = new Texture2DSlot(obj_ctx_, "Constant");

    if(!ior_)
    {
        auto tex = new RangeWidget({ real(1.01), 3, real(1.5) });
        ior_ = new Texture2DSlot(obj_ctx_, "Range", tex, "Range");
    }

    if(!color_refr_)
        color_refr_ = new Texture2DSlot(obj_ctx_, "Constant");

    use_color_refr_ = new QCheckBox("Seperate Refraction Color");
    use_color_refr_->setChecked(init_data.use_color_refr);

    bssrdf_ = init_data.bssrdf;
    if(!bssrdf_)
        bssrdf_ = new BSSRDFSurfaceWidget<true, false>({}, obj_ctx);

    color_refr_->setDisabled(!init_data.use_color_refr);

    Collapsible *color_sec = new Collapsible(this, "Color");
    Collapsible *ior_sec   = new Collapsible(this, "IOR");
    Collapsible *adv_sec   = new Collapsible(this, "Advanced");
    Collapsible *sss_sec   = new Collapsible(this, "BSSRDF");

    QWidget     *adv_widget = new QWidget;
    QVBoxLayout *adv_layout = new QVBoxLayout(adv_widget);
    adv_layout->addWidget(use_color_refr_);
    adv_layout->addWidget(color_refr_);
    adv_widget->setContentsMargins(0, 0, 0, 0);
    adv_layout->setContentsMargins(0, 0, 0, 0);

    color_sec->set_content_widget(color_);
    ior_sec  ->set_content_widget(ior_);
    adv_sec  ->set_content_widget(adv_widget);
    sss_sec  ->set_content_widget(bssrdf_);

    color_sec->open();
    ior_sec  ->open();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(color_sec);
    layout->addWidget(ior_sec);
    layout->addWidget(adv_sec);
    layout->addWidget(sss_sec);

    setContentsMargins(0, 0, 0, 0);
    layout->setContentsMargins(0, 0, 0, 0);

    color_     ->set_dirty_callback([=] { set_dirty_flag(); });
    ior_       ->set_dirty_callback([=] { set_dirty_flag(); });
    color_refr_->set_dirty_callback([=] { set_dirty_flag(); });

    bssrdf_->set_dirty_callback([=] { set_dirty_flag(); });

    connect(use_color_refr_, &QCheckBox::stateChanged, [=](int)
    {
        color_refr_->setDisabled(!use_color_refr_->isChecked());
        set_dirty_flag();
    });

    do_update_tracer_object();
}

ResourceWidget<tracer::Material> *GlassWidget::clone()
{
    InitData init_data;
    init_data.color          = color_->clone();
    init_data.ior            = ior_  ->clone();
    init_data.color_refr     = color_refr_->clone();
    init_data.use_color_refr = use_color_refr_->isChecked();
    init_data.bssrdf         = bssrdf_->clone();
    return new GlassWidget(init_data, obj_ctx_);
}

Box<ResourceThumbnailProvider> GlassWidget::get_thumbnail(
    int width, int height) const
{
    return newBox<MaterialThumbnailProvider>(width, height, tracer_object_);
}

void GlassWidget::save_asset(AssetSaver &saver)
{
    color_->save_asset(saver);
    ior_->save_asset(saver);

    saver.write(uint8_t(use_color_refr_->isChecked() ? 1 : 0));
    color_refr_->save_asset(saver);

    bssrdf_->save_asset(saver);
}

void GlassWidget::load_asset(AssetLoader &loader)
{
    color_->load_asset(loader);
    ior_->load_asset(loader);

    const bool use_color_refr = loader.read<uint8_t>() != 0;
    use_color_refr_->setChecked(use_color_refr);
    color_refr_->load_asset(loader);

    bssrdf_->load_asset(loader);

    do_update_tracer_object();
}

RC<tracer::ConfigNode> GlassWidget::to_config(JSONExportContext &ctx) const
{
    auto grp = newRC<tracer::ConfigGroup>();
    grp->insert_str("type", "glass");

    if(use_color_refr_->isChecked())
    {
        grp->insert_child("color_reflection_map", color_->to_config(ctx));
        grp->insert_child("color_refraction_map", color_refr_->to_config(ctx));
    }
    else
    {
        grp->insert_child("color_map", color_->to_config(ctx));
    }

    grp->insert_child("ior", ior_->to_config(ctx));

    bssrdf_->to_config(*grp, ctx);

    return grp;
}

void GlassWidget::update_tracer_object_impl()
{
    do_update_tracer_object();
}

void GlassWidget::do_update_tracer_object()
{
    auto color      = color_->get_tracer_object();
    auto ior        = ior_->get_tracer_object();

    auto color_refr = color;
    if(use_color_refr_->isChecked())
        color_refr = color_refr_->get_tracer_object();

    auto bssrdf = bssrdf_->create_tracer_object({}, ior);

    tracer_object_ = create_glass(
        color, color_refr, ior, bssrdf);
}

ResourceWidget<tracer::Material> *GlassWidgetCreator::create_widget(
    ObjectContext &obj_ctx) const
{
    return new GlassWidget({}, obj_ctx);
}

AGZ_EDITOR_END
