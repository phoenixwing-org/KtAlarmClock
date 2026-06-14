#include "KtAlarmClockCmd.h"
#include "KtAlarmClockSettings.h"
#include "KtTranslationManager.h"

#include <QApplication>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QQuickStyle>
#include <QString>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

class KtSingleInstanceGuard {
public:
    KtSingleInstanceGuard() {
#ifdef _WIN32
        const QString mutexName = QStringLiteral("Local\\") + QStringLiteral(PROJECT_DOMAIN)
                                  + QStringLiteral(".SingleInstance");
        ::SetLastError(ERROR_SUCCESS);
        mutex_ = ::CreateMutexW(nullptr, TRUE, reinterpret_cast<LPCWSTR>(mutexName.utf16()));
        const DWORD lastError = ::GetLastError();
        alreadyRunning_ = lastError == ERROR_ALREADY_EXISTS || lastError == ERROR_ACCESS_DENIED;
#endif
    }

    ~KtSingleInstanceGuard() {
#ifdef _WIN32
        if (mutex_) {
            if (!alreadyRunning_)
                ::ReleaseMutex(mutex_);
            ::CloseHandle(mutex_);
        }
#endif
    }

    bool alreadyRunning() const {
        return alreadyRunning_;
    }

private:
    bool alreadyRunning_ = false;
#ifdef _WIN32
    HANDLE mutex_ = nullptr;
#endif
};

} // namespace

int main(int argc, char* argv[]) {
    QCoreApplication::setOrganizationName(QStringLiteral(PROJECT_COMPANY));
    QCoreApplication::setOrganizationDomain(QStringLiteral(PROJECT_DOMAIN));
    QCoreApplication::setApplicationName(QStringLiteral(PROJECT_APP_NAME));
    QCoreApplication::setApplicationVersion(QStringLiteral(PROJECT_VERSION));
    QGuiApplication::setApplicationDisplayName(QStringLiteral(PROJECT_APP_DISPLAY_NAME));

    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QQuickStyle::setStyle(QStringLiteral("Basic"));

    KtSingleInstanceGuard singleInstance;
    if (singleInstance.alreadyRunning())
        return 0;

    QApplication          app(argc, argv);
    KtTranslationManager translations;
    KtAlarmClockCmd      cmd;

    translations.install(KtAlarmClockSettings::read_locale());

    cmd.setTranslationManager(&translations);
    cmd.setExePath(argv[0]);

    const int code = cmd.build();
    if (code)
        QCoreApplication::exit(-1);

    return app.exec();
}
