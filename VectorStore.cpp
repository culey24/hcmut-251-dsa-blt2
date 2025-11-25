// NOTE: Per assignment rules, only this single include is allowed here.
#include "VectorStore.h"
#include "main.h"
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include <vector>

// =====================================
// Helper functions
// =====================================

// Helper function to print n spaces for tree visualization
void printNSpace(int n)
{
    for (int i = 0; i < n; i++)
    {
        cout << " ";
    }
}

// =====================================
// AVLTree<K, T> implementation
// =====================================

template <class K, class T>
void AVLTree<K, T>::printTreeStructure() const
{
    int height = this->getHeight();
    if (this->root == NULL)
    {
        cout << "NULL\n";
        return;
    }
    queue<AVLNode *> q;
    q.push(root);
    AVLNode *temp;
    int count = 0;
    int maxNode = 1;
    int level = 0;
    int space = pow(2, height);
    printNSpace(space / 2);
    while (!q.empty())
    {
        temp = q.front();
        q.pop();
        if (temp == NULL)
        {
            cout << " ";
            q.push(NULL);
            q.push(NULL);
        }
        else
        {
            cout << temp->data;
            q.push(temp->pLeft);
            q.push(temp->pRight);
        }
        printNSpace(space);
        count++;
        if (count == maxNode)
        {
            cout << endl;
            count = 0;
            maxNode *= 2;
            level++;
            space /= 2;
            printNSpace(space / 2);
        }
        if (level == height)
            return;
    }
}

template<class K, class T> 
typename AVLTree<K, T>::AVLNode *AVLTree<K, T>::rotateRight(AVLNode *&node) {
    AVLNode* newRoot = node->pLeft;
    node->pLeft = newRoot->pRight;
    newRoot->pRight = node;
    return newRoot;
}

template<class K, class T> 
typename AVLTree<K, T>::AVLNode *AVLTree<K, T>::rotateLeft(AVLNode *&node) {
    AVLNode* newRoot = node->pRight;
    node->pRight = newRoot->pLeft;
    newRoot->pLeft = node;
    return newRoot;
}

template<class K, class T>
typename AVLTree<K, T>::AVLNode *AVLTree<K, T>::balanceLeftHeavyCase(AVLNode*& node, bool& broIsTaller) {
    /*
        Balance the node that have become Left-Heavy case after insertion has performed on AVL tree.    
        broIsTaller is to improve time complexity, 
    */
    AVLNode* leftChild = node->pLeft;

    // Case 1: ll case
    if (leftChild->balance == LH) {
        node->balance = EH;
        leftChild->balance = EH;
        node = rotateRight(node);
    }
    // Case 2: lr case
    else {
        AVLNode* rightOfLeftChild = leftChild->pRight;
        if (rightOfLeftChild->balance == LH) {
            node->balance = RH;
            leftChild->balance = EH;
            rightOfLeftChild->balance = EH;
        }
        else if (rightOfLeftChild->balance == RH) {
            node->balance = EH;
            leftChild->balance = LH; 
            rightOfLeftChild->balance = EH;
        }
        else {
            node->balance = EH;
            leftChild->balance = EH;
            rightOfLeftChild->balance = EH;
        }
        node->pLeft = rotateLeft(leftChild);
        node = rotateRight(node);
    }
    broIsTaller = false;
    return node;
}

template<class K, class T>
typename AVLTree<K, T>::AVLNode *AVLTree<K, T>::balanceRightHeavyCase(AVLNode*& node, bool& broIsTaller) {
    /*(
    Similar to above functions, but right 
    )*/
    AVLNode* rightChild = node->pRight;
    // Case 1: rr
    if (rightChild->balance == RH) {
        node->balance = EH;
        rightChild->balance = EH;
        node = rotateLeft(node);
    }
    // Case 2: rl
    else {
        AVLNode* leftOfRightChild = rightChild->pLeft;
        if (leftOfRightChild->balance == RH) {
            node->balance = LH;
            rightChild->balance = EH;
            leftOfRightChild->balance = EH;
        }
        else if (leftOfRightChild->balance == LH) {
            node->balance = EH;
            rightChild->balance = RH;
            leftOfRightChild->balance = EH;
        }
        else {
            node->balance = EH;
            rightChild->balance = EH;
            leftOfRightChild->balance = EH;
        }
        node->pRight = rotateRight(rightChild);
        node = rotateLeft(node);
    }
    broIsTaller = false;
    return node;
}

template <class K, class T>
typename AVLTree<K, T>::AVLNode* AVLTree<K, T>:: removeBalanceLeft(AVLNode*& node, bool& broIsShorter) {
    if (node->balance == LH) node->balance = EH;
    else if (node->balance == EH) {
        node->balance = RH;
        broIsShorter = false;
    }
    else {
        AVLNode* rightChild = node->pRight;
        if (rightChild->balance == RH) {
            node->balance = EH;
            rightChild->balance = EH;
            node = rotateLeft(node);
        }
        else if (rightChild->balance == EH) {
            node->balance = RH;
            rightChild->balance = LH;
            node = rotateLeft(node);
            broIsShorter = false;
        }
        else {
            AVLNode* leftOfRightChild = rightChild->pLeft;
            if (leftOfRightChild->balance == RH) {
                node->balance = LH;
                rightChild->balance = EH;
                leftOfRightChild->balance = EH;
            }
            else if (leftOfRightChild->balance == LH) {
                node->balance = EH;
                rightChild->balance = RH;
                leftOfRightChild->balance = EH;
            }
            else {
                node->balance = EH;
                rightChild->balance = EH;
                leftOfRightChild->balance = EH;
            }
            node->pRight = rotateRight(rightChild);
            node = rotateLeft(node);
        }
    }
    return node;
}

template <class K, class T>
typename AVLTree<K, T>::AVLNode* AVLTree<K, T>:: removeBalanceRight(AVLNode*& node, bool& broIsShorter) {
    if (node->balance == RH) node->balance = EH;
    else if (node->balance == EH) {
        node->balance = LH;
        broIsShorter = false;
    }
    else {
        AVLNode* leftChild = node->pLeft;
        if (leftChild->balance == LH) {
            node->balance = EH;
            leftChild->balance = EH;
            node = rotateRight(node);
        }
        else if (leftChild->balance == EH) {
            node->balance = LH;
            leftChild->balance = RH;
            node = rotateRight(node);
            broIsShorter = false;
        }
        else {
            AVLNode* rightOfLeftChild = leftChild->pRight;
            if (rightOfLeftChild->balance == LH) {
                node->balance = RH;
                leftChild->balance = EH;
                rightOfLeftChild->balance = EH;
            }
            else if (rightOfLeftChild->balance == RH) {
                node->balance = EH;
                leftChild->balance = LH;
                rightOfLeftChild->balance = EH;
            }
            else {
                node->balance = EH;
                leftChild->balance = EH;
                rightOfLeftChild->balance = EH;
            }
            node->pLeft = rotateLeft(leftChild);
            node = rotateRight(node);
        }
    }
    return node;
}

