#ifndef VECTORSTORE_H
#define VECTORSTORE_H

#include <vector>
#pragma once
// NOTE: Per assignment rules, only this single include is allowed here.
#include "main.h"



// ------------------------------
// AVL balance enum
// ------------------------------
enum BalanceValue {
    LH = -1, // Left Higher
    EH = 0,  // Equal Height
    RH = 1   // Right Higher
};

// ------------------------------
// Generic AVL Tree (template)
// ------------------------------
template <class K, class T>
class AVLTree {
    friend class VectorStore; // Allow VectorStore to access protected/private members
    
    public:
        class AVLNode {
        public:
            K key;
            T data;
            AVLNode* pLeft;
            AVLNode* pRight;
            BalanceValue balance;
            
            AVLNode(const K& key, const T& value)
                : key(key), data(value), pLeft(nullptr), pRight(nullptr), balance(EH) {}
                
            friend class VectorStore; // Allow VectorStore to access AVLNode members
        };

    protected:
        AVLNode* root;

        AVLNode* rotateRight(AVLNode*& node);
        AVLNode* rotateLeft(AVLNode*& node);

        // MANUALLY DEFINED FUNCTIONS 
        AVLNode* balanceLeftHeavyCase(AVLNode*& node, bool& broIsTaller);
        AVLNode* balanceRightHeavyCase(AVLNode*& node, bool& broIsTaller);
        AVLNode* removeBalanceLeft(AVLNode*& node, bool& broIsShorter);
        AVLNode* removeBalanceRight(AVLNode*& node, bool& broIsShorter);
        AVLNode* insertHelper(AVLNode*& node, const K& key, const T& value, bool& broIsTaller); 
        AVLNode* removeMinNode(AVLNode*& node, AVLNode*& minNode, bool& broIsShorter);
        AVLNode* removeHelper(AVLNode*& node, const K& key, bool& broIsShorter);
        bool containsHelper(AVLNode* node, const K& key) const;
        int getHeightHelper(AVLNode* node) const;
        int getSizeHelper(AVLNode* node) const;
        void clearHelper(AVLNode*& node);
        void inOrderTraversalHelper(AVLNode* node, void (*action)(const T&)) const;

    public:
        AVLTree();
        ~AVLTree();

        void insert(const K& key, const T& value);
        void remove(const K& key);
        bool contains(const K& key) const;

        int getHeight() const;
        int getSize() const;
        bool empty() const;
        void clear();

        void printTreeStructure() const;

        void inorderTraversal(void (*action)(const T&)) const;

        AVLNode* getRoot() const { return root; }

};

enum Color { RED, BLACK };

// RedBlackTree class
template <class K, class T>
class RedBlackTree {
    friend class VectorStore; // Allow VectorStore to access protected/private members
    
public:
// RBTNode class

class RBTNode {
    public:
        K key;
        T data;
        Color color;
        RBTNode* parent;
        RBTNode* left;
        RBTNode* right;

        // Constructor
        RBTNode(const K& key, const T& value);
        
        void recolorToRed();
        void recolorToBlack();
        
        friend class VectorStore; // Allow VectorStore to access RBTNode members
    };

private:
    RBTNode* root;

protected:
    void rotateLeft(RBTNode* node);
    void rotateRight(RBTNode* node);

    RBTNode* lowerBoundNode(const K& key) const;
    RBTNode* upperBoundNode(const K& key) const;

    /*
    MANUALLY DEFINED FUNCTIONS 
    */
    int getSizeHelper(RBTNode* node) const;
    void clearHelper(RBTNode* node);
    
    //insert hlpers
    void handleLCase(RBTNode*& node);
    void handleRCase(RBTNode*& node);
    void insertRebalance(RBTNode*& node);

    //remove helpers
    RBTNode* findMaxNode(RBTNode* node);
    void changeParent(RBTNode* u, RBTNode* v);
    void removeRebalanceLCase(RBTNode*& node, RBTNode*& parent);
    void removeRebalanceRCase(RBTNode*& node, RBTNode*& parent);
    void removeRebalance(RBTNode*& node, RBTNode*& parent);

    RBTNode* lowerBoundHelper(const K &key) const;
    RBTNode* upperBoundHelper(const K &key) const;

public:
    RedBlackTree();
    ~RedBlackTree();
    
    bool empty() const;
    int size() const;
    void clear();
    void insert(const K& key, const T& value);
    void remove(const K& key);
    RBTNode* find(const K& key) const;
    bool contains(const K& key) const;

    RBTNode* lowerBound(const K& key, bool& found) const;
    RBTNode* upperBound(const K& key, bool& found) const;

    void printTreeStructure() const;
};


// ------------------------------
// VectorRecord
// ------------------------------
class VectorRecord {
    public:
        int id;                             
        std::string rawText;                
        int rawLength;                      
        std::vector<float>* vector;         
        double distanceFromReference;       

        VectorRecord()
            : id(-1), rawLength(0), vector(nullptr), distanceFromReference(0.0) {}

        VectorRecord(int _id,
                    const std::string& _rawText,
                    std::vector<float>* _vec,
                    double _dist)
            : id(_id),
            rawText(_rawText),
            rawLength(static_cast<int>(_rawText.size())),
            vector(_vec),
            distanceFromReference(_dist) {}

        // Overload operator << to print only the id
        friend std::ostream& operator<<(std::ostream& os, const VectorRecord& record);
};

// ------------------------------
// VectorStore
// ------------------------------
class VectorStore {
    private:
        AVLTree<double, VectorRecord>* vectorStore;
        RedBlackTree<double, VectorRecord>* normIndex;

