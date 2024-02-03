#include <QTest>
#include <QVector>
#include "../src/DocumentUtils.h"
#include "Document.h"

static QVector<QString> fileContent {
    "---",
    "title: 'some title'",
    "pinned: false",
    "favorited: true",
    "deleted: false",
    "tags: [a/b/c, d/e/f, g, h]",
    "---",
    "",
    "Some title",
    "============",
    ""
};


class DocumentUtilsTest : public QObject {
    Q_OBJECT
private slots:
    void LAB();
    void arrayToStringTest();
    void deconstructTagTest();
    void isMDTest();
    void getHeaderTest();
    void getHeaderLineTest();
    void getHeaderValueTest();
    void hasHeaderKeyTest();
    void setHeaderValueTest();
    void deleteHeaderLineTest();
};


void DocumentUtilsTest::LAB() {
}

void DocumentUtilsTest::arrayToStringTest() {
    QStringVector sl{ "Earth/Africa/Algeria", "Sciences/Physics" };
    QString expected = "[Earth/Africa/Algeria, Sciences/Physics]";
    auto actual      = Doc::Utils::arrayToString(sl);

    QCOMPARE(actual, expected);
}


void DocumentUtilsTest::deconstructTagTest() {
    const QString tag   = "a/b/c";
    QStringVector expected = { "a", "b", "c" };
    QStringVector actual   = Doc::Utils::deconstructTag(tag);

    QCOMPARE(actual, expected);
}


void DocumentUtilsTest::isMDTest() {
    QVERIFY(Doc::Utils::isMD("abc.md"));
    QVERIFY(Doc::Utils::isMD("abc.a.md"));
    QVERIFY(Doc::Utils::isMD("abc.md.md"));
    QVERIFY(Doc::Utils::isMD("abc.MD"));
    QVERIFY(Doc::Utils::isMD("abc.mD"));
    QVERIFY(Doc::Utils::isMD("abc.Md"));
    QVERIFY(Doc::Utils::isMD("abc.b.Md"));

    QVERIFY(Doc::Utils::isMD("abc.markdown"));
    QVERIFY(Doc::Utils::isMD("abc.MARKDOWN"));
    QVERIFY(Doc::Utils::isMD("abc.Markdown"));

    QVERIFY(!Doc::Utils::isMD("abc.markdown."));
    QVERIFY(!Doc::Utils::isMD("abc.md."));
}


void DocumentUtilsTest::getHeaderTest() {
    QVector result{Doc::Utils::getHeader(fileContent)};
    QVector expected{ QVector(fileContent.begin() + 1, fileContent.begin() + 6)};

    QCOMPARE(result.size(), expected.size());

    for (int i = 0; i < result.size(); i++) {
        QCOMPARE(result.at(i), expected.at(i));
    }
}


void DocumentUtilsTest::getHeaderLineTest() {
    QVector header{Doc::Utils::getHeader(fileContent)};

    QString trueTitle = *(fileContent.begin()+1);
    QString truePinned = *(fileContent.begin()+2);
    QString trueFavorited = *(fileContent.begin()+3);
    QString trueDeleted = *(fileContent.begin()+4);
    QString trueTags = *(fileContent.begin()+5);
    
    QCOMPARE(trueTitle, Doc::Utils::getHeaderLine("title", header));
    QCOMPARE(truePinned, Doc::Utils::getHeaderLine("pinned", header));
    QCOMPARE(trueFavorited, Doc::Utils::getHeaderLine("favorited", header));
    QCOMPARE(trueDeleted, Doc::Utils::getHeaderLine("deleted", header));
    QCOMPARE(trueTags, Doc::Utils::getHeaderLine("tags", header));
    QCOMPARE(QString(), Doc::Utils::getHeaderLine("EMPTY", header));
}


void DocumentUtilsTest::getHeaderValueTest() {

    QVector header{Doc::Utils::getHeader(fileContent)};

    QCOMPARE(QString("some title"), Doc::Utils::getHeaderValue("title", header));
    QCOMPARE(QString("false"), Doc::Utils::getHeaderValue("pinned", header));
    QCOMPARE(QString("true"), Doc::Utils::getHeaderValue("favorited", header));
    QCOMPARE(QString("false"), Doc::Utils::getHeaderValue("deleted", header));
    QCOMPARE(QString("[a/b/c, d/e/f, g, h]"), Doc::Utils::getHeaderValue("tags", header));
    QCOMPARE(QString(""), Doc::Utils::getHeaderValue("EMPTY", header));
}


void DocumentUtilsTest::hasHeaderKeyTest() {
    auto header = Doc::Utils::getHeader(fileContent);
    QVERIFY(Doc::Utils::hasHeaderKey("title", header));
    QVERIFY(Doc::Utils::hasHeaderKey("pinned", header));
    QVERIFY(Doc::Utils::hasHeaderKey("favorited", header));
    QVERIFY(Doc::Utils::hasHeaderKey("deleted", header));
    QVERIFY(Doc::Utils::hasHeaderKey("tags", header));
    QVERIFY(!Doc::Utils::hasHeaderKey("abc", header));
}


void DocumentUtilsTest::setHeaderValueTest() {

    auto header = Doc::Utils::getHeader(fileContent);
    QString newTitle = "custom title";
    QString newTags = "[aaa]";

    Doc::Utils::setHeaderValue("title", newTitle, header);
    Doc::Utils::setHeaderValue("tags", newTags, header);

    QCOMPARE(Doc::Utils::getHeaderValue("title", header), newTitle);
    QCOMPARE(Doc::Utils::getHeaderValue("tags", header), newTags);
}


void DocumentUtilsTest::deleteHeaderLineTest() {
    auto header = Doc::Utils::getHeader(fileContent);
    int size = header.size();

    QVERIFY(Doc::Utils::hasHeaderKey("title", header));
    QVERIFY(Doc::Utils::deleteHeaderLine("title", header));
    QVERIFY(!Doc::Utils::hasHeaderKey("title", header));
    QCOMPARE(header.size(), size-1);
}


QTEST_MAIN(DocumentUtilsTest)
#include "DocumentUtilsTest.moc"
