// Phiên bản: 1.0 (Mới)
// Vị trí: /src/services/taskbarmanager.h
// Mô tả: Lớp Wrapper để quản lý ITaskbarList3 cho tích hợp Taskbar Windows.

#pragma once

#include <QObject>
#include <qt_windows.h>
#include <shobjidl.h>

class TaskbarManager : public QObject
{
    Q_OBJECT
public:
    explicit TaskbarManager(QWidget* parentWindow);
    ~TaskbarManager();

    void setProgressState(TBPFLAG state);
    void setProgressValue(ULONGLONG completed, ULONGLONG total);

private:
    ITaskbarList3* m_taskbarList = nullptr;
    HWND m_hwnd = nullptr;
};
