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
#include "infowindow.hpp"
#include "./ui_infowindow.h"

void InfoWindow::saveResults()
{
    //! Открываем диалоговое окно для сохранения файла
    QString filePath = QFileDialog::getSaveFileName(this,
                                                    tr("Сохранить результаты"),
                                                    "",
                                                    tr("Текстовый файл (*.txt)"));
    //! Проверяем, что путь не пустой
    if (filePath.isEmpty())
        return;

    QFile file(filePath); //! Открываем файл для записи
    //! Проверяем, что файл успешно открыт
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        //! Если файл не удалось открыть, показываем сообщение об ошибке
        this->ui_->statusLabel->setText(tr("Не удалось открыть файл для записи!"));
        QTimer::singleShot(3000, this, [&]() {
            this->ui_->statusLabel->clear(); //! Очищаем метку статуса через 3 секунды
        });
        return;
    }

    QTextStream out(&file); //! Записываем результаты в файл

    //! Записываем количество спутников
    out << tr("Количество спутников: %1\n").arg(this->records_.size());
    //! Записываем дату самого старого спутника
    out << tr("Дата самого старого спутника: %1\n").arg(this->ui_->inputDateLabel->text());

    //! Записываем количество спутников в каждом году
    out << tr("Количество запусков по годам:\n");
    for (int i = 0; i < this->ui_->yearTableView->model()->rowCount(); ++i) {
        QString year = this->ui_->yearTableView->model()->index(i, 0).data().toString();
        QString count = this->ui_->yearTableView->model()->index(i, 1).data().toString();
        out << tr("%1: %2\n").arg(year, count);
    }

    //! Записываем количество спутников по наклону
    out << tr("Количество спутников по наклону:\n");
    for (int i = 0; i < this->ui_->inclinationTableView->model()->rowCount(); ++i) {
        QString inclination = this->ui_->inclinationTableView->model()->index(i, 0).data().toString();
        QString count = this->ui_->inclinationTableView->model()->index(i, 1).data().toString();
        out << tr("%1°: %2\n").arg(inclination, count);
    }

    file.close(); //! Закрываем файл после записи

    //! Показываем сообщение об успешном сохранении
    this->ui_->statusLabel->setText(tr("Результаты успешно сохранены в файл ") + filePath);
    QTimer::singleShot(3000, this, [&]() {
        this->ui_->statusLabel->clear(); //! Очищаем метку статуса через 3 секунды
    });
}

void InfoWindow::copyResults()
{
    // TODO: Реализовать копирование результатов в буфер обмена
    QClipboard *clipboard = QApplication::clipboard(); //! Получаем буфер обмена
    QString results;                                   //! Строка для хранения результатов
    results += tr("Количество спутников: %1\n").arg(this->records_.size());
    results += tr("Дата самого старого спутника: %1\n").arg(this->ui_->inputDateLabel->text());
    results += tr("Количество запусков по годам:\n");
    for (int i = 0; i < this->ui_->yearTableView->model()->rowCount(); ++i) {
        QString year = this->ui_->yearTableView->model()->index(i, 0).data().toString();
        QString count = this->ui_->yearTableView->model()->index(i, 1).data().toString();
        results += tr("%1: %2\n").arg(year, count);
    }
    results += tr("Количество спутников по наклону:\n");
    for (int i = 0; i < this->ui_->inclinationTableView->model()->rowCount(); ++i) {
        QString inclination = this->ui_->inclinationTableView->model()->index(i, 0).data().toString();
        QString count = this->ui_->inclinationTableView->model()->index(i, 1).data().toString();
        results += tr("%1°: %2\n").arg(inclination, count);
    }

    clipboard->setText(results); //! Устанавливаем текст в буфер обмена

    this->ui_->statusLabel->setText(tr("Информация скопирована в буфер обмена."));
    QTimer::singleShot(3000, this, [&]() {
        this->ui_->statusLabel->clear(); //! Очищаем метку статуса через 3 секунды
    });
}

