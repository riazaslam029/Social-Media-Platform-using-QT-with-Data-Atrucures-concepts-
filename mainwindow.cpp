#include "mainwindow.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QMessageBox>
#include <QInputDialog>
#include <QFrame>
#include <set>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("DSA Social Media - Qt GUI");
    resize(1000, 700);

    // Initialize Stacked Widget
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // ==========================================
    // PAGE 1: Login / Register Page
    // ==========================================
    loginPage = new QWidget();
    QVBoxLayout* authLayout = new QVBoxLayout();
    authLayout->setAlignment(Qt::AlignCenter); // Center the login box

    QWidget* authContainer = new QWidget();
    authContainer->setFixedWidth(300); // Fixed width for cleaner look
    QVBoxLayout* containerLayout = new QVBoxLayout(authContainer);

    QLabel* titleLabel = new QLabel("<h1>Social Media App</h1>");
    titleLabel->setAlignment(Qt::AlignCenter);

    usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText("Username");
    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);

    loginBtn = new QPushButton("Login");
    registerBtn = new QPushButton("Register");

    connect(loginBtn, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(registerBtn, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);

    containerLayout->addWidget(titleLabel);
    containerLayout->addSpacing(20);
    containerLayout->addWidget(usernameEdit);
    containerLayout->addWidget(passwordEdit);
    containerLayout->addSpacing(10);
    containerLayout->addWidget(loginBtn);
    containerLayout->addWidget(registerBtn);

    authLayout->addWidget(authContainer);
    loginPage->setLayout(authLayout);
    stackedWidget->addWidget(loginPage); // Index 0

    // ==========================================
    // PAGE 2: Main Application Page
    // ==========================================
    mainPage = new QWidget();
    QVBoxLayout* mainPageLayout = new QVBoxLayout();

    // Top Bar
    QHBoxLayout* topBarLayout = new QHBoxLayout();
    QLabel* feedLabel = new QLabel("<h3>For You Feed</h3>");
    logoutBtn = new QPushButton("Logout");
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);

    topBarLayout->addWidget(feedLabel);
    topBarLayout->addStretch();
    topBarLayout->addWidget(logoutBtn);

    // Feed and post details
    feedList = new QListWidget();
    // Styling to make spacing nicer
    feedList->setSpacing(5);
    feedList->setAlternatingRowColors(true);

    postDetailLabel = new QLabel("Select a post to view extended details");
    postDetailLabel->setWordWrap(true);
    postDetailLabel->setMinimumWidth(300);
    postDetailLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    connect(feedList, &QListWidget::itemSelectionChanged, this, &MainWindow::onPostSelected);

    // Right side buttons (Legacy/Backup interactions)
    likeBtn = new QPushButton("Toggle Like (Selected)");
    addCommentBtn = new QPushButton("Add Comment (Selected)");
    viewCommentsBtn = new QPushButton("View Comments (Selected)");

    connect(likeBtn, &QPushButton::clicked, this, &MainWindow::onLikeClicked);
    connect(addCommentBtn, &QPushButton::clicked, this, &MainWindow::onAddCommentClicked);
    connect(viewCommentsBtn, &QPushButton::clicked, this, &MainWindow::onViewCommentsClicked);

    QVBoxLayout* detailLayout = new QVBoxLayout();
    detailLayout->addWidget(new QLabel("<b>Details Panel:</b>"));
    detailLayout->addWidget(postDetailLabel);
    detailLayout->addStretch();
    detailLayout->addWidget(likeBtn);
    detailLayout->addWidget(addCommentBtn);
    detailLayout->addWidget(viewCommentsBtn);

    QSplitter* splitter = new QSplitter();
    QWidget* leftWidget = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(new QLabel("<b>Feed:</b>"));
    leftLayout->addWidget(feedList);
    leftWidget->setLayout(leftLayout);

    QWidget* rightWidget = new QWidget();
    rightWidget->setLayout(detailLayout);

    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setStretchFactor(0, 2); // Give feed more space
    splitter->setStretchFactor(1, 1);

    // Bottom actions
    createPostBtn = new QPushButton("Create Post");
    addFriendBtn = new QPushButton("Add Friend");
    suggestFriendsBtn = new QPushButton("Suggest Friends");
    searchUserBtn = new QPushButton("Search User");
    myProfileBtn = new QPushButton("My Profile");

    connect(createPostBtn, &QPushButton::clicked, this, &MainWindow::onCreatePostClicked);
    connect(addFriendBtn, &QPushButton::clicked, this, &MainWindow::onAddFriendClicked);
    connect(suggestFriendsBtn, &QPushButton::clicked, this, &MainWindow::onSuggestFriendsClicked);
    connect(searchUserBtn, &QPushButton::clicked, this, &MainWindow::onSearchUserClicked);
    connect(myProfileBtn, &QPushButton::clicked, this, &MainWindow::onMyProfileClicked);

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(createPostBtn);
    bottomLayout->addWidget(addFriendBtn);
    bottomLayout->addWidget(suggestFriendsBtn);
    bottomLayout->addWidget(searchUserBtn);
    bottomLayout->addWidget(myProfileBtn);

    mainPageLayout->addLayout(topBarLayout);
    mainPageLayout->addWidget(splitter);
    mainPageLayout->addLayout(bottomLayout);

    mainPage->setLayout(mainPageLayout);
    stackedWidget->addWidget(mainPage); // Index 1

    // Initial state
    updateUiForAuth();
}

