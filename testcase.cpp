#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <functional>
#include <cmath>
#include <algorithm>
#include <sstream> // Include trước để tránh lỗi redeclared access

// --- THỦ THUẬT: MỞ QUYỀN TRUY CẬP ĐỂ TEST ---
// Chỉ định nghĩa SAU KHI đã include hết thư viện chuẩn
#define protected public
#define private public

#include "VectorStore.cpp"

// Tắt định nghĩa ngay lập tức
#undef protected
#undef private

using namespace std;

// =================================================================
// CÁC HÀM HỖ TRỢ (HELPER)
// =================================================================

// Biến toàn cục để hứng dữ liệu test Inorder
vector<int> global_inorder_buffer;
void captureInt(const int& value) {
    global_inorder_buffer.push_back(value);
}

// In tiêu đề
void printHeader(string title) {
    cout << "\n--------------------------------------------------" << endl;
    cout << "[RUNNING] " << title << endl;
    cout << "--------------------------------------------------" << endl;
}

// --- HELPER CHO RBT (TYPE-AGNOSTIC) ---
// Tự động nhận diện kiểu Node để tránh lỗi "undeclared identifier"

template <typename NodePtr>
int getBlackHeightRecursive(NodePtr node) {
    if (node == nullptr) return 1; 
    
    int leftBH = getBlackHeightRecursive(node->left);
    if (leftBH == -1) return -1;
    
    int rightBH = getBlackHeightRecursive(node->right);
    if (rightBH == -1) return -1;
    
    if (leftBH != rightBH) return -1; 
    
    return leftBH + (node->color == BLACK ? 1 : 0);
}

template <class K, class T>
bool validateRBProperties(RedBlackTree<K, T>& tree) {
    // Tự động suy luận kiểu con trỏ từ cây
    using NodePtr = decltype(tree.root); 
    
    NodePtr root = tree.root;

    // Rule 1: Root phải là BLACK
    if (root != nullptr && root->color == RED) {
        cout << "FAILED: Root is RED!" << endl;
        return false;
    }

    // Rule 2: Red node không được có con Red (No Double Red)
    auto checkRedRed = [](auto&& self, NodePtr node) -> bool {
        if (!node) return true;
        if (node->color == RED) {
            if ((node->left && node->left->color == RED) || 
                (node->right && node->right->color == RED)) {
                cout << "FAILED: Red node has Red child!" << endl;
                return false;
            }
        }
        return self(self, node->left) && self(self, node->right);
    };

    if (!checkRedRed(checkRedRed, root)) return false;

    // Rule 3: Black Height phải đồng nhất
    if (getBlackHeightRecursive(root) == -1) {
        cout << "FAILED: Black Height violation!" << endl;
        return false;
    }

    return true;
}

// =================================================================
// AVL TEST CASES (01-20)
// =================================================================

void tc_01_AVL_BasicInsert() {
    AVLTree<int, int> tree;
    tree.insert(10, 10);
    tree.insert(20, 20);
    tree.insert(5, 5);
    
    assert(tree.getSize() == 3);
    assert(!tree.empty());
    cout << "-> Check Size & Empty: OK" << endl;
}

void tc_02_AVL_Rotation_RR() {
    AVLTree<int, int> tree;
    tree.insert(10, 10);
    tree.insert(20, 20);
    tree.insert(30, 30);
    
    assert(tree.getHeight() <= 2); 
    if (tree.root) assert(tree.root->data == 20);
    cout << "-> RR Rotation: OK" << endl;
}

void tc_03_AVL_Rotation_LL() {
    AVLTree<int, int> tree;
    tree.insert(30, 30);
    tree.insert(20, 20);
    tree.insert(10, 10);

    assert(tree.getHeight() <= 2);
    if (tree.root) assert(tree.root->data == 20);
    cout << "-> LL Rotation: OK" << endl;
}

void tc_04_AVL_Rotation_LR() {
    AVLTree<int, int> tree;
    tree.insert(30, 30);
    tree.insert(10, 10);
    tree.insert(20, 20); 
    if (tree.root) assert(tree.root->data == 20);
    cout << "-> LR Rotation: OK" << endl;
}

void tc_05_AVL_Rotation_RL() {
    AVLTree<int, int> tree;
    tree.insert(10, 10);
    tree.insert(30, 30);
    tree.insert(20, 20); 
    if (tree.root) assert(tree.root->data == 20);
    cout << "-> RL Rotation: OK" << endl;
}

