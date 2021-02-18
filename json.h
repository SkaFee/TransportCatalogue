#pragma once

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <functional>
#include <utility>

namespace json {

    class Node;
    using Dict  = std::unordered_map<std::string, Node, std::hash<std::string>>;
    using Array = std::vector<Node>;

    class ParsingError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    using NodeValue = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

    class Node final {
    public:
        Node();
        template <typename T>
        Node(T v)
            : value_(std::make_shared<NodeValue>(std::move(v)))
        {}

        bool operator==(const Node& node) const noexcept;
        bool operator!=(const Node& node) const noexcept;

        int                 AsInt()     const;
        double              AsDouble()  const;
        const std::string&  AsString()  const;
        bool                AsBool()    const;
        const Array&        AsArray()   const;
        const Dict&         AsMap()     const;

        bool IsNull()       const noexcept;
        bool IsInt()        const noexcept;
        bool IsDouble()     const noexcept;
        bool IsPureDouble() const noexcept;
        bool IsString()     const noexcept;
        bool IsBool()       const noexcept;
        bool IsArray()      const noexcept;
        bool IsMap()        const noexcept;

        void Print(std::ostream& out) const noexcept;

    private:
        std::shared_ptr<NodeValue> value_;

        struct Printer {
            std::ostream& output;

            void operator()(std::nullptr_t)             const noexcept;
            void operator()(const Array& arr)           const noexcept;
            void operator()(const Dict& dict)           const noexcept;
            void operator()(const bool b)               const noexcept;
            void operator()(const int num)              const noexcept;
            void operator()(const double num)           const noexcept;
            void operator()(const std::string_view str) const noexcept;
        };
    };

    class Document final {
    public:
        explicit Document(Node root);

        bool operator==(const Document& doc) const noexcept;
        bool operator!=(const Document& doc) const noexcept;

        const Node& GetRoot() const;

    private:
        std::shared_ptr<Node> root_;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);
}