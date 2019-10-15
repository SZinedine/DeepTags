#include "catch.hpp"
#include "../src/element/baseelement.h"

namespace be = BaseElement;

TEST_CASE("BaseElement class", "[BaseElement][baseelement]") {
    const fs::path path("./_new md file.md");
    const std::string title = "new markdown file title";
    
    be::createNewFile(path, title);
    REQUIRE(fs::exists(path));
    REQUIRE(be::hasHeader(path));  // should have a header

    SECTION("title", "[title]") {
        REQUIRE( be::hasTitleKey( be::getHeader(path)) );
        CHECK( be::makeTitleLine() == "title: untitled" );
        CHECK( be::makeTitleLine("good title") == "title: good title" );
        CHECK( be::makeTitleLine("good title  ") == "title: good title" );  // should be trimed
        CHECK( be::makeTitleLine("    good title") == "title: good title" );
        CHECK( be::makeTitleLine("    good title   ") == "title: good title" );

        std::string n_title = "new title";
        be::setTitle(path, n_title);

        INFO("verify if the title item has changed inside the file");
        StringList header = be::getHeader(path);       // get the header of the file
        CHECK( header.size() == 1);
        std::string title_line = be::findTitle(header);
        CHECK( title_line == "title: new title" );
        std::string title_value = be::getValue(title_line);
        CHECK( title_value == "new title" );
    }

    SECTION("tests on Tags", "[tag][tags]") {
        INFO("validity of tags");
        CHECK( be::validTagToAdd("normal_tag"));
        CHECK( be::validTagToAdd("normal/tag"));
        CHECK( be::validTagToAdd("normal tag"));
        CHECK_FALSE( be::validTagToAdd("normal,tag"));
        CHECK_FALSE( be::validTagToAdd("All Notes"));
        CHECK_FALSE( be::validTagToAdd("Untagged"));
        CHECK_FALSE( be::validTagToAdd("Favorite"));
        CHECK_FALSE( be::validTagToAdd("Trash"));
        // CHECK_FALSE( be::validTagToAdd("/something"));

        INFO("test the tags composition");
        const StringList tgs{ "Notebooks", "summaries", "history" };
        const std::string combined = be::combineTags(tgs);
        REQUIRE( combined == "Notebooks/summaries/history");

        INFO("split a nested tag into particles");
        const StringList splited = be::split(combined, "/");
        REQUIRE( splited[0] == "Notebooks");
        CHECK(   splited[1] == "summaries");
        CHECK(   splited[2] == "history");

        const std::string raw_tags_line = "tags: [Notebooks/summaries, status/done]";
        const StringList tagsList = be::parseArray(raw_tags_line);
        REQUIRE(tagsList[0] == "Notebooks/summaries");
        REQUIRE(tagsList[1] == "status/done");

        INFO("recompose a list of nested tags");
        const std::string recomposed = be::makeTagsLine(tagsList);
        CHECK(recomposed == raw_tags_line);


        /**
         *      CONTINUE TESTING TAGS
         */
    }

    SECTION("deleted item testing wwith BaseElement class", "[BaseElement][deleted]") {
        INFO("isn't supposed to have a deleted item at this stage");
        CHECK_FALSE( be::hasDeletedKey(be::getHeader(path)) );

        be::addDeletedItem("deleted: true", path);
        REQUIRE( be::hasDeletedKey(be::getHeader(path)) );

        INFO("changing the value of the already existing line");
        be::setDeleted(path, false);
        REQUIRE( be::hasDeletedKey(be::getHeader(path)) );
        REQUIRE_FALSE( be::isDeleted(path) );

        be::setDeleted(path, true);
        REQUIRE( be::isDeleted(path) );
        REQUIRE( be::hasDeletedKey(be::getHeader(path)) );

        be::removeDeletedItemFromHeader(path);
        REQUIRE_FALSE( be::hasDeletedKey(be::getHeader(path)) );
    }

    fs::remove(path);
}
