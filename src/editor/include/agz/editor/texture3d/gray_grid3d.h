#pragma once

#include <agz/editor/texture3d/texture3d_common_params.h>
#include <agz/editor/texture3d/texture3d.h>
#include <agz/editor/ui/utility/collapsible.h>
#include <agz/editor/ui/utility/elided_label.h>

AGZ_EDITOR_BEGIN

class GrayGrid3DWidget : public Texture3DWidget
{
    Q_OBJECT

public:

    struct InitData
    {
        RC<const Image3D<real>> img_data;
        Texture3DCommonParamsWidget *adv = nullptr;
    };

    explicit GrayGrid3DWidget(const InitData &clone_state);

    ResourceWidget<tracer::Texture3D> *clone() override;

    Box<ResourceThumbnailProvider> get_thumbnail(
        int width, int height) const override;

    void save_asset(AssetSaver &saver) override;

    void load_asset(AssetLoader &loader) override;

    RC<tracer::ConfigNode> to_config(JSONExportContext &ctx) const override;

protected:

    void update_tracer_object_impl() override;

private:

    void do_update_tracer_object();

    void init_ui(const InitData &clone_state);

    void browse_filename();

    RC<const Image3D<real>> img_data_;

    QVBoxLayout *layout_ = nullptr;

    QPushButton *filename_button_  = nullptr;

    Collapsible                 *adv_section_ = nullptr;
    Texture3DCommonParamsWidget *adv_widget_  = nullptr;
};

class GrayGrid3DWidgetCreator : public Texture3DWidgetCreator
{
public:

    QString name() const override
    {
        return "Gray Grid";
    }

    ResourceWidget<tracer::Texture3D> *create_widget(
        ObjectContext &obj_ctx) const override;
};

AGZ_EDITOR_END