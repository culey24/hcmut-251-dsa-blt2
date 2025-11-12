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
                node->balance = LH;
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
        node->pLeft = insertHelper(node->pRight, key, value, broIsTaller);
        if (broIsTaller) {
            if (node->balance == LH) {
                node->balance = EH;
                broIsTaller = false;
            }
            else if (node->balance == EH) node->balance = RH;
            else node->balance = balanceRightHeavyCase(node, broIsTaller);
        }
    }
    else {
        broIsTaller = false;
    }
    return node;
}

template <class K, class T>
void AVLTree<K, T>::insert(const K& key, const T& value) {
    bool broIsTaller = false;
    this->root = insertHelper(this->root, key, value, broIsTaller);
}

template <class K, class T>
typename AVLTree<K, T>::AVLNode* AVLTree<K, T>::removeHelper(AVLNode* node, const K& key, bool& broIsShorter) {
    if (node == nullptr) {
        broIsShorter = false;
        return nullptr;
    }

    if (key < node->key) {
        
    }
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
