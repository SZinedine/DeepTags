#include "../src/element/baseelement.h"

#include "catch.hpp"
#include <QFile>
#include <QString>

namespace be = BaseElement;

TEST_CASE("BaseElement class", "[BaseElement][baseelement]") {
    const QString    path("./_new md file.md");
    const QString title = "new markdown file title";

    be::createNewFile(path, title);
    REQUIRE(QFile::exists(path));

    SECTION("reading a newly created file") {
        REQUIRE(be::hasHeader(path));
        const StringList  header  = be::getHeader(path);
        const QString tagline = be::findTags(header);
        CHECK_FALSE(be::hasTagsKey(header));
    }

    SECTION("low level functions") {
        INFO("composing a header line from key and value");
        CHECK(be::composeStringItem(" some_key ", "  some value  ") == "some_key: 'some value'");
        CHECK(be::composeStringItem(" some_key ", "  some value'  ") == "some_key: 'some value''");
        CHECK(be::composeBoolItem("_good ", true) == "_good: true");
        CHECK(be::composeBoolItem("_good __ ", false) == "_good __: false");
        CHECK(be::composeArrayItem("array ",
                                   {" first one/subFirst", "second elem", "another for fun"}) ==
              "array: [first one/subFirst, second elem, another for fun]");

        CHECK(be::makeTitleLine(" random title ") == "title: 'random title'");
        CHECK(be::makeTitleLine(" this is a title  '").toStdString() == std::string("title: 'this is a title ''"));
        CHECK(be::makePinnedLine(true) == "pinned: true");
        CHECK(be::makePinnedLine(false) == "pinned: false");
        CHECK(be::makeFavoritedLine(true) == "favorited: true");
        CHECK(be::makeFavoritedLine(false) == "favorited: false");
        CHECK(be::makeDeletedLine(true) == "deleted: true");
        CHECK(be::makeDeletedLine(false) == "deleted: false");
        CHECK(be::makeTagsLine({"first/sub-first", "second/second2", "third/sub-third"}) ==
              "tags: [first/sub-first, second/second2, third/sub-third]");

        INFO("Test operations on header (vector)");
        const StringList header = {"pinned: true", "favorited: false",
                                   "tags: [first, second/third, fourth/fifth, sixth]",
                                   "title: 'random title'"};
        REQUIRE(be::hasPinnedKey(header));
        REQUIRE(be::hasFavoritedKey(header));
        REQUIRE(be::hasTagsKey(header));
        REQUIRE(be::hasTitleKey(header));
        REQUIRE_FALSE(be::hasDeletedKey(header));

        CHECK(be::findPinned(header) == header.at(0));
        CHECK(be::findFavorited(header) == header.at(1));
        CHECK(be::findTags(header) == header.at(2));
        CHECK(be::findTitle(header) == header.at(3));
        CHECK(be::findDeleted(header) == "");

        CHECK(be::parseString(header.at(3)) == "'random title'");
        CHECK(be::parseBool(header.at(0)));
        CHECK_FALSE(be::parseBool(header.at(1)));

        CHECK(be::getValue(header.at(0)) == "true");
        CHECK(be::getValue(header.at(1)) == "false");
        CHECK(be::getValue(header.at(2)) == "[first, second/third, fourth/fifth, sixth]");
        CHECK(be::getValue(header.at(3)) == "'random title'");

        CHECK((be::parseArray(header.at(2))).size() == 4);
        CHECK(be::parseArray(header.at(2))[0] == "first");
        CHECK(be::parseArray(header.at(2))[1] == "second/third");
        CHECK(be::parseArray(header.at(2))[2] == "fourth/fifth");
        CHECK(be::parseArray(header.at(2))[3] == "sixth");

        QString tag = " / tag / ";
        REQUIRE(be::processTag(tag) == "tag");
    }

    SECTION("title", "[title]") {
        REQUIRE(be::hasTitleKey(be::getHeader(path)));
        CHECK(be::makeTitleLine() == "title: 'untitled'");
        CHECK(be::makeTitleLine("good title") == "title: 'good title'");
        CHECK(be::makeTitleLine("good title  ") == "title: 'good title'");
        CHECK(be::makeTitleLine("    good title") == "title: 'good title'");
        CHECK(be::makeTitleLine("    good title   ") == "title: 'good title'");

        QString n_title = "new title";
        be::setTitle(path, n_title);

        INFO("verify if the title item has changed inside the file");
        StringList header = be::getHeader(path);
        CHECK(header.size() == 1);
        QString title_line = be::findTitle(header);
        CHECK(title_line == "title: 'new title'");
        QString title_value = be::getValue(title_line);
        CHECK(title_value == "'new title'");
    }

    SECTION("tests on Tags", "[tag][tags]") {
        INFO("validity of tags");
        CHECK(be::validTagToAdd("normal_tag"));
        CHECK(be::validTagToAdd("normal/tag"));
        CHECK(be::validTagToAdd("normal tag"));
        CHECK_FALSE(be::validTagToAdd("normal,tag"));
        CHECK_FALSE(be::validTagToAdd("All Notes"));
        CHECK_FALSE(be::validTagToAdd("Untagged"));
        CHECK_FALSE(be::validTagToAdd("Favorite"));
        CHECK_FALSE(be::validTagToAdd("Trash"));
        CHECK_FALSE(be::validTagToAdd("/something"));

        INFO("test the tags composition");
        const StringList  tgs{"Notebooks", "summaries", "history"};
        const QString combined = be::combineTags(tgs);
        REQUIRE(combined == "Notebooks/summaries/history");

        INFO("split a nested tag into particles");
        const StringList splited = be::split(combined, "/");
        REQUIRE(splited[0] == "Notebooks");
        CHECK(splited[1] == "summaries");
        CHECK(splited[2] == "history");

        const QString raw_tags_line = "tags: [Notebooks/summaries, status/done]";
        const StringList  tagsList      = be::parseArray(raw_tags_line);
        REQUIRE(tagsList[0] == "Notebooks/summaries");
        REQUIRE(tagsList[1] == "status/done");

        INFO("recompose a list of nested tags");
        const QString recomposed = be::makeTagsLine(tagsList);
        CHECK(recomposed == raw_tags_line);


        /**
         *      CONTINUE TESTING TAGS
         */
    }

    SECTION("deleted item testing wwith BaseElement class", "[BaseElement][deleted]") {
        INFO("isn't supposed to have a deleted item at this stage");
        CHECK_FALSE(be::hasDeletedKey(be::getHeader(path)));

        be::addDeletedItem("deleted: true", path);
        REQUIRE(be::hasDeletedKey(be::getHeader(path)));

        INFO("changing the value of the already existing line");
        be::setDeleted(path, false);
        REQUIRE(be::hasDeletedKey(be::getHeader(path)));
        REQUIRE_FALSE(be::isDeleted(path));

        be::setDeleted(path, true);
        REQUIRE(be::isDeleted(path));
        REQUIRE(be::hasDeletedKey(be::getHeader(path)));

        be::removeDeletedItemFromHeader(path);
        REQUIRE_FALSE(be::hasDeletedKey(be::getHeader(path)));
    }

    QFile::remove(path);
}
