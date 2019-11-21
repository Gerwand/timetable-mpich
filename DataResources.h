#pragma once

#include "Resource.h"

class DataResources
{
  public:
	const Teachers &getTeacher() { return _teachers; }
	const Rooms &getRoom() { return _rooms; }
	const Subjects &getSubject() { return _subjects; }
	const Classes &getClass() { return _classes; }

	void addTeacher(Teacher *teacher) { _teachers.add(teacher); }
	void addRoom(Room *room) { _rooms.add(room); }
	void addSubject(Subject *subject) { _subjects.add(subject); }
	void addClass(Class *classObj) { _classes.add(classObj); }

  private:
	Teachers _teachers;
	Classes _classes;
	Rooms _rooms;
	Subjects _subjects;
};