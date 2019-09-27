#include <agz/tracer/core/texture.h>

AGZ_TRACER_BEGIN

class Constant : public Texture
{
    Spectrum texel_;

protected:

    Spectrum sample_spectrum_impl(const Vec2&) const noexcept override
    {
        return texel_;
    }

public:

    explicit Constant(CustomedFlag customed_flag = DEFAULT_CUSTOMED_FLAG)
    {
        object_customed_flag_ = customed_flag;
    }

    static std::string description()
    {
        return R"___(
constant [Texture]
    texel [Spectrum] texel value
)___";
    }

    void initialize(const Config &params, obj::ObjectInitContext &) override
    {
        AGZ_HIERARCHY_TRY

        init_customed_flag(params);

        init_common_params(params);
        texel_ = params.child_spectrum("texel");

        AGZ_HIERARCHY_WRAP("in initializing constant texture")
    }

    void initialize(const TextureCommonParams &common_params, const Spectrum &texel)
    {
        init_common_params(common_params);
        texel_ = texel;
    }

    int width() const noexcept override
    {
        return 1;
    }

    int height() const noexcept override
    {
        return 1;
    }
};

class StaticConstantTexture : public Texture
{
    Spectrum texel_;

    Spectrum sample_spectrum_impl(const Vec2&) const noexcept override
    {
        return texel_;
    }

public:

    explicit StaticConstantTexture(const Spectrum &texel) noexcept
        : texel_(texel)
    {
        
    }

    int width() const noexcept override
    {
        return 1;
    }

    int height() const noexcept override
    {
        return 1;
    }
};

#define ADD_STATIC_CONSTANT_TEXTURE(ClassName, NAME, TEXEL) \
    namespace \
    { \
        class ClassName : public obj::ObjectCreator<Texture> \
        { \
            static std::string desc_func() \
            { \
                return NAME " [Texture]"; \
            } \
        public: \
            ClassName() : ObjectCreator(NAME, &desc_func) \
            { \
            } \
            Texture *create(const ::agz::tracer::Config &, ::agz::tracer::obj::ObjectInitContext &) const override \
            { \
                static StaticConstantTexture ret(TEXEL); \
                return &ret; \
            } \
        }; \
        AGZT_ADD_CREATOR(Texture, ClassName) \
    }

Texture *create_constant_texture(
    const TextureCommonParams &common_params,
    const Spectrum &texel,
    obj::Object::CustomedFlag customed_flag,
    Arena &arena)
{
    auto ret = arena.create<Constant>(customed_flag);
    ret->initialize(common_params, texel);
    return ret;
}

Texture *create_constant_texture(
    const TextureCommonParams &common_params,
    real texel,
    obj::Object::CustomedFlag customed_flag,
    Arena &arena)
{
    return create_constant_texture(common_params, Spectrum(texel), customed_flag, arena);
}

AGZT_IMPLEMENTATION(Texture, Constant, "constant")

ADD_STATIC_CONSTANT_TEXTURE(AllZero, "all_zero", rgb255(0,   0,   0))
ADD_STATIC_CONSTANT_TEXTURE(AllOne,  "all_one",  rgb255(255, 255, 255))

