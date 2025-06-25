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

    for (int i = 0; i < ui_->infoComboBox->count(); ++i)
        //! Записываем каждый элемент списка в файл
        out << this->ui_->infoComboBox->itemText(i) << "\n";

    file.close(); //! Закрываем файл после записи
    //! Показываем сообщение об успешном сохранении
    QMessageBox::information(this, tr("Успех"), tr("Результаты успешно сохранены."));
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

    //! Заполнение выпадающего списка названиями спутников
    for (TleRecord record : records)
        if (!record.name.isEmpty())
            //! Если имя спутника не пустое, добавляем его в список
            ui_->infoComboBox->addItem(record.name);
        else
            //! Если имя спутника пустое, добавляем номер спутника
            ui_->infoComboBox->addItem("Спутник №" + QString::number(record.catalogNumber));
}

InfoWindow::~InfoWindow()
{
    delete this->ui_; //! Освобождение ресурсов пользовательского интерфейса
}
