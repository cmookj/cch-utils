#include "fileio.h"
#include "filesystem.h"
#include "rnd.h"
#include "str.h"

#include <list>

#include <gtest/gtest.h>

using namespace cch::str;

TEST (StringTest, TrimReduce) {
  const std::string foo = "    too much\t   \tspace\t\t\t  ";
  const std::string bar = "one\ntwo";

  EXPECT_EQ (trim (foo), "too much\t   \tspace");
  EXPECT_EQ (reduce (foo), "too much space");
  EXPECT_EQ (reduce (foo, "-"), "too-much-space");
  EXPECT_EQ (trim (bar), "one\ntwo");
}

TEST (StringViewTest, Trim) {
  std::string str = "   hello world   ";
  EXPECT_EQ (ltrim (str), "hello world   ");
  EXPECT_EQ (rtrim (str), "   hello world");
  EXPECT_EQ (trim (str), "hello world");
}

TEST (String, KMP) {
  {
    std::string S = "ABC ABCDAB ABCDABCDABDE";
    std::string W = "ABCDABD";

    // std::vector<int> indices = positions_in_string (S, W);
    std::vector<int> indices = search (W, S);

    EXPECT_EQ (indices.size(), 1);
    EXPECT_EQ (indices[0], 15);
  }

  {
    std::string S = "aabaacaadaabaaba";
    std::string W = "aaba";

    // std::vector<int> indices = positions_in_string (S, W);
    std::vector<int> indices = search (W, S);

    EXPECT_EQ (indices.size(), 3);
    EXPECT_EQ (indices[0], 0);
    EXPECT_EQ (indices[1], 9);
    EXPECT_EQ (indices[2], 12);
  }

  {
    std::string S = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    std::string W = "aaaa";

    // std::vector<int> indices = positions_in_string (S, W);
    std::vector<int> indices = search (W, S);

    EXPECT_EQ (indices.size(), 40);
    for (int i = 0; i < 40; ++i) {
      EXPECT_EQ (indices[i], i);
    }
  }

  {
    std::string S = "aabaaabaaaabaaaaabaaaaaabaaac";
    std::string W = "aabaaac";

    // std::vector<int> indices = positions_in_string (S, W);
    std::vector<int> indices = search (W, S);

    EXPECT_EQ (indices.size(), 1);
    EXPECT_EQ (indices[0], 22);
  }
}

TEST (FileIO, BinaryIO) {
  std::size_t data_size = 1 << 24;

  {
    // Generate random data
    std::vector<int>         data (data_size);
    cch::random_int_gen<int> gen;

    for (auto& elem : data) {
      elem = gen();
    }

    // Write to file
    std::string file_path = "/tmp/cch_fileio_test_int";
    cch::fileio::write_file (file_path, data);

    // Read file into another vector
    auto data_in = cch::fileio::read_file_binary<std::vector<int>> (file_path);

    // Compare
    EXPECT_EQ (data.size(), data_in.size());
    EXPECT_EQ (std::equal (data.begin(), data.end(), data_in.begin()), true);

    // Read file into a list
    auto data_in_list = cch::fileio::read_file_binary<std::list<int>> (file_path);

    // Compare
    EXPECT_EQ (data.size(), data_in_list.size());
    EXPECT_EQ (std::equal (data.begin(), data.end(), data_in_list.begin()), true);
  }
}
