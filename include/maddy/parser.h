/*
 * This project is licensed under the MIT license. For more information see the
 * LICENSE file.
 */
#pragma once

// -----------------------------------------------------------------------------

#include <functional>
#include <memory>
#include <string>

#include "maddy/parserconfig.h"

// BlockParser
#include "maddy/checklistparser.h"
#include "maddy/codeblockparser.h"
#include "maddy/headlineparser.h"
#include "maddy/horizontallineparser.h"
#include "maddy/htmlparser.h"
#include "maddy/orderedlistparser.h"
#include "maddy/paragraphparser.h"
#include "maddy/quoteparser.h"
#include "maddy/tableparser.h"
#include "maddy/unorderedlistparser.h"

// LineParser
#include <iostream>

#include "maddy/breaklineparser.h"
#include "maddy/emphasizedparser.h"
#include "maddy/imageparser.h"
#include "maddy/inlinecodeparser.h"
#include "maddy/italicparser.h"
#include "maddy/linkparser.h"
#include "maddy/strikethroughparser.h"
#include "maddy/strongparser.h"

// -----------------------------------------------------------------------------

namespace maddy {

// -----------------------------------------------------------------------------

/**
 * Parser
 *
 * Transforms Markdown to HTML
 *
 * @class
 */
class Parser {
 public:
  /**
   * ctor
   *
   * Initializes all `LineParser`
   *
   * @method
   */
  Parser(ParserConfig config = ParserConfig{}) : config(config) {}

  /**
   * Parse
   *
   * @method
   * @param {const std::istream&} markdown
   * @return {std::string} HTML
   */
  std::string Parse(std::istream& markdown) const {
    std::string result = "";
    std::unique_ptr<BlockParser> currentBlockParser = nullptr;

    for (std::string line; std::getline(markdown, line);) {
      // std::cout << "[" << line << "]" << std::endl;
      if (!currentBlockParser) {
        currentBlockParser = getBlockParserForLine(line);
      }

      if (currentBlockParser) {
        currentBlockParser->AddLine(line);

        if (currentBlockParser->IsFinished()) {
          result += currentBlockParser->GetResult().str();
          currentBlockParser = nullptr;
        }
      }
    }

    // make sure, that all parsers are finished
    if (currentBlockParser) {
      std::string emptyLine = "";
      currentBlockParser->AddLine(emptyLine);
      if (currentBlockParser->IsFinished()) {
        result += currentBlockParser->GetResult().str();
        currentBlockParser = nullptr;
      }
    }

    return result;
  }

 private:
  const ParserConfig config;

  const BreakLineParser breakLineParser;
  const EmphasizedParser emphasizedParser;
  const ImageParser imageParser;
  const InlineCodeParser inlineCodeParser;
  const ItalicParser italicParser;
  const LinkParser linkParser;
  const StrikeThroughParser strikeThroughParser;
  const StrongParser strongParser;

  // block parser have to run before
  void runLineParser(std::string& line) const {
    // Attention! ImageParser has to be before LinkParser
    imageParser.Parse(line);
    linkParser.Parse(line);

    // Attention! StrongParser has to be before EmphasizedParser
    strongParser.Parse(line);

    if (config.isEmphasizedParserEnabled) {
      emphasizedParser.Parse(line);
    }

    strikeThroughParser.Parse(line);
    inlineCodeParser.Parse(line);
    italicParser.Parse(line);
    breakLineParser.Parse(line);
  }

  std::unique_ptr<BlockParser> getBlockParserForLine(
      const std::string& line) const {
    std::unique_ptr<BlockParser> parser;

    if (maddy::CodeBlockParser::IsStartingLine(line)) {
      parser = std::make_unique<maddy::CodeBlockParser>(nullptr, nullptr);
    } else if (maddy::HeadlineParser::IsStartingLine(line)) {
      parser = std::make_unique<maddy::HeadlineParser>(nullptr, nullptr);
    } else if (maddy::HorizontalLineParser::IsStartingLine(line)) {
      parser = std::make_unique<maddy::HorizontalLineParser>(nullptr, nullptr);
    } else if (maddy::QuoteParser::IsStartingLine(line)) {
      parser = std::make_unique<maddy::QuoteParser>(
          [this](std::string& line) { this->runLineParser(line); },
          [this](const std::string& line) {
            return this->getBlockParserForLine(line);
          });
    } else if (maddy::TableParser::IsStartingLine(line)) {
      parser = std::make_unique<maddy::TableParser>(
          [this](std::string& line) { this->runLineParser(line); }, nullptr);
    } else if (maddy::ChecklistParser::IsStartingLine(line)) {
      parser = this->createChecklistParser();
    } else if (maddy::OrderedListParser::IsStartingLine(line)) {
      parser = this->createOrderedListParser();
    } else if (maddy::UnorderedListParser::IsStartingLine(line)) {
      parser = this->createUnorderedListParser();
    } else if (!config.isHTMLWrappedInParagraph &&
               maddy::HtmlParser::IsStartingLine(line)) {
      parser = std::make_unique<maddy::HtmlParser>(nullptr, nullptr);
    } else if (maddy::ParagraphParser::IsStartingLine(line)) {
      parser = std::make_unique<maddy::ParagraphParser>(
          [this](std::string& line) { this->runLineParser(line); }, nullptr);
    }
    return parser;
  }

  std::unique_ptr<BlockParser> createChecklistParser() const {
    return std::make_unique<maddy::ChecklistParser>(
        [this](std::string& line) { this->runLineParser(line); },
        [this](const std::string& line) {
          std::unique_ptr<BlockParser> parser;

          if (maddy::ChecklistParser::IsStartingLine(line)) {
            parser = this->createChecklistParser();
          }
          return parser;
        });
  }

  std::unique_ptr<BlockParser> createOrderedListParser() const {
    return std::make_unique<maddy::OrderedListParser>(
        [this](std::string& line) { this->runLineParser(line); },
        [this](const std::string& line) {
          std::unique_ptr<BlockParser> parser;
          if (maddy::OrderedListParser::IsStartingLine(line)) {
            parser = this->createOrderedListParser();
          } else if (maddy::UnorderedListParser::IsStartingLine(line)) {
            parser = this->createUnorderedListParser();
          }
          return parser;
        });
  }

  std::unique_ptr<BlockParser> createUnorderedListParser() const {
    return std::make_unique<maddy::UnorderedListParser>(
        [this](std::string& line) { this->runLineParser(line); },
        [this](const std::string& line) {
          std::unique_ptr<BlockParser> parser;
          if (maddy::OrderedListParser::IsStartingLine(line)) {
            parser = this->createOrderedListParser();
          } else if (maddy::UnorderedListParser::IsStartingLine(line)) {
            parser = this->createUnorderedListParser();
          }
          return parser;
        });
  }
};  // class Parser

// -----------------------------------------------------------------------------

}  // namespace maddy
