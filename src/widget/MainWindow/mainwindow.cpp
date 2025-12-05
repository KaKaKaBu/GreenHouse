#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../RealTimeDate/realtimedate.h"
#include "../HistoryData/test.h"
#include "../Login/login.h"
#include "../UserInfo/userinfo.h"
#include "../HomePage/homepage.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QStackedWidget>
#include <QTimer>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QFont>
#include <QIcon>
#include <QMap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_sidebar(nullptr)
    , m_sidebarLayout(nullptr)
    , m_logoLabel(nullptr)
    , m_navMenu(nullptr)
    , m_navLayout(nullptr)
    , m_userInfoFrame(nullptr)
    , m_contentArea(nullptr)
    , m_contentLayout(nullptr)
    , m_headerLayout(nullptr)
    , m_pageTitle(nullptr)
    , m_timeLabel(nullptr)
    , m_refreshBtn(nullptr)
    , m_stackedWidget(nullptr)
    , m_login(nullptr)
    , m_homePage(nullptr)
    , m_realTimeDate(nullptr)
    , m_historyData(nullptr)
    , m_userInfoPage(nullptr)
    , m_currentPageIndex(0)
    , m_timeTimer(nullptr)
    , m_isLoggedIn(false)
{
    ui->setupUi(this);
    
    // 设置窗口属性
    setWindowTitle("农业环境监控系统");
    setMinimumSize(1000, 700);
    resize(1400, 900);
    
    // 初始化 UI
    setupUI();
    
    // 加载样式表
    loadStyleSheet();
    
    // 初始化登录状态（未登录）
    setLoginState(false);
    
    // 初始化页面（默认显示登录页面）
    switchToPage(0);  // 0 = Login
    
    qDebug() << "✅ MainWindow 初始化完成（默认显示登录页面）";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // 创建主布局（水平布局：侧边栏 + 内容区）
    QWidget* centralWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    setCentralWidget(centralWidget);
    
    // 创建侧边栏
    setupSidebar();
    mainLayout->addWidget(m_sidebar);
    
    // 创建主内容区
    setupContentArea();
    mainLayout->addWidget(m_contentArea, 1);  // 拉伸因子为1
    
    // 设置导航
    setupNavigation();
    connect(m_login,&Login::snedUserInfo,m_userInfoPage,&UserInfo::setCurrentUser);
    connect(m_userInfoPage,&UserInfo::logOut,this,&MainWindow::setLoginState);
    connect(m_realTimeDate,&RealTimeDate::sensorDataReceived,m_homePage,&HomePage::updateEnvironmentData);
    connect(this->m_refreshBtn,&QPushButton::clicked,this,&MainWindow::sendGetData);
    connect(this,&MainWindow::sendGetData,m_realTimeDate,&RealTimeDate::on_RefreshClicked);

}