template<class K, class T>
AVLTree<K, T>::AVLTree() : root(nullptr) {}

template <class K, class T>
AVLTree<K, T>::~AVLTree() {
    this->clear();    
}

template <class K, class T>
typename AVLTree<K, T>::AVLNode* AVLTree<K, T>::insertHelper(AVLNode*& node, const K& key, const T& value, bool& broIsTaller) {
    if (node == nullptr) {
        broIsTaller = true;
        return new AVLNode(key, value);
    }
    if (key < node->key) {
        node->pLeft = insertHelper(node->pLeft, key, value, broIsTaller);
        if (broIsTaller) {
            if (node->balance == LH) node = balanceLeftHeavyCase(node, broIsTaller);
            else if (node->balance == EH) node->balance = LH;
            else {
                node->balance = EH;
                broIsTaller = false;
            }
        }
    }
    else if (key > node->key) {
        node->pRight = insertHelper(node->pRight, key, value, broIsTaller);
        if (broIsTaller) {
            if (node->balance == LH) {
                node->balance = EH;
                broIsTaller = false;
            }
            else if (node->balance == EH) node->balance = RH;
            else node = balanceRightHeavyCase(node, broIsTaller);
        }
    }
    else broIsTaller = false;
    return node;
}

template <class K, class T>
void AVLTree<K, T>::insert(const K& key, const T& value) {
    bool broIsTaller = false;
    this->root = insertHelper(this->root, key, value, broIsTaller);
}

template <class K, class T>
typename AVLTree<K, T>::AVLNode* AVLTree<K, T>::removeMinNode(AVLNode*& node, AVLNode*& minNode, bool& broIsShorter) {
    if (node->pLeft != nullptr) {
        node->pLeft = removeMinNode(node->pLeft, minNode, broIsShorter);
        if (broIsShorter) node = removeBalanceLeft(node, broIsShorter);
    }
    else {
        minNode = node;
        node = node->pRight;
        broIsShorter = true;
    }
    return node;
}

template <class K, class T>
typename AVLTree<K, T>::AVLNode* AVLTree<K, T>::removeHelper(AVLNode*& node, const K& key, bool& broIsShorter) {
    if (node == nullptr) {
        broIsShorter = false;
        return nullptr;
    }

    if (key < node->key) {
        node->pLeft = removeHelper(node->pLeft, key, broIsShorter);
        if (broIsShorter) node = removeBalanceLeft(node, broIsShorter);
    }
    else if (key > node->key) {
        node->pRight = removeHelper(node->pRight, key, broIsShorter);
        if (broIsShorter) node = removeBalanceRight(node, broIsShorter);
    }
    else {
        if (node->pLeft == nullptr) {
            AVLNode* rightChild = node->pRight;
            delete node;
            node = rightChild;
            broIsShorter = true;
        }
        else if (node->pRight == nullptr) {
            AVLNode* leftChild = node->pLeft;
            delete node;
            node = leftChild;
            broIsShorter = true;
        }
        else {
            AVLNode* minNode = nullptr;
            node->pRight = removeMinNode(node->pRight, minNode, broIsShorter);
            node->key = minNode->key;
            node->data = minNode->data;
            delete minNode;
            if (broIsShorter) node = removeBalanceRight(node, broIsShorter);
        }
    }
    return node;
}

template <class K, class T>
void AVLTree<K, T>::remove(const K& key) {
    bool broIsShorter = false;
    this->root = removeHelper(this->root, key, broIsShorter);
}

template <class K, class T>
bool AVLTree<K, T>::containsHelper(AVLNode* node, const K& key) const {
    if (node == nullptr) return false;
    if (key < node->key) return containsHelper(node->pLeft, key);
    else if (key > node->key) return containsHelper(node->pRight, key);
    else return true; 
}

template <class K, class T>
bool AVLTree<K, T>::contains(const K& key) const {
    return containsHelper(this->root, key);
}

template <class K, class T>
int AVLTree<K, T>::getHeightHelper(AVLNode* node) const {
    if (node == nullptr) return 0;
    return 1 + max(getHeightHelper(node->pLeft), getHeightHelper(node->pRight));
}

template <class K, class T>
int AVLTree<K, T>::getHeight() const {
    return getHeightHelper(this->root);
}

template <class K, class T>
int AVLTree<K, T>::getSizeHelper(AVLNode* node) const {
    if (node == nullptr) return 0;
    return 1 + getSizeHelper(node->pLeft) + getSizeHelper(node->pRight);
}

template <class K, class T>
int AVLTree<K, T>::getSize() const {
    return getSizeHelper(this->root);
}

template <class K, class T>
bool AVLTree<K, T>::empty() const {
    return (this->root == nullptr);
}

template <class K, class T>
void AVLTree<K, T>::clearHelper(AVLNode*& node) {
    if (node != nullptr) {
        clearHelper(node->pLeft);
        clearHelper(node->pRight);
        delete node;
        node = nullptr;
    }
}

template <class K, class T>
void AVLTree<K, T>::clear() {
    clearHelper(this->root);
}

template <class K, class T>
void AVLTree<K, T>::inOrderTraversalHelper(AVLNode* node, void (*action)(const T&)) const {
    if (node != nullptr) {
        inOrderTraversalHelper(node->pLeft, action);
        action(node->data);
        inOrderTraversalHelper(node->pRight, action);
    }
}

template <class K, class T>
void AVLTree<K, T>::inorderTraversal(void (*action)(const T&)) const {
    inOrderTraversalHelper(this->root, action);
}

// =====================================
// RBTNode implementation
// =====================================

template <class K, class T>
RedBlackTree<K, T>::RBTNode::RBTNode(const K& key, const T& value) {
    // inits bunch of value wall of text warning
    this->key = key;
    this->data = value;
    this->color = RED;
    this->left = nullptr;
    this->right = nullptr;
    this->parent = nullptr;
}

