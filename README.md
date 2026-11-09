<div align="center">

# Social Media Platform in Qt

### A data-structures-focused desktop social network built with C++ and Qt

[![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![Qt](https://img.shields.io/badge/Qt-5%2F6-41CD52?style=for-the-badge&logo=qt&logoColor=white)](https://www.qt.io/)
[![CMake](https://img.shields.io/badge/CMake-3.16%2B-064F8C?style=for-the-badge&logo=cmake&logoColor=white)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-F7DF1E?style=for-the-badge)](LICENSE)

<p>
  <a href="#features">Features</a> •
  <a href="#technology-stack">Technology</a> •
  <a href="#getting-started">Getting Started</a> •
  <a href="#project-structure">Structure</a> •
  <a href="#contributing">Contributing</a>
</p>

</div>

## 📌 Overview

**Social Media Platform in Qt** is a third-semester Data Structures and Algorithms project that turns foundational DSA concepts into a desktop application. It combines a Qt graphical interface with custom C++ data structures and file-based persistence to model users, posts, interactions, and friend relationships.

The project is intentionally educational: core structures are implemented and used directly so their behavior, trade-offs, and complexity remain visible in the application.

## ✨ Features

### 👤 User management

- User registration and login
- Profile creation and customization
- Friend requests with accept/reject flows

### 📝 Posts and feed

- Create, edit, and delete posts
- Timestamp-based post ordering
- Binary-search-tree-backed feed organization
- File-based persistence for application data

### 💬 Social interactions

- Stack-based likes using LIFO behavior
- Queue-based comments using FIFO behavior
- Friend suggestions powered by graph traversal
- Mutual-friend discovery and network visualization

### 🔎 Search and discovery

- Hash-based user lookup
- Optimized retrieval operations
- Recommendations based on the social graph

## 🧠 Data Structures in Action

| Data structure | Application of the concept |
| --- | --- |
| **Stack** | Like history and LIFO interaction behavior |
| **Queue** | Comment ordering and FIFO processing |
| **Binary Search Tree** | Feed organization and ordered retrieval |
| **Graph** | Friend connections, BFS suggestions, and mutual friends |
| **Hash Table** | Fast user search and lookup |
| **Linked List** | Custom entity and collection management |
| **Vector** | Qt-facing collection and rendering support |

### Algorithms and techniques

- Binary search tree insertion, deletion, search, and traversal
- Breadth-first search for friend suggestions
- Depth-first search for network analysis
- Hash-based lookup with average constant-time access
- Timestamp-based sorting with custom comparators
- Time and space complexity analysis for key operations

## 🛠️ Technology Stack

| Layer | Technology |
| --- | --- |
| Language | C++17 |
| GUI | Qt Widgets (Qt 5 or Qt 6) |
| Build systems | CMake and qmake |
| Persistence | Plain-text file handling |
| IDE | Qt Creator (recommended) |
| Platforms | Windows, Linux, and macOS where Qt is available |

## 🏗️ Architecture

The application is organized into three logical layers:

```text
┌─────────────────────────────────────┐
│ Qt Widgets / GUI                    │
│ Main window • Login • Feed          │
└──────────────────┬──────────────────┘
                   │
┌──────────────────▼──────────────────┐
│ Application managers                │
│ Users • Posts • Friends • Comments  │
└──────────────────┬──────────────────┘
                   │
┌──────────────────▼──────────────────┐
│ Custom data structures              │
│ Stack • Queue • BST • Graph • Hash  │
└──────────────────┬──────────────────┘
                   │
┌──────────────────▼──────────────────┐
│ File-based persistence              │
│ users.txt • posts.txt • friends.txt│
└─────────────────────────────────────┘
```

## 🚀 Getting Started

### Prerequisites

- A C++17-compatible compiler (GCC, Clang, or MSVC)
- Qt 5 or Qt 6 with the Widgets module
- CMake 3.16 or newer, or Qt Creator with qmake support

### Option 1: Build with CMake

```bash
git clone https://github.com/riazaslam029/Social-Media-Platform-using-QT-with-Data-Atrucures-concepts-.git
cd Social-Media-Platform-using-QT-with-Data-Atrucures-concepts-
cmake -S . -B build
cmake --build build
```

Run the generated executable from the `build` directory. The exact executable name depends on the platform and generator.

### Option 2: Open with Qt Creator

1. Open `CMakeLists.txt` or `SocialMediaApp.pro` in Qt Creator.
2. Select a desktop kit with Qt Widgets installed.
3. Choose a Debug or Release configuration.
4. Build and run the project.

> **Note:** The application stores runtime data in text files. Run it from a location where it has permission to create or update those files.

## 📂 Project Structure

```text
├── main.cpp                    # Application entry point
├── mainwindow.*                # Main Qt window and UI
├── backend_social_media.h      # Core application backend
├── users.txt                   # User data
├── posts.txt                   # Post data
├── comments.txt                # Comment data
├── friends.txt                 # Friend relationship data
├── CMakeLists.txt              # CMake build configuration
├── SocialMediaApp.pro          # qmake project configuration
└── README.md
```

The custom data-structure implementations and domain models are maintained in the project source files and headers at the repository root.

## 📊 Performance Notes

The following are the intended average-case characteristics of the main optimized operations:

| Operation | Expected complexity |
| --- | --- |
| Hash-based user lookup | `O(1)` |
| BST search | `O(log n)` for a balanced tree; `O(n)` worst case |
| Graph traversal | `O(V + E)` |
| Linked-list traversal | `O(n)` |
| Feed ordering | Depends on the selected sorting strategy |

These values describe algorithmic behavior and can vary with data distribution and implementation details.

## 🤝 Contributing

Contributions, issue reports, and improvement ideas are welcome.

1. Fork the repository.
2. Create a focused branch, such as `feature/improved-search`.
3. Make and test your changes.
4. Commit with a clear message.
5. Open a pull request describing the change.

Please keep contributions focused, document meaningful behavior changes, and preserve the project’s educational emphasis on explicit data-structure implementations.

## 📄 License

This project is distributed under the [MIT License](LICENSE).

## 👨‍💻 Author

**Riaz Aslam**  
[GitHub: @riazaslam029](https://github.com/riazaslam029)

<div align="center">

If this project helped you learn Qt or data structures, consider giving it a ⭐.

</div>