void MainWindow::setupSidebar()
{
    // 创建侧边栏容器
    m_sidebar = new QWidget(this);
    m_sidebar->setFixedWidth(240);
    m_sidebar->setObjectName("sidebar");
    
    // 创建侧边栏布局
    m_sidebarLayout = new QVBoxLayout(m_sidebar);
    m_sidebarLayout->setContentsMargins(0, 0, 0, 0);
    m_sidebarLayout->setSpacing(0);
    
    // Logo 区域
    m_logoLabel = new QLabel(m_sidebar);
    m_logoLabel->setText("🌱 农业监控系统");
    m_logoLabel->setObjectName("logo");
    m_logoLabel->setAlignment(Qt::AlignCenter);
    QFont logoFont("Microsoft YaHei", 18, QFont::Bold);
    m_logoLabel->setFont(logoFont);
    m_logoLabel->setStyleSheet("color: white; padding: 25px 20px; border-bottom: 1px solid rgba(255, 255, 255, 0.1);");
    m_sidebarLayout->addWidget(m_logoLabel);
    
    // 导航菜单
    m_navMenu = new QFrame(m_sidebar);
    m_navMenu->setObjectName("navMenu");
    m_navLayout = new QVBoxLayout(m_navMenu);
    m_navLayout->setContentsMargins(0, 20, 0, 20);
    m_navLayout->setSpacing(0);
    m_sidebarLayout->addWidget(m_navMenu, 1);  // 拉伸因子为1
    
    // 用户信息区域
    m_userInfoFrame = new QFrame(m_sidebar);
    m_userInfoFrame->setObjectName("userInfo");
    m_userInfoFrame->setFixedHeight(100);
    QVBoxLayout* userLayout = new QVBoxLayout(m_userInfoFrame);
    userLayout->setContentsMargins(20, 15, 20, 15);
    
    QLabel* userAvatar = new QLabel("👤", m_userInfoFrame);
    userAvatar->setAlignment(Qt::AlignCenter);
    userAvatar->setStyleSheet("font-size: 24px; background-color: #4fc3f7; border-radius: 20px; width: 40px; height: 40px;");
    userAvatar->setFixedSize(40, 40);
    
    QLabel* userName = new QLabel("管理员", m_userInfoFrame);
    userName->setStyleSheet("color: white; font-weight: bold; font-size: 14px;");
    
    QLabel* userRole = new QLabel("系统管理员", m_userInfoFrame);
    userRole->setStyleSheet("color: rgba(255, 255, 255, 0.7); font-size: 12px;");
    
    QHBoxLayout* userInfoLayout = new QHBoxLayout();
    userInfoLayout->addWidget(userAvatar);
    QVBoxLayout* userTextLayout = new QVBoxLayout();
    userTextLayout->addWidget(userName);
    userTextLayout->addWidget(userRole);
    userInfoLayout->addLayout(userTextLayout);
    
    userLayout->addLayout(userInfoLayout);
    m_userInfoFrame->setStyleSheet("border-top: 1px solid rgba(255, 255, 255, 0.1);");
    
    m_sidebarLayout->addWidget(m_userInfoFrame);
}

void MainWindow::setupContentArea()
{
    // 创建主内容区（支持窗口大小调整）
    m_contentArea = new QWidget(this);
    m_contentArea->setObjectName("contentArea");
    m_contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    m_contentLayout = new QVBoxLayout(m_contentArea);
    m_contentLayout->setContentsMargins(20, 20, 20, 20);
    m_contentLayout->setSpacing(20);
    
    // 创建头部（标题 + 时间 + 刷新按钮）
    QFrame* header = new QFrame(m_contentArea);
    header->setObjectName("header");
    header->setFixedHeight(60);
    m_headerLayout = new QHBoxLayout(header);
    m_headerLayout->setContentsMargins(0, 0, 0, 0);
    
    m_pageTitle = new QLabel("农业环境监控仪表板", header);
    m_pageTitle->setObjectName("pageTitle");
    QFont titleFont("Microsoft YaHei", 24, QFont::Bold);
    m_pageTitle->setFont(titleFont);
    m_pageTitle->setStyleSheet("color: #1a3a8f;");
    
    m_headerLayout->addWidget(m_pageTitle);
    m_headerLayout->addStretch();
    
    // 时间显示
    m_timeLabel = new QLabel(header);
    m_timeLabel->setObjectName("timeLabel");
    m_timeLabel->setStyleSheet(
        "background-color: white; padding: 8px 15px; border-radius: 20px; "
        "box-shadow: 0 3px 10px rgba(0, 120, 0, 0.05); "
        "font-weight: 600; color: #2a5bd7; font-size: 14px;"
    );
    m_headerLayout->addWidget(m_timeLabel);
    
    // 刷新按钮
    m_refreshBtn = new QPushButton("刷新数据", header);
    m_refreshBtn->setObjectName("refreshBtn");
    m_refreshBtn->setStyleSheet(
        "background-color: #2a5bd7; color: white; border: none; "
        "padding: 8px 20px; border-radius: 6px; font-size: 14px;"
        "QPushButton:hover { background-color: #1a3a8f; }"
    );
    m_headerLayout->addWidget(m_refreshBtn);
    
    header->setStyleSheet("border-bottom: 1px solid #e0e7ff; padding-bottom: 15px;");
    m_contentLayout->addWidget(header);
    
    // 创建页面堆叠（支持窗口大小调整）
    m_stackedWidget = new QStackedWidget(m_contentArea);
    m_stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_contentLayout->addWidget(m_stackedWidget, 1);  // 拉伸因子为1，支持自适应
    
    // 创建子页面（注意顺序：Login 作为第一个，HomePage 作为登录后的首页）
    m_login = new Login(this);
    m_stackedWidget->addWidget(m_login);  // 索引 0
    
    m_homePage = new HomePage(this);
    m_homePage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_stackedWidget->addWidget(m_homePage);  // 索引 1 - 首页
    
    m_realTimeDate = new RealTimeDate(this);
    m_realTimeDate->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_stackedWidget->addWidget(m_realTimeDate);  // 索引 2
    
    m_historyData = new test(this);
    m_historyData->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_stackedWidget->addWidget(m_historyData);  // 索引 3
    
    m_userInfoPage = new UserInfo(this);
    m_userInfoPage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_stackedWidget->addWidget(m_userInfoPage);  // 索引 4
    
    // 连接登录页面的信号
    connect(m_login, &Login::loginSuccess, this, &MainWindow::onLoginSuccess);
    connect(m_login, &Login::logout, this, &MainWindow::onLogout);

    
    // 启动时间更新定时器
    m_timeTimer = new QTimer(this);
    connect(m_timeTimer, &QTimer::timeout, this, &MainWindow::updateTimeDisplay);
    m_timeTimer->start(1000);  // 每秒更新
    updateTimeDisplay();  // 立即更新一次
}

