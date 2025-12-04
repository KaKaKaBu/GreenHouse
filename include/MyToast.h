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
        toast->setMinimumHeight(90);
        toast->setMinimumWidth(450);
        // toast->setTitleFont(QFont("Arial", 20, QFont::Weight::Bold));
        // toast->setTextFont(QFont("Arial", 13));

        toast->show();
    }
};
#endif //GREENHOUSEAPP_MYTOAST_H