InfoWindow::InfoWindow(const QList<TleRecord> &records, QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::InfoWindow)
    , records_(records)
{
    // FIXME: ИСПРАВИТЬ СОРТИРОВКУ, ЧТОБЫ СОРТИРОВАЛОСЬ ПО ЧИСЛАМ
    this->ui_->setupUi(this); //! Инициализация пользовательского интерфейса

    //! Установка заголовка окна
    this->connect(this->ui_->aboutAction, &QAction::triggered, this, [&]() {
        QMessageBox::about(this,
                           tr("О программе"),
                           tr("\"Спутники\" - это программа для работы с TLE-файлами."));
    });

    //! Подключение слота для сохранения результатов к кнопке
    connect(this->ui_->saveButton, &QPushButton::clicked, this, &InfoWindow::saveResults);
    //! Подключение слота для сохранения результатов к действию меню
    connect(this->ui_->saveAction, &QAction::triggered, this, &InfoWindow::saveResults);
    //! Подключение слота для копирования результатов к кнопке
    connect(this->ui_->copyButton, &QPushButton::clicked, this, &InfoWindow::copyResults);
    //! Подключение слота для копирования результатов к действию меню
    connect(this->ui_->copyAction, &QAction::triggered, this, &InfoWindow::copyResults);

    //! Установка количества спутников в метке
    this->ui_->inputCountLabel->setText(QString::number(records.size()));

    //! Находим самую старую дату
    QDateTime oldest = QDateTime::currentDateTime(); //! Текущая дата и время
    bool first = true;                               //! Флаг для первого элемента
    for (const auto &record : this->records_) {
        int intEpoch = int(record.epoch);                //! Целая часть эпохи
        int yy = intEpoch / 1000;                        //! Год (последние 2 цифры)
        int ddd = intEpoch % 1000;                       //! День года (от 1 до 366)
        double frac = record.epoch - intEpoch;           //! Дробная часть эпохи (доля дня)
        int year = yy < 57 ? 2000 + yy : 1900 + yy;      //! Преобразование года в полный формат
        QDate date = QDate(year, 1, 1).addDays(ddd - 1); //! Создание даты из года и дня года
        QTime time = QTime(0, 0).addSecs(
            int(frac * 86400));                   //! Создание времени из дробной части эпохи
        QDateTime dt(date, time, QTimeZone::UTC); //! Создание QDateTime из даты и времени
        if (first || dt < oldest) {
            //! Если это первый элемент или дата меньше текущей самой старой даты
            oldest = dt;
            first = false;
        }
    }

    this->ui_->inputDateLabel->setText(oldest.toString("dd.MM.yyyy")); //! Установка даты в метку

    QMap<int, int> launchesPerYear; //! Карта для хранения количества запусков по годам
    for (const auto &rec : records) {
        int yy = rec.yearLaunch;                          //! Год запуска (последние 2 цифры)
        int launchYear = yy < 57 ? 2000 + yy : 1900 + yy; //! Преобразование года в полный формат
        launchesPerYear[launchYear]++; //! Увеличиваем счетчик запусков для данного года
    }

    QMap<int, int> inclinationBins; //! Карта для хранения количества спутников по наклону
    for (const auto &rec : records) {
        int deg = qRound(rec.inclination); //! Округляем наклон до ближайшего целого числа
        inclinationBins[deg]++;            //! Увеличиваем счетчик спутников для данного наклона
    }

    //! Создание моделей для таблиц с данными о запусках
    QStandardItemModel *yearLaunchModel = new QStandardItemModel(this->ui_->yearTableView);
    yearLaunchModel->setHorizontalHeaderLabels({tr("Год"), tr("Число запусков")});

    //! Заполнение модели данными о запусках по годам
    for (auto it = launchesPerYear.constBegin(); it != launchesPerYear.constEnd(); ++it) {
        // для каждого ключа/значения сразу создаём строку и добавляем её в модель
        QList<QStandardItem *> row;
        row << new QStandardItem(QString::number(it.key()))
            << new QStandardItem(QString::number(it.value()));
        yearLaunchModel->appendRow(row);
    }

    this->ui_->yearTableView->setModel(yearLaunchModel); //! Установка модели для таблицы по годам
    //! Установка режима растяжения заголовков
    this->ui_->yearTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStandardItemModel *inclinationLaunchModel = new QStandardItemModel(
        this->ui_->inclinationTableView);
    inclinationLaunchModel->setHorizontalHeaderLabels({tr("Наклонение (°)"), tr("Число запусков")});

    //! Заполнение модели данными о запусках по наклону
    for (auto it = inclinationBins.constBegin(); it != inclinationBins.constEnd(); ++it) {
        QList<QStandardItem *> row;
        row << new QStandardItem(QString::number(it.key()))
            << new QStandardItem(QString::number(it.value()));
        inclinationLaunchModel->appendRow(row);
    }

    this->ui_->inclinationTableView->setModel(inclinationLaunchModel);
    this->ui_->inclinationTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

InfoWindow::~InfoWindow()
{
    delete this->ui_; //! Освобождение ресурсов пользовательского интерфейса
}
