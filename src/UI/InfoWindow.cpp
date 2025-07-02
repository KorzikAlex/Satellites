/*!
 * \file infowindow.cpp
 * \brief Исходный файл для окна информации о спутниках
 * \details
 * Этот файл содержит реализацию класса InfoWindow,
 * который представляет окно с информацией о спутниках из TLE-файлов.
 * \author KorzikAlex
 * \copyright This project is released under the MIT License.
 * \date 2025
 */
#include "InfoWindow.hpp"
#include "./ui_InfoWindow.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
void InfoWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::ThemeChange)
        updateStyles();              //! Обновляем стили при смене темы
    QMainWindow::changeEvent(event); //! Вызываем базовую реализацию для обработки других событий
}

void InfoWindow::updateStyles()
{
    //! Проверяем, темная ли тема
    bool dark = (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark);

    //! Устанавливаем суффикс для иконок в зависимости от цветовой схемы
    QString suffix = dark ? QStringLiteral("_dark") : QStringLiteral("_light");

    //! Устанавливаем иконки для действий в тулбаре
    QList<QPair<QAction *, QString>> actions = {
        {this->ui_->fileOpenAction, QLatin1String("upload")},
        {this->ui_->urlOpenAction, QLatin1String("cloud_download")},
        {this->ui_->copyAction, QLatin1String("copy_all")},
        {this->ui_->saveAction, QLatin1String("save")},
    };

    //! Проходим по всем действиям и устанавливаем иконки
    for (auto &p : actions)
        p.first->setIcon(QIcon(QLatin1String(":/icons/") + p.second + suffix + "-24.svg"));

    //! Устанавливаем стили для тулбара
    this->ui_->toolBar->setStyleSheet(R"(
    QToolBar {
        border: none;
        background: transparent;
    }

    QToolBar QToolButton, QToolBar QToolButton:hover, QToolBar QToolButton:pressed {
        color: palette(windowText);
    })");
}
#endif

void InfoWindow::saveResults()
{
    //! Открываем диалоговое окно для сохранения файла
    QString filePath = QFileDialog::getSaveFileName(this,
                                                    tr("Сохранить результаты"),
                                                    "tle.txt",
                                                    tr("Текстовый файл (*.txt)"));

    //! Проверяем, что путь не пустой
    if (filePath.isEmpty())
        return;

    QFile file(filePath); //! Открываем файл для записи

    //! Проверяем, что файл успешно открыт
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        //! Если файл не удалось открыть, показываем сообщение об ошибке
        emit errorOccurred(tr("Не удалось открыть файл для записи: %1").arg(filePath));
        return;
    }

    QTextStream out(&file); //! Записываем результаты в файл

    out << this->formattedResults(); //! Записываем отформатированные результаты

    file.close(); //! Закрываем файл после записи

    //! Показываем сообщение о сохранении в статусной строке
    this->statusBar()->showMessage(tr("Сохранено в файл: %1").arg(filePath), 2500);
}

void InfoWindow::copyResults()
{
    //! Копируем отформатированные результаты в буфер обмена
    QApplication::clipboard()->setText(this->formattedResults());
    //! Показываем сообщение о копировании в статусной строке
    this->statusBar()->showMessage(tr("Скопировано!"), 2500);
}

QString InfoWindow::formattedResults() const
{
    QString out; //! Строка для хранения отформатированных результатов
    //! Добавляем количество спутников
    out += tr("Количество спутников: %1\n").arg(this->stats_.records.size());
    //! Добавляем дату самых старых данных
    out += tr("Дата самых старых данных: %1\n").arg(this->ui_->inputDateLabel->text());
    out += tr("Количество запусков по годам:\n"); //! Заголовок для количества запусков по годам
    for (auto it = this->stats_.launchesPerYear.constBegin();
         it != this->stats_.launchesPerYear.constEnd();
         ++it)
        out += tr("%1: %2\n").arg(it.key()).arg(it.value()); //! Добавляем количество запусков по годам
    //! Заголовок для количества спутников по наклону
    out += tr("Количество спутников по наклону:\n");
    for (auto it = this->stats_.inclinationBins.constBegin();
         it != stats_.inclinationBins.constEnd();
         ++it)
        out += tr("%1°: %2\n")
                   .arg(it.key())
                   .arg(it.value()); //! Добавляем количество спутников по наклону
    return out;
}

QAbstractItemModel *InfoWindow::modelFromMap(const QMap<int, int> &map, const QStringList &headers)
{
    QStandardItemModel *model = new QStandardItemModel(this); //! Создаем новую модель
    model->setHorizontalHeaderLabels(headers);                //! Устанавливаем заголовки для модели
    //! Заполняем модель данными из QMap
    for (auto it = map.constBegin(); it != map.constEnd(); ++it)
        model->appendRow({new QStandardItem(QString::number(it.key())),
                          new QStandardItem(QString::number(it.value()))});
    return model; //! Возвращаем указатель на созданную модель
}

void InfoWindow::showError(const QString &message)
{
    QMessageBox::critical(this, tr("Ошибка"), message); //! Отображение сообщения об ошибке в окне
}

void InfoWindow::bindActions()
{
    //! Подключение слота для отображения ошибок
    this->connect(this, &InfoWindow::errorOccurred, this, &InfoWindow::showError);

    //! Подключение слота для сохранения результатов к действию меню
    this->connect(this->ui_->saveAction, &QAction::triggered, this, &InfoWindow::saveResults);
    //! Подключение слота для копирования результатов к действию меню
    this->connect(this->ui_->copyAction, &QAction::triggered, this, &InfoWindow::copyResults);
    //! Подключение слота для открытия локального файла к действию меню
    this->connect(this->ui_->fileOpenAction,
                  &QAction::triggered,
                  this,
                  &InfoWindow::requestOpenLocalFile);

    //! Подключение слота для открытия URL к действию меню
    this->connect(this->ui_->urlOpenAction, &QAction::triggered, this, &InfoWindow::requestOpenUrl);
}

void InfoWindow::fillUiFromStats()
{
    //! Установка количества спутников в метке
    this->ui_->inputCountLabel->setText(QString::number(this->stats_.records.size()));

    //! Установка даты и времени в метку
    this->ui_->inputDateLabel->setText(this->stats_.oldestEpoch.toString("dd.MM.yyyy hh:mm:ss"));

    //! Установка моделей для таблиц с данными
    this->ui_->yearTableView->setModel(
        this->modelFromMap(this->stats_.launchesPerYear, {tr("Год"), tr("Число запусков")}));
    this->ui_->inclinationTableView->setModel(
        this->modelFromMap(this->stats_.inclinationBins,
                           {tr("Наклонение (°)"), tr("Число запусков")}));

    //! Установка выравнивание заголовков таблиц
    this->ui_->yearTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->ui_->inclinationTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

InfoWindow::InfoWindow(const QVector<TleRecord> &records, QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::InfoWindow)
    , stats_(records)
{
    this->ui_->setupUi(this); //! Инициализация пользовательского интерфейса

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    this->updateStyles(); //! Обновление стилей приложения при смене темы
#endif

    this->bindActions(); //! Связываем действия с соответствующими слотами

    this->fillUiFromStats(); //! Заполнение пользовательского интерфейса статистикой
}

InfoWindow::~InfoWindow()
{
    delete this->ui_; //! Освобождение ресурсов пользовательского интерфейса
}
