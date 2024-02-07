#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <cstdlib>
#include <string>
#include <ctime>


/**
 * \struct Employee
 * \brief Структура, представляющая сотрудника с полями: имя, должность, отдел, зарплата.
 */
typedef unsigned int uint;
struct Employee {
    std::string name;        ///< Имя сотрудника
    std::string position;    ///< Должность сотрудника
    std::string department;  ///< Отдел, в котором работает сотрудник
    double salary;           ///< Заработная плата сотрудника

    // Перегрузка операторов сравнения
    bool operator>(const Employee& other) const {
        if (department != other.department)
            return department > other.department;
        if (name != other.name)
            return name > other.name;
        return salary > other.salary;
    }

    bool operator<(const Employee& other) const {
        if (department != other.department)
            return department < other.department;
        if (name != other.name)
            return name < other.name;
        return salary < other.salary;
    }

    bool operator>=(const Employee& other) const {
        return !(*this < other);
    }

    bool operator<=(const Employee& other) const {
        return !(*this > other);
    }

    bool operator==(const Employee& other) const {
    return name == other.name && position == other.position && department == other.department && salary == other.salary;
}
};


void measureSortingTime(const std::string& filename, void (*sortFunction)(std::vector<Employee>&));
void generateRandomData(const std::string& filename, int size);
std::vector<Employee> readDataFromFile(const std::string& filename);
void writeDataToFile(const std::vector<Employee>& data, const std::string& filename);
void insertionSort(std::vector<Employee>& data);
void shakerSort(std::vector<Employee>& data);
std::vector<Employee> merge(std::vector<Employee>& left, std::vector<Employee>& right);
std::vector<Employee> mergeSort(std::vector<Employee>& datadata);


/**
 * \brief Функция для измерения времени сортировки и записи результатов в файлы.
 * \param filename Имя файла с данными.
 * \param sortFunction Указатель на функцию сортировки.
 * \param f Индекс сортировки (1 - вставками, 2 - шейкером, 3 - слиянием).
 * \return Нет.
 */
void measureSortingTime(const std::string& filename, void (*sortFunction)(std::vector<Employee>&), int f) {
    // Чтение данных из файла
    std::vector<Employee> data = readDataFromFile(filename);

    // Замер времени сортировки
    auto start = std::chrono::high_resolution_clock::now();
    sortFunction(data);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    // Формирование названия файла для записи времени
    std::string timeFilename;
    switch (f) {
        case 1:
            timeFilename = "time_insertionSort.txt";
            break;
        case 2:
            timeFilename = "time_shakerSort.txt";
            break;
        case 3:
            timeFilename = "time_mergeSort.txt";
            break;
        default:
            std::cerr << "Invalid sorting function specified.\n";
            return;
    }

    // Запись времени сортировки в файл
    std::ofstream timeFile(timeFilename, std::ios::app);
    if (timeFile.is_open()) {
        timeFile << "Sorting time for " << filename <<": "<< duration.count() << " seconds\n";
        timeFile.close();
        std::cout << "Sorting time for " << filename << " has been written to " << timeFilename << "\n";
    } else {
        std::cerr << "Unable to open time file for writing.\n";
    }

    // Формирование названия файла для записи результатов
    std::string outputFilename;
    switch (f) {
        case 1:
            outputFilename = "output_insertionSort_" + filename +".txt";
            break;
        case 2:
            outputFilename = "output_shakerSort_" + filename+".txt";
            break;
        case 3:
            outputFilename = "output_mergeSort_" + filename+".txt";
            break;
        default:
            std::cerr << "Invalid sorting function specified.\n";
            return;
    }

    // Запись отсортированных данных в файл
    writeDataToFile(data, outputFilename);
}


std::string randomString(uint l = 15, std::string charIndex = "abcdefghijklmnaoqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890")
{ 
    uint length = rand() % l + 1; 

    uint ri[15]; 

    for (uint i = 0; i < length; ++i) 
        ri[i] = rand() % charIndex.length();

    std::string rs = ""; 

    for (uint i = 0; i < length; ++i) 
        rs += charIndex[ri[i]];

    if (rs.empty()) randomString(l, charIndex); 

    else return rs;
}

/**
 * \brief Функция для генерации случайных данных и записи их в файл.
 * \param filename Имя файла для записи данных.
 * \param size Размер данных.
 * \return Нет.
 */
