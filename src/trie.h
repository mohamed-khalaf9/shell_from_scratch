#ifndef TRIE_H
#define TRIE_H
#include <unordered_map>
#include <string>
#include <vector>

struct TrieNode
{
    std::unordered_map<char, TrieNode *> children;
    bool isEndOfWord = false;
};

class Trie
{
private:
    TrieNode *root;

    void collectSuggestions(TrieNode *node, std::string prefix, std::vector<std::string> &suggestions)
    {
        if (node->isEndOfWord)
        {
            suggestions.push_back(prefix);
        }
        for (auto &child : node->children)
        {
            collectSuggestions(child.second, prefix + child.first, suggestions);
        }
    }

public:
    Trie()
    {
        root = new TrieNode();
    }

    void insert(const std::string &word)
    {
        TrieNode *current = root;
        for (char ch : word)
        {
            if (current->children.find(ch) == current->children.end())
            {
                current->children[ch] = new TrieNode();
            }
            current = current->children[ch];
        }
        current->isEndOfWord = true;
    }

    std::vector<std::string> searchPrefix(const std::string &prefix)
    {
        TrieNode *current = root;
        for (char ch : prefix)
        {
            if (current->children.find(ch) == current->children.end())
            {
                return {};
            }
            current = current->children[ch];
        }

        std::vector<std::string> suggestions;
        collectSuggestions(current, prefix, suggestions);
        return suggestions;
    }
};

#endif