MainWindow::~MainWindow() {}

QString MainWindow::selectedPostID() const {
    auto sel = feedList->selectedItems();
    return sel.empty() ? QString() : sel.first()->data(Qt::UserRole).toString();
}

void MainWindow::updateUiForAuth() {
    bool loggedIn = !backend.currentUsername().empty();

    if (loggedIn) {
        stackedWidget->setCurrentIndex(1); // Show Main Page
        populateFeed();
    } else {
        stackedWidget->setCurrentIndex(0); // Show Login Page
        usernameEdit->clear();
        passwordEdit->clear();
    }
}

// ==========================================================
// MODIFIED POPULATE FEED FUNCTION - ADDS BUTTONS TO EACH POST
// ==========================================================
void MainWindow::populateFeed() {
    feedList->clear();

    if (backend.currentUsername().empty()) return;

    // Get posts ordered Newest First
    vector<Post> feedPosts = backend.getFeedPosts();

    for (const Post& p : feedPosts) {
        // 1. Create a custom widget to hold the post UI
        QWidget* postWidget = new QWidget();
        QVBoxLayout* postLayout = new QVBoxLayout(postWidget);
        postLayout->setContentsMargins(10, 10, 10, 10);

        // 2. Author Header
        QLabel* authorLabel = new QLabel("<b>" + QString::fromStdString(p.authorUsername) + "</b>");
        authorLabel->setStyleSheet("color: #2c3e50; font-size: 14px;");
        postLayout->addWidget(authorLabel);

        // 3. Post Content
        QLabel* contentLabel = new QLabel(QString::fromStdString(p.content));
        contentLabel->setWordWrap(true);
        contentLabel->setStyleSheet("font-size: 13px; margin-top: 5px; margin-bottom: 5px;");
        postLayout->addWidget(contentLabel);

        // 4. Action Buttons Layout (Bottom of post)
        QHBoxLayout* actionLayout = new QHBoxLayout();
        actionLayout->setContentsMargins(0, 5, 0, 0);

        int likeCount = backend.getLikeCount(p.postID);
        QPushButton* itemLikeBtn = new QPushButton("Like (" + QString::number(likeCount) + ")");
        itemLikeBtn->setCursor(Qt::PointingHandCursor);

        QPushButton* itemCommentBtn = new QPushButton("Comment");
        itemCommentBtn->setCursor(Qt::PointingHandCursor);

        QPushButton* itemViewCommentsBtn = new QPushButton("View Comments");
        itemViewCommentsBtn->setCursor(Qt::PointingHandCursor);

        actionLayout->addWidget(itemLikeBtn);
        actionLayout->addWidget(itemCommentBtn);
        actionLayout->addWidget(itemViewCommentsBtn);
        actionLayout->addStretch(); // Push buttons to the left

        postLayout->addLayout(actionLayout);

        // 5. Divider Line (Optional visual separation inside the item)
        QFrame* line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        postLayout->addWidget(line);

        // 6. Create the List Item
        QListWidgetItem* item = new QListWidgetItem(feedList);
        // Important: Set size hint so ListWidget knows how much space to allocate
        item->setSizeHint(postWidget->sizeHint());
        // Store PostID in UserRole for selection logic
        item->setData(Qt::UserRole, QString::fromStdString(p.postID));

        // 7. Assign Widget to Item
        feedList->setItemWidget(item, postWidget);

        // 8. Connect Inline Buttons
        // We use lambdas that capture 'p' by value to perform actions on specific posts

        // LIKE BUTTON
        connect(itemLikeBtn, &QPushButton::clicked, this, [this, p, itemLikeBtn]() {
            backend.toggleLike(p.postID);
            int newCount = backend.getLikeCount(p.postID);
            itemLikeBtn->setText("Like (" + QString::number(newCount) + ")");

            // Refresh detail view if this post happens to be selected
            if (selectedPostID() == QString::fromStdString(p.postID)) {
                onPostSelected();
            }
        });

        // COMMENT BUTTON
        connect(itemCommentBtn, &QPushButton::clicked, this, [this, p]() {
            bool ok;
            QString text = QInputDialog::getMultiLineText(this, "Add Comment", "Comment on " + QString::fromStdString(p.authorUsername) + "'s post:", "", &ok);
            if (ok && !text.trimmed().isEmpty()) {
                backend.addComment(p.postID, text.toStdString());
                QMessageBox::information(this, "Success", "Comment added.");
            }
        });

        // VIEW COMMENTS BUTTON
        connect(itemViewCommentsBtn, &QPushButton::clicked, this, [this, p]() {
            vector<Comment> comments = backend.getComments(p.postID);
            QString all;
            if (comments.empty()) all = "No comments yet.";
            else {
                for (const Comment& c : comments) {
                    all += QString::fromStdString("[" + c.authorUsername + "]: " + c.content + "\n");
                }
            }
            QMessageBox::information(this, "Comments", all);
        });
    }

    if (feedList->count() == 0) {
        feedList->addItem("No posts from friends or suggested friends.");
    }
}