template <class K, class T>
void RedBlackTree<K, T>::RBTNode::recolorToRed() {
    this->color = RED;  
}

template <class K, class T>
void RedBlackTree<K, T>::RBTNode::recolorToBlack() {
    this->color = BLACK;  
}

// =====================================
// RedBlackTree<K, T> implementation
// =====================================

template <class K, class T>
void RedBlackTree<K, T>::printTreeStructure() const
{
    if (this->root == nullptr)
    {
        cout << "NULL\n";
        return;
    }

    queue<RBTNode *> q;
    q.push(root);
    RBTNode *temp;
    int count = 0;
    int maxNode = 1;
    int level = 0;

    // Calculate tree height for spacing
    int height = 0;
    queue<RBTNode *> heightQ;
    heightQ.push(root);
    while (!heightQ.empty())
    {
        int size = heightQ.size();
        height++;
        for (int i = 0; i < size; i++)
        {
            RBTNode *node = heightQ.front();
            heightQ.pop();
            if (node->left)
                heightQ.push(node->left);
            if (node->right)
                heightQ.push(node->right);
        }
    }

    int space = pow(2, height);
    printNSpace(space / 2);

    while (!q.empty())
    {
        temp = q.front();
        q.pop();

        if (temp == nullptr)
        {
            cout << "NULL";
            q.push(nullptr);
            q.push(nullptr);
        }
        else
        {
            // Print node data and color as tuple: (data, color)
            cout << "(" << temp->data << ",";
            if (temp->color == RED)
            {
                cout << "R)";
            }
            else
            {
                cout << "B)";
            }

            q.push(temp->left);
            q.push(temp->right);
        }

        printNSpace(space);
        count++;

        if (count == maxNode)
        {
            cout << endl;
            count = 0;
            maxNode *= 2;
            level++;
            space /= 2;
            printNSpace(space / 2);
        }

        if (level == height)
        {
            return;
        }
    }
}

// TODO: Implement all other RedBlackTree<K, T> methods here
template <class K, class T>
void RedBlackTree<K, T>::rotateLeft(RBTNode* node) {
    RBTNode* newRoot = node->right;
    node->right = newRoot->left;
    if (node->right != nullptr) node->right->parent = node; // FIXED
    newRoot->parent = node->parent;
    if (node->parent == nullptr) this->root = newRoot;
    else if (node->parent->left == node) node->parent->left = newRoot;
    else node->parent->right = newRoot;
    newRoot->left = node;
    node->parent = newRoot;
}

template <class K, class T>
void RedBlackTree<K, T>::rotateRight(RBTNode* node) {
    RBTNode* newRoot = node->left;
    node->left = newRoot->right;
    if (node->left != nullptr) node->left->parent = node; // FIXED
    newRoot->parent = node->parent;
    if (node->parent == nullptr) this->root = newRoot;
    else if (node->parent->left == node) node->parent->left = newRoot;
    else node->parent->right = newRoot;
    newRoot->right = node;
    node->parent = newRoot;
}

template <class K, class T> 
typename RedBlackTree<K, T>::RBTNode* RedBlackTree<K, T>::lowerBoundNode(const K &key) const {
    RBTNode* cursor = this->root;
    RBTNode* result = nullptr;
    while (cursor != nullptr) {
        if (cursor->key >= key) {
            result = cursor;
            cursor = cursor->left;
        }
        else cursor = cursor->right;
    }
    return result;
}

template <class K, class T> 
typename RedBlackTree<K, T>::RBTNode* RedBlackTree<K, T>::upperBoundNode(const K &key) const {
    RBTNode* cursor = this->root;
    RBTNode* result = nullptr;
    while (cursor != nullptr) {
        if (cursor->key > key) {
            result = cursor;
            cursor = cursor->left;
        }
        else cursor = cursor->right;
    }
    return result;
}

template <class K, class T>
RedBlackTree<K, T>::RedBlackTree() {
    this->root = nullptr;
}

template <class K, class T>
RedBlackTree<K, T>::~RedBlackTree() {
    this->clear();
}

template <class K, class T>
bool RedBlackTree<K, T>::empty() const {
    return (this->root == nullptr);
}

template<class K, class T>
int RedBlackTree<K, T>::getSizeHelper(RBTNode* node) const {
    if (node == nullptr) return 0;
    else return 1 + getSizeHelper(node->left) + getSizeHelper(node->right);
}

template <class K, class T>
int RedBlackTree<K, T>::size() const {
    return getSizeHelper(this->root);
}
template <class K, class T> 
void RedBlackTree<K, T>::clearHelper(RBTNode *node) {
    if (node == nullptr) return;
    clearHelper(node->left);
    clearHelper(node->right);
    delete node;
    return;
}

template <class K, class T>
void RedBlackTree<K, T>::clear() {
    clearHelper(this->root);
    this->root = nullptr;
}

template<class K, class T> 
void RedBlackTree<K, T>::handleLCase(RBTNode*& node) {
    // Case where parent is grandparent->left i think
    RBTNode* parent = node->parent;
    RBTNode* grandparent = parent->parent;
    RBTNode* uncle = grandparent->right;

    if (uncle != nullptr && uncle->color == RED) {
        grandparent->recolorToRed();
        parent->recolorToBlack();
        uncle->recolorToBlack();
        node = grandparent;
    } 
    else {
        if (node == parent->right) {
            rotateLeft(parent);
            node = parent;
            parent = node->parent;
        }
        rotateRight(grandparent);
        Color temp = parent->color;
        parent->color = grandparent->color;
        grandparent->color = temp;
        node = parent;
    }
}

template<class K, class T> 
void RedBlackTree<K, T>::handleRCase(RBTNode*& node) {
    // Case where parent is grandparent->right im pretty sure
    RBTNode* parent = node->parent;
    RBTNode* grandparent = parent->parent;
    RBTNode* uncle = grandparent->left;
    if (uncle != nullptr && uncle->color == RED) {
        grandparent->recolorToRed();
        parent->recolorToBlack();
        uncle->recolorToBlack();
        node = grandparent;
    }
    else {
        if (node == parent->left) {
            rotateRight(parent);
            node = parent;
            parent = node->parent;
        }
        rotateLeft(grandparent);
        Color temp = parent->color;
        parent->color = grandparent->color;
        grandparent->color = temp;
        node = parent;
    }
}

