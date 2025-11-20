#pragma once

#include <jni.h>

class ListWrapper {
private:
	int size = 0;
	jobject listObject = nullptr;

public:
	ListWrapper(jobject listObject);

	jobject Get(int index);

	int Size() { return size; }
};