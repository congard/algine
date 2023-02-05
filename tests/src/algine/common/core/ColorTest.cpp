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
})

int main() {
    ColorTest().runTests();
    return 0;
}
