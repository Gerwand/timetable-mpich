#include "DataParser.h"

using namespace std;

int
DataParser::getResources(DataResources& resources, DataTuples& tuples)
{
    if (!_inputData.is_open())
        return -1;

    string line;
    DataType type = UNKNOWN;
    while (!_inputData.eof()) {
        getline(_inputData, line);

        if (line.empty())
            continue;
        // Check for option
        if (line[0] == '>') {
            if (line == (">CLASSES:"))
                type = CLASSES;
            else if (line == (">ROOMS:"))
                type = ROOMS;
            else if (line == (">TEACHERS:"))
                type = TEACHERS;
            else if (line == (">SUBJECTS:"))
                type = SUBJECTS;
            else if (line == (">TUPLES:"))
                type = TUPLES;
            else
                type = UNKNOWN;
        } else {

            switch (type) {
                case CLASSES:
                    resources.addClass(new Class(line));
                    break;

                case ROOMS:
                    resources.addRoom(new Room(line));
                    break;

                case TEACHERS:
                    resources.addTeacher(new Teacher(line));
                    break;

                case SUBJECTS: {
                    vector<string> columns;
                    split(line, ';', columns);

                    string& name = columns[0];
                    const Teacher* teacher =
                      resources.getTeacher().get(columns[1]);
                    const Room* room = resources.getRoom().get(columns[2]);

                    if (teacher == nullptr) {
                        cout << "Teacher " << columns[1]
                             << " not existing - subject " << name << endl;
                        return -2;
                    }
                    if (teacher == nullptr) {
                        cout << "Room " << columns[2]
                             << " not existing - subject " << name << endl;
                        return -2;
                    }

                    resources.addSubject(new Subject(name, teacher, room));
                    break;
                }
                case TUPLES: {
                    vector<string> columns;
                    split(line, ';', columns);

                    const Class* classObj =
                      resources.getClass().get(columns[0]);
                    const Subject* subject =
                      resources.getSubject().get(columns[1]);
                    if (classObj == nullptr) {
                        cout << "Subject " << columns[1]
                             << " not existing - class " << columns[0] << endl;
                        return -2;
                    }
                    if (subject == nullptr) {
                        cout << "Class " << columns[0]
                             << " not existing - subject " << columns[1]
                             << endl;
                        return -2;
                    }

                    tuples.add(new DataTuple(classObj, subject));
                    break;
                }

                default:
                    continue;
            }
        }
    }

    return 0;
}
