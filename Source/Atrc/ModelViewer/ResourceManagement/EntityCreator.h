#pragma once

#include <Atrc/ModelViewer/ResourceManagement/ResourceManager.h>

void RegisterEntityCreators(ResourceManager &rscMgr);

void BeginEntityRendering();

void EndEntityRendering();

class GeometricDiffuseLightCreator : public EntityCreator
{
public:

    GeometricDiffuseLightCreator() : EntityCreator("diffuse") { }

    std::shared_ptr<EntityInstance> Create(std::string name) const override;
};

class GeometricEntityCreator : public EntityCreator
{
public:

    GeometricEntityCreator() : EntityCreator("geometric") { }

    std::shared_ptr<EntityInstance> Create(std::string name) const override;
};
