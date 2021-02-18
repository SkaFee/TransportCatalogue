#include "json.h"

namespace json {

    namespace {
        using namespace std::literals;

        char ReadNextChar(std::istream& input) {
            if (char c; input >> c) {
                return c;
            }
            throw ParsingError("Failed to read from stream"s);
        }

        char ReadOneChar(std::istream& input) {
            if (!input) {
                throw ParsingError("Failed to read from stream"s);
            }
            return static_cast<char>(input.get());
        }

        Node LoadNode(std::istream& input);

        Node LoadNull(std::istream& input) {
            std::string s;
            for (int i = 0; i < 4; ++i) {
                s += ReadOneChar(input);
            }

            if (s != "null"s) {
                throw ParsingError("Failed to read \"null\" from stream"s);
            }
            return Node();
        }

        Node LoadArray(std::istream& input) {
            Array result;

            for (char c = ReadNextChar(input); c != ']'; c = ReadNextChar(input)) {
                if (c != ',') {
                    input.putback(c);
                }
                result.emplace_back(std::move(LoadNode(input)));
            }

            return Node(std::move(result));
        }

        Node LoadNumber(std::istream& input) {
            std::string parsed_num;

            auto read_char = [&parsed_num, &input] {
                parsed_num += ReadOneChar(input);
                if (!input) {
                    throw ParsingError("Failed to read from stream"s);
                }
            };

            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }

            if (input.peek() == '0') {
                read_char();
            } else {
                read_digits();
            }

