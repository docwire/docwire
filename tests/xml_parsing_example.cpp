/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "df0.hpp"
#include <iostream>
#include <string>
#include <ranges>
#include <algorithm>

int main()
{
    using namespace df0;

    // Sample XML data representing a product catalog
    std::string xml_data = R"(
        <catalog>
            <product id="101" category="electronics">
                <name>Smartphone</name>
                <price currency="USD">699.99</price>
                <stock>50</stock>
            </product>
            <product id="102" category="books">
                <name>C++ Programming</name>
                <price currency="USD">49.99</price>
                <stock>120</stock>
            </product>
        </catalog>
    )";

    try
    {
        int products_found = 0;

        // Initialize the XML reader
        xml::reader reader(xml_data); // You can use xml::reader<relaxed> instead for maximum speed (skips safety checks)

        // Filter nodes using C++20 views
        auto products = xml::children(xml::root_element(reader))
                      | std::views::filter([](auto n) { return n.name() == "product"; });

        for (auto product_node : products)
        {
            // Iterate over attributes
            for (auto attr : xml::attributes(product_node))
            {
                if (attr.name() == "id")
                {
                    non_negative<int> id = convert::to<int>(attr);
                }
            }

            // Retrieve attributes with automatic type conversion
            non_negative<int> id = *xml::attribute_value<int>(product_node, "id");
            // attribute_value() returns checked<std::optional<T>>. Dereferencing (*) throws if the attribute is missing (preventing undefined behavior), unless xml::reader<relaxed> is used
            auto children = xml::children(product_node);
            auto category = xml::attribute_value(product_node, "category").value_or("General");
            if (id == 102) enforce(category == "books", "category"_v = category);

            // Find node using range algorithm
            // Note: Dereferencing the iterator throws if the element is not found (in strict mode)
            auto name = std::ranges::find_if(children, [](auto n) { return n.name() == "name"; })->string_value();
            if (id == 101) enforce(name == "Smartphone", "name"_v = name);
            else if (id == 102) enforce(name == "C++ Programming", "name"_v = name);

            // Find node using range algorithm
            auto price_node = std::ranges::find_if(children, [](auto n) { return n.name() == "price"; });

            // Convert node content to double
            double price = convert::to<double>(*price_node);
            auto currency = xml::attribute_value(*price_node, "currency").value_or("USD");
            enforce(currency == "USD", "currency"_v = currency);

            if (id == 101) enforce(price > 699 && price < 700, "price");
            products_found++;
        }
        enforce(products_found == 2);
    }
    catch (const std::exception& e) { std::cerr << "Error: " << errors::diagnostic_message(e) << std::endl; return 1; }
    return 0;
}