void tc_06_AVL_Contains() {
    AVLTree<int, int> tree;
    tree.insert(15, 15);
    tree.insert(10, 10);
    tree.insert(20, 20);

    assert(tree.contains(10) == true);
    assert(tree.contains(99) == false);
    cout << "-> Contains Check: OK" << endl;
}

void tc_07_AVL_RemoveLeaf() {
    AVLTree<int, int> tree;
    tree.insert(20, 20);
    tree.insert(10, 10);
    tree.insert(30, 30);
    
    tree.remove(10);
    
    assert(tree.getSize() == 2);
    assert(tree.contains(10) == false);
    cout << "-> Remove Leaf: OK" << endl;
}

void tc_08_AVL_RemoveOneChild() {
    AVLTree<int, int> tree;
    tree.insert(20, 20);
    tree.insert(10, 10);
    tree.insert(30, 30);
    tree.insert(5, 5); 
    
    tree.remove(10); 
    
    assert(tree.contains(10) == false);
    assert(tree.contains(5) == true);
    if (tree.root && tree.root->pLeft) assert(tree.root->pLeft->data == 5);
    cout << "-> Remove 1 Child Node: OK" << endl;
}

void tc_09_AVL_RemoveTwoChildren() {
    AVLTree<int, int> tree;
    tree.insert(50, 50);
    tree.insert(30, 30);
    tree.insert(70, 70);
    tree.insert(60, 60);
    tree.insert(80, 80);
    
    tree.remove(50);
    
    assert(tree.contains(50) == false);
    assert(tree.contains(60) == true); 
    if (tree.root) assert(tree.root->data == 60);
    cout << "-> Remove 2 Children: OK" << endl;
}

void tc_10_AVL_InorderTraversal() {
    AVLTree<int, int> tree;
    tree.insert(50, 50);
    tree.insert(20, 20);
    tree.insert(80, 80);
    tree.insert(10, 10);
    tree.insert(30, 30);

    global_inorder_buffer.clear();
    tree.inorderTraversal(captureInt);

    vector<int> expected = {10, 20, 30, 50, 80};
    assert(global_inorder_buffer == expected);
    cout << "-> Inorder Traversal: OK" << endl;
}

void tc_11_AVL_EmptyTree() {
    AVLTree<int, int> tree;
    tree.remove(100); 
    assert(tree.getSize() == 0);
    cout << "-> Empty Tree Ops: OK" << endl;
}

void tc_12_AVL_DuplicateInsert() {
    AVLTree<int, int> tree;
    tree.insert(10, 100);
    tree.insert(10, 200); 
    assert(tree.getSize() == 1);
    cout << "-> Duplicate Insert: OK" << endl;
}

void tc_13_AVL_RemoveNonExistent() {
    AVLTree<int, int> tree;
    tree.insert(10, 10);
    tree.remove(999);
    assert(tree.getSize() == 1);
    cout << "-> Remove Non-Existent: OK" << endl;
}

void tc_14_AVL_SingleNodeLifecycle() {
    AVLTree<int, int> tree;
    tree.insert(10, 10);
    tree.remove(10);
    assert(tree.empty() == true);
    cout << "-> Single Node Lifecycle: OK" << endl;
}

void tc_15_AVL_Stress_Ascending() {
    AVLTree<int, int> tree;
    for(int i=1; i<=10; i++) tree.insert(i, i);
    assert(tree.getHeight() <= 5); 
    cout << "-> Ascending Insert (Stress LL): OK" << endl;
}

void tc_16_AVL_Stress_Descending() {
    AVLTree<int, int> tree;
    for(int i=10; i>=1; i--) tree.insert(i, i);
    assert(tree.getHeight() <= 5);
    cout << "-> Descending Insert (Stress RR): OK" << endl;
}

void tc_17_AVL_RemoveRootCollapse() {
    AVLTree<int, int> tree;
    tree.insert(10, 10);
    tree.insert(20, 20);
    tree.remove(10);
    assert(tree.getSize() == 1);
    if (tree.root) assert(tree.root->data == 20);
    cout << "-> Remove Root Collapse: OK" << endl;
}

void tc_18_AVL_ClearAndReuse() {
    AVLTree<int, int> tree;
    tree.insert(10, 10);
    tree.clear();
    tree.insert(5, 5);
    assert(tree.getSize() == 1);
    cout << "-> Clear & Reuse: OK" << endl;
}

