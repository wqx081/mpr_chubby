#include "base/io/path.h"
#include <gtest/gtest.h>

namespace base {
namespace io {

TEST(PathTest, JoinPath) {
  EXPECT_EQ("/foo/bar", JoinPath("/foo", "bar"));
  EXPECT_EQ("foo/bar", JoinPath("foo", "bar"));
  EXPECT_EQ("foo/bar", JoinPath("foo", "/bar"));
  EXPECT_EQ("/foo/bar", JoinPath("/foo", "/bar"));

  EXPECT_EQ("/bar", JoinPath("", "/bar"));
  EXPECT_EQ("bar", JoinPath("", "bar"));
  EXPECT_EQ("/foo", JoinPath("/foo", ""));

  EXPECT_EQ("/foo/bar/baz/blah/blink/biz",
            JoinPath("/foo/bar/baz/", "/blah/blink/biz"));
  EXPECT_EQ("/foo/bar/baz/blah", JoinPath("/foo", "bar", "baz", "blah"));
}

TEST(PathTest, IsAbsolutePath) {
  EXPECT_FALSE(IsAbsolutePath(""));
  EXPECT_FALSE(IsAbsolutePath("../foo"));
  EXPECT_FALSE(IsAbsolutePath("foo"));
  EXPECT_FALSE(IsAbsolutePath("./foo"));
  EXPECT_FALSE(IsAbsolutePath("foo/bar/baz/"));
  EXPECT_TRUE(IsAbsolutePath("/foo"));
  EXPECT_TRUE(IsAbsolutePath("/foo/bar/../baz"));
}

TEST(PathTest, Dirname) {
  EXPECT_EQ("/hello", Dirname("/hello/"));
  EXPECT_EQ("/", Dirname("/hello"));
  EXPECT_EQ("hello", Dirname("hello/world"));
  EXPECT_EQ("hello", Dirname("hello/"));
  EXPECT_EQ("", Dirname("world"));
  EXPECT_EQ("/", Dirname("/"));
  EXPECT_EQ("", Dirname(""));
}

TEST(PathTest, Basename) {
  EXPECT_EQ("", Basename("/hello/"));
  EXPECT_EQ("hello", Basename("/hello"));
  EXPECT_EQ("world", Basename("hello/world"));
  EXPECT_EQ("", Basename("hello/"));
  EXPECT_EQ("world", Basename("world"));
  EXPECT_EQ("", Basename("/"));
  EXPECT_EQ("", Basename(""));
}

TEST(PathTest, Extension) {
  EXPECT_EQ("gif", Extension("foo.gif"));
  EXPECT_EQ("", Extension("foo."));
  EXPECT_EQ("", Extension(""));
  EXPECT_EQ("", Extension("/"));
  EXPECT_EQ("", Extension("foo"));
  EXPECT_EQ("", Extension("foo/"));
  EXPECT_EQ("gif", Extension("/a/path/to/foo.gif"));
  EXPECT_EQ("html", Extension("/a/path.bar/to/foo.html"));
  EXPECT_EQ("", Extension("/a/path.bar/to/foo"));
  EXPECT_EQ("baz", Extension("/a/path.bar/to/foo.bar.baz"));
}

TEST(PathTest, CleanPath) {
  EXPECT_EQ(".", CleanPath(""));
  EXPECT_EQ("x", CleanPath("x"));
  EXPECT_EQ("/a/b/c/d", CleanPath("/a/b/c/d"));
  EXPECT_EQ("/a/b/c/d/*", CleanPath("/a/b/c/d/*"));
  EXPECT_EQ("/a/b/c/d", CleanPath("/a/b/c/d/"));
  EXPECT_EQ("/a/b", CleanPath("/a//b"));
  EXPECT_EQ("/a/b", CleanPath("//a//b/"));
  EXPECT_EQ("/", CleanPath("/.."));
  EXPECT_EQ("/", CleanPath("/././././"));
  EXPECT_EQ("/a", CleanPath("/a/b/.."));
  EXPECT_EQ("/", CleanPath("/a/b/../../.."));
  EXPECT_EQ("/", CleanPath("//a//b/..////../..//"));
  EXPECT_EQ("/x", CleanPath("//a//../x//"));
  EXPECT_EQ("x", CleanPath("x"));
  EXPECT_EQ("../../a/c", CleanPath("../../a/b/../c"));
  EXPECT_EQ("../..", CleanPath("../../a/b/../c/../.."));
  EXPECT_EQ("../../bar", CleanPath("foo/../../../bar"));
}

}  // namespace io
}  // namespace base