void MainWindow::onLoginClicked() {
    string u = usernameEdit->text().toStdString();
    string p = passwordEdit->text().toStdString();
    if (backend.userLogin(u, p)) {
        QMessageBox::information(this, "Login", "Welcome, " + QString::fromStdString(u));
        updateUiForAuth();
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
    }
}

void MainWindow::onRegisterClicked() {
    string u = usernameEdit->text().toStdString();
    string p = passwordEdit->text().toStdString();
    if (u.empty() || p.empty()) { QMessageBox::warning(this, "Register", "Username and password required."); return; }
    if (backend.userRegistration(u, p)) {
        QMessageBox::information(this, "Register", "Registration successful. You can now log in.");
    } else {
        QMessageBox::warning(this, "Register", "Username already exists.");
    }
}

void MainWindow::onLogoutClicked() {
    backend.logout();
    QMessageBox::information(this, "Logout", "Logged out.");
    updateUiForAuth();
    feedList->clear();
    postDetailLabel->clear();
}

void MainWindow::onCreatePostClicked() {
    bool ok;
    QString content = QInputDialog::getMultiLineText(this, "Create Post", "Content:", "", &ok);
    if (ok && !content.trimmed().isEmpty()) {
        if (backend.createPost(content.toStdString())) {
            QMessageBox::information(this, "Post", "Post created.");
            populateFeed();
        } else {
            QMessageBox::warning(this, "Post", "Failed to create post.");
        }
    }
}

