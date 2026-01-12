#ifndef BACKEND_SOCIAL_MEDIA_H
#define BACKEND_SOCIAL_MEDIA_H

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <stdexcept>
#include <vector>

using namespace std;

// --- Utility: Fixed-size String List for Manual Parsing ---
class StringList {
public:
    string data[20];
    int size = 0;
};

inline void splitString(const string& str, char delimiter, StringList& result) {
    result.size = 0;
    string current;
    for (char c : str) {
        if (c == delimiter) {
            if (result.size < 20) {
                result.data[result.size++] = current;
            }
            current = "";
        }
        else {
            current += c;
        }
    }
    if (result.size < 20) {
        result.data[result.size++] = current;
    }
}

inline unsigned int simple_hash(const string& str) {
    unsigned int hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % 100; // HASHTABLE_CAPACITY = 100
}


// --- CORE CLASSES (Data Payload) ---
class User {
public:
    string userID;
    string username;
    string password;

    User(string id = "", string u = "", string p = "")
        : userID(id), username(u), password(p) {
    }

    string toString() const {
        return userID + "|" + username + "|" + password;
    }

    static User fromString(const string& line) {
        StringList parts;
        splitString(line, '|', parts);
        if (parts.size >= 3) {
            return User(parts.data[0], parts.data[1], parts.data[2]);
        }
        return User();
    }
};

class Comment {
public:
    string commentID;
    string postID;
    string authorUsername;
    string content;

    Comment(string cid = "", string pid = "", string author = "", string c = "")
        : commentID(cid), postID(pid), authorUsername(author), content(c) {
    }

    string toString() const {
        return commentID + "|" + postID + "|" + authorUsername + "|" + content;
    }

    static Comment fromString(const string& line) {
        StringList parts;
        splitString(line, '|', parts);
        if (parts.size >= 4) {
            return Comment(parts.data[0], parts.data[1], parts.data[2], parts.data[3]);
        }
        return Comment();
    }
};

class Post {
public:
    string postID;
    string authorUsername;
    string content;

    Post(string pid = "", string author = "", string c = "")
        : postID(pid), authorUsername(author), content(c) {
    }

    bool operator<(const Post& other) const {
        return postID < other.postID;
    }
    bool operator>(const Post& other) const {
        return postID > other.postID;
    }
    bool operator<=(const Post& other) const {
        return postID <= other.postID;
    }

    string toString(int likes = 0) const {
        return postID + "|" + authorUsername + "|" + content + "|" + to_string(likes);
    }

    static Post fromString(const string& line, int* outLikes = nullptr) {
        StringList parts;
        splitString(line, '|', parts);
        if (parts.size >= 3) {
            int likes = 0;
            if (parts.size >= 4) {
                try { likes = stoi(parts.data[3]); } catch(...) { likes = 0; }
            }
            if (outLikes) *outLikes = likes;
            return Post(parts.data[0], parts.data[1], parts.data[2]);
        }
        return Post();
    }
};


// --- DATA STRUCTURES ---
class PostNode {
public:
    Post data;
    PostNode* next;
    PostNode(const Post& val) : data(val), next(nullptr) {}
};

class CommentNode {
public:
    Comment data;
    CommentNode* next;
    CommentNode(const Comment& val) : data(val), next(nullptr) {}
};

class StringNode {
public:
    string data;
    StringNode* next;
    StringNode(const string& val) : data(val), next(nullptr) {}
};


