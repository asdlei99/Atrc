#include <QVBoxLayout>

#include <agz/editor/renderer/renderer_widget/ao_widget.h>
#include <agz/editor/renderer/renderer_widget/path_tracer_widget.h>

AGZ_EDITOR_BEGIN

void RendererWidgetFactory::add_creator(std::unique_ptr<RendererWidgetCreator> creator)
{
    assert(creator);
    std::string name = creator->name();
    name2creator_.insert(std::make_pair(std::move(name), std::move(creator)));
}

RendererWidget *RendererWidgetFactory::create_widget(std::string_view name, QWidget *parent) const
{
    auto it = name2creator_.find(name);
    if(it == name2creator_.end())
        return nullptr;
    return it->second->create_widget(parent);
}

std::vector<std::string> RendererWidgetFactory::get_type_names() const
{
    std::vector<std::string> ret;
    for(auto &it : name2creator_)
        ret.push_back(it.second->name());
    return ret;
}

RendererPanel::RendererPanel(QWidget *parent, const std::string &default_renderer_type)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    factory_.add_creator(std::make_unique<AOWidgetCreator>());
    factory_.add_creator(std::make_unique<PathTracerWidgetCreator>());

    const auto raw_type_names = factory_.get_type_names();

    QStringList renderer_types;
    renderer_types.reserve(static_cast<int>(raw_type_names.size()));
    for(auto &name : raw_type_names)
        renderer_types.push_back(QString::fromStdString(name));

    layout_ = new QVBoxLayout(this);

    renderer_type_selector_ = new QComboBox(this);
    renderer_type_selector_->addItems(renderer_types);
    renderer_type_selector_->setCurrentText(QString::fromStdString(default_renderer_type));

    renderer_widget_ = factory_.create_widget(default_renderer_type, this);
    if(!renderer_widget_)
        throw std::runtime_error("failed to create default renderer widget: " + default_renderer_type);

    connect(renderer_type_selector_, &QComboBox::currentTextChanged, [=](const QString &new_type)
    {
        delete renderer_widget_;
        renderer_widget_ = factory_.create_widget(new_type.toStdString(), this);
        layout_->addWidget(renderer_widget_);
        connect(renderer_widget_, &RendererWidget::change_renderer_params, this, &RendererPanel::on_change_renderer_params);
        emit change_renderer_type();
    });

    connect(renderer_widget_, &RendererWidget::change_renderer_params, this, &RendererPanel::on_change_renderer_params);

    layout_->addWidget(renderer_type_selector_);
    layout_->addWidget(renderer_widget_);
}

std::unique_ptr<Renderer> RendererPanel::create_renderer(
    std::shared_ptr<tracer::Scene> scene, const Vec2i &framebuffer_size, bool enable_preview) const
{
    if(renderer_widget_)
        return renderer_widget_->create_renderer(std::move(scene), framebuffer_size, enable_preview);
    return nullptr;
}

void RendererPanel::on_change_renderer_params()
{
    emit change_renderer_params();
}

AGZ_EDITOR_END