void MainWindow::onPostSelected() {
    QString postID = selectedPostID();
    if (postID.isEmpty()) { postDetailLabel->clear(); return; }

    vector<Post> posts = backend.getAllPosts();
    for (const Post& p : posts) {
        if (QString::fromStdString(p.postID) == postID) {
            postDetailLabel->setText(QString::fromStdString(backend.postSummary(p)));
            return;
        }
    }
    postDetailLabel->setText("Post not found.");
}

// Legacy slot for the right-hand panel button
void MainWindow::onLikeClicked() {
    QString postID = selectedPostID();
    if (postID.isEmpty()) { QMessageBox::information(this, "Like", "Select a post first."); return; }
    if (backend.toggleLike(postID.toStdString())) {
        QMessageBox::information(this, "Like", "Toggled like for " + postID);
        onPostSelected(); // Refresh details
        populateFeed(); // Refresh list buttons
    } else {
        QMessageBox::warning(this, "Like", "Action failed.");
    }
}

// Legacy slot for the right-hand panel button
void MainWindow::onAddCommentClicked() {
    QString postID = selectedPostID();
    if (postID.isEmpty()) { QMessageBox::information(this, "Comment", "Select a post first."); return; }
    bool ok;
    QString text = QInputDialog::getMultiLineText(this, "Comment", "Comment:", "", &ok);
    if (ok && !text.trimmed().isEmpty()) {
        if (backend.addComment(postID.toStdString(), text.toStdString())) {
            QMessageBox::information(this, "Comment", "Comment added.");
        } else {
            QMessageBox::warning(this, "Comment", "Failed to add comment.");
        }
    }
}

// Legacy slot for the right-hand panel button
void MainWindow::onViewCommentsClicked() {
    QString postID = selectedPostID();
    if (postID.isEmpty()) { QMessageBox::information(this, "Comments", "Select a post first."); return; }
    vector<Comment> comments = backend.getComments(postID.toStdString());
    if (comments.empty()) {
        QMessageBox::information(this, "Comments", "No comments yet.");
        return;
    }
    QString all;
    for (const Comment& c : comments) {
        all += QString::fromStdString("[" + c.authorUsername + "]: " + c.content + "\n");
    }
    QMessageBox::information(this, "Comments for " + postID, all);
}

void MainWindow::onAddFriendClicked() {
    bool ok;
    QString uname = QInputDialog::getText(this, "Add Friend", "Username to add:", QLineEdit::Normal, "", &ok);
    if (ok && !uname.trimmed().isEmpty()) {
        if (backend.addFriend(uname.toStdString())) {
            QMessageBox::information(this, "Friend", "Friend added.");
            populateFeed();
        } else {
            QMessageBox::warning(this, "Friend", "Failed to add friend (maybe not found or already friends).");
        }
    }
}