void tc_19_AVL_ComplexRebalancing() {
    AVLTree<int, int> tree;
    tree.insert(50, 50);
    tree.insert(30, 30);
    tree.insert(70, 70);
    tree.insert(20, 20);
    tree.insert(40, 40);
    tree.insert(60, 60);
    tree.insert(80, 80);
    tree.insert(10, 10);
    
    tree.remove(80);
    tree.remove(70);
    tree.remove(60); 
    
    assert(tree.getHeight() <= 4);
    cout << "-> Complex Rebalancing: OK" << endl;
}

void tc_20_AVL_TemplateString() {
    AVLTree<string, int> tree;
    tree.insert("apple", 1);
    tree.insert("banana", 2);
    tree.insert("cherry", 3);
    assert(tree.getSize() == 3);
    tree.remove("banana");
    assert(tree.contains("banana") == false);
    cout << "-> Template String Key: OK" << endl;
}

// =================================================================
// RBT TEST CASES (21-35)
// =================================================================

void tc_21_RBT_BasicInsert() {
    RedBlackTree<int, int> rbt;
    rbt.insert(10, 10);
    assert(rbt.size() == 1);
    if(rbt.root) assert(rbt.root->color == BLACK);
    cout << "-> RBT Basic Insert: OK" << endl;
}

void tc_22_RBT_Recolor() {
    RedBlackTree<int, int> rbt;
    rbt.insert(10, 10);
    rbt.insert(20, 20);
    rbt.insert(5, 5);   
    rbt.insert(4, 4);   
    
    assert(validateRBProperties(rbt));
    cout << "-> RBT Recolor Logic: OK" << endl;
}

void tc_23_RBT_Rotation_Line() {
    RedBlackTree<int, int> rbt;
    rbt.insert(30, 30);
    rbt.insert(20, 20);
    rbt.insert(10, 10); 
    
    if(rbt.root) {
        assert(rbt.root->key == 20);
        assert(rbt.root->color == BLACK);
    }
    assert(validateRBProperties(rbt));
    cout << "-> RBT Rotation Line: OK" << endl;
}

void tc_24_RBT_Rotation_Triangle() {
    RedBlackTree<int, int> rbt;
    rbt.insert(30, 30);
    rbt.insert(10, 10);
    rbt.insert(20, 20); 
    
    if(rbt.root) assert(rbt.root->key == 20);
    assert(validateRBProperties(rbt));
    cout << "-> RBT Rotation Triangle: OK" << endl;
}

void tc_25_RBT_Find_Contains() {
    RedBlackTree<int, int> rbt;
    rbt.insert(50, 50);
    rbt.insert(20, 20);
    
    assert(rbt.contains(20) == true);
    assert(rbt.contains(99) == false);
    
    auto node = rbt.find(50);
    assert(node != nullptr && node->key == 50);
    cout << "-> RBT Find/Contains: OK" << endl;
}

void tc_26_RBT_LowerBound() {
    RedBlackTree<int, int> rbt;
    rbt.insert(10, 10);
    rbt.insert(30, 30);
    rbt.insert(50, 50);
    
    bool found = false;
    auto node = rbt.lowerBound(25, found); 
    assert(found == true);
    if(node) assert(node->key == 30);
    
    node = rbt.lowerBound(60, found); 
    assert(found == false);
    cout << "-> RBT LowerBound: OK" << endl;
}

void tc_27_RBT_UpperBound() {
    RedBlackTree<int, int> rbt;
    rbt.insert(10, 10);
    rbt.insert(30, 30);
    
    bool found = false;
    auto node = rbt.upperBound(10, found); 
    assert(found == true);
    if(node) assert(node->key == 30);
    cout << "-> RBT UpperBound: OK" << endl;
}

void tc_28_RBT_Size_Clear() {
    RedBlackTree<int, int> rbt;
    rbt.insert(10, 10);
    rbt.clear(); 
    assert(rbt.size() == 0);
    assert(rbt.root == nullptr);
    cout << "-> RBT Size & Clear: OK" << endl;
}

void tc_29_RBT_Remove_RedLeaf() {
    RedBlackTree<int, int> rbt;
    rbt.insert(20, 20); 
    rbt.insert(10, 10); 
    rbt.insert(30, 30); 
    
    rbt.remove(10);
    assert(rbt.size() == 2);
    assert(validateRBProperties(rbt));
    cout << "-> RBT Remove Red Leaf: OK" << endl;
}

