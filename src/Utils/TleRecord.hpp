/*!
 * \file TleRecord.hpp
 * \brief Заголовочный файл для структуры TleRecord
 * \details
 * Этот файл содержит определение структуры TleRecord,
 * которая используется для хранения данных о спутниках в формате TLE (Two-Line Element).
 * \author KorzikAlex
 * \copyright This project is released under the MIT License.
 * \date 2025
 */
#ifndef TLERECORD_HPP
#define TLERECORD_HPP

#include <QString>

/*!
 * \brief TleRecord - структура для хранения данных TLE
 * \details
 * Эта структура содержит поля для хранения информации о спутниках в формате TLE.
 */
struct TleRecord
{
    //! Поля для хранения данных TLE:
    QString name;  //! Заголовок (имя спутника или объекта)
    QString line1; //! Первая строка TLE (начинается с '1 ')
    QString line2; //! Вторая строка TLE (начинается с '2 ')

    //! Поля из первой строки TLE:
    int catalogNumber;      //! Номер спутника (из line1)
    QString classification; //! Класс ('U' — unclassified, 'C' — classified, 'S' — secret)
    int yearLaunch;         //! Последние 2 цифры года запуска
    int numberLaunch;       //! Номер запуска
    QString launchPiece;    //! Часть запуска
    int epochYearSuffix;    //! Последние две цифры года эпохи
    double epochTime;       //! Часть эпохи, отвечающая за день (цела часть) и время (дробная часть)
    double meanMotionFirstDerivative;  //! Первая производная от среднего движения (rev/day^2)
    double meanMotionSecondDerivative; //! Вторая производная от среднего движения (rev/day^3)
    QString brakingCoefficient;        //! Коэффициент торможения B*
    int ephemerisType;                 //! Тип эфемерид (обычно 0)
    int elementSetNumber;              //! Номер элемента
    int checksum1;                     //! Контрольная сумма (из line1)

    //! Поля из второй строки TLE:
    double inclination;          //! Наклонение (градусы)
    double rightAscension;       //! Долгота восходящего узла (градусы)
    double eccentricity;         //! Эксцентриситет (но без точки, например, "0006703" → 0.0006703)
    double argPerigee;           //! Аргумент перигея (градусы)
    double meanAnomaly;          //! Средняя аномалия (градусы)
    double meanMotion;           //! Среднее движение (обращения в день)
    int revolutionNumberOfEpoch; //! Номер обращения
    int checksum2;               //! Контрольная сумма (из line2)
};

#endif // TLERECORD_HPP