template <class K, class T> 
void RedBlackTree<K, T>::insertRebalance(RBTNode*& node) {
    while (node != this->root && node->color == RED && node->parent->color == RED) {
        RBTNode* parent = node->parent;
        RBTNode* grandparent = parent->parent;
        if (parent == grandparent->left) {
            handleLCase(node);
        }
        else if (parent == grandparent->right) {
            handleRCase(node);
        }
    }
    this->root->recolorToBlack();
}

template <class K, class T> 
void RedBlackTree<K, T>::insert(const K &key, const T &value) {
    RBTNode* newNode = new RBTNode(key, value);
    RBTNode* parent = nullptr;
    RBTNode* cursor = this->root;
    while (cursor != nullptr) {
        parent = cursor;
        if (key < cursor->key) {
            cursor = cursor->left;
        }
        else if (key > cursor->key) {
            cursor = cursor->right;
        }
        else {
            // i dont personally think waht is going on here
            delete newNode;
            return;
        }
    }
    newNode->parent = parent;
    if (parent == nullptr) this->root = newNode;
    else if (key < parent->key) {
        parent->left = newNode;
    }
    else if (key > parent->key) {
        parent->right = newNode;
    }   
    else {
        // case where we deleted so i just put this comment here for aura farming
    }
    if (newNode->parent != nullptr) insertRebalance(newNode);
    else newNode->recolorToBlack(); // root
}

template <class K, class T>
typename RedBlackTree<K, T>::RBTNode* RedBlackTree<K, T>::find(const K& key) const {
    RBTNode* cursor = this->root;
    while (cursor != nullptr) {
        if (key < cursor->key) cursor = cursor->left;
        else if (key > cursor->key) cursor = cursor->right;
        else return cursor;
    }
    return nullptr;
}

template <class K, class T>
bool RedBlackTree<K, T>::contains(const K& key) const {
    return (find(key) != nullptr);
}

template <class K, class T>
typename RedBlackTree<K, T>::RBTNode* RedBlackTree<K, T>::findMaxNode(RBTNode* node) {
    RBTNode* cursor = node;
    while (cursor->right != nullptr) cursor = cursor->right;
    return cursor;
}

template <class K, class T>
void RedBlackTree<K, T>::changeParent(RBTNode* a, RBTNode* b) {
    if (a == nullptr) return;
    if (a->parent == nullptr) this->root = b;
    else if (a == a->parent->left) a->parent->left = b;
    else if (a == a->parent->right) a->parent->right = b;
    if (b != nullptr) b->parent = a->parent;
    return;
}

template <class K, class T>
void RedBlackTree<K, T>::removeRebalanceLCase(RBTNode*& node, RBTNode*& parent) {
    RBTNode* stepBro = parent->right;
    if (stepBro->color == RED) {
        stepBro->recolorToBlack();
        parent->recolorToRed();
        rotateLeft(parent);
        stepBro = parent->right;
    }

    if ((stepBro->left == nullptr || stepBro->left->color == BLACK) && (stepBro->right == nullptr || stepBro->right->color == BLACK)) {
        stepBro->recolorToRed();
        node = parent;
        parent = node->parent;
    }
    else {
        if (stepBro->right == nullptr || stepBro->right->color == BLACK) {
            if (stepBro->left != nullptr) stepBro->left->recolorToBlack();
            stepBro->recolorToRed();
            rotateRight(stepBro);
            stepBro = parent->right;
        }
        
        stepBro->color = parent->color;
        parent->recolorToBlack();
        if (stepBro->right != nullptr) stepBro->right->recolorToBlack();
        rotateLeft(parent);
        node = this->root;
    }
    return;
}

template <class K, class T>
void RedBlackTree<K, T>::removeRebalanceRCase(RBTNode*& node, RBTNode*& parent) {
    RBTNode* stepBro = parent->left;
    if (stepBro->color == RED) {
        stepBro->recolorToBlack();
        parent->recolorToRed();
        rotateRight(parent);
        stepBro = parent->left;
    }
    if ((stepBro->right == nullptr || stepBro->right->color == BLACK) && (stepBro->left == nullptr || stepBro->left->color == BLACK)) {
        stepBro->recolorToRed();
        node = parent;
        parent = node->parent;
    } 
    else {
        if (stepBro->left == nullptr || stepBro->left->color == BLACK) {
            if (stepBro->right != nullptr) stepBro->right->recolorToBlack();
            stepBro->recolorToRed();
            rotateLeft(stepBro);
            stepBro = parent->left;

        }
        stepBro->color = parent->color;
        parent->recolorToBlack();
        if (stepBro->left != nullptr) stepBro->left->recolorToBlack();
        rotateRight(parent);
        node = this->root;
    }
    return;
}

template <class K, class T>
void RedBlackTree<K, T>::removeRebalance(RBTNode*& node, RBTNode*& parent) {
    while (node != this->root && (node == nullptr || node->color == BLACK)) {
        if (node == parent->left) this->removeRebalanceLCase(node, parent);
        else if (node == parent->right) this->removeRebalanceRCase(node, parent);
    }
    if (node != nullptr) node->recolorToBlack();
    return;
}

template <class K, class T>
void RedBlackTree<K, T>::remove(const K& key) {
    RBTNode* node = find(key);
    if (node == nullptr) return; // Nothing founded
    RBTNode* cursor = node;
    RBTNode* child = nullptr;
    RBTNode* parent = nullptr;
    
    Color cursorColor = node->color;

    if (node->left == nullptr) {
        child = node->right;
        parent = node->parent;
        changeParent(node, node->right);
    }
    else if (node->right == nullptr) {
        child = node->left;
        parent = node->parent;
        changeParent(node, node->left);
    }
    else {
        cursor = findMaxNode(node->left);
        cursorColor = cursor->color;
        child = cursor->left; // since max node doesnt have right child
        if (cursor->parent == node) parent = cursor;
        else {
            parent = cursor->parent;
            changeParent(cursor, cursor->left);
            cursor->left = node->left;
            cursor->left->parent = cursor;
        }
        changeParent(node, cursor);
        cursor->right = node->right;
        cursor->right->parent = cursor;
        cursor->color = node->color;
    }
    delete node;
    if (cursorColor == BLACK) removeRebalance(child, parent);
}

template <class K, class T>
typename RedBlackTree<K, T>::RBTNode* RedBlackTree<K, T>::lowerBound(const K& key, bool& found) const {
    RBTNode* temp = lowerBoundNode(key);
    if (temp == nullptr) {
        found = false;
        return nullptr;
    }
    else {
        found = true;
        return temp;
    }
}