void tc_30_RBT_Template() {
    RedBlackTree<string, int> rbt;
    rbt.insert("apple", 1);
    rbt.insert("banana", 2);
    
    assert(rbt.size() == 2);
    bool found;
    auto node = rbt.lowerBound("b", found);
    assert(found && node->key == "banana");
    cout << "-> RBT Template String: OK" << endl;
}

void tc_31_RBT_Duplicate() {
    RedBlackTree<int, int> rbt;
    rbt.insert(10, 100);
    rbt.insert(10, 200); 
    assert(rbt.size() == 1);
    cout << "-> RBT Duplicate Insert: OK" << endl;
}

void tc_32_RBT_Remove_BlackNode() {
    RedBlackTree<int, int> rbt;
    rbt.insert(20, 20);
    rbt.insert(10, 10);
    rbt.insert(30, 30);
    rbt.insert(5, 5);
    
    rbt.remove(30);
    assert(validateRBProperties(rbt));
    cout << "-> RBT Remove Black Node: OK" << endl;
}

void tc_33_RBT_Remove_Root() {
    RedBlackTree<int, int> rbt;
    rbt.insert(10, 10);
    rbt.remove(10); 
    assert(rbt.size() == 0);
    assert(rbt.root == nullptr);
    cout << "-> RBT Remove Root: OK" << endl;
}

void tc_34_RBT_Stress_Ascending() {
    RedBlackTree<int, int> rbt;
    for(int i=1; i<=20; ++i) {
        rbt.insert(i, i);
    }
    assert(validateRBProperties(rbt));
    cout << "-> RBT Ascending Stress: OK" << endl;
}

void tc_35_RBT_Remove_NonExist() {
    RedBlackTree<int, int> rbt;
    rbt.insert(10, 10);
    rbt.remove(999); 
    assert(rbt.size() == 1);
    cout << "-> RBT Remove Non-Existent: OK" << endl;
}

// =================================================================
// NEW ADVANCED AVL CASES (36-40)
// =================================================================

void tc_36_AVL_Mix_Insert_Delete() {
    AVLTree<int, int> tree;
    // Chèn 10 phần tử
    for(int i=0; i<10; ++i) tree.insert(i, i);
    assert(tree.getSize() == 10);
    
    // Xóa các số chẵn: 0, 2, 4, 6, 8
    for(int i=0; i<10; i+=2) tree.remove(i);
    assert(tree.getSize() == 5);
    
    // Kiểm tra số lẻ còn lại
    for(int i=1; i<10; i+=2) assert(tree.contains(i));
    assert(tree.getHeight() <= 3); // log2(5) ~ 2.3
    cout << "-> AVL Mix Insert & Delete: OK" << endl;
}

void tc_37_AVL_Delete_All() {
    AVLTree<int, int> tree;
    int n = 20;
    for(int i=0; i<n; ++i) tree.insert(i, i);
    
    // Xóa từ gốc dần dần (hoặc xóa ngẫu nhiên)
    for(int i=0; i<n; ++i) {
        tree.remove(i);
        if(i < n-1) assert(!tree.empty());
    }
    assert(tree.empty());
    assert(tree.getSize() == 0);
    cout << "-> AVL Delete All: OK" << endl;
}

void tc_38_AVL_ZigZag_Stress() {
    // Tạo cây ZigZag để ép xoay kép liên tục khi xóa
    AVLTree<int, int> tree;
    // Chèn theo thứ tự gây mất cân bằng: 10, 5, 15, 2, 7, 12, 20...
    vector<int> nums = {10, 5, 15, 2, 7, 12, 20, 1, 3, 6, 8};
    for(int x : nums) tree.insert(x, x);
    
    // Xóa node trung gian để ép rebalance
    tree.remove(5); 
    tree.remove(15);
    
    assert(tree.contains(5) == false);
    assert(tree.contains(10) == true);
    assert(tree.getHeight() <= 4);
    cout << "-> AVL ZigZag Stress: OK" << endl;
}

void tc_39_AVL_Large_Scale() {
    AVLTree<int, int> tree;
    int scale = 100; // Test với 100 node
    for(int i=0; i<scale; ++i) tree.insert(i, i);
    assert(tree.getSize() == scale);
    
    // Xóa 50 node đầu
    for(int i=0; i<scale/2; ++i) tree.remove(i);
    assert(tree.getSize() == scale/2);
    assert(tree.getHeight() <= 7); // log2(50) ~ 5.6
    cout << "-> AVL Large Scale (100 nodes): OK" << endl;
}

