#include <stdio.h>
#include <iostream>
#include <vector>
#include <jni.h>
#include <thread>
#include <future>
#include <unordered_map>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dlfcn.h>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#include <whale.h>

#include "ElfImg.h"
#include "Utils.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "APKKiller", __VA_ARGS__)

#define apk_asset_path "original.apk"
#define apk_fake_name "original.apk"

std::vector<std::vector<uint8_t>> apk_signatures {
    {
        0x30,0x82,0x05,0x89,0x30,0x82,0x03,0x71,0xA0,0x03,0x02,0x01,0x02,0x02,0x15,0x00,
        0xBE,0x97,0xE4,0x67,0xE9,0x7D,0xB4,0xE7,0x65,0xB5,0x40,0x86,0x26,0x0F,0x69,0xCB,
        0xDA,0xEE,0x7A,0x73,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,
        0x0B,0x05,0x00,0x30,0x74,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,
        0x55,0x53,0x31,0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x08,0x13,0x0A,0x43,0x61,0x6C,
        0x69,0x66,0x6F,0x72,0x6E,0x69,0x61,0x31,0x16,0x30,0x14,0x06,0x03,0x55,0x04,0x07,
        0x13,0x0D,0x4D,0x6F,0x75,0x6E,0x74,0x61,0x69,0x6E,0x20,0x56,0x69,0x65,0x77,0x31,
        0x14,0x30,0x12,0x06,0x03,0x55,0x04,0x0A,0x13,0x0B,0x47,0x6F,0x6F,0x67,0x6C,0x65,
        0x20,0x49,0x6E,0x63,0x2E,0x31,0x10,0x30,0x0E,0x06,0x03,0x55,0x04,0x0B,0x13,0x07,
        0x41,0x6E,0x64,0x72,0x6F,0x69,0x64,0x31,0x10,0x30,0x0E,0x06,0x03,0x55,0x04,0x03,
        0x13,0x07,0x41,0x6E,0x64,0x72,0x6F,0x69,0x64,0x30,0x20,0x17,0x0D,0x32,0x31,0x30,
        0x38,0x30,0x31,0x31,0x37,0x34,0x37,0x33,0x37,0x5A,0x18,0x0F,0x32,0x30,0x35,0x31,
        0x30,0x38,0x30,0x31,0x31,0x37,0x34,0x37,0x33,0x37,0x5A,0x30,0x74,0x31,0x0B,0x30,
        0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x31,0x13,0x30,0x11,0x06,0x03,
        0x55,0x04,0x08,0x13,0x0A,0x43,0x61,0x6C,0x69,0x66,0x6F,0x72,0x6E,0x69,0x61,0x31,
        0x16,0x30,0x14,0x06,0x03,0x55,0x04,0x07,0x13,0x0D,0x4D,0x6F,0x75,0x6E,0x74,0x61,
        0x69,0x6E,0x20,0x56,0x69,0x65,0x77,0x31,0x14,0x30,0x12,0x06,0x03,0x55,0x04,0x0A,
        0x13,0x0B,0x47,0x6F,0x6F,0x67,0x6C,0x65,0x20,0x49,0x6E,0x63,0x2E,0x31,0x10,0x30,
        0x0E,0x06,0x03,0x55,0x04,0x0B,0x13,0x07,0x41,0x6E,0x64,0x72,0x6F,0x69,0x64,0x31,
        0x10,0x30,0x0E,0x06,0x03,0x55,0x04,0x03,0x13,0x07,0x41,0x6E,0x64,0x72,0x6F,0x69,
        0x64,0x30,0x82,0x02,0x22,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,
        0x01,0x01,0x05,0x00,0x03,0x82,0x02,0x0F,0x00,0x30,0x82,0x02,0x0A,0x02,0x82,0x02,
        0x01,0x00,0xA5,0x6A,0x0B,0x68,0xF4,0x2C,0xB1,0xAD,0x29,0x59,0xC4,0x6D,0x1D,0x75,
        0x68,0x82,0x22,0xAD,0x57,0xF9,0x1F,0xB4,0xD1,0x32,0x1A,0x29,0xC5,0x1D,0x5D,0xF8,
        0x4B,0xD6,0xC4,0x97,0x5A,0x8D,0xBC,0x10,0x09,0x69,0xBA,0x70,0xA6,0xFF,0xE3,0x1F,
        0xCF,0xDB,0xD0,0xDE,0x59,0x18,0x4A,0xCB,0x29,0x90,0x5F,0xD5,0xD1,0xD7,0xE1,0x30,
        0xE4,0x1B,0xDD,0x68,0x2C,0x77,0x64,0x59,0xD6,0xF9,0x0F,0x15,0xE1,0xC6,0x09,0x62,
        0x29,0x39,0xD8,0x14,0xDB,0x8F,0x4E,0x65,0xF2,0x28,0xB1,0x46,0xDB,0x42,0xEC,0x17,
        0xB0,0x57,0x49,0x75,0xDF,0x35,0xC1,0x22,0x8F,0xB6,0xE7,0xA0,0xC0,0x2C,0x30,0xA5,
        0xC2,0xCC,0x93,0x3B,0x1F,0x4D,0xA8,0x23,0x73,0x16,0xE8,0xA3,0xCA,0xB2,0x41,0x0A,
        0x6D,0xD0,0xEA,0x25,0xC6,0xC9,0xE5,0x4D,0xB8,0x99,0x6F,0xD9,0xEF,0xCC,0x5F,0xD4,
        0xA2,0xD7,0x83,0xE0,0x10,0x41,0x61,0xB9,0x5C,0x7C,0x8A,0x60,0x3D,0xF5,0x28,0x80,
        0x6F,0xB7,0xFE,0xAE,0x66,0x9E,0x8E,0x89,0x23,0xD7,0xE6,0x1D,0x1A,0x62,0xB3,0xB5,
        0x31,0x5E,0x0B,0xD2,0x83,0xC4,0x97,0x45,0x88,0xAC,0x87,0x88,0xE3,0x9D,0x5F,0xE7,
        0xA3,0x09,0xA9,0x9B,0x25,0x65,0xC3,0x0C,0x0B,0xAA,0x4C,0x0F,0x82,0x92,0x1D,0xA9,
        0x70,0xA3,0x11,0x8F,0xE5,0x22,0xED,0x24,0x1E,0x2B,0x66,0xB0,0xDB,0xFE,0x5F,0xAF,
        0xF9,0xCE,0x2B,0x8E,0xB4,0x93,0x75,0xA7,0x41,0x90,0xA1,0x12,0x06,0x48,0xC7,0xB1,
        0x84,0x29,0xA8,0xB6,0x76,0x52,0xC8,0x18,0xD9,0x9C,0x5E,0xB2,0xDF,0x63,0xCF,0x81,
        0x74,0x98,0x24,0x84,0x8C,0xD8,0xD9,0x2A,0xD7,0x5D,0x0C,0xCA,0xCE,0xC9,0x47,0x77,
        0x5A,0x36,0x8F,0xF5,0xDD,0x69,0x04,0x6D,0xCC,0x2E,0x55,0x73,0x28,0xBE,0xE2,0x4D,
        0x08,0xDD,0x6C,0x17,0xDA,0xC2,0xC9,0xBD,0x06,0x7F,0xDA,0x41,0xFC,0xA9,0x22,0x19,
        0x19,0xBF,0x0C,0xF0,0xD3,0xB7,0x61,0x5C,0x14,0xD1,0x80,0xEF,0x1C,0x5F,0x1E,0x25,
        0xCA,0x68,0xB6,0xAA,0x9A,0x1F,0xAB,0x82,0x78,0x11,0x45,0xD0,0x5C,0x74,0xC8,0xDD,
        0x34,0x5A,0xCF,0x99,0xA3,0xA7,0x7F,0xE8,0x81,0xAC,0x55,0xD4,0x40,0x56,0x62,0x97,
        0x60,0xA1,0x2C,0xFE,0x15,0x3F,0x78,0x72,0x05,0x98,0x51,0xE0,0x79,0x11,0x6D,0x19,
        0x02,0xE3,0xA9,0x5D,0x4F,0xB5,0x1E,0xC3,0xE5,0x9B,0x21,0x7D,0x25,0x76,0x1A,0xE0,
        0x44,0xA9,0x02,0xF8,0x98,0x2B,0x50,0xA2,0xD2,0x13,0x02,0xE9,0xB7,0x5C,0x67,0xF7,
        0xA3,0x4D,0xF9,0x85,0x19,0x8C,0x46,0x7D,0xC1,0x69,0x02,0x0D,0x6F,0xD1,0xF7,0x86,
        0x54,0x1E,0x4D,0x8B,0x5B,0xE1,0xD4,0x40,0x64,0x44,0xA7,0x37,0xC5,0x34,0xB2,0x22,
        0x2E,0xEB,0xCF,0x8F,0x49,0xCD,0x46,0x52,0x33,0x51,0x31,0x91,0x49,0x48,0x86,0x8D,
        0xD2,0x3E,0x51,0x27,0x54,0x92,0x6E,0x6C,0x83,0xFB,0x0D,0x8B,0xF6,0x59,0x7D,0x30,
        0x63,0xD4,0xD6,0xFE,0xCA,0xA0,0x9B,0x3A,0x49,0x62,0x23,0xA4,0x11,0xDE,0x76,0x04,
        0x3F,0x7A,0xBF,0x2A,0x7B,0xD7,0xF5,0xA1,0x25,0x67,0x93,0xD2,0x17,0x57,0x32,0x50,
        0x76,0x0B,0xDB,0x09,0x16,0xF7,0x9A,0x1A,0xB5,0xCF,0x7B,0xD3,0x6F,0x38,0x89,0x76,
        0x78,0x55,0x02,0x03,0x01,0x00,0x01,0xA3,0x10,0x30,0x0E,0x30,0x0C,0x06,0x03,0x55,
        0x1D,0x13,0x04,0x05,0x30,0x03,0x01,0x01,0xFF,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,
        0x86,0xF7,0x0D,0x01,0x01,0x0B,0x05,0x00,0x03,0x82,0x02,0x01,0x00,0x54,0xE7,0x40,
        0xC2,0xD9,0x68,0x2D,0x0A,0x7D,0xD0,0xD0,0xB5,0x1E,0xB3,0x50,0x58,0x94,0xF0,0x85,
        0xAA,0xA0,0xF6,0x1F,0x0F,0x30,0x1F,0x15,0xD5,0xC7,0x35,0x0A,0xC6,0xB4,0x1B,0xCD,
        0xE3,0x42,0xE5,0xD1,0x18,0x99,0x96,0xD5,0x79,0x29,0x33,0xD7,0x10,0x52,0xF2,0x8F,
        0x80,0x9D,0x8A,0x30,0xCC,0xF2,0x0B,0xBC,0x47,0xE9,0x0B,0x7D,0xEB,0xAE,0x4B,0xF0,
        0xFD,0xB2,0x8A,0x1E,0xE8,0x29,0xD1,0x1C,0xD4,0xE6,0x85,0x09,0xF8,0x4F,0xBD,0x04,
        0x98,0x62,0x41,0x60,0x0E,0x40,0xD2,0x98,0xBB,0xB5,0xE0,0x39,0xD9,0x2B,0x96,0x6F,
        0x03,0xB3,0x9A,0x92,0x8B,0xAE,0xB6,0x0F,0xB5,0x3E,0x6D,0xBF,0x4F,0x6C,0x81,0x3E,
        0xAC,0x82,0x21,0xB5,0x39,0xD6,0x27,0xE3,0x8E,0x4D,0xA7,0xCC,0x51,0xC3,0x39,0xE5,
        0xA8,0x40,0x74,0x8E,0xBD,0x31,0x87,0x5C,0x14,0xE8,0xD0,0xF8,0xDE,0x04,0xF6,0x7E,
        0x47,0xC5,0x45,0xBC,0xD4,0x85,0xCC,0x72,0x5E,0x45,0xDC,0xB3,0x9A,0x68,0x0D,0x10,
        0xB5,0x3B,0x16,0xC5,0x45,0xD1,0xCD,0x67,0x1F,0x75,0x22,0xBF,0x7F,0xFF,0x65,0x94,
        0x52,0x9E,0xAE,0x9B,0x94,0xEC,0x9D,0x40,0xE3,0x8A,0x17,0xE5,0x61,0xCE,0x23,0x4F,
        0x48,0x72,0xF1,0x48,0x98,0xB3,0x20,0x7D,0x13,0x44,0x63,0x70,0xFE,0xF2,0x83,0x9C,
        0x19,0x34,0x80,0xBE,0x33,0xB2,0xCC,0x00,0xBC,0xD4,0x2E,0xD8,0xE6,0xEA,0x69,0x2F,
        0xD1,0xCD,0x69,0xF7,0x82,0xA0,0x26,0xD1,0x8E,0x18,0x85,0x17,0x69,0xE2,0x24,0x2F,
        0xA4,0x04,0xFF,0x0C,0xE6,0xF8,0x2A,0x73,0x61,0x41,0x2C,0xF5,0x0C,0x1D,0x17,0xB4,
        0x78,0xB0,0x78,0xE8,0x1E,0xC4,0x28,0x23,0xD7,0xB2,0xFB,0x78,0x10,0xCA,0xF0,0x59,
        0xE4,0x44,0x40,0xBA,0xEC,0xB9,0xF4,0xFD,0x22,0x09,0xD9,0xC4,0x97,0xE0,0x6C,0x43,
        0x86,0x7C,0x6E,0xCE,0xFC,0x81,0x67,0x54,0x8D,0x4A,0x94,0xCC,0xD7,0xA2,0xC0,0x79,
        0xFF,0x02,0x09,0x2D,0x62,0x87,0x81,0xFC,0x42,0xFB,0xE7,0xAE,0x01,0x7D,0xDA,0x40,
        0x85,0x8F,0x4B,0x97,0x31,0x5F,0x41,0x48,0xEF,0x20,0xF9,0xE2,0xCD,0xB7,0xBF,0xDA,
        0x11,0xCC,0xA2,0x72,0x7A,0x04,0xF3,0xD6,0xC2,0xBF,0x66,0x66,0x32,0xF7,0x83,0xB8,
        0x98,0xAA,0x91,0x0A,0xB5,0x0B,0x83,0xCF,0x5C,0xE6,0xB1,0xA3,0x38,0xCB,0xEF,0x25,
        0x14,0x24,0x3C,0x59,0x26,0x2D,0x8F,0x75,0xA0,0x9E,0x4B,0x18,0x25,0xA9,0x15,0x7C,
        0x3C,0xD3,0x43,0x75,0xAC,0x6F,0xEC,0xF7,0xF4,0x7F,0x89,0x9E,0x81,0x37,0xE1,0xBD,
        0x73,0xB4,0x43,0xC8,0xD7,0xA3,0x22,0x3C,0xEA,0xF4,0xC2,0x63,0x0D,0xA3,0x7C,0xF4,
        0x9C,0x56,0x69,0x9D,0x53,0x3A,0xE2,0x3E,0x39,0x14,0x2E,0xB0,0x07,0xBD,0xD6,0xE2,
        0xC2,0xCB,0x6F,0x1C,0xD4,0x62,0x23,0x2A,0xB3,0x7D,0x1D,0xA7,0x4E,0x65,0x48,0x70,
        0x02,0x1F,0x8F,0x1C,0x00,0xDD,0x36,0x4A,0x62,0xAF,0x5E,0x83,0x97,0xB3,0xD6,0x38,
        0x12,0xCD,0xFF,0x8D,0x92,0x5B,0x61,0xAF,0xA8,0x15,0xA4,0xC8,0x97,0x5C,0x31,0x40,
        0x67,0xC3,0x1A,0xEF,0xED,0x7A,0x11,0x8A,0x8D,0xD2,0x3E,0x5F,0xE4,0xB6,0x3A,0x1A,
        0xB6,0x2B,0x8C,0x92,0xA2,0x4B,0x28,0xFB,0x4D,0x1F,0xE8,0x97,0xF8
    }
};

