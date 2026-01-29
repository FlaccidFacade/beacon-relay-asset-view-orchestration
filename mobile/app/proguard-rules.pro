# Add project specific ProGuard rules here.
# You can control the set of applied configuration files using the
# proguardFiles setting in build.gradle.

# Keep model classes for JSON serialization
-keep class com.bravo.mobile.models.** { *; }

# Keep Gson classes
-keepattributes Signature
-keepattributes *Annotation*
-dontwarn sun.misc.**
-keep class com.google.gson.** { *; }

# Keep OSMDroid classes
-keep class org.osmdroid.** { *; }
-dontwarn org.osmdroid.**

# Keep USB Serial classes
-keep class com.hoho.android.usbserial.** { *; }
-dontwarn com.hoho.android.usbserial.**

# Keep Nordic BLE library classes
-keep class no.nordicsemi.android.ble.** { *; }
-dontwarn no.nordicsemi.android.ble.**