            bool is_int = true;
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    try {
                        return Node(std::stoi(parsed_num));
                    } catch (...) {

                    }
                }
                return Node(std::stod(parsed_num));
            } catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        Node LoadString(std::istream& input) {
            std::string line;

            while (input.peek() != '\"') {
                line += ReadOneChar(input);
                if (line.back() == '\\') {
                    char c = ReadOneChar(input);
                    line.pop_back();
                    switch (c) {
                    case '\"':
                        line += '\"';
                        break; 
                    case 'n': 
                        line += '\n'; 
                        break; 
                    case 'r': 
                        line += '\r';
                        break;
                    case '\\': 
                        line += '\\'; 
                        break; 
                    case 't': 
                        line += '\t'; 
                        break; 
                    default: 
                        throw ParsingError("bad string"s); 
                    }
                }
            }
            ReadOneChar(input);

            return Node(std::move(line));
        }

        Node LoadBool(std::istream& input) {
            std::string s;

            int sz = (input.peek() == 'f') ? 5 : 4;
            for (int i = 0; i < sz; ++i) {
                s += ReadOneChar(input);
            }

            if (s == "false"s) {
                return Node(false);
            } else if (s == "true"s) {
                return Node(true);
            }
            throw ParsingError("Failed load bool"s);
        }

        Node LoadDict(std::istream& input) {
            Dict result;

            for (char c = ReadNextChar(input); c != '}'; c = ReadNextChar(input)) {
                if (c == ',') {
                    c = ReadNextChar(input);
                }
                std::string key = LoadString(input).AsString();
                c = ReadNextChar(input);
                if (c != ':') {
                    throw ParsingError("Failed to load dict"s);
                }
                result.insert({ std::move(key), std::move(LoadNode(input)) });
            }

            return Node(std::move(result));
        }

        Node LoadNode(std::istream& input) {
            char c = ReadNextChar(input);
            switch (c) {
            case '[':
                return LoadArray(input);
            case '{':
                return LoadDict(input);
            case '"':
                return LoadString(input);
            case 'f':
                input.putback(c);
                return LoadBool(input);
            case 't':
                input.putback(c);
                return LoadBool(input);
            case 'n':
                input.putback(c);
                return LoadNull(input);
            default:
                input.putback(c);
                return LoadNumber(input);
            }
        }

    }

    Node::Node() 
        : value_(std::make_shared<NodeValue>(std::nullptr_t{}))
    {}

    bool Node::operator==(const Node& node) const noexcept {
        return *value_.get() == *node.value_.get();
    }

    bool Node::operator!=(const Node& node) const noexcept {
        return !(*this == node);
    }

    int Node::AsInt() const {
        if (IsInt()) {
            return std::get<int>(*value_.get());
        }
        throw std::logic_error("Failed return int"s);
    }

    double Node::AsDouble() const {
        if (IsInt()) {
            return static_cast<double>(std::get<int>(*value_.get()));
        } else if (IsPureDouble()) {
            return std::get<double>(*value_.get());
        }
        throw std::logic_error("Failed return double, wrong type"s);
    }

    const std::string& Node::AsString() const {
        if (IsString()) {
            return std::get<std::string>(*value_.get());
        }
        throw std::logic_error("Failed return string, wrong type"s);
    }

    bool Node::AsBool() const {
        if (IsBool()) {
            return std::get<bool>(*value_.get());
        }
        throw std::logic_error("Failed return bool, wrong type"s);
    }

    const Array& Node::AsArray() const {
        if (IsArray()) {
            return std::get<Array>(*value_.get());
        }
        throw std::logic_error("Failed return Array, wrong type"s);
    }

    const Dict& Node::AsMap() const {
        if (IsMap()) {
            return std::get<Dict>(*value_.get());
        }
        throw std::logic_error("Failed return Array, wrong type"s);
    }

    bool Node::IsNull() const noexcept {
        return std::holds_alternative<std::nullptr_t>(*value_.get());
    }

    bool Node::IsInt() const noexcept {
        return std::holds_alternative<int>(*value_.get());
    }

    bool Node::IsDouble() const noexcept {
        return IsPureDouble() || IsInt();
    }

    bool Node::IsPureDouble() const noexcept {
        return std::holds_alternative<double>(*value_.get());
    }

    bool Node::IsString() const noexcept {
        return std::holds_alternative<std::string>(*value_.get());
    }

    bool Node::IsBool() const noexcept {
        return std::holds_alternative<bool>(*value_.get());
    }

    bool Node::IsArray() const noexcept {
        return std::holds_alternative<Array>(*value_.get());
    }

    bool Node::IsMap() const noexcept {
        return std::holds_alternative<Dict>(*value_.get());
    }

    void Node::Print(std::ostream& out) const noexcept {
        visit(Printer{ out }, *value_.get());
    }

    void Node::Printer::operator()(std::nullptr_t) const noexcept {
        output << "null"sv;
    }

    void Node::Printer::operator()(const Array& arr) const noexcept {
        output << "["sv;
        for (auto it = arr.begin(); it != arr.end(); ++it) {
            if (it != arr.begin()) {
                output << ", "sv;
            }
            (*it).Print(output);
        }
        output << "]"sv;
    }

    void Node::Printer::operator()(const Dict& dict) const noexcept {
        output << "{"sv;
        for (auto it = dict.begin(); it != dict.end(); ++it) {
            const auto& [str, value] = *it;
            if (it != dict.begin()) {
                output << ", "sv;
            }
            Printer{ output }(str);
            output << ": "sv;
            value.Print(output);
        }
        output << "}"sv;
    }

    void Node::Printer::operator()(const bool b) const noexcept {
        output << (b) ? "true"sv : "false"sv;
    }

    void Node::Printer::operator()(const int num) const noexcept {
        output << num;
    }

    void Node::Printer::operator()(const double num) const noexcept {
        output << num;
    }

    void Node::Printer::operator()(const std::string_view str) const noexcept {
        std::string result = "\""s;
        for (const char letter : str) {
            switch (letter) {
            case '\\':
                result += "\\\\"s;
                break;
            case '\"':
                result += "\\\""s;
                break;
            case '\n':
                result += "\\n"s;
                break;
            case '\r':
                result += "\\r"s;
                break;
            case '\t':
                result += "\\t"s;
                break;
            default:
                result += letter;
            }
        }
        output << std::move(result) + "\""s;
    }

    Document::Document(Node root)
        : root_(std::make_shared<Node>(std::move(root))) {
    }

    bool Document::operator==(const Document& doc) const noexcept {
        return *root_.get() == *doc.root_.get();
    }

    bool Document::operator!=(const Document& doc) const noexcept {
        return !(*this == doc);
    }

    const Node& Document::GetRoot() const {
        return *root_.get();
    }

    Document Load(std::istream& input) {
        return Document{ std::move(LoadNode(input)) };
    }

    void Print(const Document& doc, std::ostream& output) {
        doc.GetRoot().Print(output);
    }

}