JavaVM *g_vm;
JNIEnv *g_env;
std::string g_pkgName;
jstring g_apkPath;
jobject g_proxy, g_pkgMgr;

const char *getProcessName() {
    FILE *f = fopen("/proc/self/cmdline", "rb");
    if (f) {
        char *buf = new char[64];
        fread(buf, sizeof(char), 64, f);
        fclose(f);
        return buf;
    }
    return 0;
}

std::string getPackageName(jobject obj) {
    auto contextClass = g_env->FindClass("android/content/Context");
    auto getPackageNameMethod = g_env->GetMethodID(contextClass, "getPackageName", "()Ljava/lang/String;");
    return g_env->GetStringUTFChars((jstring) g_env->CallObjectMethod(obj, getPackageNameMethod), 0);
}

jobject getApplicationContext(jobject obj) {
    auto contextClass = g_env->FindClass("android/content/Context");
    auto getApplicationContextMethod = g_env->GetMethodID(contextClass, "getApplicationContext", "()Landroid/content/Context;");
    return g_env->CallObjectMethod(obj, getApplicationContextMethod);
}

jobject getPackageManager(jobject obj) {
    auto contextClass = g_env->FindClass("android/content/Context");
    auto getPackageManagerMethod = g_env->GetMethodID(contextClass, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    return g_env->CallObjectMethod(obj, getPackageManagerMethod);
}

class Reference {
public:
    JNIEnv *env;
    jobject reference;
public:
    Reference(JNIEnv *env, jobject obj) {
        this->env = env;
        this->reference = env->NewGlobalRef(obj);
    }

    Reference(jobject reference) {
        this->env = g_env;
        this->reference = reference;
    }

    jobject get() {
        auto referenceClass = env->FindClass("java/lang/ref/Reference");
        auto get = env->GetMethodID(referenceClass, "get", "()Ljava/lang/Object;");
        return env->CallObjectMethod(reference, get);
    }
};

class WeakReference : public Reference {
public:
    WeakReference(JNIEnv *env, jobject weakReference) : Reference(env, weakReference) {
    }

    WeakReference(jobject weakReference) : Reference(weakReference) {
    }

    static jobject Create(jobject obj) {
        auto weakReferenceClass = g_env->FindClass("java/lang/ref/WeakReference");
        auto weakReferenceClassConstructor = g_env->GetMethodID(weakReferenceClass, "<init>", "(Ljava/lang/Object;)V");
        return g_env->NewObject(weakReferenceClass, weakReferenceClassConstructor, obj);
    }
};

class ArrayList {
private:
    JNIEnv *env;
    jobject arrayList;
public:
    ArrayList(JNIEnv *env, jobject arrayList) {
        this->env = env;
        this->arrayList = arrayList;
    }

    ArrayList(jobject arrayList) {
        this->env = g_env;
        this->arrayList = arrayList;
    }

    jobject getObj() {
        return arrayList;
    }

    jobject get(int index) {
        auto arrayListClass = env->FindClass("java/util/ArrayList");
        auto getMethod = env->GetMethodID(arrayListClass, "get", "(I)Ljava/lang/Object;");
        return env->CallObjectMethod(arrayList, getMethod, index);
    }

    void set(int index, jobject value) {
        auto arrayListClass = env->FindClass("java/util/ArrayList");
        auto setMethod = env->GetMethodID(arrayListClass, "set", "(ILjava/lang/Object;)Ljava/lang/Object;");
        env->CallObjectMethod(arrayList, setMethod, index, value);
    }

    int size() {
        auto arrayListClass = env->FindClass("java/util/ArrayList");
        auto sizeMethod = env->GetMethodID(arrayListClass, "size", "()I");
        return env->CallIntMethod(arrayList, sizeMethod);
    }
};

class ArrayMap {
private:
    JNIEnv *env;
    jobject arrayMap;
public:
    ArrayMap(JNIEnv *env, jobject arrayMap) {
        this->env = env;
        this->arrayMap = arrayMap;
    }

    ArrayMap(jobject arrayMap) {
        this->env = g_env;
        this->arrayMap = arrayMap;
    }

    jobject getObj() {
        return arrayMap;
    }

    jobject valueAt(int index) {
        auto arrayMapClass = env->FindClass("android/util/ArrayMap");
        auto valueAtMethod = env->GetMethodID(arrayMapClass, "valueAt", "(I)Ljava/lang/Object;");
        return env->CallObjectMethod(arrayMap, valueAtMethod, index);
    }

    jobject setValueAt(int index, jobject value) {
        auto arrayMapClass = env->FindClass("android/util/ArrayMap");
        auto setValueAtMethod = env->GetMethodID(arrayMapClass, "setValueAt", "(ILjava/lang/Object;)Ljava/lang/Object;");
        return env->CallObjectMethod(arrayMap, setValueAtMethod, index, value);
    }

    int size() {
        auto arrayMapClass = env->FindClass("android/util/ArrayMap");
        auto sizeMethod = env->GetMethodID(arrayMapClass, "size", "()I");
        return env->CallIntMethod(arrayMap, sizeMethod);
    }
};

class Method {
private:
    JNIEnv *env;
    jobject method;
    jmethodID getNameMethod;
    jmethodID invokeMethod;

    void initMethod(jobject method) {
        this->method = method;

        jclass methodClass = env->FindClass("java/lang/reflect/Method");
        getNameMethod = env->GetMethodID(methodClass, "getName", "()Ljava/lang/String;");
        invokeMethod = env->GetMethodID(methodClass, "invoke", "(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;");

        auto setAccessibleMethod = env->GetMethodID(methodClass, "setAccessible", "(Z)V");
        env->CallVoidMethod(method, setAccessibleMethod, true);
    }

public:
    Method(JNIEnv *env, jobject method) {
        this->env = env;
        initMethod(method);
    }

    Method(jobject method) {
        this->env = g_env;
        initMethod(method);
    }

    ~Method() {
        env->DeleteLocalRef(method);
    }

    const char *getName() {
        return env->GetStringUTFChars((jstring) env->CallObjectMethod(method, getNameMethod), 0);
    }

    jobject invoke(jobject object, jobjectArray args = 0) {
        return env->CallObjectMethod(method, invokeMethod, object, args);
    }
};

class Field {
private:
    JNIEnv *env;
    jobject field;
    jmethodID getMethod;
    jmethodID setMethod;
public:
    Field(JNIEnv *env, jobject field) {
        this->env = env;
        this->field = field;

        jclass fieldClass = env->FindClass("java/lang/reflect/Field");
        getMethod = env->GetMethodID(fieldClass, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
        setMethod = env->GetMethodID(fieldClass, "set", "(Ljava/lang/Object;Ljava/lang/Object;)V");

        auto setAccessibleMethod = env->GetMethodID(fieldClass, "setAccessible", "(Z)V");
        env->CallVoidMethod(field, setAccessibleMethod, true);
    }

    ~Field() {
        env->DeleteGlobalRef(field);
    }

    jobject getField() {
        return field;
    }

    jobject get(jobject obj) {
        return env->CallObjectMethod(field, getMethod, obj);
    }

    void set(jobject obj, jobject value) {
        env->CallVoidMethod(field, setMethod, obj, value);
    }
};

class Class {
private:
    JNIEnv *env;
    jobject clazz;

    void initClass(const char *className) {
        auto classClass = env->FindClass("java/lang/Class");
        auto forNameMethod = env->GetStaticMethodID(classClass, "forName", "(Ljava/lang/String;)Ljava/lang/Class;");
        clazz = env->NewGlobalRef(env->CallStaticObjectMethod(classClass, forNameMethod, env->NewStringUTF(className)));
    }

public:
    Class(JNIEnv *env, const char *className) {
        this->env = env;
        initClass(className);
    }

    Class(const char *className) {
        this->env = g_env;
        initClass(className);
    }

    ~Class() {
        env->DeleteGlobalRef(clazz);
    }

    jobject getClass() {
        return clazz;
    }

    Field getField(const char *fieldName) {
        auto future = std::async([&]() -> jobject {
            JNIEnv *env;
            g_vm->AttachCurrentThread(&env, NULL);

            auto classClass = env->FindClass("java/lang/Class");
            jmethodID getDeclaredFieldMethod = env->GetMethodID(classClass, "getDeclaredField", "(Ljava/lang/String;)Ljava/lang/reflect/Field;");

            auto fieldNameObj = env->NewStringUTF(fieldName);
            auto result = env->CallObjectMethod(clazz, getDeclaredFieldMethod, fieldNameObj);
            if (env->ExceptionCheck()) {
                env->ExceptionDescribe();
                env->ExceptionClear();
            }

            if (result) {
                result = env->NewGlobalRef(result);
            }

            env->DeleteLocalRef(fieldNameObj);
            env->DeleteLocalRef(classClass);

            g_vm->DetachCurrentThread();
            return result;
        });
        return Field(env, future.get());
    }
};

void patch_ApplicationInfo(jobject obj) {
    if (obj) {
        LOGI("-------- Patching ApplicationInfo - %p", obj);
        Class applicationInfoClass("android.content.pm.ApplicationInfo");

        auto sourceDirField = applicationInfoClass.getField("sourceDir");
        auto publicSourceDirField = applicationInfoClass.getField("publicSourceDir");

        sourceDirField.set(obj, g_apkPath);
        publicSourceDirField.set(obj, g_apkPath);
    }
}

void patch_LoadedApk(jobject obj) {
    if (obj) {
        LOGI("-------- Patching LoadedApk - %p", obj);
        Class loadedApkClass("android.app.LoadedApk");

        auto mApplicationInfoField = loadedApkClass.getField("mApplicationInfo");
        patch_ApplicationInfo(mApplicationInfoField.get(obj));

        auto mAppDirField = loadedApkClass.getField("mAppDir");
        auto mResDirField = loadedApkClass.getField("mResDir");

        mAppDirField.set(obj, g_apkPath);
        mResDirField.set(obj, g_apkPath);
    }
}

void patch_AppBindData(jobject obj) {
    if (obj) {
        LOGI("-------- Patching AppBindData - %p", obj);
        Class appBindDataClass("android.app.ActivityThread$AppBindData");

        auto infoField = appBindDataClass.getField("info");
        patch_LoadedApk(infoField.get(obj));

        auto appInfoField = appBindDataClass.getField("appInfo");
        patch_ApplicationInfo(appInfoField.get(obj));
    }
}

void patch_ContextImpl(jobject obj) {
    if (obj) {
        LOGI("-------- Patching ContextImpl - %p", obj);
        Class contextImplClass("android.app.ContextImpl");

        auto mPackageInfoField = contextImplClass.getField("mPackageInfo");
        patch_LoadedApk(mPackageInfoField.get(obj));

        auto mPackageManagerField = contextImplClass.getField("mPackageManager");
        mPackageManagerField.set(obj, g_proxy);
    }
}

void patch_Application(jobject obj) {
    if (obj) {
        LOGI("-------- Patching Application - %p", obj);
        Class applicationClass("android.app.Application");

        auto mLoadedApkField = applicationClass.getField("mLoadedApk");
        patch_LoadedApk(mLoadedApkField.get(obj));
    }
}

AAssetManager *g_assetManager;

void extractAsset(std::string assetName, std::string extractPath) {
    LOGI("-------- Extracting %s to %s", assetName.c_str(), extractPath.c_str());
    AAssetManager *assetManager = g_assetManager;
    AAsset *asset = AAssetManager_open(assetManager, assetName.c_str(), AASSET_MODE_UNKNOWN);
    if (!asset) {
        return;
    }

    int fd = open(extractPath.c_str(), O_CREAT | O_WRONLY, 0644);
    if (fd < 0) {
        AAsset_close(asset);
        return;
    }

    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    int bytesRead;
    while ((bytesRead = AAsset_read(asset, buffer, BUFFER_SIZE)) > 0) {
        int bytesWritten = write(fd, buffer, bytesRead);
        if (bytesWritten != bytesRead) {
            AAsset_close(asset);
            close(fd);
            return;
        }
    }

    AAsset_close(asset);
    close(fd);
}

void patch_PackageManager(jobject obj) {
    if (!obj) return;

    Class activityThreadClass("android.app.ActivityThread");
    auto sCurrentActivityThreadField = activityThreadClass.getField("sCurrentActivityThread");
    auto sCurrentActivityThread = sCurrentActivityThreadField.get(NULL);

    auto sPackageManagerField = activityThreadClass.getField("sPackageManager");
    g_pkgMgr = g_env->NewGlobalRef(sPackageManagerField.get(NULL));

    Class iPackageManagerClass("android.content.pm.IPackageManager");

    auto classClass = g_env->FindClass("java/lang/Class");
    auto getClassLoaderMethod = g_env->GetMethodID(classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");

    auto classLoader = g_env->CallObjectMethod(iPackageManagerClass.getClass(), getClassLoaderMethod);
    auto classArray = g_env->NewObjectArray(1, classClass, NULL);
    g_env->SetObjectArrayElement(classArray, 0, iPackageManagerClass.getClass());

    auto apkKillerClass = g_env->FindClass("com/kuro/APKKiller");
    auto myInvocationHandlerField = g_env->GetStaticFieldID(apkKillerClass, "myInvocationHandler", "Ljava/lang/reflect/InvocationHandler;");
    auto myInvocationHandler = g_env->GetStaticObjectField(apkKillerClass, myInvocationHandlerField);

    auto proxyClass = g_env->FindClass("java/lang/reflect/Proxy");
    auto newProxyInstanceMethod = g_env->GetStaticMethodID(proxyClass, "newProxyInstance", "(Ljava/lang/ClassLoader;[Ljava/lang/Class;Ljava/lang/reflect/InvocationHandler;)Ljava/lang/Object;");
    g_proxy = g_env->NewGlobalRef(g_env->CallStaticObjectMethod(proxyClass, newProxyInstanceMethod, classLoader, classArray, myInvocationHandler));

    sPackageManagerField.set(sCurrentActivityThread, g_proxy);

    auto pm = getPackageManager(obj);
    Class applicationPackageManagerClass("android.app.ApplicationPackageManager");
    auto mPMField = applicationPackageManagerClass.getField("mPM");
    mPMField.set(pm, g_proxy);
}

void APKKill(JNIEnv *env, jclass clazz, jobject context) {
    env->PushLocalFrame(64);

    g_env = env;

    g_assetManager = AAssetManager_fromJava(env, env->CallObjectMethod(context, env->GetMethodID(env->FindClass("android/content/Context"), "getAssets", "()Landroid/content/res/AssetManager;")));

    std::string apkPkg = getPackageName(context);
    g_pkgName = apkPkg;

    auto procName = getProcessName();
    LOGI("-------- Killing %s", procName);

    char apkDir[512];
    sprintf(apkDir, "/data/data/%s/cache", apkPkg.c_str());
    mkdir(apkDir, 0777);

    std::string apkPath = "/data/data/";
    apkPath += apkPkg;
    apkPath += "/cache/";
    apkPath += apk_fake_name;

    if (access(apkPath.c_str(), F_OK) == -1) {
        extractAsset(apk_asset_path, apkPath);
    }

    g_apkPath = (jstring) env->NewGlobalRef(g_env->NewStringUTF(apkPath.c_str()));

    Class activityThreadClass("android.app.ActivityThread");
    auto sCurrentActivityThreadField = activityThreadClass.getField("sCurrentActivityThread");
    auto sCurrentActivityThread = sCurrentActivityThreadField.get(NULL);

    auto mBoundApplicationField = activityThreadClass.getField("mBoundApplication");
    patch_AppBindData(mBoundApplicationField.get(sCurrentActivityThread));

    auto mInitialApplicationField = activityThreadClass.getField("mInitialApplication");
    patch_Application(mInitialApplicationField.get(sCurrentActivityThread));

    auto mAllApplicationsField = activityThreadClass.getField("mAllApplications");
    auto mAllApplications = mAllApplicationsField.get(sCurrentActivityThread);
    ArrayList list(mAllApplications);
    for (int i = 0; i < list.size(); i++) {
        auto application = list.get(i);
        patch_Application(application);
        list.set(i, application);
    }
    mAllApplicationsField.set(sCurrentActivityThread, list.getObj());

    auto mPackagesField = activityThreadClass.getField("mPackages");
    auto mPackages = mPackagesField.get(sCurrentActivityThread);
    ArrayMap mPackagesMap(mPackages);
    for (int i = 0; i < mPackagesMap.size(); i++) {
        WeakReference loadedApk(mPackagesMap.valueAt(i));
        patch_LoadedApk(loadedApk.get());
        mPackagesMap.setValueAt(i, WeakReference::Create(loadedApk.get()));
    }
    mPackagesField.set(sCurrentActivityThread, mPackagesMap.getObj());

    auto mResourcePackagesField = activityThreadClass.getField("mResourcePackages");
    auto mResourcePackages = mResourcePackagesField.get(sCurrentActivityThread);
    ArrayMap mResourcePackagesMap(mResourcePackages);
    for (int i = 0; i < mResourcePackagesMap.size(); i++) {
        WeakReference loadedApk(mResourcePackagesMap.valueAt(i));
        patch_LoadedApk(loadedApk.get());
        mResourcePackagesMap.setValueAt(i, WeakReference::Create(loadedApk.get()));
    }
    mResourcePackagesField.set(sCurrentActivityThread, mResourcePackagesMap.getObj());

    patch_PackageManager(context);

    env->PopLocalFrame(0);
}

jobject processInvoke(JNIEnv *env, jclass clazz, jobject method, jobjectArray args) {
    env->PushLocalFrame(64);

    auto Integer_intValue = [&](jobject param) -> int {
        auto integerClass = env->FindClass("java/lang/Integer");
        auto intValueMethod = env->GetMethodID(integerClass, "intValue", "()I");
        return env->CallIntMethod(param, intValueMethod);
    };

    Method mMethod(env, method);
    const char *mName = mMethod.getName();
    auto mResult = mMethod.invoke(g_pkgMgr, args);

    if (!strcmp(mName, "getPackageInfo")) {
        const jobject packageInfo = mResult;
        if (packageInfo) {
            const char *packageName = env->GetStringUTFChars((jstring) env->GetObjectArrayElement(args, 0), 0);
            int flags = Integer_intValue(env->GetObjectArrayElement(args, 1));
            if (!strcmp(packageName, g_pkgName.c_str())) {
                if ((flags & 0x40) != 0) {
                    Class packageInfoClass(env, "android.content.pm.PackageInfo");
                    auto applicationInfoField = packageInfoClass.getField("applicationInfo");
                    auto applicationInfo = applicationInfoField.get(packageInfo);
                    if (applicationInfo) {
                        Class applicationInfoClass(env, "android.content.pm.ApplicationInfo");
                        auto sourceDirField = applicationInfoClass.getField("sourceDir");
                        auto publicSourceDirField = applicationInfoClass.getField("publicSourceDir");

                        sourceDirField.set(applicationInfo, g_apkPath);
                        publicSourceDirField.set(applicationInfo, g_apkPath);
                    }
                    applicationInfoField.set(packageInfo, applicationInfo);
                    auto signaturesField = packageInfoClass.getField("signatures");

                    auto signatureClass = env->FindClass("android/content/pm/Signature");
                    auto signatureConstructor = env->GetMethodID(signatureClass, "<init>", "([B)V");
                    auto signatureArray = env->NewObjectArray(apk_signatures.size(), signatureClass, NULL);
                    for (int i = 0; i < apk_signatures.size(); i++) {
                        auto signature = env->NewByteArray(apk_signatures[i].size());
                        env->SetByteArrayRegion(signature, 0, apk_signatures[i].size(), (jbyte *) apk_signatures[i].data());
                        env->SetObjectArrayElement(signatureArray, i, env->NewObject(signatureClass, signatureConstructor, signature));
                    }
                    signaturesField.set(packageInfo, signatureArray);
                } else if ((flags & 0x8000000) != 0) {
                    Class packageInfoClass(env, "android.content.pm.PackageInfo");
                    auto applicationInfoField = packageInfoClass.getField("applicationInfo");
                    auto applicationInfo = applicationInfoField.get(packageInfo);
                    if (applicationInfo) {
                        Class applicationInfoClass(env, "android.content.pm.ApplicationInfo");
                        auto sourceDirField = applicationInfoClass.getField("sourceDir");
                        auto publicSourceDirField = applicationInfoClass.getField("publicSourceDir");

                        sourceDirField.set(applicationInfo, g_apkPath);
                        publicSourceDirField.set(applicationInfo, g_apkPath);
                    }
                    applicationInfoField.set(packageInfo, applicationInfo);

                    auto signingInfoField = packageInfoClass.getField("signingInfo");
                    auto signingInfo = signingInfoField.get(packageInfo);

                    Class signingInfoClass(env, "android.content.pm.SigningInfo");
                    auto mSigningDetailsField = signingInfoClass.getField("mSigningDetails");
                    auto mSigningDetails = mSigningDetailsField.get(signingInfo);

                    Class signingDetailsClass(env, "android.content.pm.PackageParser$SigningDetails");
                    auto signaturesField = signingDetailsClass.getField("signatures");
                    auto pastSigningCertificatesField = signingDetailsClass.getField("pastSigningCertificates");

                    auto signatureClass = env->FindClass("android/content/pm/Signature");
                    auto signatureConstructor = env->GetMethodID(signatureClass, "<init>", "([B)V");
                    auto signatureArray = env->NewObjectArray(apk_signatures.size(), signatureClass, NULL);
                    for (int i = 0; i < apk_signatures.size(); i++) {
                        auto signature = env->NewByteArray(apk_signatures[i].size());
                        env->SetByteArrayRegion(signature, 0, apk_signatures[i].size(), (jbyte *) apk_signatures[i].data());
                        env->SetObjectArrayElement(signatureArray, i, env->NewObject(signatureClass, signatureConstructor, signature));
                    }
                    signaturesField.set(mSigningDetails, signatureArray);
                    pastSigningCertificatesField.set(mSigningDetails, signatureArray);
                } else {
                    Class packageInfoClass(env, "android.content.pm.PackageInfo");
                    auto applicationInfoField = packageInfoClass.getField("applicationInfo");
                    auto applicationInfo = applicationInfoField.get(packageInfo);
                    if (applicationInfo) {
                        Class applicationInfoClass(env, "android.content.pm.ApplicationInfo");
                        auto sourceDirField = applicationInfoClass.getField("sourceDir");
                        auto publicSourceDirField = applicationInfoClass.getField("publicSourceDir");

                        sourceDirField.set(applicationInfo, g_apkPath);
                        publicSourceDirField.set(applicationInfo, g_apkPath);
                    }
                    applicationInfoField.set(packageInfo, applicationInfo);
                }
            }
        }
    } else if (!strcmp(mName, "getApplicationInfo")) {
        const char *packageName = env->GetStringUTFChars((jstring) env->GetObjectArrayElement(args, 0), 0);
        if (!strcmp(packageName, g_pkgName.c_str())) {
            auto applicationInfo = mResult;
            if (applicationInfo) {
                Class applicationInfoClass(env, "android.content.pm.ApplicationInfo");

                auto sourceDirField = applicationInfoClass.getField("sourceDir");
                auto publicSourceDirField = applicationInfoClass.getField("publicSourceDir");

                sourceDirField.set(applicationInfo, g_apkPath);
                publicSourceDirField.set(applicationInfo, g_apkPath);
            }
        }
    } else if (!strcmp(mName, "getInstallerPackageName")) {
        const char *packageName = env->GetStringUTFChars((jstring) env->GetObjectArrayElement(args, 0), 0);
        if (!strcmp(packageName, g_pkgName.c_str())) {
            mResult = env->NewStringUTF("com.android.vending");
        }
    }

    if (mResult) {
        mResult = env->NewGlobalRef(mResult);
    }
    return env->PopLocalFrame(mResult);
}
