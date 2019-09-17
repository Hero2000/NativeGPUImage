/*
 * Copyright (c) 2014 Spotify AB
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef __JniHelpers_h__
#define __JniHelpers_h__

#include "JniHelpersCommon.h"

#include "ByteArray.h"
#include "JavaClass.h"
#include "ClassRegistry.h"
#include "JavaClassUtils.h"
#include "JavaExceptionUtils.h"
#include "JavaString.h"
#include "JavaThreadUtils.h"
#include "JniGlobalRef.h"
#include "JniLocalRef.h"
#include "NativeObject.h"
#include "ShortArray.h"
#include "JavaStringArray.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
static ben::jni::ClassRegistry registry;
EXPORT JNIEnv *jniHelpersInitialize(JavaVM *jvm);
EXPORT JNIEnv *JniCurrentEnv(JavaVM *jvm);

#endif // __JniHelpers_h__