template <class K, class T>
typename RedBlackTree<K, T>::RBTNode* RedBlackTree<K, T>::upperBound(const K& key, bool& found) const {
    RBTNode* vendih = upperBoundNode(key);
    if (vendih == nullptr) {
        found = false;
        return nullptr;
    }
    else {
        found = true;
        return vendih;
    }
}

// =====================================
// VectorRecord implementation
// =====================================

// Overload operator << for VectorRecord - prints only the id
std::ostream &operator<<(std::ostream &os, const VectorRecord &record)
{
    os << record.id;
    return os;
}

bool operator<(const VectorRecord& a, const VectorRecord& b) {
    return a.distanceFromReference < b.distanceFromReference;
}

// =====================================
// VectorStore implementation
// =====================================

// TODO: Implement all VectorStore methods here

VectorStore::VectorStore(int dimension,
                    std::vector<float>* (*embeddingFunction)(const std::string&),
                    const std::vector<float>& referenceVector) {
    this->dimension = dimension;
    this->embeddingFunction = embeddingFunction;
    this->referenceVector = new std::vector<float>(referenceVector);
    
    this->vectorStore = new AVLTree<double, VectorRecord>;
    this->normIndex = new RedBlackTree<double, VectorRecord>;
    this->count = 0;
    this->averageDistance = 0.0;
    this->rootVector = nullptr;
}   

VectorStore::~VectorStore() {
    this->clear();
}

int VectorStore::size() {
    return count;
}

bool VectorStore::empty() {
    return (count == 0);
}

void VectorStore::clear() {
    this->vectorStore->clear();
    this->normIndex->clear();
}

std::vector<float>* VectorStore::preprocessing(std::string rawText) {
    std::vector<float>* preprocessedVector = embeddingFunction(rawText);
    if (preprocessedVector->size() == this->dimension) {
        // aura farming
    }
    else if (preprocessedVector->size() > dimension) preprocessedVector->resize(this->dimension);
    else preprocessedVector->resize(this->dimension, 0);
    return preprocessedVector;
}

double VectorStore::calculateNorm(const std::vector<float>& vector) {
    double sum = 0.0;
    for (float i : vector) sum += i * i;
    return sqrt(sum);
}

void VectorStore::getAllRecords(AVLTree<double, VectorRecord>::AVLNode* node, std::vector<VectorRecord>& list) {
    if (node == nullptr) return;
    getAllRecords(node->pLeft, list);
    list.push_back(node->data);
    getAllRecords(node->pRight, list);
}

void VectorStore::getAllRecordPointers(AVLTree<double, VectorRecord>::AVLNode* node, std::vector<VectorRecord*>& list) {
    if (node == nullptr) return;
    getAllRecordPointers(node->pLeft, list);
    list.push_back(&(node->data));
    getAllRecordPointers(node->pRight, list);
}

AVLTree<double, VectorRecord>::AVLNode* VectorStore::rebuildAVL(const std::vector<VectorRecord>& sortedList, int start, int end) {
    if (start > end) return nullptr;
    int middleIndex = (start + end) / 2;
    const VectorRecord& record = sortedList[middleIndex];
    AVLTree<double, VectorRecord>::AVLNode* newNode = new AVLTree<double, VectorRecord>::AVLNode(record.distanceFromReference, record);
    newNode->balance = EH;
    newNode->pLeft = rebuildAVL(sortedList, start, middleIndex - 1);
    newNode->pRight = rebuildAVL(sortedList, middleIndex + 1, end);
    return newNode;
}

void VectorStore::addText(string rawText) {
    std::vector<float>* preprocessedVector = preprocessing(rawText);

    VectorRecord record;
    record.id = this->count;
    record.rawText = rawText;
    record.rawLength = rawText.size();
    record.vector = preprocessedVector;
    record.distanceFromReference = l2Distance(*preprocessedVector, *referenceVector);
    
    if (this->count == 0) this->averageDistance = record.distanceFromReference;
    else {
        double oldTotalDistance = this->averageDistance * this->count;
        this->averageDistance = (oldTotalDistance + record.distanceFromReference) / (this->count + 1);
    }
    this->count++;
    
    if (this->vectorStore->empty()) this->vectorStore->insert(record.distanceFromReference, record);
    else {
        double currentRootDistance = this->vectorStore->root->key;
        double deltaNewDistance = abs(record.distanceFromReference - this->averageDistance);
        double deltaRootDistance = abs(currentRootDistance - this->averageDistance);
        if (deltaNewDistance < deltaRootDistance) {
            std::vector<VectorRecord> allRecords;
            getAllRecords(this->vectorStore->root, allRecords);
            allRecords.push_back(record);
            Sorter<VectorRecord> sorter(allRecords.data(), allRecords.size());
            sorter.sort();
            this->vectorStore->clear();
            this->vectorStore->root = rebuildAVL(allRecords, 0, allRecords.size() - 1);
        }
        else {
            this->vectorStore->insert(record.distanceFromReference, record);
        }
    }
    if (this->vectorStore->root != nullptr) this->rootVector = &(this->vectorStore->root->data);
    this->normIndex->insert(calculateNorm(*preprocessedVector), record);
    return;
}

VectorRecord* VectorStore::getRecord(AVLTree<double, VectorRecord>::AVLNode* node, int& currentIndex, int targetIndex) {
    if (node == nullptr) return nullptr;
    VectorRecord* leftCase = getRecord(node->pLeft, currentIndex, targetIndex);
    if (leftCase != nullptr) {
        return leftCase;
    }
    if (currentIndex == targetIndex) {
        return &(node->data);
    }
    currentIndex++; // if fails
    return getRecord(node->pRight, currentIndex, targetIndex);
}

VectorRecord* VectorStore::getVector(int index) {
    if (index < 0 || index >= this->count) throw std::out_of_range("Index is invalid!");
    int currentIndex = 0;
    return getRecord(this->vectorStore->root, currentIndex, index);
}

string VectorStore::getRawText(int index) {
    VectorRecord* record = getVector(index);
    return record->rawText;
}

int VectorStore::getId(int index) {
    VectorRecord* record = getVector(index);
    return record->id;
}

