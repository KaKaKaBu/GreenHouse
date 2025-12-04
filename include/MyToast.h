//
// Created by zhu on 2025/12/4.
//

#ifndef GREENHOUSEAPP_MYTOAST_H
#define GREENHOUSEAPP_MYTOAST_H
#include "Toast.h"
#include <QString>
#include <QWidget>

class MyToast
{
public:
    static void info(const QString& text,
                     const QString& title = "提示",
                     QWidget* parent = nullptr)
    {
        showPreset(text, title, ToastPreset::INFORMATION, parent);
    }

    static void success(const QString& text,
                        const QString& title = "成功",
                        QWidget* parent = nullptr)
    {
        showPreset(text, title, ToastPreset::SUCCESS, parent);
    }

    static void warning(const QString& text,
                        const QString& title = "警告",
                        QWidget* parent = nullptr)
    {
        showPreset(text, title, ToastPreset::WARNING, parent);
    }

    static void error(const QString& text,
                      const QString& title = "错误",
                      QWidget* parent = nullptr)
    {
        showPreset(text, title, ToastPreset::ERROR, parent);
    }

private:
    static void showPreset(const QString& text,
                           const QString& title,
                           ToastPreset preset,
                           QWidget* parent)
    {
        Toast* toast = new Toast(parent);
        toast->setTitle(title);
        toast->setText(text);
        toast->applyPreset(preset);
        toast->setDuration(3000);
        toast->show();
    }
};
#endif //GREENHOUSEAPP_MYTOAST_H
