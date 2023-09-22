#include <algine/core/Color.h>
#include <gtest/gtest.h>

using namespace algine;

static const Color color(32, 64, 128, 255);

TEST(ColorTest, RgbBasic) {
    auto rgb = color.getRgb();
    EXPECT_TRUE(rgb == glm::ivec3(32, 64, 128));
    EXPECT_TRUE(color.value() == 0xff204080);
}

TEST(ColorTest, Hsv) {
    auto hsv = color.toHsv();
    EXPECT_TRUE(hsv.getHsv() == glm::ivec3(219, 191, 128));
    EXPECT_TRUE(color == hsv);
}

TEST(ColorTest, Luv) {
    auto luv = color.toLuv();
    EXPECT_TRUE(luv.getLuv() == glm::ivec3(72, 70, 86));
    EXPECT_TRUE(color == luv);
}

TEST(ColorTest, Lch) {
    auto lch = color.toLch();
    EXPECT_TRUE(lch.getLch() == glm::ivec3(72, 76, 257));
    EXPECT_TRUE(color == lch);
}

TEST(ColorTest, Hsluv) {
    auto hsluv = color.toHsluv();
    EXPECT_TRUE(hsluv.getHsluv() == glm::ivec3(257, 209, 72));
    EXPECT_TRUE(color == hsluv);
}

TEST(ColorTest, rgbMix) {
    auto x = Color::fromRgbF(glm::vec4(0.0f));
    auto y = Color::fromRgbF(glm::vec4(1.0f));
    auto z = Color::rgbMix(x, y, 0.25f);
    EXPECT_TRUE(z == Color::fromRgbF(glm::vec4(0.25f)));
}

TEST(ColorTest, hsvMix) {
    auto x = Color::fromRgb(158, 40, 40);
    auto y = Color::fromRgb(40, 40, 158);
    auto z = Color::hsvMix(x, y, 0.25f);
    EXPECT_TRUE(z == Color::fromRgb(158, 40, 99));
}

TEST(ColorTest, luvMix) {
    auto x = Color::fromLuvF(glm::vec4(0.0f));
    auto y = Color::fromLuvF(glm::vec4(1.0f));
    auto z = Color::luvMix(x, y, 0.25f);
    EXPECT_TRUE(z == Color::fromLuvF(glm::vec4(0.25f)));
}

TEST(ColorTest, lchMix) {
    auto x = Color::fromRgb(158, 40, 40);
    auto y = Color::fromRgb(40, 40, 158);
    auto z = Color::lchMix(x, y, 0.25f);
    EXPECT_TRUE(z == Color::fromRgb(159, 0, 94));
}

TEST(ColorTest, hsluvMix) {
    auto x = Color::fromRgb(158, 40, 40);
    auto y = Color::fromRgb(40, 40, 158);
    auto z = Color::hsluvMix(x, y, 0.25f);
    EXPECT_TRUE(z == Color::fromRgb(138, 39, 89));
}
