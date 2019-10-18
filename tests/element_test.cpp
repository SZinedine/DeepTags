#include "../src/element/element.h"

#include "catch.hpp"


SCENARIO("Element", "[element]") {
    const fs::path    filename("./_new md file_.md");
    const std::string title = "new markdown file title";
    be::createNewFile(filename, title);
    Element element(filename);

    GIVEN("a new created element to read from") {
        REQUIRE(be::hasHeader(element.path()));
        CHECK(element.hasTitleLine());
        CHECK(element.title() == title);
        CHECK_FALSE(element.hasPinnedLine());
        CHECK_FALSE(element.hasDeletedLine());
        CHECK_FALSE(element.hasFavoritedLine());
        CHECK_FALSE(element.hasTagsLine());
        CHECK_FALSE(element.pinned());
        CHECK_FALSE(element.favorited());
        CHECK_FALSE(element.deleted());
        CHECK(element.untagged());
    }

    GIVEN("an element without tags") {
        REQUIRE(element.tags().empty());
        CHECK(element.untagged());

        WHEN("Writing tags") {
            INFO("first time writing");
            const StringList tgs{"Notebooks/test", "status/in_test"};
            element.changeTags(tgs);
            const StringList new_tags = be::getUnparsedTags(element.getHeader());

            THEN("the file should contain 2 tags entries") {
                REQUIRE(new_tags.size() == 2);
                CHECK(new_tags[0] == tgs[0]);
                CHECK(new_tags[1] == tgs[1]);
            }

            INFO("append non existing tag");
            const std::string toAppend = "new/tag/append";
            element.appendTag(toAppend);

            THEN("should have 3 tags") {
                const StringList new_tags2 = be::getUnparsedTags(element.getHeader());
                REQUIRE(new_tags2.size() == 3);
                CHECK(new_tags2[2] == toAppend);
            }


            INFO("append existing tag");
            element.appendTag(toAppend);

            THEN("should remain with 3 tags == shouldn't have changed") {
                const StringList new_tags2 = be::getUnparsedTags(element.getHeader());
                REQUIRE(new_tags2.size() == 3);
                CHECK(new_tags2[0] == tgs[0]);
                CHECK(new_tags2[1] == tgs[1]);
                CHECK(new_tags2[2] == toAppend);
            }

            INFO("removing the existing tags");
            element.removeTagsLine();

            THEN("should have 0 tags after this") {
                const StringList new_tags3 = be::getUnparsedTags(element.getHeader());
                REQUIRE(new_tags3.size() == 0);
                CHECK_FALSE(element.hasTagsLine());
            }
        }
    }

    GIVEN("a new empty element without pinned line") {
        THEN("shouldn't have a pinned line") {
            REQUIRE_FALSE(element.hasPinnedLine());
            CHECK_FALSE(element.pinned());
        }

        WHEN("We change the value of pinned to true") {
            element.changePinned(true);

            THEN(" create a header line with the key pinned") {
                CHECK(element.hasPinnedLine());
                CHECK(element.pinned());
            }
        }

        WHEN("remove the pinned element from the file") {
            element.removePinnedLine();

            THEN("no 'pinned' key should be found in header")
            CHECK_FALSE(element.pinned());
            CHECK_FALSE(element.hasPinnedLine());
        }

        WHEN("the pinned item is set to false") {
            element.changePinned(false);

            THEN("the pinned key should be removed") {
                CHECK_FALSE(element.pinned());
                CHECK_FALSE(element.hasPinnedLine());
            }
        }
    }

    GIVEN("a new empty element without favorited line") {
        WHEN("a file is created") {
            THEN("it shouldn't contain favorited key") {
                REQUIRE_FALSE(element.hasFavoritedLine());
                CHECK_FALSE(element.favorited());
            }
        }

        WHEN("favorited is set to true") {
            element.changeFavorited(true);
            THEN("it should create the key and initialize the local variable") {
                CHECK(element.hasFavoritedLine());
                CHECK(element.favorited());
            }
        }

        WHEN("favorited is set to false") {
            element.changeFavorited(false);
            THEN("it should remove the favorited key") {
                CHECK_FALSE(element.hasFavoritedLine());
                CHECK_FALSE(element.favorited());
            }
        }
    }


    GIVEN("a newly created file that hasn't deleted key in its header") {
        WHEN("a file is created") {
            THEN("it shouldn't have 'deleted' key in its header") {
                REQUIRE_FALSE(element.hasDeletedLine());
                CHECK_FALSE(element.deleted());
            }
        }

        WHEN("deleted is set to true") {
            element.changeDeleted(true);
            THEN("the key is created with the value true") {
                CHECK(element.hasDeletedLine());
                CHECK(element.deleted());
            }
        }

        WHEN("deleted is set to false") {
            element.changeDeleted(false);
            THEN("the key should be removeed from the header") {
                CHECK_FALSE(element.hasDeletedLine());
                CHECK_FALSE(element.deleted());
            }
        }
    }

    fs::remove(filename);
}
