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
        emit errorOccurred(tr("Не удалось открыть файл для записи: %1").arg(filePath));
        return;
    }

    QTextStream out(&file); //! Записываем результаты в файл

    //! Записываем количество спутников
    out << tr("Количество спутников: %1\n").arg(this->records_.size());
    //! Записываем дату самого старого спутника
    out << tr("Дата самых старых данных: %1\n").arg(this->ui_->inputDateLabel->text());

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
    this->statusBar()->showMessage(tr("Сохранено в файл: %1").arg(filePath), 2500);
}

void InfoWindow::copyResults()
{
    QClipboard *clipboard = QApplication::clipboard(); //! Получаем буфер обмена
    QString results;                                   //! Строка для хранения результатов
    //! Формируем текст результатов

    //! Добавляем количество спутников
    results += tr("Количество спутников: %1\n").arg(this->records_.size());
    //! Добавляем дату самого старых данных
    results += tr("Дата самых старых данных: %1\n").arg(this->ui_->inputDateLabel->text());

    //! Добавляем количество запусков по годам
    results += tr("Количество запусков по годам:\n");
    for (int i = 0; i < this->ui_->yearTableView->model()->rowCount(); ++i) {
        QString year = this->ui_->yearTableView->model()->index(i, 0).data().toString();
        QString count = this->ui_->yearTableView->model()->index(i, 1).data().toString();
        results += tr("%1: %2\n").arg(year, count);
    }
    //! Добавляем количество спутников по наклону
    results += tr("Количество спутников по наклону:\n");
    for (int i = 0; i < this->ui_->inclinationTableView->model()->rowCount(); ++i) {
        QString inclination = this->ui_->inclinationTableView->model()->index(i, 0).data().toString();
        QString count = this->ui_->inclinationTableView->model()->index(i, 1).data().toString();
        results += tr("%1°: %2\n").arg(inclination, count);
    }

    clipboard->setText(results); //! Устанавливаем текст в буфер обмена
    this->statusBar()->showMessage(tr("Скопировано!"), 2500);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
void InfoWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::ThemeChange)
        updateStyles();
    QMainWindow::changeEvent(event);
}

void InfoWindow::updateStyles()
{
    //! Получаем текущую цветовую схему
    const auto scheme = QGuiApplication::styleHints()->colorScheme();

    const QString prefix = (scheme == Qt::ColorScheme::Dark ? QStringLiteral(":/dark_icons/")
                                                            : QStringLiteral(":/light_icons/"));

    const QString suffix = (scheme == Qt::ColorScheme::Dark ? QStringLiteral("_dark")
                                                            : QStringLiteral("_light"));

    auto applyIcon = [&](auto *obj, const QString &baseName, int size) {
        const QString path = prefix + baseName + suffix + QLatin1Char('-') + QString::number(size)
                             + QLatin1String(".svg");
        obj->setIcon(QIcon(path));
    };

    applyIcon(this->ui_->fileOpenAction, QStringLiteral("upload"), 24);
    applyIcon(this->ui_->urlOpenAction, QStringLiteral("cloud_download"), 24);
    applyIcon(this->ui_->copyAction, QStringLiteral("copy_all"), 24);
    applyIcon(this->ui_->saveAction, QStringLiteral("save"), 24);
    applyIcon(this->ui_->aboutAction, QStringLiteral("info"), 24);

    this->ui_->toolBar->setStyleSheet(R"(
    QToolBar {
        border: none;
        background: transparent;
    }
    /* все кнопки в тулбаре */
    QToolBar QToolButton,
    QToolBar QToolButton:hover,
    QToolBar QToolButton:pressed {
        color: palette(windowText);
    })");
}
#endif

void InfoWindow::showError(const QString &message)
{
    QMessageBox::critical(this, tr("Ошибка"), message);
}

InfoWindow::InfoWindow(const QVector<TleRecord> &records, QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::InfoWindow)
    , records_(records)
{
    this->ui_->setupUi(this); //! Инициализация пользовательского интерфейса

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    this->updateStyles(); //! Обновление стилей приложения при смене темы
#endif
    //! Установка заголовка окна
    this->connect(this->ui_->aboutAction, &QAction::triggered, this, &InfoWindow::requestShowAbout);

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

    //! Установка количества спутников в метке
    this->ui_->inputCountLabel->setText(QString::number(records.size()));

    //! Находим самую старую дату
    QDateTime oldest = QDateTime::currentDateTime(); //! Текущая дата и время
    bool first = true;                               //! Флаг для первого элемента
    for (const TleRecord &record : this->records_) {
        int day = int(record.epochTime);
        double frac = record.epochTime - day;
        int year = record.epochYearSuffix < 57 ? 2000 + record.epochYearSuffix
                                               : 1900 + record.epochYearSuffix;

        QDate date = QDate(year, 1, 1).addDays(day - 1); //! Создание даты из года и дня года
        //! Создание времени из дробной части эпохи
        QTime time = QTime(0, 0).addSecs(int(frac * 86400));
        //! Создание QDateTime из даты и времени
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QDateTime dt(date, time, Qt::UTC);
#else
        QDateTime dt(date, time, QTimeZone::UTC);
#endif
        if (first || dt < oldest) {
            //! Если это первый элемент или дата меньше текущей самой старой даты
            oldest = dt;
            first = false;
        }
    }
    qDebug() << oldest; //! Отладочный вывод даты
    this->ui_->inputDateLabel->setText(
        oldest.toString("dd.MM.yyyy hh:mm:ss")); //! Установка даты в метку

    QMap<int, int> launchesPerYear; //! Карта для хранения количества запусков по годам
    for (const auto &rec : records) {
        int yearSuffix = rec.yearLaunch; //! Год запуска (последние 2 цифры)
        //! Преобразование года в полный формат
        int launchYear = yearSuffix < 57 ? 2000 + yearSuffix : 1900 + yearSuffix;
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
