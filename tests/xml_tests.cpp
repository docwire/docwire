#include "docwire.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace docwire;

TEST(XmlTests, IterateChildren)
{
    std::string xml = "<root><item>A</item><item>B</item></root>";
    xml::reader reader(xml);

    std::vector<std::string> contents;
    for (auto node : xml::children(xml::root_element(reader)))
    {
        if (node.name() == "item")
            contents.push_back(std::string(node.string_value()));
    }

    ASSERT_EQ(contents.size(), 2);
    EXPECT_EQ(contents[0], "A");
    EXPECT_EQ(contents[1], "B");
}

TEST(XmlTests, AttributeConversion)
{
    std::string xml = "<root><item id='42' cost='12.50' valid='true' /></root>";
    xml::reader reader(xml);

    bool found = false;
    for (auto node : xml::children(xml::root_element(reader)))
    {
        if (node.name() == "item")
        {
            found = true;
            auto id = xml::attribute_value<int>(node, "id");
            auto cost = xml::attribute_value<double>(node, "cost");
            
            ASSERT_TRUE(id.has_value());
            EXPECT_EQ(*id, 42);

            ASSERT_TRUE(cost.has_value());
            EXPECT_DOUBLE_EQ(*cost, 12.50);
        }
    }
    EXPECT_TRUE(found);
}

TEST(XmlTests, NestedStructure)
{
    std::string xml = R"(
        <library>
            <section name="Fiction"><book>Book 1</book></section>
            <section name="Non-Fiction"><book>Book 2</book></section>
        </library>
    )";
    xml::reader reader(xml);

    int book_count = 0;
    for (auto section : xml::children(xml::root_element(reader)))
        for (auto book : xml::children(section))
            if (book.name() == "book")
                book_count++;

    EXPECT_EQ(book_count, 2);
}

TEST(XmlTests, Descendants)
{
    std::string xml = "<root><item>A</item><group><item>B</item></group></root>";
    xml::reader reader(xml);

    std::vector<std::string> contents;
    for (auto node : xml::descendants(reader))
    {
        if (node.type() == xml::node_type::element && node.name() == "item")
            contents.push_back(std::string(node.string_value()));
    }

    ASSERT_EQ(contents.size(), 2);
    EXPECT_EQ(contents[0], "A");
    EXPECT_EQ(contents[1], "B");
}

TEST(XmlTests, IterateAttributes)
{
    std::string xml = "<root><item id='1' color='red' /></root>";
    xml::reader reader(xml);

    bool found = false;
    for (auto node : xml::children(xml::root_element(reader)))
    {
        if (node.name() == "item")
        {
            found = true;
            std::map<std::string, std::string> attrs;
            for (auto attr : xml::attributes(node))
            {
                attrs[std::string(attr.name())] = std::string(attr.value());
            }
            ASSERT_EQ(attrs.size(), 2);
            EXPECT_EQ(attrs["id"], "1");
            EXPECT_EQ(attrs["color"], "red");
        }
    }
    EXPECT_TRUE(found);
}

TEST(XmlTests, Namespaces)
{
    std::string xml = "<root xmlns:h='http://www.w3.org/TR/html4/'><h:table><h:tr><h:td>Apples</h:td></h:tr></h:table></root>";
    xml::reader reader(xml);

    bool found_td = false;
    for (auto node : xml::descendants(xml::root_element(reader)))
    {
        if (node.type() == xml::node_type::element && node.name() == "td")
        {
            found_td = true;
            EXPECT_EQ(node.string_value(), "Apples");
            EXPECT_EQ(std::string(node.full_name()), "h:td");
        }
    }
    EXPECT_TRUE(found_td);
}

TEST(XmlTests, ConvertNode)
{
    std::string xml = "<root><item>123.45</item></root>";
    xml::reader reader(xml);
    auto root = xml::root_element(reader);
    auto item = *xml::children(root).begin();
    EXPECT_DOUBLE_EQ(convert::to<double>(item), 123.45);
}

TEST(XmlTests, BlanksOption)
{
    std::string xml = "<root> <item>A</item> </root>";
    
    // Default behavior: keep blanks
    {
        xml::reader reader(xml);
        int count = 0;
        // <root> children: text(" "), item, text(" ")
        for (auto node : xml::children(xml::root_element(reader))) count++;
        EXPECT_EQ(count, 3);
    }

    // Ignore blanks
    {
        xml::reader reader(xml, xml::reader_blanks::ignore);
        int count = 0;
        // <root> children: item
        for (auto node : xml::children(xml::root_element(reader))) count++;
        EXPECT_EQ(count, 1);
    }
}

TEST(XmlTests, NodeDepth)
{
    std::string xml = "<root><level1><level2/></level1></root>";
    xml::reader reader(xml);
    auto root = xml::root_element(reader);
    EXPECT_EQ(root.depth(), 0);
    
    for(auto l1 : xml::children(root)) {
        EXPECT_EQ(l1.depth(), 1);
        for(auto l2 : xml::children(l1)) {
            EXPECT_EQ(l2.depth(), 2);
        }
    }
}

TEST(XmlTests, ConvertAttributeRef)
{
    std::string xml = "<item id='123' />";
    xml::reader reader(xml);
    auto root = xml::root_element(reader);
    bool found = false;
    for (auto attr : xml::attributes(root)) {
        if (attr.name() == "id") {
            EXPECT_EQ(convert::to<int>(attr), 123);
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

TEST(XmlTests, CDataHandling)
{
    std::string xml = "<root><![CDATA[<escaped>]]></root>";
    xml::reader reader(xml);
    auto root = xml::root_element(reader);
    EXPECT_EQ(root.string_value(), "<escaped>");
}
