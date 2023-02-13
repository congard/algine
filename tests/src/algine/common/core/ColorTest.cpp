#include <algine/core/Color.h>
#include <tulz/test/Test.h>

using namespace algine;

TEST_CLASS(ColorTest, {
    Color color(32, 64, 128, 255);

    TEST(RgbBasic, {
        auto rgb = color.getRgb();
        assertTrue(rgb == glm::ivec3(32, 64, 128));
        assertTrue(color.value() == 0xff204080);
    })

    TEST(Hsv, {
        auto hsv = color.toHsv();
        assertTrue(hsv.getHsv() == glm::ivec3(219, 191, 128));
        assertTrue(color == hsv);
    })

    TEST(Luv, {
        auto luv = color.toLuv();
        assertTrue(luv.getLuv() == glm::ivec3(72, 70, 86));
        assertTrue(color == luv);
    })

    TEST(Lch, {
        auto lch = color.toLch();
        assertTrue(lch.getLch() == glm::ivec3(72, 76, 257));
        assertTrue(color == lch);
    })

    TEST(Hsluv, {
        auto hsluv = color.toHsluv();
        assertTrue(hsluv.getHsluv() == glm::ivec3(257, 209, 72));
        assertTrue(color == hsluv);
    })

    TEST(rgbMix, {
        auto x = Color::fromRgbF(glm::vec4(0.0f));
        auto y = Color::fromRgbF(glm::vec4(1.0f));
        auto z = Color::rgbMix(x, y, 0.25f);
        assertTrue(z == Color::fromRgbF(glm::vec4(0.25f)));
    })

    TEST(hsvMix, {
        auto x = Color::fromRgb(158, 40, 40);
        auto y = Color::fromRgb(40, 40, 158);
        auto z = Color::hsvMix(x, y, 0.25f);
        assertTrue(z == Color::fromRgb(158, 40, 99));
    })

    TEST(luvMix, {
        auto x = Color::fromLuvF(glm::vec4(0.0f));
        auto y = Color::fromLuvF(glm::vec4(1.0f));
        auto z = Color::luvMix(x, y, 0.25f);
        assertTrue(z == Color::fromLuvF(glm::vec4(0.25f)));
    })

    TEST(lchMix, {
        auto x = Color::fromRgb(158, 40, 40);
        auto y = Color::fromRgb(40, 40, 158);
        auto z = Color::lchMix(x, y, 0.25f);
        assertTrue(z == Color::fromRgb(159, 0, 94));
    })

    TEST(hsluvMix, {
        auto x = Color::fromRgb(158, 40, 40);
        auto y = Color::fromRgb(40, 40, 158);
        auto z = Color::hsluvMix(x, y, 0.25f);
        assertTrue(z == Color::fromRgb(138, 39, 89));
    })
})

int main() {
    ColorTest().runTests();
    return 0;
}
