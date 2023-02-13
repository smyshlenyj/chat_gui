#include <iostream>
#include "TrieNode.h"

TrieNode* getNewNode()
{
	TrieNode* pNode = new TrieNode;
	pNode->isEndOfWord = false;
	pNode->freq = 1;

	for (size_t i = 0; i < ALPHABET_SIZE; i++) pNode->children[i] = nullptr;

	return pNode;
}

void insert(TrieNode* root, const std::string& key)
{
	TrieNode* node = root;

	for (size_t i = 0; i < key.length(); i++)
	{
		int index = key[i] - 'a';

		if (!node->children[index]) node->children[index] = getNewNode();
		else (node->children[index]->freq)++;

		node = node->children[index];
	}
	node->isEndOfWord = true;
}

void autocomplete(TrieNode* root, std::string currPrefix, const std::string& keyword, std::string* inputMessage)
{
	if (root->isEndOfWord)
	{
		currPrefix = currPrefix.substr(keyword.size());
		std::cout << currPrefix;
		*inputMessage = keyword + currPrefix;
		return;
	}

	for (size_t i = 0; i < ALPHABET_SIZE; i++)
		if (root->children[i])
		{
			char child = 'a' + i;
			autocomplete(root->children[i], currPrefix + child, keyword, inputMessage);
		}
}

void printAutoSuggestions(TrieNode* root, const std::string& keyword, std::string* inputMessage)
{
	std::string query = keyword;
	TrieNode* current = root;

	for (char c : query)
	{
		int ind = c - 'a';

		if (!current->children[ind]) return;

		current = current->children[ind];
	}

	return autocomplete(current, query, keyword, inputMessage);
}

TrieNode::~TrieNode()
{
	for (size_t i = 0; i < ALPHABET_SIZE; ++i)
		delete children[i];
}