void MainWindow::onSuggestFriendsClicked() {
    vector<string> suggestions = backend.suggestFriends();
    if (suggestions.empty()) {
        QMessageBox::information(this, "Suggestions", "No suggestions right now.");
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle("Suggested Friends");
    dlg.setMinimumWidth(300);

    QVBoxLayout* layout = new QVBoxLayout(&dlg);

    QListWidget* listWidget = new QListWidget();

    for (const string& s : suggestions) {
        QWidget* itemWidget = new QWidget();
        QHBoxLayout* itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(5, 5, 5, 5);

        QLabel* nameLabel = new QLabel(QString::fromStdString(s));
        QPushButton* addBtn = new QPushButton("Add Friend");

        itemLayout->addWidget(nameLabel);
        itemLayout->addStretch();
        itemLayout->addWidget(addBtn);

        connect(addBtn, &QPushButton::clicked, this, [this, s, addBtn]() {
            if (backend.addFriend(s)) {
                addBtn->setText("Added");
                addBtn->setEnabled(false);
                populateFeed();
            } else {
                QMessageBox::warning(nullptr, "Error", "Could not add friend.");
            }
        });

        QListWidgetItem* item = new QListWidgetItem(listWidget);
        item->setSizeHint(itemWidget->sizeHint());
        listWidget->setItemWidget(item, itemWidget);
    }

    layout->addWidget(listWidget);

    QPushButton* closeBtn = new QPushButton("Close");
    connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    layout->addWidget(closeBtn);

    dlg.exec();
}

void MainWindow::onSearchUserClicked() {
    bool ok;
    QString uname = QInputDialog::getText(this, "Search User", "Username:", QLineEdit::Normal, "", &ok);
    if (ok && !uname.trimmed().isEmpty()) {
        User u;
        if (backend.searchUser(uname.toStdString(), u)) {
            QMessageBox::information(this, "User Found", "Username: " + QString::fromStdString(u.username) + "\nID: " + QString::fromStdString(u.userID));
        } else {
            QMessageBox::information(this, "Search", "User not found.");
        }
    }
}

void MainWindow::onMyProfileClicked() {
    QString me = QString::fromStdString(backend.currentUsername());
    if (me.isEmpty()) { QMessageBox::information(this, "Profile", "Not logged in."); return; }

    QDialog dlg(this);
    dlg.setWindowTitle("My Profile - Manage Posts");
    dlg.resize(400, 500);
    QVBoxLayout* dlgLayout = new QVBoxLayout();

    QListWidget* myPostsList = new QListWidget();
    vector<Post> posts = backend.getPostsByUser(backend.currentUsername());
    for (const Post& p : posts) {
        QString text = QString::fromStdString(p.postID + " : " + (p.content.size() > 80 ? p.content.substr(0,80) + "..." : p.content));
        QListWidgetItem* it = new QListWidgetItem(text);
        it->setData(Qt::UserRole, QString::fromStdString(p.postID));
        myPostsList->addItem(it);
    }

    dlgLayout->addWidget(new QLabel("Select a post to edit, delete, or view likes/comments:"));
    dlgLayout->addWidget(myPostsList);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* editBtn = new QPushButton("Edit");
    QPushButton* deleteBtn = new QPushButton("Delete");
    QPushButton* viewBtn = new QPushButton("View Likes/Comments");
    QPushButton* friendListBtn = new QPushButton("Friend List");
    QPushButton* closeBtn = new QPushButton("Close");

    btnLayout->addWidget(editBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addWidget(viewBtn);
    btnLayout->addWidget(friendListBtn);
    btnLayout->addWidget(closeBtn);

    dlgLayout->addLayout(btnLayout);
    dlg.setLayout(dlgLayout);

    connect(friendListBtn, &QPushButton::clicked, this, &MainWindow::showFriendsDialog);

    connect(editBtn, &QPushButton::clicked, this, [this, myPostsList]() {
        auto sel = myPostsList->selectedItems();
        if (sel.empty()) { QMessageBox::information(this, "Edit", "Select a post first."); return; }
        QString pid = sel.first()->data(Qt::UserRole).toString();

        vector<Post> posts = backend.getPostsByUser(backend.currentUsername());
        QString original;
        for (const Post& p : posts) {
            if (QString::fromStdString(p.postID) == pid) { original = QString::fromStdString(p.content); break; }
        }
        bool ok;
        QString newContent = QInputDialog::getMultiLineText(this, "Edit Post", "Content:", original, &ok);
        if (ok && !newContent.trimmed().isEmpty()) {
            if (backend.editPost(pid.toStdString(), newContent.toStdString())) {
                QMessageBox::information(this, "Edit", "Post updated.");
                populateFeed();

                myPostsList->clear();
                vector<Post> newPosts = backend.getPostsByUser(backend.currentUsername());
                for (const Post& p : newPosts) {
                    QString text = QString::fromStdString(p.postID + " : " + (p.content.size() > 80 ? p.content.substr(0,80) + "..." : p.content));
                    QListWidgetItem* it = new QListWidgetItem(text);
                    it->setData(Qt::UserRole, QString::fromStdString(p.postID));
                    myPostsList->addItem(it);
                }
            } else {
                QMessageBox::warning(this, "Edit", "Failed to edit post.");
            }
        }
    });

    connect(deleteBtn, &QPushButton::clicked, this, [this, myPostsList]() {
        auto sel = myPostsList->selectedItems();
        if (sel.empty()) { QMessageBox::information(this, "Delete", "Select a post first."); return; }
        QString pid = sel.first()->data(Qt::UserRole).toString();
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete Post", "Are you sure you want to delete this post?", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            if (backend.deletePost(pid.toStdString())) {
                QMessageBox::information(this, "Delete", "Post deleted.");
                populateFeed();
                delete sel.first();
            } else {
                QMessageBox::warning(this, "Delete", "Failed to delete post.");
            }
        }
    });

    connect(viewBtn, &QPushButton::clicked, this, [this, myPostsList]() {
        auto sel = myPostsList->selectedItems();
        if (sel.empty()) { QMessageBox::information(this, "View", "Select a post first."); return; }
        QString pid = sel.first()->data(Qt::UserRole).toString();
        int likes = backend.getLikeCount(pid.toStdString());
        vector<Comment> comments = backend.getComments(pid.toStdString());
        QString out = QString::number(likes) + " likes\n\n";
        if (comments.empty()) out += "No comments.";
        else {
            for (const Comment& c : comments) out += QString::fromStdString("[" + c.authorUsername + "]: " + c.content + "\n");
        }
        QMessageBox::information(this, "Likes & Comments for " + pid, out);
    });

    connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

    dlg.exec();
}

