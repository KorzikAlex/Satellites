/*!
 * \file TleRecord.hpp
 * \brief Заголовочный файл для структуры TleStatistics
 * \details
 * Этот файл содержит определение структуры TleStatistics,
 * которая используется для хранения статистики по спутникам в формате TLE.
 * \author KorzikAlex
 * \copyright This project is released under the MIT License.
 * \date 2025
 */
#ifndef TLESTATISTICS_HPP
#define TLESTATISTICS_HPP

#include <QDateTime>
#include <QMap>
#include <QTimeZone>
#include <QVector>

#include "TleRecord.hpp"

/*!
 * \brief TleStatistics - структура для хранения статистики по спутникам TLE
 * \details
 * Эта структура содержит методы для вычисления статистики по спутникам,
 * таким как количество запусков по годам, наклонение и дата самой старой эпохи.
 */
struct TleStatistics
{
    QString name;
    QVector<TleRecord> records;                           //! Список записей TLE
    QDateTime oldestEpoch = QDateTime::currentDateTime(); //! Самая старая дата из записей TLE
    QMap<int, int> launchesPerYear;                       //! Количество запусков спутников по годам
    QMap<int, int> inclinationBins; //! Количество спутников в каждом диапазоне наклонения

    TleStatistics(const QVector<TleRecord> &recs)
        : records(recs)
    {
        computeOldest();          //! Вычисляем самую старую дату из записей TLE
        computeLaunchesPerYear(); //! Вычисляем количество запусков по годам
        computeInclinationBins(); //! Вычисляем количество спутников для каждого наклонения
    }

private:
    /*!
     * \brief computeOldest - вычисляет самую старую дату из записей TLE
     * \details
     * Этот метод проходит по всем записям TLE и находит самую раннюю дату,
     * основываясь на значении epochTime и epochYearSuffix.
     */
    void computeOldest()
    {
        //! Находим самую старую дату
        bool first = true; //! Флаг для первого элемента
        for (const TleRecord &record : this->records) {
            int day = int(record.epochTime); //! Целая часть epochTime представляет собой день года
            //! Дробная часть epochTime представляет собой время в одном дне
            double frac = record.epochTime - day;
            //! Определяем год из epochYearSuffix
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
            if (first || dt < this->oldestEpoch) {
                //! Если это первый элемент или дата меньше текущей самой старой даты
                this->oldestEpoch = dt;
                first = false;
            }
        }
    }

    /*!
     * \brief computeLaunchesPerYear - вычисляет количество запусков спутников по годам
     * \details
     * Этот метод проходит по всем записям TLE и подсчитывает количество запусков
     * по годам, основываясь на значении yearLaunch.
     */
    void computeLaunchesPerYear()
    {
        for (auto &rec : this->records) {
            //! Определяем год запуска
            int year = rec.yearLaunch < 57 ? 2000 + rec.yearLaunch : 1900 + rec.yearLaunch;
            launchesPerYear[year]++; //! Увеличиваем счетчик запусков для этого года
        }
    }

    /*!
     * \brief computeInclinationBins - вычисляет количество спутников в каждом диапазоне наклонения
     * \details
     * Этот метод проходит по всем записям TLE и подсчитывает количество спутников
     * в каждом градусе наклонениня.
     */
    void computeInclinationBins()
    {
        for (auto &rec : this->records) {
            int deg = qRound(rec.inclination); //! Округляем наклонение до ближайшего целого числа
            inclinationBins[deg]++; //! Увеличиваем счетчик спутников для этого наклонения
        }
    }
};

#endif // TLESTATISTICS_HPP
