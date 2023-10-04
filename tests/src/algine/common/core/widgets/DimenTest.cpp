#include <algine/core/widgets/Dimen.h>
#include <gtest/gtest.h>

using namespace algine;

TEST(DimenTest, ParsePx) {
    Dimen d("123.45px");
    EXPECT_FLOAT_EQ(d.getValue(), 123.45f);
    EXPECT_EQ(d.getUnit(), Unit::px);
}

TEST(DimenTest, ParseDp) {
    Dimen d("123.45dp");
    EXPECT_FLOAT_EQ(d.getValue(), 123.45f);
    EXPECT_EQ(d.getUnit(), Unit::dp);
}

TEST(DimenTest, ParseFailed) {
    EXPECT_FALSE(Dimen().parse("123..45px"));
    EXPECT_FALSE(Dimen().parse("123.45 px"));
    EXPECT_FALSE(Dimen().parse("123.45PX"));
    EXPECT_FALSE(Dimen().parse("123.45"));
}

TEST(DimenTest, ParseOk) {
    EXPECT_TRUE(Dimen().parse("123.45px"));
    EXPECT_TRUE(Dimen().parse("123.45dp"));
    EXPECT_TRUE(Dimen().parse("123px"));
    EXPECT_TRUE(Dimen().parse("+123px"));
    EXPECT_TRUE(Dimen().parse("-123px"));
}

TEST(DimenTest, ToString) {
    {
        Dimen d("123.45px");
        EXPECT_EQ(d.toString(), "123.45px");
    }

    {
        Dimen d("123.45dp");
        EXPECT_EQ(d.toString(), "123.45dp");
    }
}
