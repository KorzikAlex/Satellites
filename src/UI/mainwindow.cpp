/*!
 * \file mainwindow.cpp
 * \brief Исходный файл для главного окна приложения
 * \details
 * Этот файл содержит реализацию класса MainWindow,
 * который представляет главное окно приложения.
 * \author KorzikAlex
 * \copyright This project is released under the MIT License.
 * \date 2025
 */
#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
void MainWindow::changeEvent(QEvent *event)
{
    //! Обработка события смены темы
    if (event->type() == QEvent::ThemeChange)
        updateStyles();
    //! Вызов базовой реализации для обработки других событий
    QMainWindow::changeEvent(event);
}

void MainWindow::updateStyles()
{
    //! Получаем текущую цветовую схему
    const auto scheme = QGuiApplication::styleHints()->colorScheme();
    //! Устанавливаем иконки в зависимости от цветовой схемы
    const QString prefix = (scheme == Qt::ColorScheme::Dark ? QStringLiteral(":/dark_icons/")
                                                            : QStringLiteral(":/light_icons/"));
    //! Устанавливаем суффикс для иконок в зависимости от цветовой схемы
    const QString suffix = (scheme == Qt::ColorScheme::Dark ? QStringLiteral("_dark")
                                                            : QStringLiteral("_light"));
    //! Лямбда-функция для установки иконок на кнопки и действия
    auto applyIcon = [&](auto *obj, const QString &baseName, int size) {
        const QString path = prefix + baseName + suffix + QLatin1Char('-') + QString::number(size)
                             + QLatin1String(".svg");
        obj->setIcon(QIcon(path));
    };

    //! Применяем иконки к кнопкам и действиям для файла
    applyIcon(this->ui_->filePushButton, QStringLiteral("upload"), 32);
    applyIcon(this->ui_->fileAction, QStringLiteral("upload"), 24);
    //! Применяем иконки к кнопкам и действиям для URL
    applyIcon(this->ui_->urlPushButton, QStringLiteral("cloud_download"), 32);
    applyIcon(this->ui_->urlAction, QStringLiteral("cloud_download"), 24);
    //! Применяем иконку к кнопке "О программе"
    applyIcon(this->ui_->aboutAction, QStringLiteral("info"), 24);
}
#endif

void MainWindow::showAbout()
{
    //! Показываем сообщение "О программе"
    QMessageBox::about(this,
                       tr("О программе"),
                       tr("\"Спутники\" - это программа для работы с TLE-файлами."));
}

void MainWindow::openLocalFile()
{
    //! Открытие диалогового окна для выбора файла
    const QString filePath = QFileDialog::getOpenFileName(this,
                                                          tr("Открыть TLE файл"),
                                                          "",
                                                          tr("TLE файлы (*.txt *.tle)"));
    //! Если пользователь выбрал файл
    this->tleParser_->loadFromFile(filePath);
}

void MainWindow::openUrl()
{
    // //! Запрос URL у пользователя
    // const QString urlPath = QInputDialog::getText(this,
    //                                               tr("Введите ссылку"),
    //                                               tr("Ссылка на TLE-файл:"));
    QInputDialog dlg(this);
    dlg.setWindowTitle(tr("Введите ссылку"));
    dlg.setLabelText(tr("Ссылка на TLE‑файл:"));
    dlg.setMinimumWidth(500);      // тут настраиваете желаемую ширину
    dlg.resize(600, dlg.height()); // можно явно задать ширину
    if (dlg.exec() == QDialog::Accepted) {
        QString urlPath = dlg.textValue();
        if (urlPath.isEmpty())
            return;
        QUrl url(urlPath);
        if (!url.isValid()) {
            this->showError(tr("Неверный URL"));
            return;
        }
        //! loadFromUrl запускает асинхронный запрос, parsingFinished придёт позже
        this->tleParser_->loadFromUrl(url);
    }
}

void MainWindow::showError(const QString &message)
{
    //! Показываем сообщение об ошибке при загрузке данных
    QMessageBox::critical(this, tr("Ошибка"), message);
}

void MainWindow::showInfoWindow(const QVector<TleRecord> &records)
{
    //! Создание нового окна с информацией о спутниках
    InfoWindow *infoWindow_ = new InfoWindow(records);

    //! Подключаем сигнал для обработки открытия локального файла в окне InfoWindow
    this->connect(infoWindow_, &InfoWindow::requestOpenLocalFile, this, &MainWindow::openLocalFile);
    //! Подключаем сигнал для обработки открытия URL в окне InfoWindow
    this->connect(infoWindow_, &InfoWindow::requestOpenUrl, this, &MainWindow::openUrl);
    //! Подключаем сигнал для обработки копирования результатов в окне InfoWindow
    this->connect(infoWindow_, &InfoWindow::requestShowAbout, this, &MainWindow::showAbout);

    //! Устанавливаем атрибут для автоматического удаления окна при закрытии
    infoWindow_->setAttribute(Qt::WA_DeleteOnClose);
    this->hide();        //! Скрываем главное окно при открытии окна с информацией
    infoWindow_->show(); //! Показываем окно с информацией о спутниках
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
    , tleParser_(new TleParser(this))
{
    this->ui_->setupUi(this); //! Инициализация пользовательского интерфейса

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    this->updateStyles(); //! Обновление стилей приложения при смене темы
#endif

    connect(this->tleParser_, &TleParser::errorOccurred, this, &MainWindow::showError);
    connect(this->tleParser_, &TleParser::parsingFinished, this, [this]() {
        showInfoWindow(this->tleParser_->records());
    });

    //! Подключаем сигнал к кнопке "filePushButton"
    this->connect(this->ui_->filePushButton,
                  &QPushButton::clicked,
                  this,
                  &MainWindow::openLocalFile);

    //! Подключаем сигнал к действию "fileAction"
    this->connect(this->ui_->fileAction, &QAction::triggered, this, &MainWindow::openLocalFile);

    //! Подключаем сигнал к кнопке "UrlPushButton"
    this->connect(this->ui_->urlPushButton, &QPushButton::clicked, this, &MainWindow::openUrl);

    //! Подключаем сигнал к действию "UrlAction"
    this->connect(this->ui_->urlAction, &QAction::triggered, this, &MainWindow::openUrl);

    //! Создание справки
    this->connect(this->ui_->aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

MainWindow::~MainWindow()
{
    delete this->ui_; //! Освобождаем ресурсы пользовательского интерфейса
}
