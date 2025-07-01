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
#include "MainWindow.hpp"
#include "./ui_MainWindow.h"

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

    bool dark = (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark);

    //! Устанавливаем суффикс для иконок в зависимости от цветовой схемы
    const QString suffix = (scheme == Qt::ColorScheme::Dark ? QStringLiteral("_dark")
                                                            : QStringLiteral("_light"));

    QList<QPair<QPushButton *, QString>> icons = {{this->ui_->filePushButton,
                                                   QLatin1String("upload")},
                                                  {this->ui_->urlPushButton,
                                                   QLatin1String("cloud_download")}};

    for (auto &p : icons)
        p.first->setIcon(QIcon(QLatin1String(":/icons/") + p.second + suffix + "-32.svg"));
}
#endif

void MainWindow::openLocalFile()
{
    //! Открытие диалогового окна для выбора файла
    const QString filePath = QFileDialog::getOpenFileName(this,
                                                          tr("Открыть TLE файл"),
                                                          "",
                                                          tr("TLE файлы (*.txt *.tle)"));
    this->tleParser_->loadFromFile(filePath); //! Если пользователь выбрал файл
}

void MainWindow::openUrl()
{
    QInputDialog dlg(this);                      //! Создание диалогового окна для ввода URL
    dlg.setWindowTitle(tr("Введите ссылку"));    //! Установка заголовка окна
    dlg.setLabelText(tr("Ссылка на TLE‑файл:")); //! Установка текста метки
    dlg.setMinimumWidth(500);                    //! Установка минимальной ширины окна
    dlg.resize(600, dlg.height());               //! Установка размера окна
    //! Если пользователь нажал "ОК" в диалоговом окне
    if (dlg.exec() == QDialog::Accepted) {
        QString urlPath = dlg.textValue(); //! Получение введённого URL
        //! Проверка на пустое поле URL
        if (urlPath.isEmpty()) {
            this->showError(tr("Пустое поле URL")); //! Если поле пустое, показываем ошибку
            return;
        }
        QUrl url(urlPath); //! Создание объекта QUrl из введённой строки
        //! Проверка валидности URL
        if (!url.isValid()) {
            this->showError(tr("Неверный URL")); //! Если URL невалиден, показываем ошибку
            return;
        }
        this->tleParser_->loadFromUrl(url); //! Загрузка данных TLE из указанного URL
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

    //! Устанавливаем атрибут для автоматического удаления окна при закрытии
    infoWindow_->setAttribute(Qt::WA_DeleteOnClose);
    this->hide();        //! Скрываем главное окно при открытии окна с информацией
    infoWindow_->show(); //! Показываем окно с информацией о спутниках
}

void MainWindow::bindActions()
{
    //! Подключаем сигнал ошибки к слоту showError
    this->connect(this->tleParser_, &TleParser::errorOccurred, this, &MainWindow::showError);
    //! Подключаем сигнал завершения парсинга к слоту showInfoWindow
    this->connect(this->tleParser_, &TleParser::parsingFinished, this, [this]() {
        this->showInfoWindow(this->tleParser_->records());
    });

    //! Подключаем сигнал к кнопке "filePushButton"
    this->connect(this->ui_->filePushButton,
                  &QPushButton::clicked,
                  this,
                  &MainWindow::openLocalFile);

    //! Подключаем сигнал к кнопке "UrlPushButton"
    this->connect(this->ui_->urlPushButton, &QPushButton::clicked, this, &MainWindow::openUrl);
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

    this->bindActions(); //! Связываем действия с соответствующими слотами
}

MainWindow::~MainWindow()
{
    delete this->ui_;        //! Освобождаем ресурсы пользовательского интерфейса
    delete this->tleParser_; //! Освобождаем ресурсы парсера TLE
}
