#include "json.h"

using namespace std;

namespace json {

    namespace {

        char ReadNextChar(istream& input) {
            if (char c; input >> c) {
                return c;
            }
            throw ParsingError("Failed to read from stream"s);
        }
        char ReadOneChar(istream& input) {
            if (!input) {
                throw ParsingError("Failed to read from stream"s);
            }
            return static_cast<char>(input.get());
        }

        Node LoadNode(istream& input);

        Node LoadNull(istream& input) {
            string s;
            for (int i = 0; i < 4; ++i) {
                s += ReadOneChar(input);
            }

            if (s != "null"s) {
                throw ParsingError("Failed to read \"null\" from stream"s);
            }
            return Node();
        }

        Node LoadArray(istream& input) {
            Array result;

            for (char c = ReadNextChar(input); c != ']'; c = ReadNextChar(input)) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }

            return Node(move(result));
        }

        Node LoadNumber(istream& input) {
            string parsed_num;

            auto read_char = [&parsed_num, &input] {
                parsed_num += ReadOneChar(input);
                if (!input) {
                    throw ParsingError("Failed to read from stream"s);
                }
            };

            auto read_digits = [&input, read_char] {
                if (!isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (isdigit(input.peek())) {
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
                        return Node(stoi(parsed_num));
                    } catch (...) {

                    }
                }
                return Node(stod(parsed_num));
            } catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        Node LoadString(istream& input) {
            string line;

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

            return Node(move(line));
        }

        Node LoadBool(istream& input) {
            string s;

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

        Node LoadDict(istream& input) {
            Dict result;

            for (char c = ReadNextChar(input); c != '}'; c = ReadNextChar(input)) {
                if (c == ',') {
                    c = ReadNextChar(input);
                }
                string key = LoadString(input).AsString();
                c = ReadNextChar(input);
                if (c != ':') {
                    throw ParsingError("Failed to load dict"s);
                }
                result.insert({ move(key), move(LoadNode(input)) });
            }

            return Node(move(result));
        }

        Node LoadNode(istream& input) {
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
        : value_(make_shared<NodeValue>(nullptr_t{}))
    {}

    bool Node::operator==(const Node& node) const noexcept {
        return *value_.get() == *node.value_.get();
    }

    bool Node::operator!=(const Node& node) const noexcept {
        return !(*this == node);
    }

    int Node::AsInt() const {
        if (IsInt()) {
            return get<int>(*value_.get());
        }
        throw logic_error("Failed return int"s);
    }

    double Node::AsDouble() const {
        if (IsInt()) {
            return static_cast<double>(get<int>(*value_.get()));
        } else if (IsPureDouble()) {
            return get<double>(*value_.get());
        }
        throw logic_error("Failed return double, wrong type"s);
    }

    const string& Node::AsString() const {
        if (IsString()) {
            return get<string>(*value_.get());
        }
        throw logic_error("Failed return string, wrong type"s);
    }

    bool Node::AsBool() const {
        if (IsBool()) {
            return get<bool>(*value_.get());
        }
        throw logic_error("Failed return bool, wrong type"s);
    }

    const Array& Node::AsArray() const {
        if (IsArray()) {
            return get<Array>(*value_.get());
        }
        throw logic_error("Failed return Array, wrong type"s);
    }

    const Dict& Node::AsMap() const {
        if (IsMap()) {
            return get<Dict>(*value_.get());
        }
        throw logic_error("Failed return Array, wrong type"s);
    }

    bool Node::IsNull() const noexcept {
        return holds_alternative<nullptr_t>(*value_.get());
    }

    bool Node::IsInt() const noexcept {
        return holds_alternative<int>(*value_.get());
    }

    bool Node::IsDouble() const noexcept {
        return IsPureDouble() || IsInt();
    }

    bool Node::IsPureDouble() const noexcept {
        return holds_alternative<double>(*value_.get());
    }

    bool Node::IsString() const noexcept {
        return holds_alternative<string>(*value_.get());
    }

    bool Node::IsBool() const noexcept {
        return holds_alternative<bool>(*value_.get());
    }

    bool Node::IsArray() const noexcept {
        return holds_alternative<Array>(*value_.get());
    }

    bool Node::IsMap() const noexcept {
        return holds_alternative<Dict>(*value_.get());
    }

    void Node::Print(ostream& out) const noexcept {
        visit(Printer{ out }, *value_.get());
    }

    void Node::Printer::operator()(nullptr_t) const noexcept {
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
        output << ((b) ? "true"sv : "false"sv);
    }

    void Node::Printer::operator()(const int num) const noexcept {
        output << num;
    }

    void Node::Printer::operator()(const double num) const noexcept {
        output << num;
    }

    void Node::Printer::operator()(const string_view str) const noexcept {
        string result = "\""s;
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
        output << move(result) + "\""s;
    }

    Document::Document(Node root)
        : root_(make_shared<Node>(move(root))) {
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

    Document Load(istream& input) {
        return Document{ move(LoadNode(input)) };
    }

    void Print(const Document& doc, ostream& output) {
        doc.GetRoot().Print(output);
    }

}