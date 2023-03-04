#include <algorithm>
#include <climits>
#include <iostream>
#include "next.h"

using namespace std;

// ---------- Code Summary -------------
// to solve this problem this code is using persistent segment tree and operations on itself
// it allows for time complexity of O(log(n)) to find the lowest index in range of
// it uses every further index as "new" updated root and one branch of persistent segment tree

// this structure represents data stored on end nodes of the tree
struct leaf {
    int originalValue;
    int index;
};

// persistent segment tree node that allows to create easily new child nodes
struct Node {
    Node *leftChild;
    Node *rightChild;
    int value = INT_MAX;

    explicit Node(int setValue) : leftChild(nullptr), rightChild(nullptr), value(setValue) { }
    explicit Node(Node *node) : leftChild(node->leftChild), rightChild(node->rightChild), value(node->value) { }
    Node(Node *leftNode, Node *rightNode) {
        leftChild = leftNode;
        rightChild = rightNode;

        int leftValue = INT_MAX;
        int rightValue = INT_MAX;

        if (leftChild != nullptr) {
            leftValue = leftChild->value;
        }
        if (rightChild != nullptr) {
            rightValue = rightChild->value;
        }

        if(leftValue <= rightValue) {
            value = leftValue;
        }
        else {
            value = rightValue;
        }
    }
};

vector<Node*> roots;
vector<leaf*> sortedInput;
vector<Node*> freeOnDone; // vector that stores node to be freed after done() function is called
int counter;

// return leave with lower value
bool compareLeaves(leaf* const& left, leaf* const& right) {
    return left->originalValue < right->originalValue;
}

// translate input array into data that is incremented by 1 while maintaining order of those elements
// it allows to easily reference given range as array indexes
vector<leaf*> remap(vector<int> &input) {
    counter = (int)input.size();
    vector<leaf*> remappedInput;

    // push back data into a new array and sort it
    for (int i = 0; i < (int)input.size(); ++i) {
        leaf* newLeaf = new leaf;
        newLeaf->originalValue = input[i];
        newLeaf->index = i + 1;
        sortedInput.push_back(newLeaf);
        remappedInput.push_back(newLeaf);
    }
    sort(sortedInput.begin(), sortedInput.end(), &compareLeaves);

    // translate data into simple 0, 1, 2, 3... (for duplicate values index is the same)
    int iterator = 1;
    sortedInput[0]->index = 1;
    for (int i = 1; i < (int)sortedInput.size(); ++i) {
        if(sortedInput[i - 1]->originalValue != sortedInput[i]->originalValue) {
            iterator++;
        }
        sortedInput[i]->index = iterator;
    }
    return remappedInput;
}

// find the lowest possible value in an input array
// that is still higher or equal to a given number and return remapped index of such value
// this function uses binary search, so it's complexity is log(n)
int binarySearchLow(int a) {
    int left = 0;
    int right = (int)sortedInput.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (sortedInput[mid]->originalValue >= a) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    if (left >= (int)sortedInput.size() || left == (int)sortedInput.size() || sortedInput[left]->originalValue < a) return -1;
    return sortedInput[left]->index;
}

// find the highest possible value in an input array
// that is still lower or equal to a given number and return remapped index of such value
// this function uses binary search, so it's complexity is log(n)
int binarySearchHigh(int b) {
    int left = 0;
    int right = (int)sortedInput.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (sortedInput[mid]->originalValue <= b) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    if (left == 0 || sortedInput[left-1]->originalValue > b) return -1;
    return sortedInput[left-1]->index;
}

// for a given range return index value of remapped input that it represents
pair<int, int> remappedRange(int a, int b) {
    int first = binarySearchLow(a);
    int last = binarySearchHigh(b);
    pair<int, int> range;
    range.first = first;
    range.second = last;
    return range;
}

// create a persistent segment tree with empty nodes that are updated for every new "version" of tree
// every update of a "version" is represented by next index (counting backwards) of an input vector
// if end leaf is injected by update function then every parent node is also updated,
// and it keeps the lowest index value of its children
// thus the time complexity of this function is O(n)
Node* buildPersistentSegmentTree(int iteration = 1, int count = counter) {
    if (iteration == count) {
        Node *newNode = new Node(INT_MAX);
        freeOnDone.push_back(newNode);
        return newNode;
    }
    else {
        Node *newNode = new Node(buildPersistentSegmentTree(iteration, (iteration + count) / 2), buildPersistentSegmentTree((iteration + count) / 2 + 1, count));
        freeOnDone.push_back(newNode);
        return newNode;
    }
}

// update end leaf of a tree and every parent connected from leaf to the root and itself
// updating the values for lowest index for every parent in such branch
// for every update it also pushes new "version" of a root for each and every nex index
Node* updateTree(Node* node, int value, int position, int left = 1, int right = counter) {
    if (left == right) {
        Node* newNode = new Node(value);
        freeOnDone.push_back(newNode);
        return newNode;
    }
    if (position > (left + right) / 2) { // position to udpate is after middle
        Node* newNode = new Node(node->leftChild, updateTree(node->rightChild, value, position, (left + right) / 2 + 1, right));
        freeOnDone.push_back(newNode);
        return newNode;
    }
    else { // position to update node is before middle
        Node* newNode = new Node(updateTree(node->leftChild, value, position, left, (left + right) / 2), node->rightChild);
        freeOnDone.push_back(newNode);
        return newNode;
    }
}

// initialize the persistent segment tree and fill it using functions above
void init(const std::vector<int> &input) {
    vector<int> inputData;
    for (auto inputValue : input) {
        inputData.push_back(inputValue);
    }
    vector<leaf*> remapped = remap(inputData);

    roots.resize(counter + 1);
    roots[counter] = buildPersistentSegmentTree();

    for (int i = counter - 1; i > -1; --i) {
        Node* v = new Node(roots[i + 1]);
        freeOnDone.push_back(v);
        roots[i] = updateTree(v, i, remapped[i]->index);
    }
}

// helper function allowing for recursive search through a tree for border values
// because it only needs to check branches of a given range it still keeps the time complexity in O(log(n))
int findLowestIndex(Node* node, int minRange, int maxRange, int left = 1, int right = counter) {
    if (left > maxRange || right < minRange) {
        return INT_MAX;
    }
    if (left >= minRange && right <= maxRange) {
        return node->value;
    }
    int mid = (left + right) / 2;
    int queryLeft = findLowestIndex(node->leftChild, minRange, maxRange, left, mid);
    int queryRight = findLowestIndex(node->rightChild, minRange, maxRange, mid + 1, right);
    return min(queryLeft, queryRight);
}

// find the lowest possible index for given range and index
// given index represents version of a tree that is going to be searched through
// and for an existing tree it reduces the tree to a given range
// because of that time complexity is still O(log(n))
int nextInRange(int index, int minRange, int maxRange) {
    pair<int, int> remappedInput = remappedRange(minRange, maxRange);

    if (remappedInput.first == -1) {
        return -1;
    }

    int ans = findLowestIndex(roots[index], remappedInput.first, remappedInput.second);
    return ((ans == INT_MAX) ? -1 : ans);
}

// free the memory of initialized tree
void done() {
    for (auto x : freeOnDone) {
        delete(x);
    }
    for (auto x : sortedInput) {
        delete(x);
    }
    freeOnDone.clear();
    sortedInput.clear();
}