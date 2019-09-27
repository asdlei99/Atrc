#include <agz/tracer/utility/triangle_aux.h>
#include "./transformed_geometry.h"

AGZ_TRACER_BEGIN

class Triangle : public TransformedGeometry
{
    Vec3 a_, b_a_, c_a_;
    Vec2 t_a_, t_b_a_, t_c_a_;
    Vec3 x_, z_;
    real surface_area_ = 1;

public:

    explicit Triangle(CustomedFlag customed_flag = DEFAULT_CUSTOMED_FLAG)
    {
        object_customed_flag_ = customed_flag;
    }

    static std::string description()
    {
        return R"___(
triangle [Geometry]
    transform [Transform[]] transform sequence
    A  [Vec3] triangle vertex A
    B  [Vec3] triangle vertex B
    C  [Vec3] triangle vertex C
    tA [Vec2] uv at A
    tB [Vec2] uv at B
    tC [Vec2] uv at C
)___";
    }

    void initialize(const Config &params, obj::ObjectInitContext &) override
    {
        AGZ_HIERARCHY_TRY

        init_customed_flag(params);

        auto local_to_world = params.child_transform3("transform");

        auto a = params.child_vec3("A");
        auto b = params.child_vec3("B");
        auto c = params.child_vec3("C");
        auto t_a = params.child_vec2("tA");
        auto t_b = params.child_vec2("tB");
        auto t_c = params.child_vec2("tC");

        initialize(a, b, c, t_a, t_b, t_c, local_to_world);

        AGZ_HIERARCHY_WRAP("in initializing triangle geometry object")
    }

    void initialize(
        const Vec3 &a, const Vec3 &b, const Vec3 &c,
        const Vec2 &t_a, const Vec2 &t_b, const Vec2 &t_c,
        const Transform3 &local_to_world)
    {
        AGZ_HIERARCHY_TRY

        init_transform(local_to_world);

        a_ = a;
        b_a_ = b - a;
        c_a_ = c - a;

        t_a_ = t_a;
        t_b_a_ = t_b - t_a;
        t_c_a_ = t_c - t_a;

        z_ = cross(b_a_, c_a_).normalize();
        x_ = dpdu_as_ex(b_a_, c_a_, t_b_a_, t_c_a_, z_);

        Vec3 world_b_a = local_to_world_.apply_to_vector(b_a_);
        Vec3 world_c_a = local_to_world_.apply_to_vector(c_a_);
        surface_area_ = triangle_area(world_b_a, world_c_a);

        AGZ_HIERARCHY_WRAP("in initializing triangle geometry object")
    }

    bool has_intersection(const Ray &r) const noexcept override
    {
        Ray local_r = to_local(r);
        return has_intersection_with_triangle(local_r, a_, b_a_, c_a_);
    }

    bool closest_intersection(const Ray &r, GeometryIntersection *inct) const noexcept override
    {
        Ray local_r = to_local(r);
        TriangleIntersectionRecord inct_rcd;
        if(!closest_intersection_with_triangle(local_r, a_, b_a_, c_a_, &inct_rcd))
            return false;

        inct->pos            = local_r.at(inct_rcd.t_ray);
        inct->geometry_coord = Coord(x_, cross(z_, x_), z_);
        inct->uv             = t_a_ + inct_rcd.uv.x * t_b_a_ + inct_rcd.uv.y * t_c_a_;
        inct->user_coord     = inct->geometry_coord;
        inct->wr             = -local_r.d;
        inct->t              = inct_rcd.t_ray;

        to_world(inct);

        return true;
    }

    AABB world_bound() const noexcept override
    {
        AABB ret;
        ret |= local_to_world_.apply_to_point(a_);
        ret |= local_to_world_.apply_to_point(a_ + b_a_);
        ret |= local_to_world_.apply_to_point(a_ + c_a_);
        for(int i = 0; i != 3; ++i)
        {
            if(ret.low[i] >= ret.high[i])
                ret.low[i] = ret.high[i] - real(0.1) * std::abs(ret.high[i]);
        }
        return ret;
    }

    real surface_area() const noexcept override
    {
        return surface_area_;
    }

    SurfacePoint sample(real *pdf, const Sample3 &sam) const noexcept override
    {
        Vec2 bi_coord = math::distribution::uniform_on_triangle(sam.u, sam.v);

        SurfacePoint spt;
        spt.pos            = a_ + bi_coord.x * b_a_ + bi_coord.y * c_a_;
        spt.geometry_coord = Coord(x_, cross(z_, x_), z_);
        spt.uv             = t_a_ + bi_coord.x * t_b_a_ + bi_coord.y * t_c_a_;
        spt.user_coord     = spt.geometry_coord;

        to_world(&spt);

        *pdf = 1 / surface_area_;
        return spt;
    }

    SurfacePoint sample(const Vec3&, real *pdf, const Sample3 &sam) const noexcept override
    {
        return sample(pdf, sam);
    }

    real pdf(const Vec3 &) const noexcept override
    {
        return 1 / surface_area_;
    }

    real pdf(const Vec3&, const Vec3 &sample) const noexcept override
    {
        return pdf(sample);
    }
};

Geometry *create_triangle(
    const Vec3 &a, const Vec3 &b, const Vec3 &c,
    const Vec2 &t_a, const Vec2 &t_b, const Vec2 &t_c,
    const Transform3 &local_to_world,
    obj::Object::CustomedFlag customed_flag,
    Arena &arena)
{
    auto ret = arena.create<Triangle>(customed_flag);
    ret->initialize(a, b, c, t_a, t_b, t_c, local_to_world);
    return ret;
}

AGZT_IMPLEMENTATION(Geometry, Triangle, "triangle")

AGZ_TRACER_END