void tc_40_AVL_Root_Change_Verify() {
    AVLTree<int, int> tree;
    tree.insert(10, 10);
    tree.insert(20, 20); // Root có thể thay đổi tùy logic xoay
    
    int oldRoot = tree.root->data;
    tree.remove(oldRoot); // Xóa root cũ
    
    assert(tree.getSize() == 1);
    assert(tree.root != nullptr);
    assert(tree.root->data != oldRoot); // Root phải đổi
    cout << "-> AVL Root Change Verify: OK" << endl;
}

// =================================================================
// NEW ADVANCED RBT CASES (41-45)
// =================================================================

void tc_41_RBT_Delete_Root_Cascade() {
    RedBlackTree<int, int> rbt;
    rbt.insert(10, 10);
    rbt.insert(5, 5);
    rbt.insert(15, 15);
    // Tree: 10(B) -> 5(R), 15(R)
    
    rbt.remove(10); // Xóa root đen có 2 con đỏ -> Phức tạp
    
    assert(rbt.size() == 2);
    assert(rbt.root != nullptr);
    assert(rbt.root->color == BLACK); // Root mới phải đen
    assert(validateRBProperties(rbt));
    cout << "-> RBT Delete Root Cascade: OK" << endl;
}

void tc_42_RBT_Case_Sibling_Red() {
    // Tái hiện trường hợp Sibling là Đỏ (Case khó của Delete)
    RedBlackTree<int, int> rbt;
    rbt.insert(20, 20);
    rbt.insert(10, 10);
    rbt.insert(30, 30);
    rbt.insert(5, 5);
    rbt.insert(15, 15);
    rbt.insert(25, 25);
    rbt.insert(35, 35);
    // Tạo thêm tầng để có màu đỏ
    rbt.insert(3, 3); // Làm 5 thành Đỏ (hoặc đen tùy insert)
    
    // Xóa một node đen để kích hoạt Double Black
    rbt.remove(25); 
    
    assert(validateRBProperties(rbt));
    cout << "-> RBT Sibling/Double Black Case: OK" << endl;
}

void tc_43_RBT_Interleaved_Ops() {
    RedBlackTree<int, int> rbt;
    // Chèn 1, Xóa 1, Chèn 2, Xóa 2... (Kiểm tra empty liên tục)
    for(int i=0; i<10; ++i) {
        rbt.insert(i, i);
        assert(rbt.size() == 1);
        rbt.remove(i);
        assert(rbt.size() == 0);
    }
    cout << "-> RBT Interleaved Ops: OK" << endl;
}

void tc_44_RBT_Delete_Max_Min() {
    RedBlackTree<int, int> rbt;
    for(int i=1; i<=10; ++i) rbt.insert(i, i);
    
    // Xóa Min (1) và Max (10)
    rbt.remove(1);
    rbt.remove(10);
    
    assert(rbt.contains(1) == false);
    assert(rbt.contains(10) == false);
    assert(rbt.contains(5) == true);
    assert(validateRBProperties(rbt));
    cout << "-> RBT Delete Min/Max: OK" << endl;
}

void tc_45_RBT_Reverse_Insert() {
    // Chèn ngược từ 20 về 1
    RedBlackTree<int, int> rbt;
    for(int i=20; i>=1; --i) rbt.insert(i, i);
    
    assert(rbt.size() == 20);
    assert(validateRBProperties(rbt));
    
    // Xóa 5 phần tử giữa
    for(int i=8; i<=12; ++i) rbt.remove(i);
    assert(validateRBProperties(rbt));
    cout << "-> RBT Reverse Insert & Middle Delete: OK" << endl;
}

// =================================================================
// STRESS TESTS & RANDOMIZED CASES (46-55)
// =================================================================

// Helper để sinh số ngẫu nhiên
int getRandomInt(int min, int max) {
    return min + (rand() % (max - min + 1));
}

void tc_46_AVL_Random_Stress() {
    AVLTree<int, int> tree;
    int n = 1000; // Thử với 1000 node
    cout << "-> Inserting " << n << " random nodes..." << endl;
    
    vector<int> keys;
    for(int i=0; i<n; ++i) {
        int key = getRandomInt(-5000, 5000);
        tree.insert(key, key);
        keys.push_back(key);
    }
    
    // Kiểm tra tính cân bằng sau khi insert
    int h = tree.getHeight();
    cout << "-> Tree Height after insert: " << h << endl;
    assert(h <= 1.44 * log2(n + 2)); // Công thức chiều cao tối đa AVL
    
    // Xóa ngẫu nhiên 50% số node
    cout << "-> Removing 50% nodes..." << endl;
    for(int i=0; i<n/2; ++i) {
        if(!keys.empty()) {
            int idx = getRandomInt(0, keys.size()-1);
            tree.remove(keys[idx]);
            keys.erase(keys.begin() + idx);
        }
    }
    
    // Kiểm tra lại chiều cao và tính toàn vẹn
    assert(tree.getHeight() <= 1.44 * log2(tree.getSize() + 2));
    cout << "-> AVL Random Stress: OK" << endl;
}

