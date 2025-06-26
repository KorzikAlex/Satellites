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
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось открыть файл для записи."));
        return;
    }

    QTextStream out(&file); //! Записываем результаты в файл

    // TODO: реализовать сохранение результатов в файл

    file.close(); //! Закрываем файл после записи

    //! Показываем сообщение об успешном сохранении
    this->ui_->statusLabel->setText("Результаты успешно сохранены в файл " + filePath);
    QTimer::singleShot(3000, this, [&]() {
        this->ui_->statusLabel->clear(); //! Очищаем метку статуса через 3 секунды
    });
}

void InfoWindow::copyResults()
{
    this->ui_->statusLabel->setText("Информация скопирована в буфер обмена.");
    // TODO: Реализовать копирование результатов в буфер обмена
    QTimer::singleShot(3000, this, [&]() {
        this->ui_->statusLabel->clear(); //! Очищаем метку статуса через 3 секунды
    });
}

InfoWindow::InfoWindow(const QList<TleRecord> &records, QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::InfoWindow)
    , records_(records)
{
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

    //! 1) Находим самую старую дату
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
            int(frac * 86400));            //! Создание времени из дробной части эпохи
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
        int yy = rec.yearLaunch;                            //! Год запуска (последние 2 цифры)
        int launchYear = (yy < 57 ? 2000 + yy : 1900 + yy); //! Преобразование года в полный формат
        launchesPerYear[launchYear]++; //! Увеличиваем счетчик запусков для данного года
    }

    QMap<int, int> inclinationBins; //! Карта для хранения количества спутников по наклону
    for (const auto &rec : records) {
        int deg = int(rec.inclination + 0.5); //! Округляем наклон до ближайшего целого числа
        inclinationBins[deg]++;               //! Увеличиваем счетчик спутников для данного наклона
    }
}

InfoWindow::~InfoWindow()
{
    delete this->ui_; //! Освобождение ресурсов пользовательского интерфейса
}
