//
// Created by zhu on 2025/12/4.
//

#ifndef GREENHOUSEAPP_MYTOAST_H
#define GREENHOUSEAPP_MYTOAST_H
#include "Toast.h"
#include <QString>
#include <QWidget>
#include <QMainWindow>
#include <QApplication>
#include <QScreen>

class MyToast
{
public:
    static void info(QWidget *parent, const QString &title,
         const QString &text)
    {
        showPreset(text, title, ToastPreset::INFORMATION, parent);
    }

    static void success(QWidget *parent, const QString &title,
         const QString &text)
    {
        showPreset(text, title, ToastPreset::SUCCESS, parent);
    }

    static void warning(QWidget *parent, const QString &title,
         const QString &text)
    {
        showPreset(text, title, ToastPreset::WARNING, parent);
    }

    static void error(QWidget *parent, const QString &title,
         const QString &text)
    {
        showPreset(text, title, ToastPreset::ERROR, parent);
    }

private:
    // 找到顶层窗口（MainWindow）
    static QWidget* findTopLevelWindow(QWidget* widget)
    {
        if (!widget) {
            // 如果没有提供 parent，使用当前活动的顶层窗口
            QWidget* activeWindow = QApplication::activeWindow();
            if (activeWindow) {
                return activeWindow;
            }
            return nullptr;
        }
        
        // 向上查找顶层窗口
        QWidget* topLevel = widget;
        while (topLevel->parentWidget() != nullptr) {
            topLevel = topLevel->parentWidget();
        }
        
        return topLevel;
    }
    
    static void showPreset(const QString& text,
                           const QString& title,
                           ToastPreset preset,
                           QWidget* parent)
    {
        // 找到顶层窗口作为 Toast 的 parent，确保位置计算正确
        QWidget* topLevelParent = findTopLevelWindow(parent);
        
        Toast* toast = new Toast(topLevelParent);
        toast->setTitle(title);
        toast->setText(text);
        toast->applyPreset(preset);
        toast->setDuration(3000);
        
        // 使用中文字体（支持中文显示，避免字体显示异常）
        QFont titleFont("Microsoft YaHei", 9, QFont::Bold);
        QFont textFont("Microsoft YaHei", 9);
        toast->setTitleFont(titleFont);
        toast->setTextFont(textFont);
        
        // 使用设置的字体计算文本宽度
        QFontMetrics fontMetrics(textFont);
        
        // 计算文本的实际宽度（考虑中文字符）
        int textWidth = fontMetrics.boundingRect(text).width();
        int titleWidth = fontMetrics.boundingRect(title).width();
        int maxTextWidth = qMax(textWidth, titleWidth);
        
        // 获取屏幕宽度
        QScreen* screen = nullptr;
        if (topLevelParent) {
            screen = topLevelParent->screen();
        } else {
            QWidget* activeWindow = QApplication::activeWindow();
            if (activeWindow) {
                screen = activeWindow->screen();
            }
        }
        if (!screen) {
            screen = QApplication::primaryScreen();
        }
        int screenWidth = screen ? screen->geometry().width() : 1920;
        
        // 计算需要的宽度：文本宽度 + 图标区域 + 边距 + 关闭按钮
        // 图标区域约 40px，边距约 70px（左右），关闭按钮约 34px（包含边距）
        int neededWidth = maxTextWidth + 40 + 70 + 34;
        
        // 设置最小宽度：至少 450px，但不超过屏幕宽度
        int minWidth = qMax(450, qMin(neededWidth, screenWidth - 100));
        
        // 设置最大宽度：接近屏幕宽度，确保文本不会因为宽度限制而换行
        // 保留 40px 边距，避免 Toast 超出屏幕
        int maxWidth = screenWidth - 40;
        
        // 设置最小高度：足够大，避免触发高度换行逻辑
        // 单行文本高度约 18px，加上边距约 36px，总计约 90px
        // 如果文本很长，设置为更大的高度
        int minHeight = 90;
        if (maxTextWidth > 400) {
            minHeight = 120;  // 长文本时增加最小高度
        }
        
        toast->setMinimumWidth(minWidth);
        toast->setMaximumWidth(maxWidth);
        toast->setMinimumHeight(minHeight);
        
        // 确保 Toast 显示在窗口右下角
        // Toast 会自动使用 parent 的几何形状计算位置
        
        toast->show();
    }
};
#endif //GREENHOUSEAPP_MYTOAST_H
