// Phiên bản: 1.0 (Mới)
// Vị trí: /src/services/taskbarmanager.cpp
// Mô tả: Triển khai logic tích hợp Taskbar sử dụng COM ITaskbarList3.

#include "taskbarmanager.h"
#include <QWidget>
#include <QDebug>

TaskbarManager::TaskbarManager(QWidget* parentWindow) : QObject(parentWindow)
{
    if (!parentWindow) return;

    m_hwnd = reinterpret_cast<HWND>(parentWindow->winId());

    HRESULT hr = CoInitialize(NULL);
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_taskbarList));
        if (FAILED(hr)) {
            qWarning() << "Failed to create ITaskbarList3 instance.";
            m_taskbarList = nullptr;
        }
    } else {
        qWarning() << "Failed to initialize COM.";
    }
}

TaskbarManager::~TaskbarManager()
{
    if (m_taskbarList) {
        m_taskbarList->Release();
    }
    CoUninitialize();
}

void TaskbarManager::setProgressState(TBPFLAG state)
{
    if (m_taskbarList && m_hwnd) {
        m_taskbarList->SetProgressState(m_hwnd, state);
    }
}

void TaskbarManager::setProgressValue(ULONGLONG completed, ULONGLONG total)
{
    if (m_taskbarList && m_hwnd) {
        m_taskbarList->SetProgressValue(m_hwnd, completed, total);
    }
}
