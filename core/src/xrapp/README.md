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
   - The framework is built atop `AppCompatActivity`, so include the appcompat dependency if you
     don't have it already
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
    - The framework is built atop `AppCompatActivity`, so specify an appcompat theme in the
      `<application>` or main `<activity>` tag if you don't have one already
      ```
      android:theme="@style/Theme.AppCompat.Light.NoActionBar"
      ```

That's all! Your app should now build and run on the Quest as a VR app.

Feel free to take a look at the [samples](../../../samples/xrapp) directory for examples.

### Current Limitations

## Additional Features

### Available passthrough environment

### Automatic support for hand tracking

### Oculus Keyboard support

## [Roadmap](ROADMAP.md)


