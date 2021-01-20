#include "input_reader.h"

#include <vector>
#include <string>
#include <tuple>
#include <iostream>
#include <utility>

using namespace std;

size_t SplitGeneralPart(const string& str, vector<string>& words) {
	auto pos = str.find_first_of(' ', 0);
	words.push_back(str.substr(0, pos));
	++pos;
	auto new_pos = str.find_first_of(':', pos);
	words.push_back(str.substr(pos, new_pos - pos));
	++new_pos;

	return new_pos;
}

tuple<vector<string>, SeparatorType> SplitToWords(const string& str) {
	vector<string> words;
	SeparatorType sep_type;

	auto pos_start = SplitGeneralPart(str, words);
	const size_t sz_str = str.size();
	string word;
	for (size_t i = pos_start; i < sz_str; ++i) {
		if (str[i] == '>' || str[i] == '-') {
			sep_type = (str[i] == '>') ? SeparatorType::GREATER_THAN : SeparatorType::DASH;
			words.push_back(word.substr(1, word.size() - 2));
			word.clear();
		} else if (str[i] == ',') {
			words.push_back(word.substr(1, word.size() - 1));
			word.clear();
		} else {
			word += str[i];
		}
	}
	words.push_back(word.substr(1, word.size() - 1));

	return { words, sep_type };
}

Request WordsToRequest(vector<string>&& words, SeparatorType sep) {
	Request result;

	result.name = words[1].substr(0);
	if (words[0] == "Stop"s) {
		result.query_type = RequestType::STOP;
		result.words.reserve(2);
		result.words.push_back(words[2]);
		result.words.push_back(words[3]);
	} else {
		result.query_type = RequestType::BUS;
		const size_t sz_words = words.size();
		result.separator_type = sep;
		for (size_t i = 2; i < sz_words; ++i) {
			result.words.push_back(words[i]);
		}
	}

	return result;
}

FillingRequests InputFillingRequests() {
	FillingRequests result;

	int quantity;
	cin >> quantity;
	string tmp;
	getline(cin, tmp);
	vector<Request> requests;
	requests.reserve(quantity);

	for (int i = 0; i < quantity; ++i) {
		string line;
		getline(cin, line);
		auto [words, sep] = SplitToWords(line);
		result.requests.push_back(WordsToRequest(move(words), sep));
	}

	return result;
}