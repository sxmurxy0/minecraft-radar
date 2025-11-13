#pragma once

#include <jni.h>

class ListWrapper {
private:
	int size = 0;
	jobject listObject = nullptr;

public:
	ListWrapper(jobject listObject);

	jobject GetElement(int index);

	int GetSize() { return size; }
};