void MainWindow::showFriendsDialog() {
    QDialog friendsDlg(this);
    friendsDlg.setWindowTitle("My Friends List");
    friendsDlg.setMinimumWidth(300);
    QVBoxLayout* layout = new QVBoxLayout();

    QListWidget* friendsListWidget = new QListWidget();

    auto refreshList = [&]() {
        friendsListWidget->clear();
        vector<string> friends = backend.getFriendList();
        if (friends.empty()) {
            friendsListWidget->addItem("No friends added yet.");
        } else {
            for (const string& f : friends) {
                friendsListWidget->addItem(QString::fromStdString(f));
            }
        }
    };

    refreshList();

    QPushButton* deleteFriendBtn = new QPushButton("Delete Selected Friend");
    QPushButton* closeBtn = new QPushButton("Close");

    layout->addWidget(friendsListWidget);
    layout->addWidget(deleteFriendBtn);
    layout->addWidget(closeBtn);
    friendsDlg.setLayout(layout);

    connect(deleteFriendBtn, &QPushButton::clicked, this, [&]() {
        QListWidgetItem* item = friendsListWidget->currentItem();
        if (!item) {
            QMessageBox::warning(&friendsDlg, "Selection", "Please select a friend to delete.");
            return;
        }
        QString friendName = item->text();
        if (friendName == "No friends added yet.") return;

        QMessageBox::StandardButton reply = QMessageBox::question(&friendsDlg, "Delete Friend",
                                                                  "Are you sure you want to remove " + friendName + " from friends?", QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            if (backend.removeFriend(friendName.toStdString())) {
                QMessageBox::information(&friendsDlg, "Removed", "Friend removed.");
                refreshList();
                populateFeed();
            } else {
                QMessageBox::warning(&friendsDlg, "Error", "Failed to remove friend.");
            }
        }
    });

    connect(closeBtn, &QPushButton::clicked, &friendsDlg, &QDialog::accept);

    friendsDlg.exec();
}