VectorRecord* VectorStore::findNewRoot(AVLTree<double, VectorRecord>::AVLNode* node) {
    if (node == nullptr) return nullptr;
    VectorRecord* newRoot = &(node->data);
    double minDelta = abs(node->data.distanceFromReference - this->averageDistance);
    VectorRecord* leftCase = findNewRoot(node->pLeft);
    if (leftCase != nullptr) {
        double leftDelta = abs(leftCase->distanceFromReference - this->averageDistance);
        if (leftDelta < minDelta) {
            minDelta = leftDelta;
            newRoot = leftCase;
        }
    }
    VectorRecord* rightCase = findNewRoot(node->pRight);
    if (rightCase != nullptr) {
        double rightDelta = abs(rightCase->distanceFromReference - this->averageDistance);
        if (rightDelta < minDelta) {
            minDelta = rightDelta;
            newRoot = rightCase;
        }
    }
    return newRoot;
}

bool VectorStore::removeAt(int index) {
    if (index < 0 || index >= this->count) throw std::out_of_range("Index is invalid!");
    VectorRecord* unlucky = getVector(index);
    double distanceFromReference = unlucky->distanceFromReference;
    double norm = calculateNorm(*unlucky->vector);
    std::vector<float>* unluckyVector = unlucky->vector;
    bool oldRootRemoved = false;
    if (this->vectorStore->root != nullptr) {
        if (unlucky->vector == this->vectorStore->root->data.vector) return true;
    }
    this->vectorStore->remove(distanceFromReference);
    this->normIndex->remove(norm);
    if (this->count > 1) {
        double oldTotalDistance = this->averageDistance * this->count;
        this->averageDistance = (oldTotalDistance - distanceFromReference) / (this->count - 1);
    }
    else {
        this->averageDistance = 0;
    }
    this->count--;
    if (unluckyVector != nullptr) delete unluckyVector;
    if (this->count > 0 && oldRootRemoved) this->rootVector = findNewRoot(this->vectorStore->root);
    else if (this->count == 0) {
        this->averageDistance = 0;
        this->rootVector = nullptr;
    }
    return true;
} 

void VectorStore::recalculateAverageDistance() {
    std::vector<VectorRecord> records;
    getAllRecords(this->vectorStore->root, records);
    double totalDistance = 0;
    for (VectorRecord& record : records) {
        record.distanceFromReference = l2Distance(*record.vector, *this->referenceVector);
        totalDistance += record.distanceFromReference;
    }
    this->averageDistance = totalDistance / records.size();
}

void VectorStore::setReferenceVector(const std::vector<float>& newReference) {
    if (this->referenceVector) delete this->referenceVector;
    this->referenceVector = new std::vector<float>(newReference);
    if (this->referenceVector->empty()) return;
    
    recalculateAverageDistance();

    std::vector<VectorRecord> records;
    getAllRecords(this->vectorStore->root, records);
    Sorter<VectorRecord> sorter(records.data(), records.size());
    sorter.sort();

    int rootIndex = 0;
    double minDelta = abs(records[0].distanceFromReference - this->averageDistance);
    for (int i = 1; i < records.size(); i++) {
        double delta = abs(records[i].distanceFromReference - this->averageDistance);
        if (delta < minDelta) {
            rootIndex = i;
            minDelta = delta;
        }
    }
    this->vectorStore->clear();
    VectorRecord newRootRecord = records[rootIndex];
    AVLTree<double, VectorRecord>::AVLNode* newRoot = new AVLTree<double, VectorRecord>::AVLNode(newRootRecord.distanceFromReference, newRootRecord);
    newRoot->balance = EH;
    newRoot->pLeft = rebuildAVL(records, 0, rootIndex - 1);
    newRoot->pRight = rebuildAVL(records, rootIndex + 1, records.size() - 1);
    this->vectorStore->root = newRoot;
    if (this->vectorStore->root != nullptr) this->rootVector = &(this->vectorStore->root->data);
}

std::vector<float>* VectorStore::getReferenceVector() const {
    return this->referenceVector;
}

VectorRecord* VectorStore::getRootVector() const {
    return this->rootVector;
}

double VectorStore::getAverageDistance() const {
    return this->averageDistance;
}

void VectorStore::setEmbeddingFunction(std::vector<float>* (*newEmbeddingFunction)(const std::string&)) {
    this->embeddingFunction = newEmbeddingFunction;
}

void VectorStore::forEachHelper(AVLTree<double, VectorRecord>::AVLNode* node, void (*action)(vector<float>&, int, string&)) {
    if (node == nullptr) return;
    forEachHelper(node->pLeft, action);
    action(*(node->data.vector), node->data.id, node->data.rawText);
    forEachHelper(node->pRight, action);
}

void VectorStore::forEach(void (*action)(vector<float>&, int, string&)) {
    forEachHelper(this->vectorStore->root, action);
}

std::vector<int> VectorStore::getAllIdsSortedByDistance() const {
    std::vector<int> idList;
    std::vector<VectorRecord> records;
    VectorStore* constantVectorStore = const_cast<VectorStore*>(this);
    constantVectorStore->getAllRecords(constantVectorStore->vectorStore->root, records);
    for (const VectorRecord& it : records) {
        idList.push_back(it.id);
    }
    return idList;
}

std::vector<VectorRecord*> VectorStore::getAllVectorsSortedByDistance() const {
    std::vector<VectorRecord*> records;
    VectorStore* constantVectorStore = const_cast<VectorStore*>(this);
    constantVectorStore->getAllRecordPointers(constantVectorStore->vectorStore->root, records);
    return records;
}

double VectorStore::cosineSimilarity(const std::vector<float>& v1, const std::vector<float>& v2) {
    double product = 0;
    double v1Length = 0;
    double v2Length = 0;
    for (int i = 0; i < v1.size(); i++) {
        product += v1[i] * v2[i];
        v1Length += v1[i] * v1[i];
        v2Length += v2[i] * v2[i];
    }
    if (v1Length == 0 || v2Length == 0) return 0;
    return product/(sqrt(v1Length) + sqrt(v2Length));
}

double VectorStore::l1Distance(const std::vector<float>& v1, const std::vector<float>& v2) {
    double sum = 0;
    for (int i = 0; i < v1.size(); i++) {
        sum += abs(v1[i] - v2[i]);
    }
    return sum;
}

double VectorStore::l2Distance(const std::vector<float>& v1, const std::vector<float>& v2) {
    double sum = 0;
    for (int i = 0; i < v1.size(); i++) {
        sum += (v1[i] - v2[i]) * (v1[i] - v2[i]);
    }
    return sqrt(sum);
}