void tc_47_AVL_V_Shape_Insert() {
    // Chèn kiểu hình chữ V: 100, 99... 50 ... 51, 52... 100
    // Để kiểm tra khả năng cân bằng khi dữ liệu giảm rồi tăng
    AVLTree<int, int> tree;
    int minVal = 50;
    int maxVal = 100;
    
    // Nhánh trái (giảm dần)
    for(int i=maxVal; i>=minVal; --i) tree.insert(i, i);
    // Nhánh phải (tăng dần)
    for(int i=minVal+1; i<=maxVal; ++i) tree.insert(i, i);
    
    assert(tree.contains(minVal));
    assert(tree.contains(maxVal));
    assert(tree.getHeight() <= 8); 
    cout << "-> AVL V-Shape Insert: OK" << endl;
}

void tc_48_AVL_Repeated_Operations() {
    // Thêm vào rồi xóa đi liên tục trên cùng 1 key
    AVLTree<int, int> tree;
    int key = 42;
    for(int i=0; i<100; ++i) {
        tree.insert(key, i);
        assert(tree.contains(key));
        tree.remove(key);
        assert(!tree.contains(key));
    }
    assert(tree.getSize() == 0);
    cout << "-> AVL Repeated Ops on Single Key: OK" << endl;
}

void tc_49_AVL_Empty_Clear_Loop() {
    // Clear liên tục xem có lỗi bộ nhớ hay pointer không
    AVLTree<int, int> tree;
    for(int i=0; i<10; ++i) {
        tree.insert(i, i);
        tree.clear();
        assert(tree.getSize() == 0);
        assert(tree.getHeight() == 0);
    }
    tree.insert(1, 1);
    assert(tree.getSize() == 1);
    cout << "-> AVL Empty/Clear Loop: OK" << endl;
}

void tc_50_AVL_Correct_Update_Ignore() {
    // Đề bài: Nếu key tồn tại, KHÔNG insert.
    AVLTree<int, int> tree;
    tree.insert(10, 100); // Giá trị gốc là 100
    tree.insert(10, 200); // Cố chèn 200
    
    // Cần lấy giá trị ra check. Vì AVL không có hàm get() public trả về value,
    // ta dùng inorderTraversal để check giá trị.
    global_inorder_buffer.clear();
    tree.inorderTraversal(captureInt);
    
    assert(global_inorder_buffer.size() == 1);
    assert(global_inorder_buffer[0] == 100); // Phải giữ giá trị cũ
    cout << "-> AVL Insert Duplicate Ignore: OK" << endl;
}

// --- RBT NEW CASES ---

void tc_51_RBT_Random_Stress() {
    RedBlackTree<int, int> rbt;
    int n = 1000;
    cout << "-> RBT Inserting " << n << " random nodes..." << endl;
    vector<int> keys;
    for(int i=0; i<n; ++i) {
        int key = getRandomInt(-5000, 5000);
        rbt.insert(key, key);
        keys.push_back(key);
    }
    
    assert(validateRBProperties(rbt));
    
    cout << "-> RBT Removing random nodes..." << endl;
    for(int i=0; i<n/2; ++i) {
        if(!keys.empty()) {
            int idx = getRandomInt(0, keys.size()-1);
            rbt.remove(keys[idx]);
            keys.erase(keys.begin() + idx);
            
            // Validate sau MỖI lần xóa (rất quan trọng)
            if (i % 50 == 0) { // Check định kỳ để đỡ chậm
                 if (!validateRBProperties(rbt)) {
                     cout << "FAILED at iteration " << i << " removing " << keys[idx] << endl;
                     assert(false);
                 }
            }
        }
    }
    assert(validateRBProperties(rbt));
    cout << "-> RBT Random Stress: OK" << endl;
}