void generateRandomData(const std::string& filename, int size) {
    std::ofstream file(filename);

    if (file.is_open()) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> salaryDistribution(1000.0, 10000.0);

        for (int i = 0; i < size; ++i) {
            Employee employee;
            employee.name = randomString();
            employee.position = randomString();
            employee.department = randomString();
            employee.salary = salaryDistribution(gen);

            file << employee.name << " " << employee.position << " " << employee.department << " " << employee.salary << "\n";
        }

        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

/**
 * \brief Функция для считывания данных из файла.
 * \param filename Имя файла с данными.
 * \return Вектор с данными о сотрудниках.
 */
std::vector<Employee> readDataFromFile(const std::string& filename) {
    std::vector<Employee> data;
    std::ifstream file(filename);

    if (file.is_open()) {
        while (!file.eof()) {
            Employee employee;
            file >> employee.name >> employee.position >> employee.department >> employee.salary;
            data.push_back(employee);
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }

    return data;
}

/**
 * \brief Функция для записи отсортированных данных в файл.
 * \param data Вектор отсортированных данных о сотрудниках.
 * \param filename Имя файла для записи данных.
 * \return Нет.
 */
void writeDataToFile(const std::vector<Employee>& data, const std::string& filename) {
    std::ofstream file(filename);

    if (file.is_open()) {
        for (const auto& employee : data) {
            file << employee.name << " " << employee.position << " " << employee.department << " " << employee.salary << "\n";
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

/**
 * \brief Функция сортировки вставками.
 * \param data Вектор данных о сотрудниках.
 * \return Нет.
 */
void insertionSort(std::vector<Employee>& data) {
    int n = data.size();
    for (int i = 1; i < n; ++i) {
        Employee key = data[i];
        int j = i - 1;

        while (j >= 0 && data[j] > key) {
            data[j + 1] = data[j];
            j = j - 1;
        }
        data[j + 1] = key;
    }
}

/**
 * \brief Функция шейкер-сортировки.
 * \param data Вектор данных о сотрудниках.
 * \return Нет.
 */
void shakerSort(std::vector<Employee>& data) {
    int n = data.size();
    bool swapped = true;
    int start = 0;
    int end = n - 1;

    while (swapped) {
        swapped = false;

        for (int i = start; i < end; ++i) {
            if (data[i] > data[i + 1]) {
                std::swap(data[i], data[i + 1]);
                swapped = true;
            }
        }

        if (!swapped) break;

        swapped = false;

        --end;

        for (int i = end - 1; i >= start; --i) {
            if (data[i] > data[i + 1]) {
                std::swap(data[i], data[i + 1]);
                swapped = true;
            }
        }

        ++start;
    }
}

/**
 * \brief Функция сортировки слиянием.
 * \param data Вектор данных о сотрудниках.
 * \return Отсортированный вектор данных.
 */
std::vector<Employee> merge(std::vector<Employee>& left, std::vector<Employee>& right) {
    std::vector<Employee> result;
    int i = 0, j = 0;

    while (i < left.size() && j < right.size()) {
        if (left[i] <= right[j]) {
            result.push_back(left[i]);
            i++;
        } else {
            result.push_back(right[j]);
            j++;
        }
    }

    while (i < left.size()) {
        result.push_back(left[i]);
        i++;
    }

    while (j < right.size()) {
        result.push_back(right[j]);
        j++;
    }

    return result;
}

/**
 * \brief Вспомогательная функция для слияния двух отсортированных векторов.
 * \param left Левый вектор.
 * \param right Правый вектор.
 * \return Вектор, представляющий результат слияния.
 */
std::vector<Employee> mergeSort(std::vector<Employee>& data) {
    if (data.size() <= 1) {
        return data;
    }

    int mid = data.size() / 2;
    std::vector<Employee> left(data.begin(), data.begin() + mid);
    std::vector<Employee> right(data.begin() + mid, data.end());

    left = mergeSort(left);
    right = mergeSort(right);

    return merge(left, right);
}

/**
 * \brief Вспомогательная функция для вызова сортировки слиянием.
 * \param data Вектор данных о сотрудниках.
 * \return Нет.
 */
void mergeSortWrapper(std::vector<Employee>& data) {
    data = mergeSort(data);
}

/**
 * \brief Основная функция программы.
 * \return Код завершения программы.
 */
int main() {
    srand(time(NULL));
    for (int size = 100; size <= 100000; size += 11100) {
    generateRandomData("input_" + std::to_string(size) + ".txt", size);
    }
    // Выполнить замеры времени для каждого набора данных и каждого алгоритма
	measureSortingTime("input_100.txt", insertionSort, 1);
	measureSortingTime("input_11200.txt", insertionSort, 1);
	measureSortingTime("input_22300.txt", insertionSort, 1);
	measureSortingTime("input_33400.txt", insertionSort, 1);
	measureSortingTime("input_44500.txt", insertionSort, 1);
	measureSortingTime("input_55600.txt", insertionSort, 1);
	measureSortingTime("input_66700.txt", insertionSort, 1);
	measureSortingTime("input_77800.txt", insertionSort, 1);
	measureSortingTime("input_88900.txt", insertionSort, 1);
	measureSortingTime("input_100000.txt", insertionSort, 1);


	measureSortingTime("input_100.txt", shakerSort, 2);
	measureSortingTime("input_11200.txt", shakerSort, 2);
	measureSortingTime("input_22300.txt", shakerSort, 2);
	measureSortingTime("input_33400.txt", shakerSort, 2);
	measureSortingTime("input_44500.txt", shakerSort, 2);
	measureSortingTime("input_55600.txt", shakerSort, 2);
	measureSortingTime("input_66700.txt", shakerSort, 2);
	measureSortingTime("input_77800.txt", shakerSort, 2);
	measureSortingTime("input_88900.txt", shakerSort, 2);
	measureSortingTime("input_100000.txt", shakerSort, 2);


	measureSortingTime("input_100.txt", mergeSortWrapper, 3);
	measureSortingTime("input_11200.txt", mergeSortWrapper, 3);
	measureSortingTime("input_22300.txt", mergeSortWrapper, 3);
	measureSortingTime("input_33400.txt", mergeSortWrapper, 3);
	measureSortingTime("input_44500.txt", mergeSortWrapper, 3);
	measureSortingTime("input_55600.txt", mergeSortWrapper, 3);
	measureSortingTime("input_66700.txt", mergeSortWrapper, 3);
	measureSortingTime("input_77800.txt", mergeSortWrapper, 3);
	measureSortingTime("input_88900.txt", mergeSortWrapper, 3);
	measureSortingTime("input_100000.txt", mergeSortWrapper, 3);
    


    return 0;
}