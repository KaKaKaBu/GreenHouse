#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QTimer>

// 前向声明
class RealTimeDate;
class test;  // HistoryData
class Login;
class UserInfo;
class HomePage;

namespace Ui {
class MainWindow;
}

/**
 * @brief 主控大屏 - 参考 HTML 蓝色主题风格
 * 
 * 功能：
 * - 侧边栏导航（类似 HTML 风格）
 * - 页面切换（RealTimeDate, HistoryData, Login, UserInfo）
 * - 用户信息显示
 * - 实时时间显示
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // 导航菜单点击
    void onNavItemClicked();
    
    // 切换到指定页面
    void switchToPage(int index);
    
    // 更新时间显示
    void updateTimeDisplay();

private:
    // 初始化函数
    void setupUI();
    void setupSidebar();
    void setupContentArea();
    void setupNavigation();
    void loadStyleSheet();
    
    // 创建导航按钮
    QPushButton* createNavButton(const QString& icon, const QString& text, int pageIndex);
    
    // UI 组件
    Ui::MainWindow *ui;
    
    // 侧边栏组件
    QWidget* m_sidebar;
    QVBoxLayout* m_sidebarLayout;
    QLabel* m_logoLabel;
    QFrame* m_navMenu;
    QVBoxLayout* m_navLayout;
    QFrame* m_userInfoFrame;  // 用户信息框架
    
    // 主内容区
    QWidget* m_contentArea;
    QVBoxLayout* m_contentLayout;
    QHBoxLayout* m_headerLayout;
    QLabel* m_pageTitle;
    QLabel* m_timeLabel;
    QPushButton* m_refreshBtn;
    
    // 页面堆叠
    QStackedWidget* m_stackedWidget;
    
    // 子页面
    Login* m_login;
    HomePage* m_homePage;
    RealTimeDate* m_realTimeDate;
    test* m_historyData;
    UserInfo* m_userInfoPage;  // 避免与成员变量 m_userInfo 冲突
    
    // 导航按钮列表
    QList<QPushButton*> m_navButtons;
    int m_currentPageIndex;
    
    // 定时器
    QTimer* m_timeTimer;
    
    // 登录状态
    bool m_isLoggedIn;
    
    // 登录状态管理
    void setLoginState(bool isLoggedIn);
    void updateNavigationButtons();
    
private slots:
    // 登录成功回调
    void onLoginSuccess();
    // 登出回调
    void onLogout();
 signals:
    void changePage(int index);
};

#endif // MAINWINDOW_H