void tc_52_RBT_Skewed_Input() {
    // RBT cực mạnh với dữ liệu đã sắp xếp (AVL cũng vậy nhưng RBT xoay ít hơn)
    RedBlackTree<int, int> rbt;
    int n = 100;
    for(int i=0; i<n; ++i) rbt.insert(i, i);
    
    assert(rbt.size() == n);
    assert(validateRBProperties(rbt));
    
    // Kiểm tra height đen
    // Chiều cao thực tế <= 2 * log2(n+1)
    // Với n=100, log2(101) ~ 6.6 -> Height <= 13
    // (Ta không lấy được height thực tế nếu không viết hàm, nhưng validateRBProperties đã check logic rồi)
    cout << "-> RBT Skewed Input (Sorted): OK" << endl;
}

void tc_53_RBT_Delete_Case_Red_Sibling() {
    // Một trường hợp xóa khó: Nút xóa là đen, anh em là đỏ
    RedBlackTree<int, int> rbt;
    // Cấu hình: 20(B) -> 10(B), 30(B) -> ... -> ép 30 có con đỏ, 10 đen
    // Cách dễ nhất là insert theo mẫu và xóa
    rbt.insert(20, 20);
    rbt.insert(10, 10);
    rbt.insert(30, 30);
    rbt.insert(5, 5);
    rbt.insert(15, 15); 
    rbt.insert(25, 25);
    rbt.insert(35, 35);
    
    // Xóa để gây mất cân bằng
    rbt.remove(5);
    rbt.remove(15);
    // Lúc này cây con trái khá trống
    assert(validateRBProperties(rbt));
    cout << "-> RBT Delete Case Red Sibling: OK" << endl;
}

void tc_54_RBT_Alternating_Insert() {
    // Chèn: 0, 100, 1, 99, 2, 98...
    RedBlackTree<int, int> rbt;
    int n = 50;
    for(int i=0; i<n; ++i) {
        rbt.insert(i, i);
        rbt.insert(1000-i, 1000-i);
    }
    assert(rbt.size() == n * 2);
    assert(validateRBProperties(rbt));
    cout << "-> RBT Alternating Insert: OK" << endl;
}

void tc_55_RBT_Search_Performance() {
    // Kiểm tra logic tìm kiếm Lower/Upper bound với dữ liệu thưa
    RedBlackTree<int, int> rbt;
    // Insert: 0, 10, 20, ... 100
    for(int i=0; i<=100; i+=10) rbt.insert(i, i);
    
    bool found;
    // Tìm 5 -> Lower bound là 10
    auto node = rbt.lowerBound(5, found);
    assert(found && node->key == 10);
    
    // Tìm 15 -> Upper bound ( > 15) là 20
    node = rbt.upperBound(15, found);
    assert(found && node->key == 20);
    
    // Tìm 100 -> Upper bound không có
    node = rbt.upperBound(100, found);
    assert(!found);
    
    cout << "-> RBT Search Bounds (Sparse): OK" << endl;
}

// =================================================================
// MAIN RUNNER
// =================================================================

struct TestCase {
    string name;
    function<void()> func;
};