double VectorStore::estimateD_Linear(const std::vector<float>& query, 
                        int k, double averageDistance, 
                        const std::vector<float>& reference, 
                        double c0_bias, 
                        double c1_slope) {
    double d_r = l2Distance(query, reference);
    double D = abs(d_r - averageDistance) + c1_slope * averageDistance * k + c0_bias;
    if (D > 0) return D;
    else return 0;
}

double VectorStore::calculateMetric(const std::vector<float>& v1, const std::vector<float>& v2, std::string metric) {
    if (metric == "cosine") return 1 - cosineSimilarity(v1, v2);
    else if (metric == "euclidean") return l2Distance(v1, v2);
    else if (metric == "manhattan") return l1Distance(v1, v2);
    else throw invalid_metric();
}

void VectorStore::findNearestHelper(AVLTree<double, VectorRecord>::AVLNode* node, 
                       const std::vector<float>& query, 
                       std::string metric, 
                       double& minDistance, 
                       int& bestId) {
    if (node == nullptr) return;
    double distance = this->calculateMetric(query, *(node->data).vector, metric);
    if (distance < minDistance) {
        minDistance = distance;
        bestId = node->data.id;
    }
    findNearestHelper(node->pLeft, query, metric, minDistance, bestId);
    findNearestHelper(node->pRight, query, metric, minDistance, bestId);
}

int VectorStore::findNearest(const vector<float>& query, string metric) {
    if (metric != "cosine" && metric != "euclidean" && metric != "manhattan") throw invalid_metric();
    if (this->vectorStore->empty()) return -1;
    double minDistance = 1.79769e+308;
    int bestID = -1;
    findNearestHelper(this->vectorStore->root, query, metric, minDistance, bestID);
    return bestID;
}

void VectorStore::RBTSearchWithRange(RedBlackTree<double, VectorRecord>::RBTNode* node, double minK, double maxK, std::vector<VectorRecord*>& candidates) {
    if (node == nullptr) return;
    double nodeKey = node->key;
    if (nodeKey >= minK) RBTSearchWithRange(node->left, minK, maxK, candidates);
    if (nodeKey >= minK && nodeKey <= maxK) candidates.push_back(&(node->data));
    if (nodeKey <= maxK) RBTSearchWithRange(node->right, minK, maxK, candidates);
    return;
}

// USING STRUCT CANDIDATE for Id and uhh distance storing
int* VectorStore::topKNearest(const std::vector<float>& query, int k, std::string metric) {
    if (metric != "cosine" && metric != "l1" && metric != "euclidean") throw std::runtime_error("invalid_metric");
    if (k <= 0 || k > this->count) throw invalid_k_value();
    double n_q = 0.0;
    for (float x : query) n_q += x * x;
    n_q = sqrt(n_q);
    double D = estimateD_Linear(query, k, this->averageDistance, *this->referenceVector);
    vector<VectorRecord*> candidates;
    double lower = n_q - D;
    double upper = n_q + D;
    RBTSearchWithRange(this->normIndex->root, lower, upper, candidates);
    cout << "Value m: " << candidates.size() << endl;
    vector<Candidate> rankedCandidates;
    rankedCandidates.reserve(candidates.size());
    for (VectorRecord* rec : candidates) {
        Candidate candidate;
        candidate.distance = calculateMetric(query, *(rec->vector), metric);
        candidate.id = rec->id;
        rankedCandidates.push_back(candidate);
    }
    Sorter<Candidate> sorter(rankedCandidates.data(), rankedCandidates.size());
    sorter.sort();
    int resultSize = (rankedCandidates.size() < k) ? rankedCandidates.size() : k;
    int* result = new int[resultSize];
    for (int i = 0; i < resultSize; ++i) {
        result[i] = rankedCandidates[i].id;
    }
    return result;
}

void VectorStore::rangeQueryFromRootHelper(AVLTree<double, VectorRecord>::AVLNode* node, 
                          double minDistance, 
                          double maxDistance, 
                          std::vector<int>& list) const {
    if (node == nullptr) return;
    int nodeKey = node->key;
    if (nodeKey >= minDistance) rangeQueryFromRootHelper(node->pLeft, minDistance, maxDistance, list);
    if (nodeKey >= minDistance && nodeKey <= maxDistance) list.push_back(node->data.id);
    if (nodeKey <= maxDistance) rangeQueryFromRootHelper(node->pRight, minDistance, maxDistance, list); 
    return;
}

int* VectorStore::rangeQueryFromRoot(double minDist, double maxDist) const {
    vector<int> idList;
    VectorStore* constantVectorStore = const_cast<VectorStore*>(this);
    rangeQueryFromRootHelper(constantVectorStore->vectorStore->root, minDist, maxDist, idList);
    if (idList.empty()) return nullptr;
    int* list = new int[idList.size()];
    for (int i = 0; i < idList.size(); i++) {
        list[i] = idList[i];
    }
    return list;
}

double VectorStore::cosineSimilarityConst(const std::vector<float>& v1, const std::vector<float>& v2) const {
    double product = 0;
    double v1Length = 0;
    double v2Length = 0;
    for (int i = 0; i < v1.size(); i++) {
        product += v1[i] * v2[i];
        v1Length += v1[i] * v1[i];
        v2Length += v2[i] * v2[i];
    }
    if (v1Length == 0 || v2Length == 0) return 0;
    return product/(sqrt(v1Length) + sqrt(v2Length));
}

double VectorStore::l1DistanceConst(const std::vector<float>& v1, const std::vector<float>& v2) const {
    double sum = 0;
    for (int i = 0; i < v1.size(); i++) {
        sum += abs(v1[i] - v2[i]);
    }
    return sum;
}

double VectorStore::l2DistanceConst(const std::vector<float>& v1, const std::vector<float>& v2) const {
    double sum = 0;
    for (int i = 0; i < v1.size(); i++) {
        sum += (v1[i] - v2[i]) * (v1[i] - v2[i]);
    }
    return sqrt(sum);
}

double VectorStore::calculateMetricConst(const std::vector<float>& v1, const std::vector<float>& v2, std::string metric) const {
    if (metric == "cosine") return 1 - cosineSimilarityConst(v1, v2);
    else if (metric == "euclidean") return l2DistanceConst(v1, v2);
    else if (metric == "manhattan") return l1DistanceConst(v1, v2);
    else throw invalid_metric();
}