void MainWindow::setupNavigation()
{
    // 创建导航按钮（页面索引：0=登录, 1=首页, 2=实时数据, 3=历史数据, 4=用户管理）
    QStringList navItems = {
        "🔐 登录",
        "🏠 首页",
        "📊 实时数据",
        "📈 历史数据",
        "👥 用户管理"
    };
    
    // 页面索引映射（导航按钮索引 -> 堆叠页面索引）
    QList<int> pageIndices = {0, 1, 2, 3, 4};  // 登录, 首页, 实时数据, 历史数据, 用户管理
    
    for (int i = 0; i < navItems.size(); ++i) {
        QPushButton* btn = createNavButton("", navItems[i], pageIndices[i]);
        m_navButtons.append(btn);
        m_navLayout->addWidget(btn);
    }
    
    // 设置第一个按钮（登录）为激活状态
    if (!m_navButtons.isEmpty()) {
        m_navButtons[0]->setProperty("active", true);
        m_navButtons[0]->style()->unpolish(m_navButtons[0]);
        m_navButtons[0]->style()->polish(m_navButtons[0]);
    }
    
    // 初始状态：只有登录按钮可用
    updateNavigationButtons();
}

QPushButton* MainWindow::createNavButton(const QString& icon, const QString& text, int pageIndex)
{
    QPushButton* btn = new QPushButton(text, m_navMenu);
    btn->setObjectName("navButton");
    btn->setProperty("pageIndex", pageIndex);
    btn->setProperty("active", false);
    btn->setStyleSheet(
        "QPushButton {"
        "    text-align: left; padding: 15px 25px; border: none; "
        "    background-color: transparent; color: white; font-size: 14px; "
        "    border-left: 4px solid transparent;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(255, 255, 255, 0.1); "
        "    border-left-color: #4fc3f7;"
        "}"
        "QPushButton[active=true] {"
        "    background-color: rgba(79, 195, 247, 0.2); "
        "    border-left-color: #4fc3f7;"
        "}"
    );
    
    connect(btn, &QPushButton::clicked, this, &MainWindow::onNavItemClicked);
    
    return btn;
}

void MainWindow::onNavItemClicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    
    int pageIndex = btn->property("pageIndex").toInt();
    switchToPage(pageIndex);
    emit changePage(pageIndex);
}