int main() {
    vector<TestCase> tests = {
        // --- CÁC TEST CŨ (01-35) GIỮ NGUYÊN ---
        {"AVL 01: Basic Insert", tc_01_AVL_BasicInsert},
        {"AVL 02: RR Rotation", tc_02_AVL_Rotation_RR},
        {"AVL 03: LL Rotation", tc_03_AVL_Rotation_LL},
        {"AVL 04: LR Rotation", tc_04_AVL_Rotation_LR},
        {"AVL 05: RL Rotation", tc_05_AVL_Rotation_RL},
        {"AVL 06: Contains", tc_06_AVL_Contains},
        {"AVL 07: Remove Leaf", tc_07_AVL_RemoveLeaf},
        {"AVL 08: Remove One Child", tc_08_AVL_RemoveOneChild},
        {"AVL 09: Remove Two Children", tc_09_AVL_RemoveTwoChildren},
        {"AVL 10: Inorder Traversal", tc_10_AVL_InorderTraversal},
        {"AVL 11: Empty Tree", tc_11_AVL_EmptyTree},
        {"AVL 12: Duplicate Insert", tc_12_AVL_DuplicateInsert},
        {"AVL 13: Remove Non-Existent", tc_13_AVL_RemoveNonExistent},
        {"AVL 14: Single Node", tc_14_AVL_SingleNodeLifecycle},
        {"AVL 15: Ascending Stress", tc_15_AVL_Stress_Ascending},
        {"AVL 16: Descending Stress", tc_16_AVL_Stress_Descending},
        {"AVL 17: Remove Root Collapse", tc_17_AVL_RemoveRootCollapse},
        {"AVL 18: Clear & Reuse", tc_18_AVL_ClearAndReuse},
        {"AVL 19: Complex Rebalancing", tc_19_AVL_ComplexRebalancing},
        {"AVL 20: Template String", tc_20_AVL_TemplateString},

        {"RBT 21: Basic Insert", tc_21_RBT_BasicInsert},
        {"RBT 22: Recolor Case", tc_22_RBT_Recolor},
        {"RBT 23: Rotation Line", tc_23_RBT_Rotation_Line},
        {"RBT 24: Rotation Triangle", tc_24_RBT_Rotation_Triangle},
        {"RBT 25: Find/Contains", tc_25_RBT_Find_Contains},
        {"RBT 26: LowerBound", tc_26_RBT_LowerBound},
        {"RBT 27: UpperBound", tc_27_RBT_UpperBound},
        {"RBT 28: Size & Clear", tc_28_RBT_Size_Clear},
        {"RBT 29: Remove Red Leaf", tc_29_RBT_Remove_RedLeaf},
        {"RBT 30: Template String", tc_30_RBT_Template},
        {"RBT 31: Duplicate", tc_31_RBT_Duplicate},
        {"RBT 32: Remove Black Node", tc_32_RBT_Remove_BlackNode},
        {"RBT 33: Remove Root", tc_33_RBT_Remove_Root},
        {"RBT 34: Stress Ascending", tc_34_RBT_Stress_Ascending},
        {"RBT 35: Remove Non-Exist", tc_35_RBT_Remove_NonExist},
        
        // --- CÁC TEST MỚI THÊM VÀO (36-45) ---
        {"AVL 36: Mix Insert Delete", tc_36_AVL_Mix_Insert_Delete},
        {"AVL 37: Delete All", tc_37_AVL_Delete_All},
        {"AVL 38: ZigZag Stress", tc_38_AVL_ZigZag_Stress},
        {"AVL 39: Large Scale", tc_39_AVL_Large_Scale},
        {"AVL 40: Root Change", tc_40_AVL_Root_Change_Verify},
        
        {"RBT 41: Delete Root Cascade", tc_41_RBT_Delete_Root_Cascade},
        {"RBT 42: Sibling Red Case", tc_42_RBT_Case_Sibling_Red},
        {"RBT 43: Interleaved Ops", tc_43_RBT_Interleaved_Ops},
        {"RBT 44: Delete Min/Max", tc_44_RBT_Delete_Max_Min},
        {"RBT 45: Reverse Insert & Mid Delete", tc_45_RBT_Reverse_Insert},
        {"AVL 46: Random Stress", tc_46_AVL_Random_Stress},
        {"AVL 47: V-Shape Insert", tc_47_AVL_V_Shape_Insert},
        {"AVL 48: Repeated Ops", tc_48_AVL_Repeated_Operations},
        {"AVL 49: Empty/Clear Loop", tc_49_AVL_Empty_Clear_Loop},
        {"AVL 50: Ignore Duplicate", tc_50_AVL_Correct_Update_Ignore},
        
        {"RBT 51: Random Stress", tc_51_RBT_Random_Stress},
        {"RBT 52: Skewed Input", tc_52_RBT_Skewed_Input},
        {"RBT 53: Del Red Sibling", tc_53_RBT_Delete_Case_Red_Sibling},
        {"RBT 54: Alternating Ins", tc_54_RBT_Alternating_Insert},
        {"RBT 55: Search Perf", tc_55_RBT_Search_Performance},
    };

    cout << "==========================================" << endl;
    cout << "STARTING TEST SUITE (" << tests.size() << " cases)" << endl;
    cout << "==========================================" << endl;

    int passed = 0;
    for (const auto& test : tests) {
        printHeader(test.name);
        try {
            test.func(); 
            cout << "=> PASSED" << endl;
            passed++;
        } catch (const exception& e) {
            cout << "=> FAILED (Exception: " << e.what() << ")" << endl;
        } catch (...) {
            cout << "=> FAILED (Unknown Error)" << endl;
        }
    }

    cout << "\n==========================================" << endl;
    cout << "SUMMARY: " << passed << "/" << tests.size() << " TESTS PASSED." << endl;
    if (passed == tests.size()) {
        cout << "CONGRATULATIONS! ALL TESTS GREEN." << endl;
    } else {
        cout << "SOME TESTS FAILED. CHECK LOGS ABOVE." << endl;
    }
    cout << "==========================================" << endl;

    return 0;
}