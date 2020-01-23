#ifndef UTILS_H
#define UTILS_H

#include <android/log.h>

namespace gast {

#define LOG_TAG "GAST"

#define ALOG_ASSERT(_cond, ...) \
	if (!(_cond)) __android_log_assert("conditional", LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
}  // namespace gast

#endif // UTILS_H
