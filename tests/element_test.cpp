#define CATCH_CONFIG_MAIN
#include "../src/element/element.h"
#include "catch.hpp"

TEST_CASE("BaseElement class", "[BaseElement][baseelement]") {
    const fs::path path("./_new md file.md");
    const std::string title = "new markdown file title";
    
    BaseElement::createNewFile(path, title);
    REQUIRE(fs::exists(path));
    REQUIRE(BaseElement::hasHeader(path));      // the file should have a header when it is created with createNewFile()

    SECTION("title", "[title]") {
        REQUIRE( BaseElement::hasTitleItem(path) );
        REQUIRE( BaseElement::makeTitleLine() == "title: untitled" );
        REQUIRE( BaseElement::makeTitleLine("good title") == "title: good title" );
        REQUIRE( BaseElement::makeTitleLine("good title  ") == "title: good title" );       // the title string is trimmed
        REQUIRE( BaseElement::makeTitleLine("    good title") == "title: good title" );
        REQUIRE( BaseElement::makeTitleLine("    good title   ") == "title: good title" );

        std::string n_title = "new title";
        BaseElement::changeTitleInFile(n_title, path);      // change the title

        INFO("verify if the title item has changed inside the file");
        StringList header = BaseElement::getHeader(path);       // get the header of the file
        REQUIRE( header.size() == 1);
        std::string title_line = BaseElement::find_title_inheader(header);      // get the "title" inside the header (vector)
        REQUIRE( title_line == "title: new title" );
        std::string title_value = BaseElement::extract_title(title_line);
        REQUIRE( title_value == "new title" );
    }

    SECTION("tests on Tags", "[tag][tags]") {
        INFO("validity of tags");
        REQUIRE( BaseElement::validTagToAdd("normal_tag"));
        REQUIRE( BaseElement::validTagToAdd("normal/tag"));
        REQUIRE( BaseElement::validTagToAdd("normal tag"));
        REQUIRE_FALSE( BaseElement::validTagToAdd("normal,tag"));

        INFO("test the tags composition");
        const StringList tgs{ "Notebooks", "summaries", "history" };
        const std::string combined = BaseElement::combineTags(tgs);
        REQUIRE( combined == "Notebooks/summaries/history");

        INFO("split a nested tag into particles");
        const StringList splited = BaseElement::split_single_tag(combined, "/");
        REQUIRE(splited[0] == "Notebooks");
        REQUIRE(splited[1] == "summaries");
        REQUIRE(splited[2] == "history");

        const std::string raw_tags_line = "tags: [Notebooks/summaries, status/done]";
        const StringList tagsList = BaseElement::extract_tags(raw_tags_line);
        REQUIRE(tagsList[0] == "Notebooks/summaries");
        REQUIRE(tagsList[1] == "status/done");

        INFO("recompose a list of nested tags");
        const std::string recomposed = BaseElement::makeTagsLine(tagsList);
        REQUIRE(recomposed == "tags: [Notebooks/summaries, status/done]");


        /**
         *      CONTINUE TESTING TAGS
         */

    }

    fs::remove(path);
}

TEST_CASE("Element class", "[Element][element]") {
    const fs::path filename("./_new md file_.md");
    const std::string title = "new markdown file title";

    INFO("Creating a new file");
    Element::createNewFile(filename, title);
    REQUIRE(fs::exists(filename));
    REQUIRE( Element::hasHeader(filename) );
    REQUIRE( Element::hasTitleItem(filename) );
    
    INFO("Allocating Element object");
    Element* element = new Element(filename);
    REQUIRE(element->title() == title);

    SECTION("writing tags on the file", "[element][tags][tag]")
    {
        REQUIRE_FALSE( element->hasTagsLine() );
        const StringList tgs{"Notebooks/test", "status/in_test"};
        element->changeTags(tgs);
        const StringList new_tags = element->getUnparsedTagsValuesFromHeader();
        INFO("the written tags should be 2");
        REQUIRE(new_tags.size() == 2);
        REQUIRE(new_tags[0] == tgs[0]);
        REQUIRE(new_tags[1] == tgs[1]);

        const std::string toAppend = "new/tag/append";
        INFO("append unixisting tag");
        element->appendTag(toAppend);
        const StringList new_tags2 = element->getUnparsedTagsValuesFromHeader();
        REQUIRE( new_tags2.size() == 3 );
        INFO("append existing tag. nothing should be changed inside the file");
        REQUIRE( new_tags2.size() == 3 );
        REQUIRE( element->hasTagsLine() );

        INFO("removing the existing tags. the file should have 0 tags after this");
        element->removeTagsLine();
        const StringList new_tags3 = element->getUnparsedTagsValuesFromHeader();
        REQUIRE( new_tags3.size() == 0 );
        REQUIRE_FALSE( element->hasTagsLine() );
    }

    SECTION("tests related to the pinned item", "[pin][pinned]") 
    {
        INFO("the line pinned isn't supposed to exist at this point");
        REQUIRE_FALSE(element->pinned());
        REQUIRE_FALSE(element->hasPinnedLine());
        REQUIRE(element->getPinnedLineFromHeader() == "");

        INFO("set pinned to true. it is supposed to create it and update itself");
        element->changePinned(true);
        REQUIRE(element->hasPinnedLine());
        REQUIRE(element->pinned());
        REQUIRE(element->getPinnedValueFromHeader());
        
        INFO("remove the pinned element from the file");
        element->removePinnedLine();
        REQUIRE_FALSE(element->pinned());
        REQUIRE_FALSE(element->getPinnedValueFromHeader());
        REQUIRE_FALSE(element->hasPinnedLine());

        INFO("set to false. this should remove the item from the file");
        element->changePinned(false);
        REQUIRE(element->getPinnedLineFromHeader() == "");
        REQUIRE_FALSE(element->getPinnedValueFromHeader());
        REQUIRE_FALSE(element->pinned());
        REQUIRE_FALSE(element->hasPinnedLine());
    }

    SECTION("tests related to the favorite item", "[favorite][favorited]")
    {
        INFO("favorited isn't supposed to exist at this point");
        REQUIRE(element->getFavoritedLineFromHeader() == "");
        REQUIRE_FALSE(element->hasFavoritedLine());
        REQUIRE_FALSE(element->favorited());

        INFO("set to true = create the item");
        element->changeFavorited(true);
        REQUIRE(element->hasFavoritedLine());
        REQUIRE(element->favorited());
        REQUIRE(element->getFavoritedLineFromHeader() == "favorited: true");
        REQUIRE(element->getFavoritedValueFromHeader());

        INFO("Change the favorited item into false. this should remove it from the file");
        element->changeFavorited(false);
        REQUIRE_FALSE(element->hasFavoritedLine());
        REQUIRE_FALSE(element->favorited());
        REQUIRE_FALSE(element->getFavoritedValueFromHeader());
        REQUIRE(element->getFavoritedLineFromHeader() == "");
    }

    SECTION("testing title", "[title]") {

    }

    delete element;
    fs::remove(filename);
}







