#include "pch.h"
#include <iostream>

import ntl.string.string;
import ntl.utils;
using namespace ne;
using namespace std;

static int ALLOC_COUNT = 0;
void* operator new(size_t n) {
	++ALLOC_COUNT;
	return malloc(n);
}

void operator delete(void* x) {
	--ALLOC_COUNT;
	free(x);
}

TEST(TestString, Construct) {
	String s1;
	EXPECT_EQ(s1.size(), 0);
	EXPECT_NE(s1.capacity(), 0);

	String s2 = "123456789asdfghjaklsdjklasd";
	EXPECT_EQ(s2.size(), 27);

	String s3("asdasdsadasd", 5);
	EXPECT_EQ(s3.size(), 5);
	EXPECT_EQ("asdas", s3);

	String s4(s3, 3);
	EXPECT_EQ(s4.size(), 3);
	EXPECT_EQ(s4, "asd");


	String s5(s2);
	EXPECT_EQ(s2, s5);

	String s6(100, '*');
	EXPECT_EQ(s6.size(), 100);
}

TEST(TestString, Assign) {
	String s;
	String s1 = "asdf";

	s = s1;
	EXPECT_EQ(s, s1);
	EXPECT_EQ(s.size(), 4);

	s = "asdfjjaksdl";
	EXPECT_EQ(s, "asdfjjaksdl");

	String s2 = "asdfhajksdhkajshdkakjsdhkjakjsdhkasdkj";
	s.assign(s2, 25);
	EXPECT_EQ(s.size(), 25);

	s.assign(10, '*');
	EXPECT_EQ(s, "**********");
	EXPECT_EQ(s.size(), 10);
	EXPECT_EQ(s.capacity(), 22);
}

TEST(TestString, Append) {
	String s1 = "asdf";
	String s2 = "123";
	s1.append(s2);
	EXPECT_EQ(s1.size(), 7);
	EXPECT_EQ(s1, "asdf123");

	s1.append("asdajshfjjkashfjkahskjf");
	EXPECT_EQ(s1.size(), 30);
	EXPECT_GE(s1.capacity(), 22);

	s1.append('a');
	EXPECT_EQ(s1.size(), 31);
	s1.append(20, '*');
	EXPECT_EQ(s1.size(), 51);
}

TEST(TestString, Prepend) {
	String s1 = "asd";
	String s2 = "123";
	s1.prepend(s2);
	EXPECT_EQ(s1.size(), 6);
	s1.prepend(3,'0');
	EXPECT_EQ(s1, "000123asd");
	s1.prepend("ajsdhkajsdhjkashdkjsahjkdhjkasdhjksadhjk");
	EXPECT_EQ(s1.size(), 49);

	String s3 = "xxkkllzz";
	s3.prepend("123");
	EXPECT_EQ(s3.size(), 11);
	EXPECT_EQ(s3, "123xxkkllzz");
}

TEST(TestString, Insert) {
	String s1 = "0123456789";
	s1.insert(0, "asd");
	EXPECT_EQ(s1, "asd0123456789");
	s1.insert(13, "zxc");
	EXPECT_EQ(s1, "asd0123456789zxc");
	s1.insert(3, "<>");
	EXPECT_EQ(s1, "asd<>0123456789zxc");
	String s2 = "012345";
	s2.insert(0, '*', 2);
	EXPECT_EQ(s2, "**012345");
	s2.insert(8, 'x', 1);
	EXPECT_EQ(s2, "**012345x");
	EXPECT_EQ(s2.size(), 9);
}

TEST(TestString, Replace) {
	String s1 = "0123456789";
	String s2 = s1;
	s1.replace(0, 1, "asdf");
	s2.replace(0, 4, "asd");
	EXPECT_EQ(s2, "asd456789");
	EXPECT_EQ(s2.size(), 9);
	s2.replace(3, 3, 'x', 1);
	EXPECT_EQ(s2, "asdx789");
	EXPECT_EQ(s2.size(), 7);
}

TEST(TestString, Remove) {
	String s1 = "0123456789";
	s1.remove(0, 3);
	EXPECT_EQ(s1, "3456789");

	String s2(100, 'x');
	s2.remove(0, 90);
	EXPECT_EQ(s2, "xxxxxxxxxx");
	EXPECT_EQ(s2.size(), 10);
	EXPECT_EQ(s2.capacity(), 22);
}

TEST(TestString, Misc) {
	String s1 = "0123456789";
	EXPECT_EQ(s1.left(5), "01234");
	EXPECT_EQ(s1.right(5), "56789");
	s1.fill('*', 100);
	EXPECT_EQ(s1.size(), 100);
	String s2 = " asdfgh  ";
	EXPECT_EQ(s2.trimmed(), "asdfgh");

	s1.fill('*', 10);
	s2.fill('x', 10);
	s1.swap(s2);
	EXPECT_EQ(s1, "xxxxxxxxxx");
	EXPECT_EQ(s2, "**********");

	s1.chop(5);
	EXPECT_EQ(s1, "xxxxx");
	EXPECT_EQ(s1.size(), 5);

	auto s3 = s2.substr(5, 2);
	EXPECT_EQ("**", s3);
}
