plugins {
    id ("com.android.library")
    kotlin("android")
    kotlin("android.extensions")
}

android {
    compileSdkVersion(Versions.compileSdk)
    buildToolsVersion = Versions.buildTools
    defaultConfig {
        minSdkVersion(Versions.minSdk)
        targetSdkVersion(Versions.targetSdk)
        versionName = "1.0"
        versionCode = 1
        externalNativeBuild {
            cmake {
                cppFlags += "-std=c++14"
            }
        }

        ndk {
            abiFilters("arm64-v8a")
        }
    }

    externalNativeBuild {
        cmake {
            setPath("src/main/cpp/CMakeLists.txt")
        }
    }
}

dependencies {
    implementation("org.jetbrains.kotlin:kotlin-stdlib-jdk7:${Versions.kotlinVersion}")
    implementation("com.android.support:appcompat-v7:28.0.0")
    compileOnly(fileTree(mapOf("dir" to "libs", "include" to listOf("godot-lib*.aar"))))
}