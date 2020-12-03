// Copyright 2020, University of Freiburg
// Authors: Axel Lehmann <lehmann@cs.uni-freiburg.de>.

#include <string>

#include "gtest/gtest.h"

#include "osm2ttl/config/Config.h"
#include "osm2ttl/ttl/Writer.h"

namespace osm2ttl::ttl {

// ____________________________________________________________________________
TEST(WriterGrammarNT, RULE_8_IRIREF) {
  // NT:  [8]    IRIREF
  //      https://www.w3.org/TR/n-triples/#grammar-production-IRIREF
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::NT> w{config, nullptr};

  ASSERT_EQ("<prefixsuffix>", w.IRIREF("prefix", "suffix"));
  ASSERT_EQ("<\\u003cprefix\\u003e\\u003csuffix\\u003e>", w.IRIREF("<prefix>", "<suffix>"));
}

// ____________________________________________________________________________
TEST(WriterGrammarNT, RULE_8_IRIREF_CONVERT) {
  // NT:  [8]    IRIREF
  //      https://www.w3.org/TR/n-triples/#grammar-production-IRIREF
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::NT> w{config, nullptr};

  ASSERT_EQ("", w.encodeIRIREF(""));
  ASSERT_EQ("allöwed", w.encodeIRIREF("allöwed"));
  ASSERT_EQ("\\u003c\\u003e\\u0022\\u007b\\u007d\\u007c\\u005e\\u0060\\u005c", w.encodeIRIREF("<>\"{}|^`\\"));
  using namespace std::literals::string_literals;
  ASSERT_EQ("\\u0000\\u0001\\u0019\\u0020", w.encodeIRIREF("\u0000\u0001\u0019\u0020"s));
}

// ____________________________________________________________________________
TEST(WriterGrammarNT, RULE_9_STRING_LITERAL_QUOTE) {
  // NT:  [9]   STRING_LITERAL_QUOTE
  //      https://www.w3.org/TR/n-triples/#grammar-production-STRING_LITERAL_QUOTE
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::NT> w{config, nullptr};

  ASSERT_EQ("\"\"", w.STRING_LITERAL_QUOTE(""));
  ASSERT_EQ("\"\\\"\"", w.STRING_LITERAL_QUOTE("\""));
  ASSERT_EQ("\"\\\\\"", w.STRING_LITERAL_QUOTE("\\"));
  ASSERT_EQ("\"\\n\"", w.STRING_LITERAL_QUOTE("\n"));
  ASSERT_EQ("\"\\r\"", w.STRING_LITERAL_QUOTE("\r"));
  ASSERT_EQ("\"\t\"", w.STRING_LITERAL_QUOTE("\t"));
  ASSERT_EQ("\"abc\"", w.STRING_LITERAL_QUOTE("abc"));
}

// ____________________________________________________________________________
TEST(WriterGrammarNT, RULE_10_UCHAR_CODEPOINT) {
  // NT:  [10]  UCHAR
  //      https://www.w3.org/TR/n-triples/#grammar-production-UCHAR
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::NT> w{config, nullptr};

  ASSERT_EQ("\\u0000", w.UCHAR(0x00U));
  ASSERT_EQ("\\u0064", w.UCHAR(0x64U));
  ASSERT_EQ("\\U000fff64", w.UCHAR(0xfff64U));
}

// ____________________________________________________________________________
TEST(WriterGrammarNT, RULE_10_UCHAR_ASCII) {
  // NT:  [10]  UCHAR
  //      https://www.w3.org/TR/n-triples/#grammar-production-UCHAR
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::NT> w{config, nullptr};

  ASSERT_EQ("\\u0000", w.UCHAR('\0'));  // NUL
  ASSERT_EQ("\\u0021", w.UCHAR('!'));
  ASSERT_EQ("\\u0031", w.UCHAR('1'));
  ASSERT_EQ("\\u0040", w.UCHAR('@'));
  ASSERT_EQ("\\u0041", w.UCHAR('A'));
  ASSERT_EQ("\\u0061", w.UCHAR('a'));
  ASSERT_EQ("\\u007f", w.UCHAR('\u007f'));  // DEL
}

// ____________________________________________________________________________
TEST(WriterGrammarNT, RULE_10_UCHAR_UTF8) {
  // NT:  [10]  UCHAR
  //      https://www.w3.org/TR/n-triples/#grammar-production-UCHAR
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::NT> w{config, nullptr};

  using namespace std::literals::string_literals;
  ASSERT_EQ("\\u0000", w.UCHAR("\u0000"s));
  ASSERT_EQ("\\u0021", w.UCHAR("\u0021"));
  ASSERT_EQ("\\u0031", w.UCHAR("\u0031"));
  ASSERT_EQ("\\u0040", w.UCHAR("\u0040"));
  ASSERT_EQ("\\u0041", w.UCHAR("\u0041"));
  ASSERT_EQ("\\u0061", w.UCHAR("\u0061"));
  ASSERT_EQ("\\u007f", w.UCHAR("\u007f"));

  ASSERT_EQ("\\u00ff", w.UCHAR("\u00ff"));
  ASSERT_EQ("\\u0fff", w.UCHAR("\u0fff"));
  ASSERT_EQ("\\uffff", w.UCHAR("\uffff"));
  ASSERT_EQ("\\uffff", w.UCHAR("\U0000ffff"));
  ASSERT_EQ("\\U000fffff", w.UCHAR("\U000fffff"));
}

// ____________________________________________________________________________
// ____________________________________________________________________________
// ____________________________________________________________________________
TEST(WriterGrammarTTL, RULE_18_IRIREF) {
  // TTL: [18]   IRIREF (same as NT)
  //      https://www.w3.org/TR/turtle/#grammar-production-IRIREF
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  ASSERT_EQ("<prefixsuffix>", w.IRIREF("prefix", "suffix"));
  ASSERT_EQ("<\\u003cprefix\\u003e\\u003csuffix\\u003e>", w.IRIREF("<prefix>", "<suffix>"));
}

// ____________________________________________________________________________
TEST(WriterGrammarTTL, RULE_18_IRIREF_CONVERT) {
  // TTL: [18]   IRIREF (same as NT)
  //      https://www.w3.org/TR/turtle/#grammar-production-IRIREF
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  ASSERT_EQ("", w.encodeIRIREF(""));
  ASSERT_EQ("allöwed", w.encodeIRIREF("allöwed"));
  ASSERT_EQ("\\u003c\\u003e\\u0022\\u007b\\u007d\\u007c\\u005e\\u0060\\u005c", w.encodeIRIREF("<>\"{}|^`\\"));
  using namespace std::literals::string_literals;
  ASSERT_EQ("\\u0000\\u0001\\u0019\\u0020", w.encodeIRIREF("\u0000\u0001\u0019\u0020"s));
}

// ____________________________________________________________________________
TEST(WriterGrammarTTL, RULE_22_STRING_LITERAL_QUOTE) {
  // TTL: [22]  STRING_LITERAL_QUOTE
  //      https://www.w3.org/TR/turtle/#grammar-production-STRING_LITERAL_QUOTE
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  ASSERT_EQ("\"\"", w.STRING_LITERAL_QUOTE(""));
  ASSERT_EQ("\"\\\"\"", w.STRING_LITERAL_QUOTE("\""));
  ASSERT_EQ("\"'\"", w.STRING_LITERAL_QUOTE("'"));
  ASSERT_EQ("\"\\\\\"", w.STRING_LITERAL_QUOTE("\\"));
  ASSERT_EQ("\"\\n\"", w.STRING_LITERAL_QUOTE("\n"));
  ASSERT_EQ("\"\\r\"", w.STRING_LITERAL_QUOTE("\r"));
  ASSERT_EQ("\"\t\"", w.STRING_LITERAL_QUOTE("\t"));
  ASSERT_EQ("\"abc\"", w.STRING_LITERAL_QUOTE("abc"));
}

// ____________________________________________________________________________
TEST(WriterGrammarTTL, RULE_23_STRING_LITERAL_SINGLE_QUOTE) {
  // TTL: [23]  STRING_LITERAL_SINGLE_QUOTE
  //      https://www.w3.org/TR/turtle/#grammar-production-STRING_LITERAL_SINGLE_QUOTE
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  ASSERT_EQ("''", w.STRING_LITERAL_SINGLE_QUOTE(""));
  ASSERT_EQ("'\"'", w.STRING_LITERAL_SINGLE_QUOTE("\""));
  ASSERT_EQ("'\\\''", w.STRING_LITERAL_SINGLE_QUOTE("'"));
  ASSERT_EQ("'\\\\'", w.STRING_LITERAL_SINGLE_QUOTE("\\"));
  ASSERT_EQ("'\\n'", w.STRING_LITERAL_SINGLE_QUOTE("\n"));
  ASSERT_EQ("'\\r'", w.STRING_LITERAL_SINGLE_QUOTE("\r"));
  ASSERT_EQ("'\t'", w.STRING_LITERAL_SINGLE_QUOTE("\t"));
  ASSERT_EQ("'abc'", w.STRING_LITERAL_SINGLE_QUOTE("abc"));
}

// ____________________________________________________________________________
TEST(WriterGrammarTTL, RULE_26_UCHAR_CODEPOINT) {
  // TTL: [26]  UCHAR
  //      https://www.w3.org/TR/turtle/#grammar-production-UCHAR
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  ASSERT_EQ("\\u0000", w.UCHAR(0x00U));
  ASSERT_EQ("\\u0064", w.UCHAR(0x64U));
  ASSERT_EQ("\\U000fff64", w.UCHAR(0xfff64U));
}

// ____________________________________________________________________________
TEST(WriterGrammarTTL, RULE_26_UCHAR_ASCII) {
  // TTL: [26]  UCHAR
  //      https://www.w3.org/TR/turtle/#grammar-production-UCHAR
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  ASSERT_EQ("\\u0000", w.UCHAR('\0'));  // NUL
  ASSERT_EQ("\\u0021", w.UCHAR('!'));
  ASSERT_EQ("\\u0031", w.UCHAR('1'));
  ASSERT_EQ("\\u0040", w.UCHAR('@'));
  ASSERT_EQ("\\u0041", w.UCHAR('A'));
  ASSERT_EQ("\\u0061", w.UCHAR('a'));
  ASSERT_EQ("\\u007f", w.UCHAR('\u007f'));  // DEL
}

// ____________________________________________________________________________
TEST(WriterGrammarTTL, RULE_26_UCHAR_UTF8) {
  // TTL: [26]  UCHAR
  //      https://www.w3.org/TR/turtle/#grammar-production-UCHAR
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  using namespace std::literals::string_literals;
  ASSERT_EQ("\\u0000", w.UCHAR("\u0000"s));
  ASSERT_EQ("\\u0021", w.UCHAR("\u0021"));
  ASSERT_EQ("\\u0031", w.UCHAR("\u0031"));
  ASSERT_EQ("\\u0040", w.UCHAR("\u0040"));
  ASSERT_EQ("\\u0041", w.UCHAR("\u0041"));
  ASSERT_EQ("\\u0061", w.UCHAR("\u0061"));
  ASSERT_EQ("\\u007f", w.UCHAR("\u007f"));

  ASSERT_EQ("\\u00ff", w.UCHAR("\u00ff"));
  ASSERT_EQ("\\u0fff", w.UCHAR("\u0fff"));
  ASSERT_EQ("\\uffff", w.UCHAR("\uffff"));
  ASSERT_EQ("\\uffff", w.UCHAR("\U0000ffff"));
  ASSERT_EQ("\\U000fffff", w.UCHAR("\U000fffff"));
}

// ____________________________________________________________________________
TEST(WriterGrammarTTL, RULE_136s_PREFIXEDNAME) {
  // TTL: [136s] PrefixedName
  //      https://www.w3.org/TR/turtle/#grammar-production-PrefixedName
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  ASSERT_EQ("prefix:suffix", w.PrefixedName("prefix", "suffix"));
  ASSERT_EQ("prefix:\\.bc", w.PrefixedName("prefix", ".bc"));
  ASSERT_EQ("prefix:a.c", w.PrefixedName("prefix", "a.c"));
  ASSERT_EQ("prefix:ab\\.", w.PrefixedName("prefix", "ab."));
  ASSERT_THROW(w.PrefixedName(".refix", ".bc"), std::domain_error);
  ASSERT_EQ("pref.x:\\.bc", w.PrefixedName("pref.x", ".bc"));
  ASSERT_THROW(w.PrefixedName("prefi.", ".bc"), std::domain_error);
}

// ____________________________________________________________________________
TEST(WriterGrammarTTL, RULE_167s_PN_PREFIX) {
  // TTL: [167s] PN_LOCAL
  //      https://www.w3.org/TR/turtle/#grammar-production-PN_PREFIX
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  ASSERT_EQ("AZaz", w.encodePN_PREFIX("AZaz"));
  // . not allowed as first or last char
  ASSERT_THROW(w.encodePN_PREFIX(".bc"), std::domain_error);
  ASSERT_EQ("a.c", w.encodePN_PREFIX("a.c"));
  ASSERT_THROW(w.encodePN_PREFIX("ab."), std::domain_error);
  // _ not allowed as first char
  ASSERT_THROW(w.encodePN_PREFIX("_bc"), std::domain_error);
  ASSERT_EQ("a_c", w.encodePN_PREFIX("a_c"));
  ASSERT_EQ("ab_", w.encodePN_PREFIX("ab_"));
  // 0 - 9 not allowed as first char
  ASSERT_THROW(w.encodePN_PREFIX("0bc"), std::domain_error);
  ASSERT_EQ("a0c", w.encodePN_PREFIX("a0c"));
  ASSERT_THROW(w.encodePN_PREFIX("9bc"), std::domain_error);
  ASSERT_EQ("a9c", w.encodePN_PREFIX("a9c"));
}

// ____________________________________________________________________________
TEST(WriterGrammarTTL, RULE_168s_PN_LOCAL) {
  // TTL: [168s] PN_LOCAL
  //      https://www.w3.org/TR/turtle/#grammar-production-PN_LOCAL
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  ASSERT_EQ("_:AZaz09", w.encodePN_LOCAL("_:AZaz09"));
  // . not allowed as first or last char
  ASSERT_EQ("\\.bc", w.encodePN_LOCAL(".bc"));
  ASSERT_EQ("a.c", w.encodePN_LOCAL("a.c"));
  ASSERT_EQ("ab\\.", w.encodePN_LOCAL("ab."));
  // - not allowed as first char
  ASSERT_EQ("\\-bc", w.encodePN_LOCAL("-bc"));
  ASSERT_EQ("a-c", w.encodePN_LOCAL("a-c"));
  ASSERT_EQ("ab-", w.encodePN_LOCAL("ab-"));
  // Escapes ... _ is handled by PN_CHARS_U -> allowed without escape
  //             . is handled by PN_LOCAL if not first or last character
  //             - is handled by PN_CHARS if not first character
  ASSERT_EQ("_\\~.-\\!\\$\\&\\'", w.encodePN_LOCAL("_~.-!$&'"));
  ASSERT_EQ("\\(\\)\\*\\+\\,\\;\\=\\/", w.encodePN_LOCAL("()*+,;=/"));
  ASSERT_EQ("\\?\\#\\@\\%", w.encodePN_LOCAL("?#@%"));
  // UTF8-Codepoint ranges ...
  ASSERT_EQ("\u00c0\u00d6", w.encodePN_LOCAL("\u00c0\u00d6"));
  ASSERT_EQ("\u00d8\u00f6", w.encodePN_LOCAL("\u00d8\u00f6"));
  ASSERT_EQ("\u00f8\u02ff", w.encodePN_LOCAL("\u00f8\u02ff"));
  ASSERT_EQ("\u0370\u037d", w.encodePN_LOCAL("\u0370\u037d"));
  ASSERT_EQ("\u037f\u1fff", w.encodePN_LOCAL("\u037f\u1fff"));
  ASSERT_EQ("\u200c\u200d", w.encodePN_LOCAL("\u200c\u200d"));
  ASSERT_EQ("\u2070\u218f", w.encodePN_LOCAL("\u2070\u218f"));
  ASSERT_EQ("\u2c00\u2fef", w.encodePN_LOCAL("\u2c00\u2fef"));
  ASSERT_EQ("\u3001\ud7ff", w.encodePN_LOCAL("\u3001\ud7ff"));
  ASSERT_EQ("\ufdf0\ufffd", w.encodePN_LOCAL("\ufdf0\ufffd"));
  ASSERT_EQ("\U00010000\U000EFFFF", w.encodePN_LOCAL("\U00010000\U000EFFFF"));
  // ... not as first:
  ASSERT_THROW(w.encodePN_LOCAL("\u00b7"), std::domain_error);
  ASSERT_EQ("a\u00b7", w.encodePN_LOCAL("a\u00b7"));
  ASSERT_THROW(w.encodePN_LOCAL("\u0300"), std::domain_error);
  ASSERT_EQ("a\u0300", w.encodePN_LOCAL("a\u0300"));
  ASSERT_THROW(w.encodePN_LOCAL("\u036f"), std::domain_error);
  ASSERT_EQ("a\u036f", w.encodePN_LOCAL("a\u036f"));
  ASSERT_THROW(w.encodePN_LOCAL("\u203f"), std::domain_error);
  ASSERT_EQ("a\u203f", w.encodePN_LOCAL("a\u203f"));
  ASSERT_THROW(w.encodePN_LOCAL("\u2040"), std::domain_error);
  ASSERT_EQ("a\u2040", w.encodePN_LOCAL("a\u2040"));
  // ... never:
  ASSERT_THROW(w.encodePN_LOCAL("\u00d7"), std::domain_error);
  ASSERT_THROW(w.encodePN_LOCAL("\u00f7"), std::domain_error);
}

// ____________________________________________________________________________
TEST(WriterGrammarTTL, RULE_170s_PERCENT_CODEPOINT) {
  // TTL: [170s] PERCENT
  //      https://www.w3.org/TR/turtle/#grammar-production-PERCENT
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  ASSERT_EQ("%00", w.encodePERCENT(0x00U));
  ASSERT_EQ("%64", w.encodePERCENT(0x64U));
  ASSERT_EQ("%0f%ff%64", w.encodePERCENT(0xfff64U));
}

// ____________________________________________________________________________
TEST(WriterGrammarTTL, RULE_170s_PERCENT_ASCII) {
  // TTL: [26]  UCHAR
  //      https://www.w3.org/TR/turtle/#grammar-production-UCHAR
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  ASSERT_EQ("%00", w.encodePERCENT('\0'));  // NUL
  ASSERT_EQ("%21", w.encodePERCENT('!'));
  ASSERT_EQ("%31", w.encodePERCENT('1'));
  ASSERT_EQ("%40", w.encodePERCENT('@'));
  ASSERT_EQ("%41", w.encodePERCENT('A'));
  ASSERT_EQ("%61", w.encodePERCENT('a'));
  ASSERT_EQ("%7f", w.encodePERCENT('\u007f'));  // DEL
}

// ____________________________________________________________________________
TEST(WriterGrammarTTL, RULE_170s_PERCENT_UTF8) {
  // TTL: [26]  UCHAR
  //      https://www.w3.org/TR/turtle/#grammar-production-UCHAR
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  using namespace std::literals::string_literals;
  ASSERT_EQ("%00", w.encodePERCENT("\u0000"s));
  ASSERT_EQ("%21", w.encodePERCENT("\u0021"));
  ASSERT_EQ("%31", w.encodePERCENT("\u0031"));
  ASSERT_EQ("%40", w.encodePERCENT("\u0040"));
  ASSERT_EQ("%41", w.encodePERCENT("\u0041"));
  ASSERT_EQ("%61", w.encodePERCENT("\u0061"));
  ASSERT_EQ("%7f", w.encodePERCENT("\u007f"));

  ASSERT_EQ("%ff", w.encodePERCENT("\u00ff"));
  ASSERT_EQ("%0f%ff", w.encodePERCENT("\u0fff"));
  ASSERT_EQ("%ff%ff", w.encodePERCENT("\uffff"));
  ASSERT_EQ("%ff%ff", w.encodePERCENT("\U0000ffff"));
  ASSERT_EQ("%0f%ff%ff", w.encodePERCENT("\U000fffff"));
}

// ____________________________________________________________________________
// ____________________________________________________________________________
// ____________________________________________________________________________

// ____________________________________________________________________________
TEST(WriterGrammar, UTF8_LENGTH_ASCII) {
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  ASSERT_EQ(1, w.utf8Length('\0'));  // NUL
  ASSERT_EQ(1, w.utf8Length('!'));
  ASSERT_EQ(1, w.utf8Length('1'));
  ASSERT_EQ(1, w.utf8Length('@'));
  ASSERT_EQ(1, w.utf8Length('A'));
  ASSERT_EQ(1, w.utf8Length('a'));
  ASSERT_EQ(1, w.utf8Length('\u007f'));  // DEL

}

// ____________________________________________________________________________
TEST(WriterGrammar, UTF8_LENGTH_UTF8) {
  // TTL: [26]  UCHAR
  //      https://www.w3.org/TR/turtle/#grammar-production-UCHAR
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  ASSERT_EQ(0, w.utf8Length(""));
  ASSERT_EQ(1, w.utf8Length("\u007F"));
  ASSERT_EQ(2, w.utf8Length("\u0080"));
  ASSERT_EQ(2, w.utf8Length("\u07FF"));
  ASSERT_EQ(3, w.utf8Length("\u0800"));
  ASSERT_EQ(3, w.utf8Length("\uFFFF"));
  ASSERT_EQ(4, w.utf8Length("\U00010000"));
}

// ____________________________________________________________________________
TEST(WriterGrammar, UTF8_CODEPOINT_ASCII) {
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  using namespace std::literals::string_literals;
  ASSERT_EQ(0x00U, w.utf8Codepoint("\u0000"s));
  ASSERT_EQ(0x21U, w.utf8Codepoint("\u0021"));
  ASSERT_EQ(0x31U, w.utf8Codepoint("\u0031"));
  ASSERT_EQ(0x40U, w.utf8Codepoint("\u0040"));
  ASSERT_EQ(0x41U, w.utf8Codepoint("\u0041"));
  ASSERT_EQ(0x61U, w.utf8Codepoint("\u0061"));
  ASSERT_EQ(0x7FU, w.utf8Codepoint("\u007f"));
}

// ____________________________________________________________________________
TEST(WriterGrammar, UTF8_CODEPOINT_UTF8) {
  // TTL: [26]  UCHAR
  //      https://www.w3.org/TR/turtle/#grammar-production-UCHAR
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> w{config, nullptr};

  ASSERT_EQ(0x7FU, w.utf8Codepoint("\u007f"));
  ASSERT_EQ(0x80U, w.utf8Codepoint("\u0080"));
  ASSERT_EQ(0x07FFU, w.utf8Codepoint("\u07ff"));
  ASSERT_EQ(0x0800U, w.utf8Codepoint("\u0800"));
  ASSERT_EQ(0xFFFFU, w.utf8Codepoint("\uffff"));
  ASSERT_EQ(0x10000U, w.utf8Codepoint("\U00010000"));
}

}  // namespace osm2ttl::ttl