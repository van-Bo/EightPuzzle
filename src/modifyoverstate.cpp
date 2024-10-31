#include "modifyoverstate.h"
#include "ui_modifyoverstate.h"
#include <QTimer>
#include <QMessageBox>

modifyOverState::modifyOverState(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::modifyOverState)
{
    ui->setupUi(this);

    this->setStyleSheet(
        "QLineEdit {"
        "    font-size: 16px;"  // 设置字体大小
        "    padding: 5px;"     // 设置内边距，使文本框更宽
        "    border: 2px solid #8f8f91;"  // 设置边框颜色和宽度
        "    border-radius: 10px;"  // 设置圆角
        "    background: #f0f0f0;"  // 设置背景颜色
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #00aaff;"  // 设置聚焦时的边框颜色
        "}"
        "QLineEdit:placeholder {"
        "    color: #a9a9a9;"  // 设置 placeholder 文本颜色
        "    font-style: italic;"  // 设置 placeholder 文本样式
        "}"
    );

    // 取消输入框的自动选中
    QTimer::singleShot(0, this, [this]() {
        ui->input->clearFocus();
    });
    // 设置 buttonBox 为默认焦点
    QTimer::singleShot(0, this, [this]() {
        ui->buttonBox->setFocus();
    });

    // 确保按钮的点击事件正确连接到槽函数
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &modifyOverState::onAccepted_clicked);
}

modifyOverState::~modifyOverState()
{
    delete ui;
}

bool modifyOverState::validateInput(const QString &input) {
    if (input.length() != 9) {
        QMessageBox::warning(this, "输入错误",
                             "<h2 style='font-family:Arial; font-size:18px; color:red;'>输入长度错误</h2>"
                             "<p style='font-family:Segoe UI; font-size:16px; color:black;'>输入长度必须为 9。</p>");
        return false;
    }

    QSet<QChar> uniqueChars;
    for (QChar ch : input) {
        if ((ch >= '1' && ch <= '8') || ch == 'x') {
            uniqueChars.insert(ch);
        } else {
            QMessageBox::warning(this, "输入错误",
                                 "<h2 style='font-family:Arial; font-size:18px; color:red;'>字符错误</h2>"
                                 "<p style='font-family:Segoe UI; font-size:16px; color:black;'>输入必须包含字符 1 到 8 以及字符x。</p>");
            return false;
        }
    }

    if (uniqueChars.size() != 9) {
        QMessageBox::warning(this, "输入错误",
                             "<h2 style='font-family:Arial; font-size:18px; color:red;'>重复字符</h2>"
                             "<p style='font-family:Segoe UI; font-size:16px; color:black;'>输入中每个字符必须唯一。</p>");
        return false;
    }

    return true;
}


void modifyOverState::onAccepted_clicked()
{
    QString res = ui->input->text();
    if (validateInput(res)) {
        emit dataEntered(res);
        qDebug() << "Emitting signal with data:" << res;
        accept(); // 关闭对话框
    }
}
