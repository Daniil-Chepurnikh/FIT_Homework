#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const unsigned short marksNumber = 11; // число предметов для расчёта рейтинга
const unsigned short len = 11;

struct Student
{
    unsigned number; // номер зачётки

    string name; // имя
    string surname; // фамилия
    string patronymic; // отчество

    unsigned char marks[marksNumber]; // перечень оценок для расчёта рейтинга
};

unsigned short InputNumber(string message, string error, int min, int max)
{
    bool isCorrect = false;
    unsigned short number;
    do
    {
        cout << message;
        cin >> number;

        if (cin.fail() || number > max || number < min) // какой-то аналог трайпарс видимо
        {
            cout << error << '\n';
            cin.clear(); // эта штука похоже говорит забудь прошлую ошибку мы всё учли будем пробовать снова
            cin.ignore(32767, '\n');   // очищает буфер от ошибочного числа которое было введено. 
            // написано, если его не очистить, то влетаем в бесконечный цикл
            // потому что продолжают читать неправильные данные
        }
        else
            isCorrect = true;

    } while (!isCorrect);
    return number;
}

void MakeArrayConsole(Student studs[], int len)
{    
    for (int p = 0; p < len; p++)
    {
        studs[p].number = p + 1;

        cout << "Введите имя студента\n";
        cin >> studs[p].name;

        cout << "Введите фамилию студента\n";
        cin >> studs[p].surname;

        cout << "Введите отчество студента\n";
        cin >> studs[p].patronymic;

        for (int q = 0; q < marksNumber; q++)
        {
            studs[p].marks[q] = InputNumber("Введите оценку за предмет\n", "Вы не ввели целое число в допустимом диапазоне от 1 до 10\n", 1, 10);
        }
    }
}

double CalculateRating(Student stud)
{
    double rating = 0;
    for (int p = 0; p < marksNumber; p++)
    {
        rating += stud.marks[p];
    }
    return rating / marksNumber;
}

string ToString(Student stud)
{
    return stud.number + " " + stud.name + " " + stud.patronymic + " " + "Рейтинг: " + to_string(CalculateRating(stud)) + '\n';
}

void PrintArrayConsole(Student studs[], int len)
{
    for (int p = 0; p < len; p++)
    {
        cout << ToString(studs[p]);
    }
}

unsigned short GetMenuAction(string option1, string option2, string message)
{
    unsigned short action;

    cout << message << '\n';
    cout << " 1.  " << option1 << '\n';
    cout << " 2.  " << option2 << '\n';
    cout << "Введите номер выбранного действия (1 или 2): ";

    cin >> action;

    return action;
}

void PrintArrayFile(Student studs[], int len) // магия чистой воды
{ 
     unsigned short action = GetMenuAction("Новый файл", "Существующий файл", "Выберете в какой файл писать");
    
     switch (action)
     {
        case 1:
        {
            string fileName;
            cout << "Как нужно назвать новый файл?";
            cin >> fileName;
            fileName = fileName + ".txt";
            
            ofstream outFile(fileName); // говорим какой файл надо создать и открыть

            if (outFile.is_open()) // проверяем, открылся ли файл
            {
                for (int p = 0; p < len; p++) // пишем в файл
                {
                    outFile << ToString(studs[p]);
                }
                outFile.close(); // закрываем файл
            }
            else
                cout << "Ошибка при открытии файла!\n";
            break;
        }
        case 2:
        {
            ofstream file("output.txt", ios::app); 

            if (file.is_open())
            {
                for (int p = 0; p < len; p++) // пишем в файл
                {
                    file << ToString(studs[p]);
                }
                file.close(); // закрываем файл
            }
            break;
        }
     }
}


int main()
{
    setlocale(LC_ALL, "ru_RU.UTF - 8"); // была какая-то жесть с выводом и товарищ интернет сказал сделать так
    system("chcp 65001"); // чтобы писались нормальные русские слова

    Student students[len];
    MakeArrayConsole(students, len);

    PrintArrayConsole(students, len);

}
