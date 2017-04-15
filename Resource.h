#pragma once

#include <ostream>
#include <cstring>

#include "ContainerWrapper.h"
/************************************************************/

class Resource
{
public:
  friend std::ostream& operator<<(std::ostream& out, const Resource& res);

  Resource(int id, const std::string& name);

  int getId() const { return _id; }
  const char* getName() const { return _name; }

protected:
  int _id;
  char _name[50];
};

inline Resource::Resource(int id, const std::string& name)
  : _id(id)
{
  std::strcpy(_name, name.c_str());
}

inline std::ostream&
operator<<(std::ostream& out, const Resource& res)
{
  return out << "{" << res._id << ", " << res._name << "}";
}

/************************************************************/

class Teacher : public Resource
{
public:
  Teacher(const std::string& name);

private:
  static int _nextId;
};

inline Teacher::Teacher(const std::string& name)
  : Resource(_nextId++, name)
{
}

/************************************************************/

class Class : public Resource
{
public:
  Class(const std::string& name);

private:
  static int _nextId;
};

inline Class::Class(const std::string& name)
  : Resource(_nextId++, name)
{
}

/************************************************************/

class Room : public Resource
{
public:
  Room(const std::string& name);

private:
  static int _nextId;
};

inline Room::Room(const std::string& name)
  : Resource(_nextId++, name)
{
}

/************************************************************/

class Subject : public Resource
{
public:
  friend std::ostream& operator<<(std::ostream& out, const Subject& sub);

  Subject(const std::string& name, Teacher* teacher, Room* room);
  Subject();

  Teacher* getTeacher() const { return _teacher; }
  Room* getRoom() const { return _room; }

private:
  Teacher* _teacher;
  Room* _room;

  static int _nextId;
};

inline Subject::Subject(const std::string& name, Teacher* teacher, Room* room)
  : Resource(_nextId++, name)
  , _teacher(teacher)
  , _room(room)
{
}

inline std::ostream&
operator<<(std::ostream& out, const Subject& sub)
{
  return out << "[" << sub.getId() << ", " << sub.getName() << ", "
             << *(sub.getTeacher()) << ", " << *(sub.getRoom()) << "]";
}

/************************************************************/

class DataTuple : public Resource
{
public:
  friend std::ostream& operator<<(std::ostream& out, const DataTuple& tuple);

  DataTuple(Class* classObj, Subject* subject);

  Class* getClass() const { return _class; }
  Subject* getSubject() const { return _subject; }

private:
  Class* _class;
  Subject* _subject;

  static int _nextId;
};

inline DataTuple::DataTuple(Class* classObj, Subject* subject)
  : Resource(_nextId++, "")
  , _class(classObj)
  , _subject(subject)
{
}

inline std::ostream&
operator<<(std::ostream& out, const DataTuple& tuple)
{
  return out << "<" << tuple.getId() << ", " << *tuple.getClass() << ", "
             << *tuple.getSubject() << ">";
}

typedef ContainerWrapper<Resource> Resources;
typedef ContainerWrapper<Teacher> Teachers;
typedef ContainerWrapper<Class> Classes;
typedef ContainerWrapper<Room> Rooms;
typedef ContainerWrapper<Subject> Subjects;
