#include "pch.h"

import ntl.string.text;

using namespace std;
using namespace ne;

TEST(TestText, Constructor) {
	Text text;
	EXPECT_TRUE(text.isNull());
	EXPECT_EQ(text.size(), 0);
	Text t1 = "asdahjhfakhfkjasf";
	EXPECT_EQ(t1.toString(), "asdahjhfakhfkjasf");
	EXPECT_EQ(t1.refcount(), 1);
	auto t2 = t1;
	EXPECT_EQ(t1.refcount(), t2.refcount());
	EXPECT_EQ(t1.refcount(), 2);

	{
		auto t3 = t1;
	}

	EXPECT_EQ(t2.refcount(), 2);
}