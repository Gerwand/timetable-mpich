#pragma once

#include <ostream>
#include <cstring>

#include "ContainerWrapper.h"
/************************************************************/

class Resource
{
  public:
	friend std::ostream &operator<<(std::ostream &out, const Resource &res);

	Resource(int id, const std::string &name);

	int getId() const { return _id; }
	const char *getName() const { return _name; }

  protected:
	int _id;
	char _name[50];
};

inline Resource::Resource(int id, const std::string &name)
	: _id(id)
{
	std::strcpy(_name, name.c_str());
}

inline std::ostream &
operator<<(std::ostream &out, const Resource &res)
{
	return out << "{" << res._id << ", " << res._name << "}";
}

/************************************************************/

class Teacher : public Resource
{
  public:
	Teacher(const std::string &name);

  private:
	static int _nextId;
};

inline Teacher::Teacher(const std::string &name)
	: Resource(_nextId++, name)
{
}

/************************************************************/

class Class : public Resource
{
  public:
	Class(const std::string &name);

  private:
	static int _nextId;
};

inline Class::Class(const std::string &name)
	: Resource(_nextId++, name)
{
}

/************************************************************/

class Room : public Resource
{
  public:
	Room(const std::string &name);

  private:
	static int _nextId;
};

inline Room::Room(const std::string &name)
	: Resource(_nextId++, name)
{
}

/************************************************************/

class Subject : public Resource
{
  public:
	friend std::ostream &operator<<(std::ostream &out, const Subject &sub);

	Subject(const std::string &name, const Teacher *teacher, const Room *room);

	const Teacher *getTeacher() const { return _teacher; }
	const Room *getRoom() const { return _room; }

  private:
	const Teacher *_teacher;
	const Room *_room;

	static int _nextId;
};

inline Subject::Subject(const std::string &name, const Teacher *teacher, const Room *room)
	: Resource(_nextId++, name), _teacher(teacher), _room(room)
{
}

inline std::ostream &
operator<<(std::ostream &out, const Subject &sub)
{
	return out << "[" << sub.getId() << ", " << sub.getName() << ", "
			   << *(sub.getTeacher()) << ", " << *(sub.getRoom()) << "]";
}

/************************************************************/

class DataTuple : public Resource
{
  public:
	friend std::ostream &operator<<(std::ostream &out, const DataTuple &tuple);

	DataTuple(const Class *classObj, const Subject *subject);

	const Class *getClass() const { return _class; }
	const Subject *getSubject() const { return _subject; }

  private:
	const Class *_class;
	const Subject *_subject;

	static int _nextId;
};

inline DataTuple::DataTuple(const Class *classObj, const Subject *subject)
	: Resource(_nextId++, ""), _class(classObj), _subject(subject)
{
}

inline std::ostream &
operator<<(std::ostream &out, const DataTuple &tuple)
{
	return out << "<" << tuple.getId() << ", " << *tuple.getClass() << ", "
			   << *tuple.getSubject() << ">";
}

typedef ContainerWrapper<Resource> Resources;
typedef ContainerWrapper<Teacher> Teachers;
typedef ContainerWrapper<Class> Classes;
typedef ContainerWrapper<Room> Rooms;
typedef ContainerWrapper<Subject> Subjects;