void MainWindow::switchToPage(int index)
{
    if (index < 0 || index >= m_stackedWidget->count()) {
        qWarning() << "无效的页面索引:" << index;
        return;
    }
    
    // 更新导航按钮状态（根据按钮的 pageIndex 属性判断）
    for (int i = 0; i < m_navButtons.size(); ++i) {
        int btnPageIndex = m_navButtons[i]->property("pageIndex").toInt();
        bool isActive = (btnPageIndex == index);
        m_navButtons[i]->setProperty("active", isActive);
        m_navButtons[i]->style()->unpolish(m_navButtons[i]);
        m_navButtons[i]->style()->polish(m_navButtons[i]);
    }
    
    // 切换页面
    m_stackedWidget->setCurrentIndex(index);
    m_currentPageIndex = index;

    
    // 更新页面标题
    QMap<int, QString> titles;
    titles[0] = "用户登录";
    titles[1] = "农业环境监控仪表板";
    titles[2] = "实时数据监控";
    titles[3] = "历史数据查询";
    titles[4] = "用户管理";
    
    if (titles.contains(index)) {
        m_pageTitle->setText(titles[index]);
    } else {
        m_pageTitle->setText("农业环境监控系统");
    }
    
    // 如果是登录页面，隐藏刷新按钮
    if (index == 0) {
        m_refreshBtn->setVisible(false);
    } else {
        m_refreshBtn->setVisible(true);
    }
    
    qDebug() << "✅ 切换到页面:" << index << titles[index];
}

void MainWindow::updateTimeDisplay()
{
    QDateTime now = QDateTime::currentDateTime();
    QString timeStr = now.toString("yyyy年MM月dd日 dddd hh:mm:ss");
    m_timeLabel->setText(timeStr);
}

void MainWindow::updateDate()
{
    emit sendGetData();
}

void MainWindow::setLoginState(bool isLoggedIn)
{
    m_isLoggedIn = isLoggedIn;
    updateNavigationButtons();
    
    if (isLoggedIn) {
        qDebug() << "✅ 用户已登录，启用所有导航按钮";
        // 登录成功后切换到首页（HomePage）
        switchToPage(1);  // 首页
        // 隐藏登录页面
        m_login->hide();

    } else {
        qDebug() << "⚠️ 用户未登录，禁用导航按钮";
        // 未登录时切换到登录页面
        switchToPage(0);  // 登录页面
        // 显示登录页面
        m_login->show();

    }
}

void MainWindow::updateNavigationButtons()
{
    // 登录按钮始终可用
    // 其他按钮：登录后可用，未登录时禁用
    for (int i = 0; i < m_navButtons.size(); ++i) {
        QPushButton* btn = m_navButtons[i];
        int pageIndex = btn->property("pageIndex").toInt();
        
        if (pageIndex == 0) {
            // 登录按钮始终可用
            btn->setEnabled(true);
        } else {
            // 其他按钮根据登录状态
            btn->setEnabled(m_isLoggedIn);
            
            // 如果禁用，添加禁用样式
            if (!m_isLoggedIn) {
                btn->setStyleSheet(
                    "QPushButton {"
                    "    text-align: left; padding: 15px 25px; border: none; "
                    "    background-color: transparent; color: rgba(255, 255, 255, 0.4); "
                    "    font-size: 14px; border-left: 4px solid transparent;"
                    "}"
                    "QPushButton:hover {"
                    "    background-color: transparent;"
                    "}"
                );
            } else {
                // 恢复正常样式
                btn->setStyleSheet(
                    "QPushButton {"
                    "    text-align: left; padding: 15px 25px; border: none; "
                    "    background-color: transparent; color: white; font-size: 14px; "
                    "    border-left: 4px solid transparent;"
                    "}"
                    "QPushButton:hover {"
                    "    background-color: rgba(255, 255, 255, 0.1); "
                    "    border-left-color: #4fc3f7;"
                    "}"
                    "QPushButton[active=true] {"
                    "    background-color: rgba(79, 195, 247, 0.2); "
                    "    border-left-color: #4fc3f7;"
                    "}"
                );
            }
        }
    }
}

void MainWindow::onLoginSuccess()
{
    setLoginState(true);
}

void MainWindow::onLogout()
{
    setLoginState(false);
}

void MainWindow::loadStyleSheet()
{
    QFile styleFile(":/widget/MainWindow/mainwindow.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        this->setStyleSheet(styleSheet);
        styleFile.close();
        qDebug() << "✅ MainWindow 样式表加载成功";
    } else {
        // 如果资源文件不存在，使用内联样式
        QString styleSheet = R"(
            #sidebar {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
                    stop:0 #1a3a8f, stop:1 #2a5bd7);
                color: white;
            }
            #contentArea {
                background-color: #f0f5ff;
            }
        )";
        this->setStyleSheet(styleSheet);
        qDebug() << "⚠️ 使用内联样式表";
    }
}