void VectorStore::rangeQueryHelper(AVLTree<double, VectorRecord>::AVLNode* node, 
                                    const std::vector<float>& query, 
                                    double radius, 
                                    std::string metric,  
                                    std::vector<int>& list) const {
    if (node == nullptr) return;
    rangeQueryHelper(node->pLeft, query, radius, metric, list);
    double distance = calculateMetricConst(query, *node->data.vector, metric);
    if (distance <= radius) list.push_back(node->data.id);
    rangeQueryHelper(node->pRight, query, radius, metric, list);
}

int* VectorStore::rangeQuery(const std::vector<float>& query, double radius, std::string metric) const {
    if (metric != "cosine" && metric != "euclidean" && metric != "manhattan") throw invalid_metric();
    vector<int> idList;
    rangeQueryHelper(this->vectorStore->root, query, radius, metric,idList);
    if (idList.empty()) return nullptr;
    int* list = new int[idList.size()];
    for (int i = 0; i < idList.size(); i++) {
        list[i] = idList[i];
    } 
    return list;
}

void VectorStore::boundingBoxHelper(AVLTree<double, VectorRecord>::AVLNode* node, 
                                    const std::vector<float>& minBound, 
                                    const std::vector<float>& maxBound, 
                                    std::vector<int>& result) const {
    if (node == nullptr) return;
    boundingBoxHelper(node->pLeft, minBound, maxBound, result);
    bool temp = true;
    const vector<float>& nodeVector = *node->data.vector;
    for (int i = 0; i < nodeVector.size(); i++) {
        if (nodeVector[i] < minBound[i] || nodeVector[i] > maxBound[i]) {
            temp = false;
            break;
        }
    }
    if (temp) result.push_back(node->data.id);
    boundingBoxHelper(node->pRight, minBound, maxBound, result);
    return;
}

int* VectorStore::boundingBoxQuery(const std::vector<float>& minBound, const std::vector<float>& maxBound) const {
    vector<int> idList;
    boundingBoxHelper(this->vectorStore->root, minBound, maxBound, idList);
    if (idList.empty()) return nullptr;
    int* list = new int[idList.size()];
    for (int i = 0; i < idList.size(); i++) {
        list[i] = idList[i];
    }
    return list;
}

double VectorStore::getMaxDistance() const {
    if (this->vectorStore->empty()) return -1;
    AVLTree<double, VectorRecord>::AVLNode* cursor = this->vectorStore->root;
    while (cursor->pRight != nullptr) {
        cursor = cursor->pRight;
    }
    return cursor->key;
}

double VectorStore::getMinDistance() const {
    if (this->vectorStore->empty()) return -1;
    AVLTree<double, VectorRecord>::AVLNode* cursor = this->vectorStore->root;
    while (cursor->pLeft != nullptr) {
        cursor = cursor->pLeft;
    }
    return cursor->key;
}

VectorRecord VectorStore::computeCentroid(const std::vector<VectorRecord*>& records) const {
    VectorRecord centroid;
    if (records.empty()) {
        centroid.id = -1;
        centroid.rawText = "centroid";
        centroid.vector = new vector<float>(this->dimension, 0.0);
        centroid.distanceFromReference = 0.0;
        return centroid; 
    }
    vector<float>* sumVector = new vector<float>(this->dimension, 0);
    for (VectorRecord* record : records) {
        const vector<float>& vector = *record->vector;
        for (int i = 0; i < this->dimension; i++) {
            (*sumVector)[i] += vector[i];
        }
    }
    for (int i = 0; i < this->dimension; ++i) {
        (*sumVector)[i] /= (float)records.size();
    }
    centroid.vector = sumVector;
    centroid.id = -1;
    centroid.rawText = "centroid";
    centroid.distanceFromReference = l2DistanceConst(*sumVector, *this->referenceVector);
    return centroid;
}

VectorRecord* VectorStore::findVectorNearestToDistance(double targetDistance) const {
    if (this->vectorStore->empty()) return nullptr;
    AVLTree<double, VectorRecord>::AVLNode* cursor = this->vectorStore->root;
    AVLTree<double, VectorRecord>::AVLNode* nearest = cursor;
    double minDelta = abs(cursor->key - targetDistance);
    while (cursor != nullptr) {
        double delta = abs(cursor->key - targetDistance);
        if (delta < minDelta) {
            minDelta = delta;
            nearest = cursor;
        }
        if (targetDistance < cursor->key) {
            cursor = cursor->pLeft;
        }
        else if (targetDistance > cursor->key) {
            cursor = cursor->pRight;
        }
        else {
            return &(cursor->data);
        }
    }
    return &(nearest->data);
}

// =====================================
// Sorter implementation
// =====================================
template <class T>
Sorter<T>::Sorter(T* array, int size) {
    this->array = array;
    this->size = size;
}

template <class T>
void Sorter<T>::merge_sort(int left_index, int right_index, T* culey_handsome) {
    // 
    if (left_index >= right_index) return;
    int mid_index = left_index + (right_index - left_index) / 2;
    
    merge_sort(left_index, mid_index, culey_handsome);
    merge_sort(mid_index + 1, right_index, culey_handsome);

    int i = left_index;
    int j = mid_index + 1;
    int k = left_index;

    while (i <= mid_index && j <= right_index) {
        if (!(array[j] < array[i])) {
            culey_handsome[k++] = array[i++];
        } else {
            culey_handsome[k++] = array[j++];
        }
    }
    while (i <= mid_index)   culey_handsome[k++] = array[i++];
    while (j <= right_index) culey_handsome[k++] = array[j++];
    for (int t = left_index; t <= right_index; ++t) {
        array[t] = culey_handsome[t];
    }
}

template <class T>
void Sorter<T>::sort() {
    if (!array || size <= 1) return;
    T* culey_handsome = new T[size];
    merge_sort(0, size - 1, culey_handsome);
    delete[] culey_handsome;
}

// Explicit template instantiation for the type used by VectorStore
template class AVLTree<double, VectorRecord>;
template class AVLTree<double, double>;
template class AVLTree<int, double>;
template class AVLTree<int, int>;
template class AVLTree<double, string>;
template class AVLTree<int, string>;

template class RedBlackTree<double, VectorRecord>;
template class RedBlackTree<double, double>;
template class RedBlackTree<int, double>;
template class RedBlackTree<int, int>;
template class RedBlackTree<double, string>;
template class RedBlackTree<int, string>;