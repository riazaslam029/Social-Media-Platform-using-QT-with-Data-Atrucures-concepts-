#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QStackedWidget>

#include "backend_social_media.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onLogoutClicked();
    void onCreatePostClicked();
    void onPostSelected();
    void onLikeClicked();
    void onAddCommentClicked();
    void onViewCommentsClicked();
    void onAddFriendClicked();
    void onSuggestFriendsClicked();
    void onSearchUserClicked();
    void updateUiForAuth();

    // My Profile
    void onMyProfileClicked();

private:
    SocialMediaSystem backend;

    // Navigation & Container
    QStackedWidget* stackedWidget;
    QWidget* loginPage;
    QWidget* mainPage;

    // Authentication widgets (Page 1)
    QLineEdit* usernameEdit;
    QLineEdit* passwordEdit;
    QPushButton* loginBtn;
    QPushButton* registerBtn;

    // Main App widgets (Page 2)
    QPushButton* logoutBtn;
    QListWidget* feedList;
    QLabel* postDetailLabel;
    QPushButton* likeBtn;
    QPushButton* addCommentBtn;
    QPushButton* viewCommentsBtn;

    // Action widgets
    QPushButton* createPostBtn;
    QPushButton* addFriendBtn;
    QPushButton* suggestFriendsBtn;
    QPushButton* searchUserBtn;
    QPushButton* myProfileBtn;

    // Helper functions
    void populateFeed();
    QString selectedPostID() const;
    void showFriendsDialog();
};

#endif // MAINWINDOW_H
