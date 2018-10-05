#include <Atrc/Atrc.h>

using namespace std;
using namespace Atrc;

constexpr uint32_t SCR_W = 640;
constexpr uint32_t SCR_H = 480;

constexpr Real SCR_ASPECT_RATIO = static_cast<Real>(SCR_W) / SCR_H;

RenderTarget<Color3b> ToSavedImage(const RenderTarget<Color3f> &origin, float gamma)
{
    float index = 1.0f / gamma;
    return origin.Map([=](const Color3f &c)
    {
        return c.Map([=](float f)
        {
            return static_cast<uint8_t>(
                Clamp(Pow(f, index), 0.0f, 1.0f) * 255);
        });
    });
}

int main()
{
    const Vec3r eye = { -5.0, 0.0, 0.0 };
    const Vec3r dir = (Vec3r(0.0) - eye).Normalize();
    PerspectiveCamera camera(
        eye, dir, { 0.0, 0.0, 1.0 },
        Degr(90.0), SCR_ASPECT_RATIO);

    ColoredSky sky({ 0.4f, 0.7f, 0.9f }, { 1.0f, 1.0f, 1.0f });
    MatGeoEntity<Sphere> ground(NewRC<DiffuseMaterial>(Spectrum(0.4f, 0.8f, 0.4f)),
                                200.0, Transform::Translate({ 0.0, 0.0, -201.0 }));
    MatGeoEntity<Sphere> centreBall(NewRC<DiffuseMaterial>(Spectrum(0.7f, 0.7f, 0.7f)),
                                    1.0, TRANSFORM_IDENTITY);
    MatGeoEntity<Sphere> rightMetalSphere(NewRC<MetalMaterial>(Spectrum(1.0f, 0.3f, 0.3f), 0.2),
                                          1.0, Transform::Translate({ 0.0, -2.0, 0.0 }));
    MatGeoEntity<Sphere> leftGlassSphere(NewRC<GlassMaterial>(Spectrum(0.8f, 0.8f, 0.8f)),
                                         1.0, Transform::Translate({ 0.0, 2.0, 0.0 }));

    Scene scene;
    scene.camera = &camera;
    scene.entities = { 
        &ground,
        &centreBall, &leftGlassSphere, &rightMetalSphere,
        &sky };

    RenderTarget<Color3f> renderTarget(SCR_W, SCR_H);

    //AmbientIntegrator integrator;
    PathTracer integrator(10);

    //ParallelRenderer<Native1sppSubareaRenderer> renderer(-1);
    ParallelRenderer<JitteredSubareaRenderer> renderer(4, 20);
    renderer.Render(scene, integrator, renderTarget);

    AGZ::Tex::TextureFile::WriteRGBToPNG("Output.png", ToSavedImage(renderTarget, 2.2f));
}