// cancel the following comment to enable constant color table
/*
ADD_STATIC_CONSTANT_TEXTURE(Snow,               "snow",                 rgb255(255, 250, 250))
ADD_STATIC_CONSTANT_TEXTURE(GhostWhite,         "ghost_white",          rgb255(248, 248, 255))
ADD_STATIC_CONSTANT_TEXTURE(WhiteSmoke,         "white_smoke",          rgb255(245, 245, 245))
ADD_STATIC_CONSTANT_TEXTURE(Gainsboro,          "gainsboro",            rgb255(220, 220, 220))
ADD_STATIC_CONSTANT_TEXTURE(FloralWhite,        "floral_white",         rgb255(255, 250, 240))
ADD_STATIC_CONSTANT_TEXTURE(OldLace,            "old_lace",             rgb255(253, 245, 230))
ADD_STATIC_CONSTANT_TEXTURE(Linen,              "linen",                rgb255(250, 240, 230))
ADD_STATIC_CONSTANT_TEXTURE(AntiqueWhite,       "antique_white",        rgb255(250, 235, 215))
ADD_STATIC_CONSTANT_TEXTURE(PapayaWhip,         "papaya_whip",          rgb255(255, 239, 213))
ADD_STATIC_CONSTANT_TEXTURE(BlanchedAlmond,     "blanched_almond",      rgb255(255, 235, 205))
ADD_STATIC_CONSTANT_TEXTURE(Bisque,             "bisque",               rgb255(255, 228, 196))
ADD_STATIC_CONSTANT_TEXTURE(PeachPuff,          "peach_puff",           rgb255(255, 218, 185))
ADD_STATIC_CONSTANT_TEXTURE(NavajoWhite,        "navajo_white",         rgb255(255, 222, 173))
ADD_STATIC_CONSTANT_TEXTURE(Moccasin,           "moccasin",             rgb255(255, 228, 181))
ADD_STATIC_CONSTANT_TEXTURE(Cornsilk,           "cornsilk",             rgb255(255, 248, 220))
ADD_STATIC_CONSTANT_TEXTURE(Ivory,              "ivory",                rgb255(255, 255, 240))
ADD_STATIC_CONSTANT_TEXTURE(LemonChiffon,       "lemon_chiffon",        rgb255(255, 250, 205))
ADD_STATIC_CONSTANT_TEXTURE(Seashell,           "seashell",             rgb255(255, 245, 238))
ADD_STATIC_CONSTANT_TEXTURE(Honeydew,           "honeydew",             rgb255(240, 255, 240))
ADD_STATIC_CONSTANT_TEXTURE(MintCream,          "mint_cream",           rgb255(245, 255, 250))
ADD_STATIC_CONSTANT_TEXTURE(Azure,              "azure",                rgb255(240, 255, 255))
ADD_STATIC_CONSTANT_TEXTURE(AliceBlue,          "alice_blue",           rgb255(240, 248, 255))
ADD_STATIC_CONSTANT_TEXTURE(lavender,           "avender",              rgb255(230, 230, 250))
ADD_STATIC_CONSTANT_TEXTURE(LavenderBlush,      "lavender_blush",       rgb255(255, 240, 245))
ADD_STATIC_CONSTANT_TEXTURE(MistyRose,          "misty_rose",           rgb255(255, 228, 225))
ADD_STATIC_CONSTANT_TEXTURE(White,              "white",                rgb255(255, 255, 255))
ADD_STATIC_CONSTANT_TEXTURE(Black,              "black",                rgb255(0, 0, 0))
ADD_STATIC_CONSTANT_TEXTURE(DarkSlateGray,      "dark_slate_gray",      rgb255(47, 79, 79))
ADD_STATIC_CONSTANT_TEXTURE(DimGrey,            "dim_grey",             rgb255(105, 105, 105))
ADD_STATIC_CONSTANT_TEXTURE(SlateGrey,          "slate_grey",           rgb255(112, 128, 144))
ADD_STATIC_CONSTANT_TEXTURE(LightSlateGray,     "light_slate_gray",     rgb255(119, 136, 153))
ADD_STATIC_CONSTANT_TEXTURE(Grey,               "grey",                 rgb255(190, 190, 190))
ADD_STATIC_CONSTANT_TEXTURE(LightGray,          "light_gray",           rgb255(211, 211, 211))
ADD_STATIC_CONSTANT_TEXTURE(MidnightBlue,       "midnight_blue",        rgb255(25, 25, 112))
ADD_STATIC_CONSTANT_TEXTURE(NavyBlue,           "navy_blue",            rgb255(0, 0, 128))
ADD_STATIC_CONSTANT_TEXTURE(CornflowerBlue,     "cornflower_blue",      rgb255(100, 149, 237))
ADD_STATIC_CONSTANT_TEXTURE(DarkSlateBlue,      "dark_slate_blue",      rgb255(72, 61, 139))
ADD_STATIC_CONSTANT_TEXTURE(SlateBlue,          "slate_blue",           rgb255(106, 90, 205))
ADD_STATIC_CONSTANT_TEXTURE(MediumSlateBlue,    "medium_slate_blue",    rgb255(123, 104, 238))
ADD_STATIC_CONSTANT_TEXTURE(LightSlateBlue,     "light_slate_blue",     rgb255(132, 112, 255))
ADD_STATIC_CONSTANT_TEXTURE(MediumBlue,         "medium_blue",          rgb255(0, 0, 205))
ADD_STATIC_CONSTANT_TEXTURE(RoyalBlue,          "royal_blue",           rgb255(65, 105, 225))
ADD_STATIC_CONSTANT_TEXTURE(Blue,               "blue",                 rgb255(0, 0, 255))
ADD_STATIC_CONSTANT_TEXTURE(DodgerBlue,         "dodger_blue",          rgb255(30, 144, 255))
ADD_STATIC_CONSTANT_TEXTURE(DeepSkyBlue,        "deep_sky_blue",        rgb255(0, 191, 255))
ADD_STATIC_CONSTANT_TEXTURE(SkyBlue,            "sky_blue",             rgb255(135, 206, 235))
ADD_STATIC_CONSTANT_TEXTURE(LightSkyBlue,       "light_sky_blue",       rgb255(135, 206, 250))
ADD_STATIC_CONSTANT_TEXTURE(SteelBlue,          "steel_blue",           rgb255(70, 130, 180))
ADD_STATIC_CONSTANT_TEXTURE(LightSteelBlue,     "light_steel_blue",     rgb255(176, 196, 222))
ADD_STATIC_CONSTANT_TEXTURE(LightBlue,          "light_blue",           rgb255(173, 216, 230))
ADD_STATIC_CONSTANT_TEXTURE(PowderBlue,         "powder_blue",          rgb255(176, 224, 230))
ADD_STATIC_CONSTANT_TEXTURE(PaleTurquoise,      "pale_turquoise",       rgb255(175, 238, 238))
ADD_STATIC_CONSTANT_TEXTURE(DarkTurquoise,      "dark_turquoise",       rgb255(0, 206, 209))
ADD_STATIC_CONSTANT_TEXTURE(MediumTurquoise,    "medium_turquoise",     rgb255(72, 209, 204))
ADD_STATIC_CONSTANT_TEXTURE(Turquoise,          "turquoise",            rgb255(64, 224, 208))
ADD_STATIC_CONSTANT_TEXTURE(Cyan,               "cyan",                 rgb255(0, 255, 255))
ADD_STATIC_CONSTANT_TEXTURE(LightCyan,          "light_cyan",           rgb255(224, 255, 255))
ADD_STATIC_CONSTANT_TEXTURE(CadetBlue,          "cadet_blue",           rgb255(95, 158, 160))
ADD_STATIC_CONSTANT_TEXTURE(MediumAquamarine,   "medium_aquamarine",    rgb255(102, 205, 170))
ADD_STATIC_CONSTANT_TEXTURE(Aquamarine,         "aquamarine",           rgb255(127, 255, 212))
ADD_STATIC_CONSTANT_TEXTURE(DarkGreen,          "dark_green",           rgb255(0, 100, 0))
ADD_STATIC_CONSTANT_TEXTURE(DarkOliveGreen,     "dark_olive_green",     rgb255(85, 107, 47))
ADD_STATIC_CONSTANT_TEXTURE(DarkSeaGreen,       "dark_sea_green",       rgb255(143, 188, 143))
ADD_STATIC_CONSTANT_TEXTURE(SeaGreen,           "sea_green",            rgb255(46, 139, 87))
ADD_STATIC_CONSTANT_TEXTURE(MediumSeaGreen,     "medium_sea_green",     rgb255(60, 179, 113))
ADD_STATIC_CONSTANT_TEXTURE(LightSeaGreen,      "light_sea_green",      rgb255(32, 178, 170))
ADD_STATIC_CONSTANT_TEXTURE(PaleGreen,          "pale_green",           rgb255(152, 251, 152))
ADD_STATIC_CONSTANT_TEXTURE(SpringGreen,        "spring_green",         rgb255(0, 255, 127))
ADD_STATIC_CONSTANT_TEXTURE(LawnGreen,          "lawn_green",           rgb255(124, 252, 0))
ADD_STATIC_CONSTANT_TEXTURE(Green,              "green",                rgb255(0, 255, 0))
ADD_STATIC_CONSTANT_TEXTURE(Chartreuse,         "chartreuse",           rgb255(127, 255, 0))
ADD_STATIC_CONSTANT_TEXTURE(MedSpringGreen,     "med_spring_green",     rgb255(0, 250, 154))
ADD_STATIC_CONSTANT_TEXTURE(GreenYellow,        "green_yellow",         rgb255(173, 255, 47))
ADD_STATIC_CONSTANT_TEXTURE(LimeGreen,          "lime_green",           rgb255(50, 205, 50))
ADD_STATIC_CONSTANT_TEXTURE(YellowGreen,        "yellow_green",         rgb255(154, 205, 50))
ADD_STATIC_CONSTANT_TEXTURE(ForestGreen,        "forest_green",         rgb255(34, 139, 34))
ADD_STATIC_CONSTANT_TEXTURE(OliveDrab,          "olive_drab",           rgb255(107, 142, 35))
ADD_STATIC_CONSTANT_TEXTURE(DarkKhaki,          "dark_khaki",           rgb255(189, 183, 107))
ADD_STATIC_CONSTANT_TEXTURE(PaleGoldenrod,      "pale_goldenrod",       rgb255(238, 232, 170))
ADD_STATIC_CONSTANT_TEXTURE(LtGoldenrodYello,   "lt_goldenrod_yello",   rgb255(250, 250, 210))
ADD_STATIC_CONSTANT_TEXTURE(LightYellow,        "light_yellow",         rgb255(255, 255, 224))
ADD_STATIC_CONSTANT_TEXTURE(Yellow,             "yellow",               rgb255(255, 255, 0))
ADD_STATIC_CONSTANT_TEXTURE(Gold,               "gold",                 rgb255(255, 215, 0))
ADD_STATIC_CONSTANT_TEXTURE(LightGoldenrod,     "light_goldenrod",      rgb255(238, 221, 130))
ADD_STATIC_CONSTANT_TEXTURE(goldenrod,          "oldenrod",             rgb255(218, 165, 32))
ADD_STATIC_CONSTANT_TEXTURE(DarkGoldenrod,      "dark_goldenrod",       rgb255(184, 134, 11))
ADD_STATIC_CONSTANT_TEXTURE(RosyBrown,          "rosy_brown",           rgb255(188, 143, 143))
ADD_STATIC_CONSTANT_TEXTURE(IndianRed,          "indian_red",           rgb255(205, 92, 92))
ADD_STATIC_CONSTANT_TEXTURE(SaddleBrown,        "saddle_brown",         rgb255(139, 69, 19))
ADD_STATIC_CONSTANT_TEXTURE(Sienna,             "sienna",               rgb255(160, 82, 45))
ADD_STATIC_CONSTANT_TEXTURE(Peru,               "peru",                 rgb255(205, 133, 63))
ADD_STATIC_CONSTANT_TEXTURE(Burlywood,          "burlywood",            rgb255(222, 184, 135))
ADD_STATIC_CONSTANT_TEXTURE(Beige,              "beige",                rgb255(245, 245, 220))
ADD_STATIC_CONSTANT_TEXTURE(Wheat,              "wheat",                rgb255(245, 222, 179))
ADD_STATIC_CONSTANT_TEXTURE(SandyBrown,         "sandy_brown",          rgb255(244, 164, 96))
ADD_STATIC_CONSTANT_TEXTURE(Tan,                "tan",                  rgb255(210, 180, 140))
ADD_STATIC_CONSTANT_TEXTURE(Chocolate,          "chocolate",            rgb255(210, 105, 30))
ADD_STATIC_CONSTANT_TEXTURE(Firebrick,          "firebrick",            rgb255(178, 34, 34))
ADD_STATIC_CONSTANT_TEXTURE(Brown,              "brown",                rgb255(165, 42, 42))
ADD_STATIC_CONSTANT_TEXTURE(DarkSalmon,         "dark_salmon",          rgb255(233, 150, 122))
ADD_STATIC_CONSTANT_TEXTURE(Salmon,             "salmon",               rgb255(250, 128, 114))
ADD_STATIC_CONSTANT_TEXTURE(LightSalmon,        "light_salmon",         rgb255(255, 160, 122))
ADD_STATIC_CONSTANT_TEXTURE(Orange,             "orange",               rgb255(255, 165, 0))
ADD_STATIC_CONSTANT_TEXTURE(DarkOrange,         "dark_orange",          rgb255(255, 140, 0))
ADD_STATIC_CONSTANT_TEXTURE(Coral,              "coral",                rgb255(255, 127, 80))
ADD_STATIC_CONSTANT_TEXTURE(LightCoral,         "light_coral",          rgb255(240, 128, 128))
ADD_STATIC_CONSTANT_TEXTURE(Tomato,             "tomato",               rgb255(255, 99, 71))
ADD_STATIC_CONSTANT_TEXTURE(OrangeRed,          "orange_red",           rgb255(255, 69, 0))
ADD_STATIC_CONSTANT_TEXTURE(Red,                "red",                  rgb255(255, 0, 0))
ADD_STATIC_CONSTANT_TEXTURE(HotPink,            "hot_pink",             rgb255(255, 105, 180))
ADD_STATIC_CONSTANT_TEXTURE(DeepPink,           "deep_pink",            rgb255(255, 20, 147))
ADD_STATIC_CONSTANT_TEXTURE(Pink,               "pink",                 rgb255(255, 192, 203))
ADD_STATIC_CONSTANT_TEXTURE(LightPink,          "light_pink",           rgb255(255, 182, 193))
ADD_STATIC_CONSTANT_TEXTURE(PaleVioletRed,      "pale_violet_red",      rgb255(219, 112, 147))
ADD_STATIC_CONSTANT_TEXTURE(Maroon,             "maroon",               rgb255(176, 48, 96))
ADD_STATIC_CONSTANT_TEXTURE(MediumVioletRed,    "medium_violet_red",    rgb255(199, 21, 133))
ADD_STATIC_CONSTANT_TEXTURE(VioletRed,          "violet_red",           rgb255(208, 32, 144))
ADD_STATIC_CONSTANT_TEXTURE(Magenta,            "magenta",              rgb255(255, 0, 255))
ADD_STATIC_CONSTANT_TEXTURE(Violet,             "violet",               rgb255(238, 130, 238))
ADD_STATIC_CONSTANT_TEXTURE(Plum,               "plum",                 rgb255(221, 160, 221))
ADD_STATIC_CONSTANT_TEXTURE(Orchid,             "orchid",               rgb255(218, 112, 214))
ADD_STATIC_CONSTANT_TEXTURE(MediumOrchid,       "medium_orchid",        rgb255(186, 85, 211))
ADD_STATIC_CONSTANT_TEXTURE(DarkOrchid,         "dark_orchid",          rgb255(153, 50, 204))
ADD_STATIC_CONSTANT_TEXTURE(DarkViolet,         "dark_violet",          rgb255(148, 0, 211))
ADD_STATIC_CONSTANT_TEXTURE(BlueViolet,         "blue_violet",          rgb255(138, 43, 226))
ADD_STATIC_CONSTANT_TEXTURE(Purple,             "purple",               rgb255(160, 32, 240))
ADD_STATIC_CONSTANT_TEXTURE(MediumPurple,       "medium_purple",        rgb255(147, 112, 219))
ADD_STATIC_CONSTANT_TEXTURE(Thistle,            "thistle",              rgb255(216, 191, 216))
ADD_STATIC_CONSTANT_TEXTURE(Snow1,              "snow_1",               rgb255(255, 250, 250))
ADD_STATIC_CONSTANT_TEXTURE(Snow2,              "snow_2",               rgb255(238, 233, 233))
ADD_STATIC_CONSTANT_TEXTURE(Snow3,              "snow_3",               rgb255(205, 201, 201))
ADD_STATIC_CONSTANT_TEXTURE(Snow4,              "snow_4",               rgb255(139, 137, 137))
ADD_STATIC_CONSTANT_TEXTURE(Seashell1,          "seashell_1",           rgb255(255, 245, 238))
ADD_STATIC_CONSTANT_TEXTURE(Seashell2,          "seashell_2",           rgb255(238, 229, 222))
ADD_STATIC_CONSTANT_TEXTURE(Seashell3,          "seashell_3",           rgb255(205, 197, 191))
ADD_STATIC_CONSTANT_TEXTURE(Seashell4,          "seashell_4",           rgb255(139, 134, 130))
ADD_STATIC_CONSTANT_TEXTURE(AntiqueWhite1,      "antique_white_1",      rgb255(255, 239, 219))
ADD_STATIC_CONSTANT_TEXTURE(AntiqueWhite2,      "antique_white_2",      rgb255(238, 223, 204))
ADD_STATIC_CONSTANT_TEXTURE(AntiqueWhite3,      "antique_white_3",      rgb255(205, 192, 176))
ADD_STATIC_CONSTANT_TEXTURE(AntiqueWhite4,      "antique_white_4",      rgb255(139, 131, 120))
ADD_STATIC_CONSTANT_TEXTURE(Bisque1,            "bisque_1",             rgb255(255, 228, 196))
ADD_STATIC_CONSTANT_TEXTURE(Bisque2,            "bisque_2",             rgb255(238, 213, 183))
ADD_STATIC_CONSTANT_TEXTURE(Bisque3,            "bisque_3",             rgb255(205, 183, 158))
ADD_STATIC_CONSTANT_TEXTURE(Bisque4,            "bisque_4",             rgb255(139, 125, 107))
ADD_STATIC_CONSTANT_TEXTURE(PeachPuff1,         "peach_puff_1",         rgb255(255, 218, 185))
ADD_STATIC_CONSTANT_TEXTURE(PeachPuff2,         "peach_puff_2",         rgb255(238, 203, 173))
ADD_STATIC_CONSTANT_TEXTURE(PeachPuff3,         "peach_puff_3",         rgb255(205, 175, 149))
ADD_STATIC_CONSTANT_TEXTURE(PeachPuff4,         "peach_puff_4",         rgb255(139, 119, 101))
ADD_STATIC_CONSTANT_TEXTURE(NavajoWhite1,       "navajo_white_1",       rgb255(255, 222, 173))
ADD_STATIC_CONSTANT_TEXTURE(NavajoWhite2,       "navajo_white_2",       rgb255(238, 207, 161))
ADD_STATIC_CONSTANT_TEXTURE(NavajoWhite3,       "navajo_white_3",       rgb255(205, 179, 139))
ADD_STATIC_CONSTANT_TEXTURE(NavajoWhite4,       "navajo_white_4",       rgb255(139, 121, 94))
ADD_STATIC_CONSTANT_TEXTURE(LemonChiffon1,      "lemon_chiffon_1",      rgb255(255, 250, 205))
ADD_STATIC_CONSTANT_TEXTURE(LemonChiffon2,      "lemon_chiffon_2",      rgb255(238, 233, 191))
ADD_STATIC_CONSTANT_TEXTURE(LemonChiffon3,      "lemon_chiffon_3",      rgb255(205, 201, 165))
ADD_STATIC_CONSTANT_TEXTURE(LemonChiffon4,      "lemon_chiffon_4",      rgb255(139, 137, 112))
ADD_STATIC_CONSTANT_TEXTURE(Cornsilk1,          "cornsilk_1",           rgb255(255, 248, 220))
ADD_STATIC_CONSTANT_TEXTURE(Cornsilk2,          "cornsilk_2",           rgb255(238, 232, 205))
ADD_STATIC_CONSTANT_TEXTURE(Cornsilk3,          "cornsilk_3",           rgb255(205, 200, 177))
ADD_STATIC_CONSTANT_TEXTURE(Cornsilk4,          "cornsilk_4",           rgb255(139, 136, 120))
ADD_STATIC_CONSTANT_TEXTURE(Ivory1,             "ivory_1",              rgb255(255, 255, 240))
ADD_STATIC_CONSTANT_TEXTURE(Ivory2,             "ivory_2",              rgb255(238, 238, 224))
ADD_STATIC_CONSTANT_TEXTURE(Ivory3,             "ivory_3",              rgb255(205, 205, 193))
ADD_STATIC_CONSTANT_TEXTURE(Ivory4,             "ivory_4",              rgb255(139, 139, 131))
ADD_STATIC_CONSTANT_TEXTURE(Honeydew1,          "honeydew_1",           rgb255(240, 255, 240))
ADD_STATIC_CONSTANT_TEXTURE(Honeydew2,          "honeydew_2",           rgb255(224, 238, 224))
ADD_STATIC_CONSTANT_TEXTURE(Honeydew3,          "honeydew_3",           rgb255(193, 205, 193))
ADD_STATIC_CONSTANT_TEXTURE(Honeydew4,          "honeydew_4",           rgb255(131, 139, 131))
ADD_STATIC_CONSTANT_TEXTURE(LavenderBlush1,     "lavender_blush_1",     rgb255(255, 240, 245))
ADD_STATIC_CONSTANT_TEXTURE(LavenderBlush2,     "lavender_blush_2",     rgb255(238, 224, 229))
ADD_STATIC_CONSTANT_TEXTURE(LavenderBlush3,     "lavender_blush_3",     rgb255(205, 193, 197))
ADD_STATIC_CONSTANT_TEXTURE(LavenderBlush4,     "lavender_blush_4",     rgb255(139, 131, 134))
ADD_STATIC_CONSTANT_TEXTURE(MistyRose1,         "misty_rose_1",         rgb255(255, 228, 225))
ADD_STATIC_CONSTANT_TEXTURE(MistyRose2,         "misty_rose_2",         rgb255(238, 213, 210))
ADD_STATIC_CONSTANT_TEXTURE(MistyRose3,         "misty_rose_3",         rgb255(205, 183, 181))
ADD_STATIC_CONSTANT_TEXTURE(MistyRose4,         "misty_rose_4",         rgb255(139, 125, 123))
ADD_STATIC_CONSTANT_TEXTURE(Azure1,             "azure_1",              rgb255(240, 255, 255))
ADD_STATIC_CONSTANT_TEXTURE(Azure2,             "azure_2",              rgb255(224, 238, 238))
ADD_STATIC_CONSTANT_TEXTURE(Azure3,             "azure_3",              rgb255(193, 205, 205))
ADD_STATIC_CONSTANT_TEXTURE(Azure4,             "azure_4",              rgb255(131, 139, 139))
ADD_STATIC_CONSTANT_TEXTURE(SlateBlue1,         "slate_blue_1",         rgb255(131, 111, 255))
ADD_STATIC_CONSTANT_TEXTURE(SlateBlue2,         "slate_blue_2",         rgb255(122, 103, 238))
ADD_STATIC_CONSTANT_TEXTURE(SlateBlue3,         "slate_blue_3",         rgb255(105, 89, 205))
ADD_STATIC_CONSTANT_TEXTURE(SlateBlue4,         "slate_blue_4",         rgb255(71, 60, 139))
ADD_STATIC_CONSTANT_TEXTURE(RoyalBlue1,         "royal_blue_1",         rgb255(72, 118, 255))
ADD_STATIC_CONSTANT_TEXTURE(RoyalBlue2,         "royal_blue_2",         rgb255(67, 110, 238))
ADD_STATIC_CONSTANT_TEXTURE(RoyalBlue3,         "royal_blue_3",         rgb255(58, 95, 205))
ADD_STATIC_CONSTANT_TEXTURE(RoyalBlue4,         "royal_blue_4",         rgb255(39, 64, 139))
ADD_STATIC_CONSTANT_TEXTURE(Blue1,              "blue_1",               rgb255(0, 0, 255))
ADD_STATIC_CONSTANT_TEXTURE(Blue2,              "blue_2",               rgb255(0, 0, 238))
ADD_STATIC_CONSTANT_TEXTURE(Blue3,              "blue_3",               rgb255(0, 0, 205))
ADD_STATIC_CONSTANT_TEXTURE(Blue4,              "blue_4",               rgb255(0, 0, 139))
ADD_STATIC_CONSTANT_TEXTURE(DodgerBlue1,        "dodger_blue_1",        rgb255(30, 144, 255))
ADD_STATIC_CONSTANT_TEXTURE(DodgerBlue2,        "dodger_blue_2",        rgb255(28, 134, 238))
ADD_STATIC_CONSTANT_TEXTURE(DodgerBlue3,        "dodger_blue_3",        rgb255(24, 116, 205))
ADD_STATIC_CONSTANT_TEXTURE(DodgerBlue4,        "dodger_blue_4",        rgb255(16, 78, 139))
ADD_STATIC_CONSTANT_TEXTURE(SteelBlue1,         "steel_blue_1",         rgb255(99, 184, 255))
ADD_STATIC_CONSTANT_TEXTURE(SteelBlue2,         "steel_blue_2",         rgb255(92, 172, 238))
ADD_STATIC_CONSTANT_TEXTURE(SteelBlue3,         "steel_blue_3",         rgb255(79, 148, 205))
ADD_STATIC_CONSTANT_TEXTURE(SteelBlue4,         "steel_blue_4",         rgb255(54, 100, 139))
ADD_STATIC_CONSTANT_TEXTURE(DeepSkyBlue1,       "deep_sky_blue_1",      rgb255(0, 191, 255))
ADD_STATIC_CONSTANT_TEXTURE(DeepSkyBlue2,       "deep_sky_blue_2",      rgb255(0, 178, 238))
ADD_STATIC_CONSTANT_TEXTURE(DeepSkyBlue3,       "deep_sky_blue_3",      rgb255(0, 154, 205))
ADD_STATIC_CONSTANT_TEXTURE(DeepSkyBlue4,       "deep_sky_blue_4",      rgb255(0, 104, 139))
ADD_STATIC_CONSTANT_TEXTURE(SkyBlue1,           "sky_blue_1",           rgb255(135, 206, 255))
ADD_STATIC_CONSTANT_TEXTURE(SkyBlue2,           "sky_blue_2",           rgb255(126, 192, 238))
ADD_STATIC_CONSTANT_TEXTURE(SkyBlue3,           "sky_blue_3",           rgb255(108, 166, 205))
ADD_STATIC_CONSTANT_TEXTURE(SkyBlue4,           "sky_blue_4",           rgb255(74, 112, 139))
ADD_STATIC_CONSTANT_TEXTURE(LightSkyBlue1,      "light_sky_blue_1",     rgb255(176, 226, 255))
ADD_STATIC_CONSTANT_TEXTURE(LightSkyBlue2,      "light_sky_blue_2",     rgb255(164, 211, 238))
ADD_STATIC_CONSTANT_TEXTURE(LightSkyBlue3,      "light_sky_blue_3",     rgb255(141, 182, 205))
ADD_STATIC_CONSTANT_TEXTURE(LightSkyBlue4,      "light_sky_blue_4",     rgb255(96, 123, 139))
ADD_STATIC_CONSTANT_TEXTURE(SlateGray1,         "slate_gray_1",         rgb255(198, 226, 255))
ADD_STATIC_CONSTANT_TEXTURE(SlateGray2,         "slate_gray_2",         rgb255(185, 211, 238))
ADD_STATIC_CONSTANT_TEXTURE(SlateGray3,         "slate_gray_3",         rgb255(159, 182, 205))
ADD_STATIC_CONSTANT_TEXTURE(SlateGray4,         "slate_gray_4",         rgb255(108, 123, 139))
ADD_STATIC_CONSTANT_TEXTURE(LightSteelBlue1,    "light_steel_blue_1",   rgb255(202, 225, 255))
ADD_STATIC_CONSTANT_TEXTURE(LightSteelBlue2,    "light_steel_blue_2",   rgb255(188, 210, 238))
ADD_STATIC_CONSTANT_TEXTURE(LightSteelBlue3,    "light_steel_blue_3",   rgb255(162, 181, 205))
ADD_STATIC_CONSTANT_TEXTURE(LightSteelBlue4,    "light_steel_blue_4",   rgb255(110, 123, 139))
ADD_STATIC_CONSTANT_TEXTURE(LightBlue1,         "light_blue_1",         rgb255(191, 239, 255))
ADD_STATIC_CONSTANT_TEXTURE(LightBlue2,         "light_blue_2",         rgb255(178, 223, 238))
ADD_STATIC_CONSTANT_TEXTURE(LightBlue3,         "light_blue_3",         rgb255(154, 192, 205))
ADD_STATIC_CONSTANT_TEXTURE(LightBlue4,         "light_blue_4",         rgb255(104, 131, 139))
ADD_STATIC_CONSTANT_TEXTURE(LightCyan1,         "light_cyan_1",         rgb255(224, 255, 255))
ADD_STATIC_CONSTANT_TEXTURE(LightCyan2,         "light_cyan_2",         rgb255(209, 238, 238))
ADD_STATIC_CONSTANT_TEXTURE(LightCyan3,         "light_cyan_3",         rgb255(180, 205, 205))
ADD_STATIC_CONSTANT_TEXTURE(LightCyan4,         "light_cyan_4",         rgb255(122, 139, 139))
ADD_STATIC_CONSTANT_TEXTURE(PaleTurquoise1,     "pale_turquoise_1",     rgb255(187, 255, 255))
ADD_STATIC_CONSTANT_TEXTURE(PaleTurquoise2,     "pale_turquoise_2",     rgb255(174, 238, 238))
ADD_STATIC_CONSTANT_TEXTURE(PaleTurquoise3,     "pale_turquoise_3",     rgb255(150, 205, 205))
ADD_STATIC_CONSTANT_TEXTURE(PaleTurquoise4,     "pale_turquoise_4",     rgb255(102, 139, 139))
ADD_STATIC_CONSTANT_TEXTURE(CadetBlue1,         "cadet_blue_1",         rgb255(152, 245, 255))
ADD_STATIC_CONSTANT_TEXTURE(CadetBlue2,         "cadet_blue_2",         rgb255(142, 229, 238))
ADD_STATIC_CONSTANT_TEXTURE(CadetBlue3,         "cadet_blue_3",         rgb255(122, 197, 205))
ADD_STATIC_CONSTANT_TEXTURE(CadetBlue4,         "cadet_blue_4",         rgb255(83, 134, 139))
ADD_STATIC_CONSTANT_TEXTURE(Turquoise1,         "turquoise_1",          rgb255(0, 245, 255))
ADD_STATIC_CONSTANT_TEXTURE(Turquoise2,         "turquoise_2",          rgb255(0, 229, 238))
ADD_STATIC_CONSTANT_TEXTURE(Turquoise3,         "turquoise_3",          rgb255(0, 197, 205))
ADD_STATIC_CONSTANT_TEXTURE(Turquoise4,         "turquoise_4",          rgb255(0, 134, 139))
ADD_STATIC_CONSTANT_TEXTURE(Cyan1,              "cyan_1",               rgb255(0, 255, 255))
ADD_STATIC_CONSTANT_TEXTURE(Cyan2,              "cyan_2",               rgb255(0, 238, 238))
ADD_STATIC_CONSTANT_TEXTURE(Cyan3,              "cyan_3",               rgb255(0, 205, 205))
ADD_STATIC_CONSTANT_TEXTURE(Cyan4,              "cyan_4",               rgb255(0, 139, 139))
ADD_STATIC_CONSTANT_TEXTURE(DarkSlateGray1,     "dark_slate_gray_1",    rgb255(151, 255, 255))
ADD_STATIC_CONSTANT_TEXTURE(DarkSlateGray2,     "dark_slate_gray_2",    rgb255(141, 238, 238))
ADD_STATIC_CONSTANT_TEXTURE(DarkSlateGray3,     "dark_slate_gray_3",    rgb255(121, 205, 205))
ADD_STATIC_CONSTANT_TEXTURE(DarkSlateGray4,     "dark_slate_gray_4",    rgb255(82, 139, 139))
ADD_STATIC_CONSTANT_TEXTURE(Aquamarine1,        "aquamarine_1",         rgb255(127, 255, 212))
ADD_STATIC_CONSTANT_TEXTURE(Aquamarine2,        "aquamarine_2",         rgb255(118, 238, 198))
ADD_STATIC_CONSTANT_TEXTURE(Aquamarine3,        "aquamarine_3",         rgb255(102, 205, 170))
ADD_STATIC_CONSTANT_TEXTURE(Aquamarine4,        "aquamarine_4",         rgb255(69, 139, 116))
ADD_STATIC_CONSTANT_TEXTURE(DarkSeaGreen1,      "dark_sea_green_1",     rgb255(193, 255, 193))
ADD_STATIC_CONSTANT_TEXTURE(DarkSeaGreen2,      "dark_sea_green_2",     rgb255(180, 238, 180))
ADD_STATIC_CONSTANT_TEXTURE(DarkSeaGreen3,      "dark_sea_green_3",     rgb255(155, 205, 155))
ADD_STATIC_CONSTANT_TEXTURE(DarkSeaGreen4,      "dark_sea_green_4",     rgb255(105, 139, 105))
ADD_STATIC_CONSTANT_TEXTURE(SeaGreen1,          "sea_green_1",          rgb255(84, 255, 159))
ADD_STATIC_CONSTANT_TEXTURE(SeaGreen2,          "sea_green_2",          rgb255(78, 238, 148))
ADD_STATIC_CONSTANT_TEXTURE(SeaGreen3,          "sea_green_3",          rgb255(67, 205, 128))
ADD_STATIC_CONSTANT_TEXTURE(SeaGreen4,          "sea_green_4",          rgb255(46, 139, 87))
ADD_STATIC_CONSTANT_TEXTURE(PaleGreen1,         "pale_green_1",         rgb255(154, 255, 154))
ADD_STATIC_CONSTANT_TEXTURE(PaleGreen2,         "pale_green_2",         rgb255(144, 238, 144))
ADD_STATIC_CONSTANT_TEXTURE(PaleGreen3,         "pale_green_3",         rgb255(124, 205, 124))
ADD_STATIC_CONSTANT_TEXTURE(PaleGreen4,         "pale_green_4",         rgb255(84, 139, 84))
ADD_STATIC_CONSTANT_TEXTURE(SpringGreen1,       "spring_green_1",       rgb255(0, 255, 127))
ADD_STATIC_CONSTANT_TEXTURE(SpringGreen2,       "spring_green_2",       rgb255(0, 238, 118))
ADD_STATIC_CONSTANT_TEXTURE(SpringGreen3,       "spring_green_3",       rgb255(0, 205, 102))
ADD_STATIC_CONSTANT_TEXTURE(SpringGreen4,       "spring_green_4",       rgb255(0, 139, 69))
ADD_STATIC_CONSTANT_TEXTURE(Green1,             "green_1",              rgb255(0, 255, 0))
ADD_STATIC_CONSTANT_TEXTURE(Green2,             "green_2",              rgb255(0, 238, 0))
ADD_STATIC_CONSTANT_TEXTURE(Green3,             "green_3",              rgb255(0, 205, 0))
ADD_STATIC_CONSTANT_TEXTURE(Green4,             "green_4",              rgb255(0, 139, 0))
ADD_STATIC_CONSTANT_TEXTURE(Chartreuse1,        "chartreuse_1",         rgb255(127, 255, 0))
ADD_STATIC_CONSTANT_TEXTURE(Chartreuse2,        "chartreuse_2",         rgb255(118, 238, 0))
ADD_STATIC_CONSTANT_TEXTURE(Chartreuse3,        "chartreuse_3",         rgb255(102, 205, 0))
ADD_STATIC_CONSTANT_TEXTURE(Chartreuse4,        "chartreuse_4",         rgb255(69, 139, 0))
ADD_STATIC_CONSTANT_TEXTURE(OliveDrab1,         "olive_drab_1",         rgb255(192, 255, 62))
ADD_STATIC_CONSTANT_TEXTURE(OliveDrab2,         "olive_drab_2",         rgb255(179, 238, 58))
ADD_STATIC_CONSTANT_TEXTURE(OliveDrab3,         "olive_drab_3",         rgb255(154, 205, 50))
ADD_STATIC_CONSTANT_TEXTURE(OliveDrab4,         "olive_drab_4",         rgb255(105, 139, 34))
ADD_STATIC_CONSTANT_TEXTURE(DarkOliveGreen1,    "dark_olive_green_1",   rgb255(202, 255, 112))
ADD_STATIC_CONSTANT_TEXTURE(DarkOliveGreen2,    "dark_olive_green_2",   rgb255(188, 238, 104))
ADD_STATIC_CONSTANT_TEXTURE(DarkOliveGreen3,    "dark_olive_green_3",   rgb255(162, 205, 90))
ADD_STATIC_CONSTANT_TEXTURE(DarkOliveGreen4,    "dark_olive_green_4",   rgb255(110, 139, 61))
ADD_STATIC_CONSTANT_TEXTURE(Khaki1,             "khaki_1",              rgb255(255, 246, 143))
ADD_STATIC_CONSTANT_TEXTURE(Khaki2,             "khaki_2",              rgb255(238, 230, 133))
ADD_STATIC_CONSTANT_TEXTURE(Khaki3,             "khaki_3",              rgb255(205, 198, 115))
ADD_STATIC_CONSTANT_TEXTURE(Khaki4,             "khaki_4",              rgb255(139, 134, 78))
ADD_STATIC_CONSTANT_TEXTURE(LightGoldenrod1,    "light_goldenrod_1",    rgb255(255, 236, 139))
ADD_STATIC_CONSTANT_TEXTURE(LightGoldenrod2,    "light_goldenrod_2",    rgb255(238, 220, 130))
ADD_STATIC_CONSTANT_TEXTURE(LightGoldenrod3,    "light_goldenrod_3",    rgb255(205, 190, 112))
ADD_STATIC_CONSTANT_TEXTURE(LightGoldenrod4,    "light_goldenrod_4",    rgb255(139, 129, 76))
ADD_STATIC_CONSTANT_TEXTURE(LightYellow1,       "light_yellow_1",       rgb255(255, 255, 224))
ADD_STATIC_CONSTANT_TEXTURE(LightYellow2,       "light_yellow_2",       rgb255(238, 238, 209))
ADD_STATIC_CONSTANT_TEXTURE(LightYellow3,       "light_yellow_3",       rgb255(205, 205, 180))
ADD_STATIC_CONSTANT_TEXTURE(LightYellow4,       "light_yellow_4",       rgb255(139, 139, 122))
ADD_STATIC_CONSTANT_TEXTURE(Yellow1,            "yellow_1",             rgb255(255, 255, 0))
ADD_STATIC_CONSTANT_TEXTURE(Yellow2,            "yellow_2",             rgb255(238, 238, 0))
ADD_STATIC_CONSTANT_TEXTURE(Yellow3,            "yellow_3",             rgb255(205, 205, 0))
ADD_STATIC_CONSTANT_TEXTURE(Yellow4,            "yellow_4",             rgb255(139, 139, 0))
ADD_STATIC_CONSTANT_TEXTURE(Gold1,              "gold_1",               rgb255(255, 215, 0))
ADD_STATIC_CONSTANT_TEXTURE(Gold2,              "gold_2",               rgb255(238, 201, 0))
ADD_STATIC_CONSTANT_TEXTURE(Gold3,              "gold_3",               rgb255(205, 173, 0))
ADD_STATIC_CONSTANT_TEXTURE(Gold4,              "gold_4",               rgb255(139, 117, 0))
ADD_STATIC_CONSTANT_TEXTURE(Goldenrod1,         "goldenrod_1",          rgb255(255, 193, 37))
ADD_STATIC_CONSTANT_TEXTURE(Goldenrod2,         "goldenrod_2",          rgb255(238, 180, 34))
ADD_STATIC_CONSTANT_TEXTURE(Goldenrod3,         "goldenrod_3",          rgb255(205, 155, 29))
ADD_STATIC_CONSTANT_TEXTURE(Goldenrod4,         "goldenrod_4",          rgb255(139, 105, 20))
ADD_STATIC_CONSTANT_TEXTURE(DarkGoldenrod1,     "dark_goldenrod_1",     rgb255(255, 185, 15))
ADD_STATIC_CONSTANT_TEXTURE(DarkGoldenrod2,     "dark_goldenrod_2",     rgb255(238, 173, 14))
ADD_STATIC_CONSTANT_TEXTURE(DarkGoldenrod3,     "dark_goldenrod_3",     rgb255(205, 149, 12))
ADD_STATIC_CONSTANT_TEXTURE(DarkGoldenrod4,     "dark_goldenrod_4",     rgb255(139, 101, 8))
ADD_STATIC_CONSTANT_TEXTURE(RosyBrown1,         "rosy_brown_1",         rgb255(255, 193, 193))
ADD_STATIC_CONSTANT_TEXTURE(RosyBrown2,         "rosy_brown_2",         rgb255(238, 180, 180))
ADD_STATIC_CONSTANT_TEXTURE(RosyBrown3,         "rosy_brown_3",         rgb255(205, 155, 155))
ADD_STATIC_CONSTANT_TEXTURE(RosyBrown4,         "rosy_brown_4",         rgb255(139, 105, 105))
ADD_STATIC_CONSTANT_TEXTURE(IndianRed1,         "indian_red_1",         rgb255(255, 106, 106))
ADD_STATIC_CONSTANT_TEXTURE(IndianRed2,         "indian_red_2",         rgb255(238, 99, 99))
ADD_STATIC_CONSTANT_TEXTURE(IndianRed3,         "indian_red_3",         rgb255(205, 85, 85))
ADD_STATIC_CONSTANT_TEXTURE(IndianRed4,         "indian_red_4",         rgb255(139, 58, 58))
ADD_STATIC_CONSTANT_TEXTURE(Sienna1,            "sienna_1",             rgb255(255, 130, 71))
ADD_STATIC_CONSTANT_TEXTURE(Sienna2,            "sienna_2",             rgb255(238, 121, 66))
ADD_STATIC_CONSTANT_TEXTURE(Sienna3,            "sienna_3",             rgb255(205, 104, 57))
ADD_STATIC_CONSTANT_TEXTURE(Sienna4,            "sienna_4",             rgb255(139, 71, 38))
ADD_STATIC_CONSTANT_TEXTURE(Burlywood1,         "burlywood_1",          rgb255(255, 211, 155))
ADD_STATIC_CONSTANT_TEXTURE(Burlywood2,         "burlywood_2",          rgb255(238, 197, 145))
ADD_STATIC_CONSTANT_TEXTURE(Burlywood3,         "burlywood_3",          rgb255(205, 170, 125))
ADD_STATIC_CONSTANT_TEXTURE(Burlywood4,         "burlywood_4",          rgb255(139, 115, 85))
ADD_STATIC_CONSTANT_TEXTURE(Wheat1,             "wheat_1",              rgb255(255, 231, 186))
ADD_STATIC_CONSTANT_TEXTURE(Wheat2,             "wheat_2",              rgb255(238, 216, 174))
ADD_STATIC_CONSTANT_TEXTURE(Wheat3,             "wheat_3",              rgb255(205, 186, 150))
ADD_STATIC_CONSTANT_TEXTURE(Wheat4,             "wheat_4",              rgb255(139, 126, 102))
ADD_STATIC_CONSTANT_TEXTURE(Tan1,               "tan_1",                rgb255(255, 165, 79))
ADD_STATIC_CONSTANT_TEXTURE(Tan2,               "tan_2",                rgb255(238, 154, 73))
ADD_STATIC_CONSTANT_TEXTURE(Tan3,               "tan_3",                rgb255(205, 133, 63))
ADD_STATIC_CONSTANT_TEXTURE(Tan4,               "tan_4",                rgb255(139, 90, 43))
ADD_STATIC_CONSTANT_TEXTURE(Chocolate1,         "chocolate_1",          rgb255(255, 127, 36))
ADD_STATIC_CONSTANT_TEXTURE(Chocolate2,         "chocolate_2",          rgb255(238, 118, 33))
ADD_STATIC_CONSTANT_TEXTURE(Chocolate3,         "chocolate_3",          rgb255(205, 102, 29))
ADD_STATIC_CONSTANT_TEXTURE(Chocolate4,         "chocolate_4",          rgb255(139, 69, 19))
ADD_STATIC_CONSTANT_TEXTURE(Firebrick1,         "firebrick_1",          rgb255(255, 48, 48))
ADD_STATIC_CONSTANT_TEXTURE(Firebrick2,         "firebrick_2",          rgb255(238, 44, 44))
ADD_STATIC_CONSTANT_TEXTURE(Firebrick3,         "firebrick_3",          rgb255(205, 38, 38))
ADD_STATIC_CONSTANT_TEXTURE(Firebrick4,         "firebrick_4",          rgb255(139, 26, 26))
ADD_STATIC_CONSTANT_TEXTURE(Brown1,             "brown_1",              rgb255(255, 64, 64))
ADD_STATIC_CONSTANT_TEXTURE(Brown2,             "brown_2",              rgb255(238, 59, 59))
ADD_STATIC_CONSTANT_TEXTURE(Brown3,             "brown_3",              rgb255(205, 51, 51))
ADD_STATIC_CONSTANT_TEXTURE(Brown4,             "brown_4",              rgb255(139, 35, 35))
ADD_STATIC_CONSTANT_TEXTURE(Salmon1,            "salmon_1",             rgb255(255, 140, 105))
ADD_STATIC_CONSTANT_TEXTURE(Salmon2,            "salmon_2",             rgb255(238, 130, 98))
ADD_STATIC_CONSTANT_TEXTURE(Salmon3,            "salmon_3",             rgb255(205, 112, 84))
ADD_STATIC_CONSTANT_TEXTURE(Salmon4,            "salmon_4",             rgb255(139, 76, 57))
ADD_STATIC_CONSTANT_TEXTURE(LightSalmon1,       "light_salmon_1",       rgb255(255, 160, 122))
ADD_STATIC_CONSTANT_TEXTURE(LightSalmon2,       "light_salmon_2",       rgb255(238, 149, 114))
ADD_STATIC_CONSTANT_TEXTURE(LightSalmon3,       "light_salmon_3",       rgb255(205, 129, 98))
ADD_STATIC_CONSTANT_TEXTURE(LightSalmon4,       "light_salmon_4",       rgb255(139, 87, 66))
ADD_STATIC_CONSTANT_TEXTURE(Orange1,            "orange_1",             rgb255(255, 165, 0))
ADD_STATIC_CONSTANT_TEXTURE(Orange2,            "orange_2",             rgb255(238, 154, 0))
ADD_STATIC_CONSTANT_TEXTURE(Orange3,            "orange_3",             rgb255(205, 133, 0))
ADD_STATIC_CONSTANT_TEXTURE(Orange4,            "orange_4",             rgb255(139, 90, 0))
ADD_STATIC_CONSTANT_TEXTURE(DarkOrange1,        "dark_orange_1",        rgb255(255, 127, 0))
ADD_STATIC_CONSTANT_TEXTURE(DarkOrange2,        "dark_orange_2",        rgb255(238, 118, 0))
ADD_STATIC_CONSTANT_TEXTURE(DarkOrange3,        "dark_orange_3",        rgb255(205, 102, 0))
ADD_STATIC_CONSTANT_TEXTURE(DarkOrange4,        "dark_orange_4",        rgb255(139, 69, 0))
ADD_STATIC_CONSTANT_TEXTURE(Coral1,             "coral_1",              rgb255(255, 114, 86))
ADD_STATIC_CONSTANT_TEXTURE(Coral2,             "coral_2",              rgb255(238, 106, 80))
ADD_STATIC_CONSTANT_TEXTURE(Coral3,             "coral_3",              rgb255(205, 91, 69))
ADD_STATIC_CONSTANT_TEXTURE(Coral4,             "coral_4",              rgb255(139, 62, 47))
ADD_STATIC_CONSTANT_TEXTURE(Tomato1,            "tomato_1",             rgb255(255, 99, 71))
ADD_STATIC_CONSTANT_TEXTURE(Tomato2,            "tomato_2",             rgb255(238, 92, 66))
ADD_STATIC_CONSTANT_TEXTURE(Tomato3,            "tomato_3",             rgb255(205, 79, 57))
ADD_STATIC_CONSTANT_TEXTURE(Tomato4,            "tomato_4",             rgb255(139, 54, 38))
ADD_STATIC_CONSTANT_TEXTURE(OrangeRed1,         "orange_red_1",         rgb255(255, 69, 0))
ADD_STATIC_CONSTANT_TEXTURE(OrangeRed2,         "orange_red_2",         rgb255(238, 64, 0))
ADD_STATIC_CONSTANT_TEXTURE(OrangeRed3,         "orange_red_3",         rgb255(205, 55, 0))
ADD_STATIC_CONSTANT_TEXTURE(OrangeRed4,         "orange_red_4",         rgb255(139, 37, 0))
ADD_STATIC_CONSTANT_TEXTURE(Red1,               "red_1",                rgb255(255, 0, 0))
ADD_STATIC_CONSTANT_TEXTURE(Red2,               "red_2",                rgb255(238, 0, 0))
ADD_STATIC_CONSTANT_TEXTURE(Red3,               "red_3",                rgb255(205, 0, 0))
ADD_STATIC_CONSTANT_TEXTURE(Red4,               "red_4",                rgb255(139, 0, 0))
ADD_STATIC_CONSTANT_TEXTURE(DeepPink1,          "deep_pink_1",          rgb255(255, 20, 147))
ADD_STATIC_CONSTANT_TEXTURE(DeepPink2,          "deep_pink_2",          rgb255(238, 18, 137))
ADD_STATIC_CONSTANT_TEXTURE(DeepPink3,          "deep_pink_3",          rgb255(205, 16, 118))
ADD_STATIC_CONSTANT_TEXTURE(DeepPink4,          "deep_pink_4",          rgb255(139, 10, 80))
ADD_STATIC_CONSTANT_TEXTURE(HotPink1,           "hot_pink_1",           rgb255(255, 110, 180))
ADD_STATIC_CONSTANT_TEXTURE(HotPink2,           "hot_pink_2",           rgb255(238, 106, 167))
ADD_STATIC_CONSTANT_TEXTURE(HotPink3,           "hot_pink_3",           rgb255(205, 96, 144))
ADD_STATIC_CONSTANT_TEXTURE(HotPink4,           "hot_pink_4",           rgb255(139, 58, 98))
ADD_STATIC_CONSTANT_TEXTURE(Pink1,              "pink_1",               rgb255(255, 181, 197))
ADD_STATIC_CONSTANT_TEXTURE(Pink2,              "pink_2",               rgb255(238, 169, 184))
ADD_STATIC_CONSTANT_TEXTURE(Pink3,              "pink_3",               rgb255(205, 145, 158))
ADD_STATIC_CONSTANT_TEXTURE(Pink4,              "pink_4",               rgb255(139, 99, 108))
ADD_STATIC_CONSTANT_TEXTURE(LightPink1,         "light_pink_1",         rgb255(255, 174, 185))
ADD_STATIC_CONSTANT_TEXTURE(LightPink2,         "light_pink_2",         rgb255(238, 162, 173))
ADD_STATIC_CONSTANT_TEXTURE(LightPink3,         "light_pink_3",         rgb255(205, 140, 149))
ADD_STATIC_CONSTANT_TEXTURE(LightPink4,         "light_pink_4",         rgb255(139, 95, 101))
ADD_STATIC_CONSTANT_TEXTURE(PaleVioletRed1,     "pale_violet_red_1",    rgb255(255, 130, 171))
ADD_STATIC_CONSTANT_TEXTURE(PaleVioletRed2,     "pale_violet_red_2",    rgb255(238, 121, 159))
ADD_STATIC_CONSTANT_TEXTURE(PaleVioletRed3,     "pale_violet_red_3",    rgb255(205, 104, 137))
ADD_STATIC_CONSTANT_TEXTURE(PaleVioletRed4,     "pale_violet_red_4",    rgb255(139, 71, 93))
ADD_STATIC_CONSTANT_TEXTURE(Maroon1,            "maroon_1",             rgb255(255, 52, 179))
ADD_STATIC_CONSTANT_TEXTURE(Maroon2,            "maroon_2",             rgb255(238, 48, 167))
ADD_STATIC_CONSTANT_TEXTURE(Maroon3,            "maroon_3",             rgb255(205, 41, 144))
ADD_STATIC_CONSTANT_TEXTURE(Maroon4,            "maroon_4",             rgb255(139, 28, 98))
ADD_STATIC_CONSTANT_TEXTURE(VioletRed1,         "violet_red_1",         rgb255(255, 62, 150))
ADD_STATIC_CONSTANT_TEXTURE(VioletRed2,         "violet_red_2",         rgb255(238, 58, 140))
ADD_STATIC_CONSTANT_TEXTURE(VioletRed3,         "violet_red_3",         rgb255(205, 50, 120))
ADD_STATIC_CONSTANT_TEXTURE(VioletRed4,         "violet_red_4",         rgb255(139, 34, 82))
ADD_STATIC_CONSTANT_TEXTURE(Magenta1,           "magenta_1",            rgb255(255, 0, 255))
ADD_STATIC_CONSTANT_TEXTURE(Magenta2,           "magenta_2",            rgb255(238, 0, 238))
ADD_STATIC_CONSTANT_TEXTURE(Magenta3,           "magenta_3",            rgb255(205, 0, 205))
ADD_STATIC_CONSTANT_TEXTURE(Magenta4,           "magenta_4",            rgb255(139, 0, 139))
ADD_STATIC_CONSTANT_TEXTURE(Orchid1,            "orchid_1",             rgb255(255, 131, 250))
ADD_STATIC_CONSTANT_TEXTURE(Orchid2,            "orchid_2",             rgb255(238, 122, 233))
ADD_STATIC_CONSTANT_TEXTURE(Orchid3,            "orchid_3",             rgb255(205, 105, 201))
ADD_STATIC_CONSTANT_TEXTURE(Orchid4,            "orchid_4",             rgb255(139, 71, 137))
ADD_STATIC_CONSTANT_TEXTURE(Plum1,              "plum_1",               rgb255(255, 187, 255))
ADD_STATIC_CONSTANT_TEXTURE(Plum2,              "plum_2",               rgb255(238, 174, 238))
ADD_STATIC_CONSTANT_TEXTURE(Plum3,              "plum_3",               rgb255(205, 150, 205))
ADD_STATIC_CONSTANT_TEXTURE(Plum4,              "plum_4",               rgb255(139, 102, 139))
ADD_STATIC_CONSTANT_TEXTURE(MediumOrchid1,      "medium_orchid_1",      rgb255(224, 102, 255))
ADD_STATIC_CONSTANT_TEXTURE(MediumOrchid2,      "medium_orchid_2",      rgb255(209, 95, 238))
ADD_STATIC_CONSTANT_TEXTURE(MediumOrchid3,      "medium_orchid_3",      rgb255(180, 82, 205))
ADD_STATIC_CONSTANT_TEXTURE(MediumOrchid4,      "medium_orchid_4",      rgb255(122, 55, 139))
ADD_STATIC_CONSTANT_TEXTURE(DarkOrchid1,        "dark_orchid_1",        rgb255(191, 62, 255))
ADD_STATIC_CONSTANT_TEXTURE(DarkOrchid2,        "dark_orchid_2",        rgb255(178, 58, 238))
ADD_STATIC_CONSTANT_TEXTURE(DarkOrchid3,        "dark_orchid_3",        rgb255(154, 50, 205))
ADD_STATIC_CONSTANT_TEXTURE(DarkOrchid4,        "dark_orchid_4",        rgb255(104, 34, 139))
ADD_STATIC_CONSTANT_TEXTURE(Purple1,            "purple_1",             rgb255(155, 48, 255))
ADD_STATIC_CONSTANT_TEXTURE(Purple2,            "purple_2",             rgb255(145, 44, 238))
ADD_STATIC_CONSTANT_TEXTURE(Purple3,            "purple_3",             rgb255(125, 38, 205))
ADD_STATIC_CONSTANT_TEXTURE(Purple4,            "purple_4",             rgb255(85, 26, 139))
ADD_STATIC_CONSTANT_TEXTURE(MediumPurple1,      "medium_purple_1",      rgb255(171, 130, 255))
ADD_STATIC_CONSTANT_TEXTURE(MediumPurple2,      "medium_purple_2",      rgb255(159, 121, 238))
ADD_STATIC_CONSTANT_TEXTURE(MediumPurple3,      "medium_purple_3",      rgb255(137, 104, 205))
ADD_STATIC_CONSTANT_TEXTURE(MediumPurple4,      "medium_purple_4",      rgb255(93, 71, 139))
ADD_STATIC_CONSTANT_TEXTURE(Thistle1,           "thistle_1",            rgb255(255, 225, 255))
ADD_STATIC_CONSTANT_TEXTURE(Thistle2,           "thistle_2",            rgb255(238, 210, 238))
ADD_STATIC_CONSTANT_TEXTURE(Thistle3,           "thistle_3",            rgb255(205, 181, 205))
ADD_STATIC_CONSTANT_TEXTURE(Thistle4,           "thistle_4",            rgb255(139, 123, 139))
ADD_STATIC_CONSTANT_TEXTURE(Grey11,             "grey_11",              rgb255(28, 28, 28))
ADD_STATIC_CONSTANT_TEXTURE(Grey21,             "grey_21",              rgb255(54, 54, 54))
ADD_STATIC_CONSTANT_TEXTURE(Grey31,             "grey_31",              rgb255(79, 79, 79))
ADD_STATIC_CONSTANT_TEXTURE(Grey41,             "grey_41",              rgb255(105, 105, 105))
ADD_STATIC_CONSTANT_TEXTURE(Grey51,             "grey_51",              rgb255(130, 130, 130))
ADD_STATIC_CONSTANT_TEXTURE(Grey61,             "grey_61",              rgb255(156, 156, 156))
ADD_STATIC_CONSTANT_TEXTURE(Grey71,             "grey_71",              rgb255(181, 181, 181))
ADD_STATIC_CONSTANT_TEXTURE(Gray81,             "gray_81",              rgb255(207, 207, 207))
ADD_STATIC_CONSTANT_TEXTURE(Gray91,             "gray_91",              rgb255(232, 232, 232))
ADD_STATIC_CONSTANT_TEXTURE(DarkGrey,           "dark_grey",            rgb255(169, 169, 169))
ADD_STATIC_CONSTANT_TEXTURE(DarkBlue,           "dark_blue",            rgb255(0, 0, 139))
ADD_STATIC_CONSTANT_TEXTURE(DarkCyan,           "dark_cyan",            rgb255(0, 139, 139))
ADD_STATIC_CONSTANT_TEXTURE(DarkMagenta,        "dark_magenta",         rgb255(139, 0, 139))
ADD_STATIC_CONSTANT_TEXTURE(DarkRed,            "dark_red",             rgb255(139, 0, 0))
ADD_STATIC_CONSTANT_TEXTURE(LightGreen,         "light_green",          rgb255(144, 238, 144))
*/

AGZ_TRACER_END