        std::vector<float>* referenceVector;
        VectorRecord* rootVector;

        int dimension;
        int count;
        double averageDistance;

        std::vector<float>* (*embeddingFunction)(const std::string&);

        double distanceByMetric(const std::vector<float>& a,
                                const std::vector<float>& b,
                                const std::string& metric) const;

        void rebuildRootIfNeeded();
        void rebuildTreeWithNewRoot(VectorRecord* newRoot);

        VectorRecord* findVectorNearestToDistance(double targetDistance) const; 

        // MANUALLY ADDED VARIABLES
        int currentMaxID;

        // MANUALLY DEFINED FUNCTIONS 
        int findMaxID(AVLTree<double, VectorRecord>::AVLNode* node);
        double calculateNorm(const std::vector<float>& vector);
        void getAllRecords(AVLTree<double, VectorRecord>::AVLNode* node, std::vector<VectorRecord>& list);
        void getAllRecordPointers(AVLTree<double, VectorRecord>::AVLNode* node, std::vector<VectorRecord*>& list);
        AVLTree<double, VectorRecord>::AVLNode* rebuildAVL(const std::vector<VectorRecord>& sortedList, int start, int end);
        VectorRecord* getRecord(AVLTree<double, VectorRecord>::AVLNode* node, int& currentIndex, int targetIndex);
        VectorRecord* findNewRoot(AVLTree<double, VectorRecord>::AVLNode* node);
        void forEachHelper(AVLTree<double, VectorRecord>::AVLNode* node, void (*action)(std::vector<float>&, int, std::string&));
        void recalculateAverageDistance();
        double calculateMetric(const std::vector<float>& v1, const std::vector<float>& v2, std::string metric);
        double calculateMetricConst(const std::vector<float>& v1, const std::vector<float>& v2, std::string metric) const;
        void findNearestHelper(AVLTree<double, VectorRecord>::AVLNode* node, const std::vector<float>& query, std::string metric, double& minDistance, int& bestId);
        void rangeQueryFromRootHelper(AVLTree<double, VectorRecord>::AVLNode* node, double minDistance, double maxDistance, std::vector<int>& list) const;
        double cosineSimilarityConst(const std::vector<float>& v1, const std::vector<float>& v2) const;
        double l1DistanceConst(const std::vector<float>& v1, const std::vector<float>& v2) const;
        double l2DistanceConst(const std::vector<float>& v1, const std::vector<float>& v2) const;
        void rangeQueryHelper(AVLTree<double, VectorRecord>::AVLNode* node, const std::vector<float>& query, double radius, std::string metric, std::vector<int>& list) const;
        void boundingBoxHelper(AVLTree<double, VectorRecord>::AVLNode* node, const std::vector<float>& minBound, const std::vector<float>& maxBound, std::vector<int>& result) const;
        void RBTSearchWithRange(RedBlackTree<double, VectorRecord>::RBTNode* node, double minK, double maxK, std::vector<VectorRecord*>& candidates);

    public:
        VectorStore(int dimension,
                    std::vector<float>* (*embeddingFunction)(const std::string&),
                    const std::vector<float>& referenceVector);
        ~VectorStore();

        int size();
        bool empty();
        void clear();

        std::vector<float>* preprocessing(std::string rawText);
        void addText(std::string rawText);

        VectorRecord* getVector(int index);        
        std::string   getRawText(int index);
        int           getId(int index);

        bool removeAt(int index);

        void setReferenceVector(const std::vector<float>& newReference);
        std::vector<float>* getReferenceVector() const; 
        VectorRecord* getRootVector() const; 
        double getAverageDistance() const;           
        void setEmbeddingFunction(std::vector<float>* (*newEmbeddingFunction)(const std::string&));

        void forEach(void (*action)(std::vector<float>&, int, std::string&));
        std::vector<int> getAllIdsSortedByDistance() const;
        std::vector<VectorRecord*> getAllVectorsSortedByDistance() const;

        double cosineSimilarity(const std::vector<float>& v1, const std::vector<float>& v2);
        double l1Distance(const std::vector<float>& v1, const std::vector<float>& v2);
        double l2Distance(const std::vector<float>& v1, const std::vector<float>& v2);

        double estimateD_Linear(const std::vector<float>& query, int k, double averageDistance, const std::vector<float>& reference, double c0_bias = 1e-9, double c1_slope = 0.05);

        int findNearest(const std::vector<float>& query, std::string metric = "cosine");
        int* topKNearest(const std::vector<float>& query, int k, std::string metric = "cosine");

        int* rangeQueryFromRoot(double minDist, double maxDist) const;
        int* rangeQuery(const std::vector<float>& query, double radius, std::string metric = "cosine") const;
        int* boundingBoxQuery(const std::vector<float>& minBound, const std::vector<float>& maxBound) const;

        double getMaxDistance() const;
        double getMinDistance() const;
        VectorRecord computeCentroid(const std::vector<VectorRecord*>& records) const;
};

template <class T>
class Sorter {
private:
    T* array;
    int size;
    void merge_sort(int left_index, int right_index, T* culey_handsome);
public:
    Sorter(T* array, int size);
    ~Sorter() = default;
    void sort();
};

struct Candidate {
    double distance;
    int id;
    bool operator<(const Candidate& other) const {
        if (std::abs(distance - other.distance) < 1e-9) {
            return id < other.id; 
        }
        return distance < other.distance;
    }
};

#endif // VECTORSTORE_H
