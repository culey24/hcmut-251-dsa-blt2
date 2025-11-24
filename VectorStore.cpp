// NOTE: Per assignment rules, only this single include is allowed here.
#include "VectorStore.h"

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

// =====================================
// VectorStore implementation
// =====================================

// TODO: Implement all VectorStore methods here

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