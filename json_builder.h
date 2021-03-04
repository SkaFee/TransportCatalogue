#pragma once

#include "json.h"

#include <vector>

namespace json {

	enum class LastUsedMetod {
		NONE,
		START_DICT,
		KEY,
		VALUE,
		END_DICT,
		START_ARRAY,
		END_ARRAY
	};

	struct NodeGetter {
		Node operator() (std::nullptr_t)		const;
		Node operator() (std::string&& value)	const;
		Node operator() (bool&& value)			const;
		Node operator() (int&& value)			const;
		Node operator() (double&& value)		const;
		Node operator() (Array&& value)			const;
		Node operator() (Dict&& value)			const;
	};

	class StartArrayCommand;
	class EndArrayCommand;
	class Key_EndDictCommands;
	class StartDictCommand;
	class KeyValueCommand;
	class ArrayValueCommand;

	class Key_ValueContext;
	class ArrayValueItemContext;
	class KeyItemContext;
	class DictItemContext;
	class ArrayItemContext;
	
	class Builder final {
	public:
		Builder() = default;

		KeyItemContext Key(std::string s);
		Builder& Value(Node::Value v);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		Builder& EndDict();
		Builder& EndArray();
		Node Build();

	private:
		Node root_;
		std::vector<Node*> nodes_stack_;
		std::vector<std::string> key_stack_;
		LastUsedMetod last_method_ = LastUsedMetod::NONE;

		Node MakeNode(Node::Value&& v);

		bool LMIsNone();
		bool LMIsStartDict();
		bool LMIsKey();
		bool LMIsValue();
		bool LMIsEndDict();
		bool LMIsStartArray();
		bool LMIsEndArray();
	};

	class KeyValueCommand {
	public:
		KeyValueCommand(Builder& b);

		Key_ValueContext Value(Node::Value v);

	private:
		Builder& builder_;
	};

	class ArrayValueCommand {
	public:
		ArrayValueCommand(Builder& b);

		ArrayValueItemContext Value(Node::Value v);

	private:
		Builder& builder_;
	};

	class StartArrayCommand {
	public:
		StartArrayCommand(Builder& b);

		ArrayItemContext StartArray();

	private:
		Builder& builder_;
	};

	class EndArrayCommand {
	public:
		EndArrayCommand(Builder& b);

		Builder& EndArray();

	private:
		Builder& builder_;
	};

	class Key_EndDictCommands {
	public:
		Key_EndDictCommands(Builder& builder);

		KeyItemContext Key(std::string s);
		Builder& EndDict();

	private:
		Builder& builder_;
	};

	class StartDictCommand {
	public:
		StartDictCommand(Builder& builder);

		DictItemContext StartDict();

	private:
		Builder& builder_;
	};

	class KeyItemContext final 
		: public StartDictCommand
		, public StartArrayCommand
		, public KeyValueCommand {
	public:
		KeyItemContext(Builder& b);
	};

	class Key_ValueContext final 
		: public Key_EndDictCommands {
	public:
		Key_ValueContext(Builder& b);
	};

	class DictItemContext final 
		: public Key_EndDictCommands {
	public:
		DictItemContext(Builder& b);
	};

	class ArrayValueItemContext final 
		: public StartDictCommand
		, public EndArrayCommand
		, public StartArrayCommand
		, public ArrayValueCommand {
	public:
		ArrayValueItemContext(Builder& b);
	};

	class ArrayItemContext final 
		: public StartDictCommand
		, public EndArrayCommand
		, public StartArrayCommand
		, public ArrayValueCommand {
	public:
		ArrayItemContext(Builder& b);
	};
}