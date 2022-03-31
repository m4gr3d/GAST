# Godot XRApp Framework

The Godot XRApp framework provides the ability to turn regular Android apps into Quest VR apps!

It unlocks a new and diverse set of VR apps that can be created by leveraging
Android developers tools as well as existing Android UI code and libraries.

## License

The framework is released under the MIT license.

## Usage

The Godot XRApp framework can be integrated within an existing code base in three steps:

1. Add the framework dependencies
   - Within the app `build.gradle` file, add the following dependencies:
   ```
    implementation "io.github.m4gr3d:godot:3.4.4.stable"
    implementation "io.github.m4gr3d:godot-openxr:1.3.0.beta5"
    implementation "io.github.m4gr3d:gast-xrapp:0.2.0"
   ```
   - The framework is built atop [`AppCompatActivity`](https://developer.android.com/reference/androidx/appcompat/app/AppCompatActivity)
     , so include the appcompat dependency if you don't have it already
     ```
     implementation 'androidx.appcompat:appcompat:1.3.1'
     ```

1. Update the app's main activity
    - Have the app's main activity extend `org.godotengine.plugin.gast.xrapp.GastActivity`
    - Overrides the `isXREnabled()` method and return `true`
        - As its name implies, this method can be used to enable / disable XR integration.

1. Update the app's `AndroidManifest.xml` file to comply with the requirements for running on Quest
    - Add the following to the main activity's `intent-filter`:
      ```
      <category android:name="com.oculus.intent.category.VR" />
      ```
    - The framework is built atop [`AppCompatActivity`](https://developer.android.com/reference/androidx/appcompat/app/AppCompatActivity)
      , so specify an appcompat theme in the `<application>` or main `<activity>` tag if you
      don't have one already. E.g:
      ```
      android:theme="@style/Theme.AppCompat.Light.NoActionBar"
      ```

That's all! Your app should now build and run on the Quest as a VR app.

Feel free to take a look at the [samples](../../../samples/xrapp) directory and the [XR
Universal Android Music Player Sample](https://github.com/m4gr3d/xruamp) for examples.

### Current Limitations

- No access to Google Play Services: The Quest platform doesn't have Google Play Services. If
  your app uses one or multiple Google Play Services APIs, it won't be able to run on the
  device.
- No support for [SurfaceView](https://developer.android.com/reference/android/view/SurfaceView)
  , [TextureView](https://developer.android.com/reference/android/view/TextureView) and their
  derivatives.
    - This means functionality like video playback are not available yet.
    - Video playback support will be enabled in future releases.
- For any other issues, please [open a github issue](https://github.com/m4gr3d/GAST/issues) with
  the [`xrapp` label](https://github.com/m4gr3d/GAST/labels/xrapp).

## Additional Features

### Use the same code for regular and VR builds

By switching `GastActivity#isXREnabled()`'s return value, you can use the same code base to
generate a regular Android app and a Quest VR app.

You can see an example of this approach in the [XR Universal Android Music Player Sample](https://github.com/m4gr3d/xruamp)
which uses [Android product
flavors](https://developer.android.com/studio/build/build-variants#product-flavors) to customize
the generated builds.

### Available passthrough environment

The framework provides the ability to toggle
[passthrough](https://support.oculus.com/articles/in-vr-experiences/oculus-features/what-is-passthrough/)
on the Quest, enabling the user access to your app while interacting with their real-time
surroundings:

- Use `GastActivity#startPassthrough()` to start passthrough
- Use `GastActivity#stopPassthrough()` to disable passthrough

You can see an example of that functionality in the [hello_vr_world](../../../samples/xrapp/hello_vr_world)
sample.

### Automatic support for hand tracking

The framework has built-in support for hand tracking, enabling access to your app without the
needs for controllers.

### Oculus Keyboard support

The framework has built-in support for the Oculus Keyboard for input.

## [Roadmap](ROADMAP.md)


