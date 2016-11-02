#include "base/strings/str_util.h"

#include <vector>
#include <gtest/gtest.h>


namespace base {

TEST(CEscape, Basic) {
  EXPECT_EQ(strings::CEscape("hello"), "hello");
  EXPECT_EQ(strings::CEscape("hello\n"), "hello\\n");
  EXPECT_EQ(strings::CEscape("hello\r"), "hello\\r");
  EXPECT_EQ(strings::CEscape("\t\r\"'"), "\\t\\r\\\"\\'");
  EXPECT_EQ(strings::CEscape("\320hi\200"), "\\320hi\\200");
}

string ExpectCUnescapeSuccess(StringPiece source) {
  string dest;
  string error;
  EXPECT_TRUE(strings::CUnescape(source, &dest, &error)) << error;
  return dest;
}

TEST(CUnescape, Basic) {
  EXPECT_EQ("hello", ExpectCUnescapeSuccess("hello"));
  EXPECT_EQ("hello\n", ExpectCUnescapeSuccess("hello\\n"));
  EXPECT_EQ("hello\r", ExpectCUnescapeSuccess("hello\\r"));
  EXPECT_EQ("\t\r\"'", ExpectCUnescapeSuccess("\\t\\r\\\"\\'"));
  EXPECT_EQ("\320hi\200", ExpectCUnescapeSuccess("\\320hi\\200"));
}

TEST(StripTrailingWhitespace, Basic) {
  string test;
  test = "hello";
  strings::StripTrailingWhitespace(&test);
  EXPECT_EQ(test, "hello");

  test = "foo  ";
  strings::StripTrailingWhitespace(&test);
  EXPECT_EQ(test, "foo");

  test = "   ";
  strings::StripTrailingWhitespace(&test);
  EXPECT_EQ(test, "");

  test = "";
  strings::StripTrailingWhitespace(&test);
  EXPECT_EQ(test, "");

  test = " abc\t";
  strings::StripTrailingWhitespace(&test);
  EXPECT_EQ(test, " abc");
}

TEST(RemoveLeadingWhitespace, Basic) {
  string text = "  \t   \n  \r Quick\t";
  StringPiece data(text);
  // check that all whitespace is removed
  EXPECT_EQ(strings::RemoveLeadingWhitespace(&data), 11);
  EXPECT_EQ(data, StringPiece("Quick\t"));
  // check that non-whitespace is not removed
  EXPECT_EQ(strings::RemoveLeadingWhitespace(&data), 0);
  EXPECT_EQ(data, StringPiece("Quick\t"));
}

TEST(RemoveLeadingWhitespace, TerminationHandling) {
  // check termination handling
  string text = "\t";
  StringPiece data(text);
  EXPECT_EQ(strings::RemoveLeadingWhitespace(&data), 1);
  EXPECT_EQ(data, StringPiece(""));

  // check termination handling again
  EXPECT_EQ(strings::RemoveLeadingWhitespace(&data), 0);
  EXPECT_EQ(data, StringPiece(""));
}

TEST(RemoveTrailingWhitespace, Basic) {
  string text = "  \t   \n  \r Quick \t";
  StringPiece data(text);
  // check that all whitespace is removed
  EXPECT_EQ(strings::RemoveTrailingWhitespace(&data), 2);
  EXPECT_EQ(data, StringPiece("  \t   \n  \r Quick"));
  // check that non-whitespace is not removed
  EXPECT_EQ(strings::RemoveTrailingWhitespace(&data), 0);
  EXPECT_EQ(data, StringPiece("  \t   \n  \r Quick"));
}

TEST(RemoveTrailingWhitespace, TerminationHandling) {
  // check termination handling
  string text = "\t";
  StringPiece data(text);
  EXPECT_EQ(strings::RemoveTrailingWhitespace(&data), 1);
  EXPECT_EQ(data, StringPiece(""));

  // check termination handling again
  EXPECT_EQ(strings::RemoveTrailingWhitespace(&data), 0);
  EXPECT_EQ(data, StringPiece(""));
}

TEST(RemoveWhitespaceContext, Basic) {
  string text = "  \t   \n  \r Quick \t";
  StringPiece data(text);
  // check that all whitespace is removed
  EXPECT_EQ(strings::RemoveWhitespaceContext(&data), 13);
  EXPECT_EQ(data, StringPiece("Quick"));
  // check that non-whitespace is not removed
  EXPECT_EQ(strings::RemoveWhitespaceContext(&data), 0);
  EXPECT_EQ(data, StringPiece("Quick"));

  // Test empty string
  text = "";
  data = text;
  EXPECT_EQ(strings::RemoveWhitespaceContext(&data), 0);
  EXPECT_EQ(data, StringPiece(""));
}

void TestConsumeLeadingDigits(StringPiece s, int64 expected,
                              StringPiece remaining) {
  uint64 v;
  StringPiece input(s);
  if (strings::ConsumeLeadingDigits(&input, &v)) {
    EXPECT_EQ(v, static_cast<uint64>(expected));
    EXPECT_EQ(input, remaining);
  } else {
    EXPECT_LT(expected, 0);
    EXPECT_EQ(input, remaining);
  }
}

TEST(ConsumeLeadingDigits, Basic) {
  using strings::ConsumeLeadingDigits;

  TestConsumeLeadingDigits("123", 123, "");
  TestConsumeLeadingDigits("a123", -1, "a123");
  TestConsumeLeadingDigits("9_", 9, "_");
  TestConsumeLeadingDigits("11111111111xyz", 11111111111ll, "xyz");

  // Overflow case
  TestConsumeLeadingDigits("1111111111111111111111111111111xyz", -1,
                           "1111111111111111111111111111111xyz");

  // 2^64
  TestConsumeLeadingDigits("18446744073709551616xyz", -1,
                           "18446744073709551616xyz");
  // 2^64-1
  TestConsumeLeadingDigits("18446744073709551615xyz", 18446744073709551615ull,
                           "xyz");
  // (2^64-1)*10+9
  TestConsumeLeadingDigits("184467440737095516159yz", -1,
                           "184467440737095516159yz");
}

void TestConsumeNonWhitespace(StringPiece s, StringPiece expected,
                              StringPiece remaining) {
  StringPiece v;
  StringPiece input(s);
  if (strings::ConsumeNonWhitespace(&input, &v)) {
    EXPECT_EQ(v, expected);
    EXPECT_EQ(input, remaining);
  } else {
    EXPECT_EQ(expected, "");
    EXPECT_EQ(input, remaining);
  }
}

TEST(ConsumeNonWhitespace, Basic) {
  TestConsumeNonWhitespace("", "", "");
  TestConsumeNonWhitespace(" ", "", " ");
  TestConsumeNonWhitespace("abc", "abc", "");
  TestConsumeNonWhitespace("abc ", "abc", " ");
}

TEST(ConsumePrefix, Basic) {
  string s("abcdef");
  StringPiece input(s);
  EXPECT_FALSE(strings::ConsumePrefix(&input, "abcdefg"));
  EXPECT_EQ(input, "abcdef");

  EXPECT_FALSE(strings::ConsumePrefix(&input, "abce"));
  EXPECT_EQ(input, "abcdef");

  EXPECT_TRUE(strings::ConsumePrefix(&input, ""));
  EXPECT_EQ(input, "abcdef");

  EXPECT_FALSE(strings::ConsumePrefix(&input, "abcdeg"));
  EXPECT_EQ(input, "abcdef");

  EXPECT_TRUE(strings::ConsumePrefix(&input, "abcdef"));
  EXPECT_EQ(input, "");

  input = s;
  EXPECT_TRUE(strings::ConsumePrefix(&input, "abcde"));
  EXPECT_EQ(input, "f");
}

TEST(JoinStrings, Basic) {
  std::vector<string> s;
  s = {"hi"};
  EXPECT_EQ(strings::Join(s, " "), "hi");
  s = {"hi", "there", "strings"};
  EXPECT_EQ(strings::Join(s, " "), "hi there strings");

  std::vector<StringPiece> sp;
  sp = {"hi"};
  EXPECT_EQ(strings::Join(sp, ",,"), "hi");
  sp = {"hi", "there", "strings"};
  EXPECT_EQ(strings::Join(sp, "--"), "hi--there--strings");
}

TEST(JoinStrings, Join3) {
  std::vector<string> s;
  s = {"hi"};
  auto l1 = [](string* out, string s) { *out += s; };
  EXPECT_EQ(strings::Join(s, " ", l1), "hi");
  s = {"hi", "there", "strings"};
  auto l2 = [](string* out, string s) { *out += s[0]; };
  EXPECT_EQ(strings::Join(s, " ", l2), "h t s");
}

TEST(Split, Basic) {
  EXPECT_TRUE(strings::Split("", ',').empty());
  EXPECT_EQ(strings::Join(strings::Split("a", ','), "|"), "a");
  EXPECT_EQ(strings::Join(strings::Split(",", ','), "|"), "|");
  EXPECT_EQ(strings::Join(strings::Split("a,b,c", ','), "|"), "a|b|c");
  EXPECT_EQ(strings::Join(strings::Split("a,,,b,,c,", ','), "|"),
            "a|||b||c|");
  EXPECT_EQ(strings::Join(
                strings::Split("a,,,b,,c,", ',', strings::SkipEmpty()), "|"),
            "a|b|c");
  EXPECT_EQ(
      strings::Join(
          strings::Split("a,  ,b,,c,", ',', strings::SkipWhitespace()), "|"),
      "a|b|c");
}

TEST(SplitAndParseAsInts, Basic) {
  std::vector<int32> nums;
  EXPECT_TRUE(strings::SplitAndParseAsInts("", ',', &nums));
  EXPECT_EQ(nums.size(), 0);

  EXPECT_TRUE(strings::SplitAndParseAsInts("134", ',', &nums));
  EXPECT_EQ(nums.size(), 1);
  EXPECT_EQ(nums[0], 134);

  EXPECT_TRUE(strings::SplitAndParseAsInts("134,2,13,-5", ',', &nums));
  EXPECT_EQ(nums.size(), 4);
  EXPECT_EQ(nums[0], 134);
  EXPECT_EQ(nums[1], 2);
  EXPECT_EQ(nums[2], 13);
  EXPECT_EQ(nums[3], -5);

  EXPECT_FALSE(strings::SplitAndParseAsInts("abc", ',', &nums));

  EXPECT_FALSE(strings::SplitAndParseAsInts("-13,abc", ',', &nums));

  EXPECT_FALSE(strings::SplitAndParseAsInts("13,abc,5", ',', &nums));
}

TEST(Lowercase, Basic) {
  EXPECT_EQ("", strings::Lowercase(""));
  EXPECT_EQ("hello", strings::Lowercase("hello"));
  EXPECT_EQ("hello world", strings::Lowercase("Hello World"));
}

TEST(Uppercase, Basic) {
  EXPECT_EQ("", strings::Uppercase(""));
  EXPECT_EQ("HELLO", strings::Uppercase("hello"));
  EXPECT_EQ("HELLO WORLD", strings::Uppercase("Hello World"));
}

TEST(TitlecaseString, Basic) {
  string s = "sparse_lookup";
  strings::TitlecaseString(&s, "_");
  ASSERT_EQ(s, "Sparse_Lookup");

  s = "sparse_lookup";
  strings::TitlecaseString(&s, " ");
  ASSERT_EQ(s, "Sparse_lookup");

  s = "dense";
  strings::TitlecaseString(&s, " ");
  ASSERT_EQ(s, "Dense");
}

}  // namespace base
