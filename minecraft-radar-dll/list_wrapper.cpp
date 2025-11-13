#include "list_wrapper.h"

extern JNIEnv* g_JniEnv;

ListWrapper::ListWrapper(jobject listObject) : listObject(listObject) {
	jclass listClass = g_JniEnv->FindClass("java/util/List");
	jmethodID listSizeMethod = g_JniEnv->GetMethodID(listClass, "size", "()I");

	size = g_JniEnv->CallIntMethod(listObject, listSizeMethod);
}

jobject ListWrapper::GetElement(int index) {
	jclass listClass = g_JniEnv->FindClass("java/util/List");
	jmethodID listGetMethodId = g_JniEnv->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");

	return g_JniEnv->CallObjectMethod(listObject, listGetMethodId, index);
}