/*
 * This project is licensed under the MIT license. For more information see the
 * LICENSE file.
 */

#include "maddy/test_maddy_parser.h"

#include "gmock/gmock.h"
#include "maddy/parser.h"

// -----------------------------------------------------------------------------

TEST(MADDY_PARSER, ItShouldParse) {
  auto parser = std::make_shared<maddy::Parser>();
  std::stringstream markdown(testMarkdown);

  const std::string output = parser->Parse(markdown);

  ASSERT_EQ(testHtml, output);
}

TEST(MADDY_PARSER, ItShouldParseWithConfig) {
  auto parser =
      std::make_shared<maddy::Parser>(maddy::ParserConfig{false, false});

  std::stringstream markdown(testMarkdown);

  const std::string output = parser->Parse(markdown);

  ASSERT_EQ(testHtml2, output);
}