// Singly Linked List (Posts)
class SinglyLinkedList_Post {
private:
    PostNode* head;
public:
    SinglyLinkedList_Post() : head(nullptr) {}
    ~SinglyLinkedList_Post() {
        PostNode* current = head;
        while (current) {
            PostNode* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
    }
    void insertAtEnd(const Post& val) {
        PostNode* newNode = new PostNode(val);
        if (!head) {
            head = newNode;
            return;
        }
        PostNode* current = head;
        while (current->next) current = current->next;
        current->next = newNode;
    }
    PostNode* getHead() const { return head; }

    vector<Post> toVector() const {
        vector<Post> out;
        PostNode* cur = head;
        while (cur) { out.push_back(cur->data); cur = cur->next; }
        return out;
    }

    bool removeByPostID(const string& pid) {
        PostNode* cur = head;
        PostNode* prev = nullptr;
        while (cur) {
            if (cur->data.postID == pid) {
                if (!prev) {
                    head = cur->next;
                } else {
                    prev->next = cur->next;
                }
                delete cur;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    bool editByPostID(const string& pid, const string& newContent) {
        PostNode* cur = head;
        while (cur) {
            if (cur->data.postID == pid) {
                cur->data.content = newContent;
                return true;
            }
            cur = cur->next;
        }
        return false;
    }
};

// Hash Node
template <typename K, typename V>
class HashNode {
public:
    K key;
    V value;
    HashNode<K, V>* next;
    HashNode(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
};

// Simple Hash Table
const int HASHTABLE_CAPACITY = 100;
template <typename K, typename V>
class SimpleHashTable {
private:
    HashNode<K, V>** table;
public:
    SimpleHashTable() { table = new HashNode<K, V>*[HASHTABLE_CAPACITY](); }
    ~SimpleHashTable() {
        for (int i = 0; i < HASHTABLE_CAPACITY; ++i) {
            HashNode<K, V>* current = table[i];
            while (current) {
                HashNode<K, V>* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] table;
    }
    void insert(const K& key, const V& value) {
        unsigned int index = simple_hash(key);
        HashNode<K, V>* current = table[index];
        while (current) {
            if (current->key == key) {
                current->value = value;
                return;
            }
            current = current->next;
        }
        HashNode<K, V>* newNode = new HashNode<K, V>(key, value);
        newNode->next = table[index];
        table[index] = newNode;
    }
    V* search(const K& key) const {
        unsigned int index = simple_hash(key);
        HashNode<K, V>* current = table[index];
        while (current) {
            if (current->key == key) return &(current->value);
            current = current->next;
        }
        return nullptr;
    }
    HashNode<K, V>** getTable() const { return table; }
};

// SimpleQueue for comments
class SimpleQueue_Comment {
private:
    CommentNode* front;
    CommentNode* rear;
public:
    SimpleQueue_Comment() : front(nullptr), rear(nullptr) {}
    ~SimpleQueue_Comment() {
        CommentNode* current = front;
        while (current) {
            CommentNode* next = current->next;
            delete current;
            current = next;
        }
        front = rear = nullptr;
    }
    void enqueue(const Comment& val) {
        CommentNode* newNode = new CommentNode(val);
        if (!rear) { front = rear = newNode; }
        else { rear->next = newNode; rear = newNode; }
    }
    bool isEmpty() const { return front == nullptr; }
    CommentNode* getFront() const { return front; }
    vector<Comment> toVector() const {
        vector<Comment> out;
        CommentNode* cur = front;
        while (cur) { out.push_back(cur->data); cur = cur->next; }
        return out;
    }
};

// SimpleQueue for strings
class SimpleQueue_String {
private:
    StringNode* front;
    StringNode* rear;
public:
    SimpleQueue_String() : front(nullptr), rear(nullptr) {}
    ~SimpleQueue_String() {
        StringNode* current = front;
        while (current) { StringNode* next = current->next; delete current; current = next; }
        front = rear = nullptr;
    }
    void enqueue(const string& val) {
        StringNode* newNode = new StringNode(val);
        if (!rear) { front = rear = newNode; }
        else { rear->next = newNode; rear = newNode; }
    }
    string dequeue() {
        if (isEmpty()) throw runtime_error("Dequeue from empty queue");
        StringNode* temp = front;
        string data = front->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete temp;
        return data;
    }
    bool isEmpty() const { return front == nullptr; }
    StringNode* getFront() const { return front; }
};

// SimpleStack for likes
class SimpleStack_String {
private:
    StringNode* top;
public:
    SimpleStack_String() : top(nullptr) {}
    ~SimpleStack_String() {
        while (top) { StringNode* temp = top; top = top->next; delete temp; }
    }
    void push(const string& val) {
        StringNode* newNode = new StringNode(val); newNode->next = top; top = newNode;
    }
    string pop() {
        if (isEmpty()) throw runtime_error("Pop from empty stack");
        StringNode* temp = top;
        string d = top->data;
        top = top->next;
        delete temp;
        return d;
    }
    string peek() const {
        if (isEmpty()) throw runtime_error("Peek from empty stack");
        return top->data;
    }
    bool isEmpty() const { return top == nullptr; }

    vector<string> toVector() const {
        vector<string> out;
        StringNode* cur = top;
        while (cur) { out.push_back(cur->data); cur = cur->next; }
        return out;
    }
    int count() const {
        int c = 0; StringNode* cur = top;
        while (cur) { ++c; cur = cur->next; }
        return c;
    }
    bool contains(const string& s) const {
        StringNode* cur = top;
        while (cur) { if (cur->data == s) return true; cur = cur->next; }
        return false;
    }
    bool remove(const string& s) {
        StringNode* cur = top;
        StringNode* prev = nullptr;
        while (cur) {
            if (cur->data == s) {
                if (!prev) top = cur->next;
                else prev->next = cur->next;
                delete cur;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }
};

// Graph adjacency list
class LinkNode {
public:
    string username;
    LinkNode* next;
    LinkNode(const string& uname) : username(uname), next(nullptr) {}
};

class AdjacencyList {
public:
    string username;
    LinkNode* head;
    AdjacencyList(const string& uname = "") : username(uname), head(nullptr) {}
    ~AdjacencyList() {
        LinkNode* current = head;
        while (current) { LinkNode* next = current->next; delete current; current = next; }
        head = nullptr;
    }
    void addFriend(const string& friendUsername) {
        LinkNode* cur = head;
        while (cur) { if (cur->username == friendUsername) return; cur = cur->next; }
        LinkNode* newNode = new LinkNode(friendUsername);
        newNode->next = head; head = newNode;
    }
    void removeFriend(const string& friendUsername) {
        LinkNode* cur = head;
        LinkNode* prev = nullptr;
        while (cur) {
            if (cur->username == friendUsername) {
                if (!prev) head = cur->next;
                else prev->next = cur->next;
                delete cur;
                return;
            }
            prev = cur;
            cur = cur->next;
        }
    }
    LinkNode* getHead() const { return head; }
};

class Graph {
private:
    SimpleHashTable<string, AdjacencyList> nodes;
public:
    void addNode(const string& username) {
        if (!nodes.search(username)) nodes.insert(username, AdjacencyList(username));
    }
    void addEdge(const string& u1, const string& u2) {
        AdjacencyList* list1 = nodes.search(u1);
        AdjacencyList* list2 = nodes.search(u2);
        if (list1 && list2) {
            list1->addFriend(u2);
            list2->addFriend(u1);
        }
    }
    void removeEdge(const string& u1, const string& u2) {
        AdjacencyList* list1 = nodes.search(u1);
        AdjacencyList* list2 = nodes.search(u2);
        if (list1) list1->removeFriend(u2);
        if (list2) list2->removeFriend(u1);
    }
    bool isFriend(const string& u1, const string& u2) const {
        AdjacencyList* list1 = nodes.search(u1);
        if (!list1) return false;
        LinkNode* cur = list1->head;
        while (cur) {
            if (cur->username == u2) return true;
            cur = cur->next;
        }
        return false;
    }
    vector<string> getFriends(const string& u) const {
        vector<string> f;
        AdjacencyList* list = nodes.search(u);
        if (list) {
            LinkNode* cur = list->head;
            while(cur) { f.push_back(cur->username); cur = cur->next; }
        }
        return f;
    }
    vector<string> suggestFriends(const string& startUsername) const {
        vector<string> result;
        SimpleQueue_String levelQueue;
        SimpleHashTable<string, bool> visited;
        levelQueue.enqueue(startUsername);
        visited.insert(startUsername, true);
        int level = 0;
        while (!levelQueue.isEmpty()) {
            int levelSize = 0;
            StringNode* tempNode = levelQueue.getFront();
            while (tempNode) { levelSize++; tempNode = tempNode->next; }
            while (levelSize-- > 0) {
                try {
                    string u = levelQueue.dequeue();
                    AdjacencyList* adjList = nodes.search(u);
                    if (adjList) {
                        LinkNode* currentFriend = adjList->head;
                        while (currentFriend) {
                            string v = currentFriend->username;
                            if (!visited.search(v)) {
                                visited.insert(v, true);
                                levelQueue.enqueue(v);
                                if (level == 1) result.push_back(v);
                            }
                            currentFriend = currentFriend->next;
                        }
                    }
                } catch(...) {}
            }
            level++;
            if (level > 1) break;
        }
        return result;
    }
    SimpleHashTable<string, AdjacencyList>* getNodesTable() { return &nodes; }
};

// BST templated
template <typename T>
class BSTNode {
public:
    T data;
    BSTNode<T>* left;
    BSTNode<T>* right;
    BSTNode(const T& val) : data(val), left(nullptr), right(nullptr) {}
};

template <typename T>
class BinarySearchTree {
private:
    BSTNode<T>* root;
    BSTNode<T>* insertRecursive(BSTNode<T>* node, const T& val) {
        if (!node) return new BSTNode<T>(val);
        if (val < node->data) node->left = insertRecursive(node->left, val);
        else node->right = insertRecursive(node->right, val); // Modified: Allow duplicates/equality to go Right
        return node;
    }
    void inOrderRecursive(BSTNode<T>* node, vector<T>& out) const {
        if (!node) return;
        inOrderRecursive(node->left, out);
        out.push_back(node->data);
        inOrderRecursive(node->right, out);
    }
    // New Helper: Traverse Right (Larger/Newer) then Left (Smaller/Older)
    void reverseInOrderRecursive(BSTNode<T>* node, vector<T>& out) const {
        if (!node) return;
        reverseInOrderRecursive(node->right, out); // Visit larger/newer first
        out.push_back(node->data);
        reverseInOrderRecursive(node->left, out);
    }

    void deleteRecursive(BSTNode<T>* node) {
        if (!node) return;
        deleteRecursive(node->left);
        deleteRecursive(node->right);
        delete node;
    }
public:
    BinarySearchTree() : root(nullptr) {}
    ~BinarySearchTree() { deleteRecursive(root); }
    void clear() { deleteRecursive(root); root = nullptr; }
    void insert(const T& val) { root = insertRecursive(root, val); }
    vector<T> toVectorInOrder() const {
        vector<T> out;
        inOrderRecursive(root, out);
        return out;
    }
    // Function to get posts from Newest to Oldest (assuming ID increases with time)
    vector<T> toVectorReverseInOrder() const {
        vector<T> out;
        reverseInOrderRecursive(root, out);
        return out;
    }
};

// --- SOCIAL MEDIA SYSTEM ---
class SocialMediaSystem {
private:
    SimpleHashTable<string, User> userHash;
    SimpleHashTable<string, SinglyLinkedList_Post> userPosts;
    SimpleHashTable<string, SimpleQueue_Comment> postComments;
    SimpleHashTable<string, SimpleStack_String> postLikes;
    Graph friendGraph;
    BinarySearchTree<Post> allPostsBST;

    User* currentUser = nullptr;

    // ID Tracking to prevent duplicates across sessions
    int maxUserID = 0;
    int maxPostID = 99; // Starts at 100
    int maxCommentID = 999; // Starts at 1000

    const string USERS_FILE = "users.txt";
    const string POSTS_FILE = "posts.txt";
    const string COMMENTS_FILE = "comments.txt";
    const string FRIENDS_FILE = "friends.txt";

    // Helper to extract numeric part from ID (e.g., P105 -> 105)
    int extractID(const string& idStr, char prefix) const {
        if (idStr.size() < 2 || idStr[0] != prefix) return 0;
        try { return stoi(idStr.substr(1)); } catch(...) { return 0; }
    }

    string generateUserID() { return "U" + to_string(++maxUserID); }
    string generatePostID() { return "P" + to_string(++maxPostID); }
    string generateCommentID() { return "C" + to_string(++maxCommentID); }

    void loadDataInternal() {
        ifstream userFile(USERS_FILE);
        string line;
        while (getline(userFile, line)) {
            User u = User::fromString(line);
            if (!u.username.empty()) {
                userHash.insert(u.username, u);
                friendGraph.addNode(u.username);
                // Update Max ID
                int idNum = extractID(u.userID, 'U');
                if (idNum > maxUserID) maxUserID = idNum;
            }
        }
        userFile.close();

        ifstream postFile(POSTS_FILE);
        while (getline(postFile, line)) {
            int likesFromFile = 0;
            Post p = Post::fromString(line, &likesFromFile);
            if (!p.postID.empty()) {
                // Update Max ID
                int idNum = extractID(p.postID, 'P');
                if (idNum > maxPostID) maxPostID = idNum;

                SinglyLinkedList_Post* postsList = userPosts.search(p.authorUsername);
                if (!postsList) { userPosts.insert(p.authorUsername, SinglyLinkedList_Post()); postsList = userPosts.search(p.authorUsername); }
                postsList->insertAtEnd(p);
                allPostsBST.insert(p); // BST now allows duplicates
                postComments.insert(p.postID, SimpleQueue_Comment());
                SimpleStack_String likesStack;
                for (int i = 0; i < likesFromFile; ++i) likesStack.push("L" + to_string(i));
                postLikes.insert(p.postID, likesStack);
            }
        }
        postFile.close();

        ifstream commentFile(COMMENTS_FILE);
        while (getline(commentFile, line)) {
            Comment c = Comment::fromString(line);
            if (!c.commentID.empty()) {
                // Update Max ID
                int idNum = extractID(c.commentID, 'C');
                if (idNum > maxCommentID) maxCommentID = idNum;

                SimpleQueue_Comment* q = postComments.search(c.postID);
                if (q) q->enqueue(c);
            }
        }
        commentFile.close();

        ifstream friendFile(FRIENDS_FILE);
        while (getline(friendFile, line)) {
            StringList parts;
            splitString(line, '|', parts);
            if (parts.size >= 2) { friendGraph.addEdge(parts.data[0], parts.data[1]); }
        }
        friendFile.close();
    }

    void rebuildAllPostsBST() {
        allPostsBST.clear();
        SimpleHashTable<string, SinglyLinkedList_Post>* mup = const_cast<SimpleHashTable<string, SinglyLinkedList_Post>*>(&userPosts);
        HashNode<string, SinglyLinkedList_Post>** pt = mup->getTable();
        for (int i = 0; i < HASHTABLE_CAPACITY; ++i) {
            HashNode<string, SinglyLinkedList_Post>* cur = pt[i];
            while (cur) {
                PostNode* pn = cur->value.getHead();
                while (pn) {
                    allPostsBST.insert(pn->data);
                    pn = pn->next;
                }
                cur = cur->next;
            }
        }
    }

public:
    SocialMediaSystem() { loadDataInternal(); }
    ~SocialMediaSystem() { saveData(); }

    void saveData() const {
        ofstream userFile(USERS_FILE);
        SimpleHashTable<string, User>* muh = const_cast<SimpleHashTable<string, User>*>(&userHash);
        HashNode<string, User>** ut = muh->getTable();
        for (int i = 0; i < HASHTABLE_CAPACITY; ++i) {
            HashNode<string, User>* cur = ut[i];
            while (cur) { userFile << cur->value.toString() << "\n"; cur = cur->next; }
        }
        userFile.close();

        ofstream postFile(POSTS_FILE);
        SimpleHashTable<string, SinglyLinkedList_Post>* mup = const_cast<SimpleHashTable<string, SinglyLinkedList_Post>*>(&userPosts);
        HashNode<string, SinglyLinkedList_Post>** pt = mup->getTable();
        for (int i = 0; i < HASHTABLE_CAPACITY; ++i) {
            HashNode<string, SinglyLinkedList_Post>* cur = pt[i];
            while (cur) {
                PostNode* pn = cur->value.getHead();
                while (pn) {
                    SimpleStack_String* s = const_cast<SimpleHashTable<string, SimpleStack_String>*>(&postLikes)->search(pn->data.postID);
                    int likesCount = s ? s->count() : 0;
                    postFile << pn->data.toString(likesCount) << "\n";
                    pn = pn->next;
                }
                cur = cur->next;
            }
        }
        postFile.close();

        ofstream commentFile(COMMENTS_FILE);
        SimpleHashTable<string, SimpleQueue_Comment>* mpc = const_cast<SimpleHashTable<string, SimpleQueue_Comment>*>(&postComments);
        HashNode<string, SimpleQueue_Comment>** ct = mpc->getTable();
        for (int i = 0; i < HASHTABLE_CAPACITY; ++i) {
            HashNode<string, SimpleQueue_Comment>* cur = ct[i];
            while (cur) {
                CommentNode* cn = cur->value.getFront();
                while (cn) { commentFile << cn->data.toString() << "\n"; cn = cn->next; }
                cur = cur->next;
            }
        }
        commentFile.close();

        ofstream friendFile(FRIENDS_FILE);
        SimpleHashTable<string, bool> savedPairs;
        HashNode<string, AdjacencyList>** ft = const_cast<Graph*>(&friendGraph)->getNodesTable()->getTable();
        for (int i = 0; i < HASHTABLE_CAPACITY; ++i) {
            HashNode<string, AdjacencyList>* cur = ft[i];
            while (cur) {
                string u1 = cur->value.username;
                LinkNode* f = cur->value.head;
                while (f) {
                    string u2 = f->username;
                    string key = (u1 < u2) ? (u1 + "_" + u2) : (u2 + "_" + u1);
                    if (!savedPairs.search(key)) { friendFile << u1 << "|" << u2 << "\n"; savedPairs.insert(key, true); }
                    f = f->next;
                }
                cur = cur->next;
            }
        }
        friendFile.close();
    }

    bool userRegistration(const string& username, const string& password) {
        if (userHash.search(username)) return false;
        string newID = generateUserID();
        userHash.insert(username, User(newID, username, password));
        friendGraph.addNode(username);
        return true;
    }
    bool userLogin(const string& username, const string& password) {
        User* u = userHash.search(username);
        if (u && u->password == password) { currentUser = u; return true; }
        return false;
    }
    void logout() { currentUser = nullptr; }
    string currentUsername() const { return currentUser ? currentUser->username : string(); }

    bool createPost(const string& content) {
        if (!currentUser) return false;
        string pid = generatePostID();
        Post p(pid, currentUser->username, content);
        SinglyLinkedList_Post* postsList = userPosts.search(currentUser->username);
        if (!postsList) { userPosts.insert(currentUser->username, SinglyLinkedList_Post()); postsList = userPosts.search(currentUser->username); }
        postsList->insertAtEnd(p);
        allPostsBST.insert(p);
        postComments.insert(pid, SimpleQueue_Comment());
        postLikes.insert(pid, SimpleStack_String());
        return true;
    }

    vector<Post> getAllPosts() const { return allPostsBST.toVectorInOrder(); }
    vector<Post> getPostsByUser(const string& username) const {
        SinglyLinkedList_Post* list = userPosts.search(username);
        if (!list) return {};
        return list->toVector();
    }

    bool addComment(const string& postID, const string& text) {
        if (!currentUser) return false;
        SimpleQueue_Comment* q = postComments.search(postID);
        if (!q) return false;
        string cid = generateCommentID();
        q->enqueue(Comment(cid, postID, currentUser->username, text));
        return true;
    }

    vector<Comment> getComments(const string& postID) const {
        SimpleQueue_Comment* q = postComments.search(postID);
        if (!q) return {};
        return q->toVector();
    }

    bool toggleLike(const string& postID) {
        if (!currentUser) return false;
        SimpleStack_String* s = postLikes.search(postID);
        if (!s) return false;
        if (s->contains(currentUser->username)) { s->remove(currentUser->username); return true; }
        else { s->push(currentUser->username); return true; }
    }

    int getLikeCount(const string& postID) const {
        SimpleStack_String* s = const_cast<SimpleHashTable<string, SimpleStack_String>*>(&postLikes)->search(postID);
        return s ? s->count() : 0;
    }

    bool addFriend(const string& friendUsername) {
        if (!currentUser) return false;
        if (currentUser->username == friendUsername) return false;
        if (!userHash.search(friendUsername)) return false;
        if (friendGraph.isFriend(currentUser->username, friendUsername)) return false;
        friendGraph.addEdge(currentUser->username, friendUsername);
        return true;
    }

    bool removeFriend(const string& friendUsername) {
        if (!currentUser) return false;
        friendGraph.removeEdge(currentUser->username, friendUsername);
        return true;
    }

    vector<string> getFriendList() const {
        if (!currentUser) return {};
        return friendGraph.getFriends(currentUser->username);
    }

    vector<string> suggestFriends() const {
        if (!currentUser) return {};
        return friendGraph.suggestFriends(currentUser->username);
    }

    bool searchUser(const string& username, User& outUser) const {
        User* u = userHash.search(username);
        if (!u) return false;
        outUser = *u;
        return true;
    }

    string postSummary(const Post& p) const {
        int likes = getLikeCount(p.postID);
        return p.postID + " | " + p.authorUsername + "\n" + p.content + "\nLikes: " + to_string(likes);
    }

    bool editPost(const string& postID, const string& newContent) {
        if (!currentUser) return false;
        SinglyLinkedList_Post* list = userPosts.search(currentUser->username);
        if (!list) return false;
        bool ok = list->editByPostID(postID, newContent);
        if (ok) { const_cast<SocialMediaSystem*>(this)->rebuildAllPostsBST(); return true; }
        return false;
    }

    bool deletePost(const string& postID) {
        if (!currentUser) return false;
        SinglyLinkedList_Post* list = userPosts.search(currentUser->username);
        if (!list) return false;
        bool ok = list->removeByPostID(postID);
        if (ok) { const_cast<SocialMediaSystem*>(this)->rebuildAllPostsBST(); return true; }
        return false;
    }

    // --- IMPROVED FOR YOU FEED ALGORITHM ---
    // Returns posts from all friends (and suggestions) ordered NEWEST first.
    vector<Post> getFeedPosts() const {
        if (!currentUser) return {};

        // 1. Get ALL posts from the BST, but in REVERSE order (Newest -> Oldest)
        // This ensures the feed looks correct time-wise.
        vector<Post> allPosts = allPostsBST.toVectorReverseInOrder();
        vector<Post> filteredFeed;

        // 2. Build quick lookup tables for friends and suggestions
        SimpleHashTable<string, bool> allowedAuthors;

        // Add Friends (Priority 1)
        AdjacencyList* friendsList = const_cast<Graph*>(&friendGraph)->getNodesTable()->search(currentUser->username);
        if (friendsList) {
            LinkNode* curr = friendsList->head;
            while(curr) {
                allowedAuthors.insert(curr->username, true);
                curr = curr->next;
            }
        }

        // Add Suggestions (Priority 2) - helps discovery
        vector<string> suggestions = friendGraph.suggestFriends(currentUser->username);
        for(const string& s : suggestions) {
            allowedAuthors.insert(s, true);
        }

        // 3. Filter the sorted posts
        for(const Post& p : allPosts) {
            // Exclude own posts
            if (p.authorUsername == currentUser->username) continue;

            // Include if author is in our allowed list (Friend or Suggestion)
            if (allowedAuthors.search(p.authorUsername)) {
                filteredFeed.push_back(p);
            }
        }
        return filteredFeed;
    }
};

#endif // BACKEND_SOCIAL_MEDIA__H
